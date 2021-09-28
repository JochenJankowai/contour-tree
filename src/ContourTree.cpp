#include "ContourTree.hpp"
#include "MergeTree.hpp"
#include <vector>
#include <deque>
#include <cassert>
#include <fstream>
#include <iostream>

namespace contourtree {

    void ContourTree::setup(const MergeTree* tree) {
        std::cout << "setting up merge process" << std::endl;;
        this->tree = tree;
        nv = tree->data->getVertexCount();
        nodesJoin.resize(nv);
        nodesSplit.resize(nv);
        ctNodes.resize(nv);
        for (int64_t i = 0; i < nv; i++) {
            // init
            nodesJoin[i].v = i;
            nodesSplit[i].v = i;
            ctNodes[i].v = i;

            // add join arcs
            int64_t to = i;
            int64_t from = tree->prev[to];
            if (from != -1) {
                nodesSplit[from].next.push_back(to);
                nodesSplit[to].prev.push_back(from);
            }

            // add split arcs
            to = tree->next[i];
            from = i;
            if (to != -1) {
                nodesJoin[from].next.push_back(to);
                nodesJoin[to].prev.push_back(from);
            }
        }
    }

    void ContourTree::computeCT() {
        std::cout << "merging join and split trees" << std::endl;;
        std::deque<int64_t> q;
        for (int64_t v = 0; v < nv; v++) {
            auto [jn_v, jn_next, jn_prev] = nodesJoin[v];
            if (auto [sn_v, sn_next, sn_prev] = nodesSplit[v]; sn_next.size() + jn_prev.size() == 1) {
                q.push_back(v);
            }
        }

        while (!q.empty()) {
            const auto xi = q.front();
            q.pop_front();
            auto [jn_v, jn_next, jn_prev] = nodesJoin[xi];
            auto [sn_v, sn_next, sn_prev] = nodesSplit[xi];
            if (sn_next.empty() && sn_prev.empty()) {
                assert((jn_next.empty() && jn_prev.empty()));
                continue;
            }

            if (sn_next.empty()) {
                if (sn_prev.size() > 1) {
                    std::cout << "Can this happen too???" << std::endl;
                    assert(false);
                }
                auto xj = sn_prev[0];
                remove(xi, nodesJoin);
                remove(xi, nodesSplit);

                const auto fr = xj;
                const auto to = xi;
                assert(fr < nv&& to < nv);
                addArc(fr, to);
                if (nodesSplit[xj].next.size() + nodesJoin[xj].prev.size() == 1) {
                    q.push_back(xj);
                }
            }
            else {
                if (jn_next.size() > 1) {
                    std::cout << "Can this happen too???" << std::endl;
                    assert(false);
                }
                auto xj = jn_next[0];
                remove(xi, nodesJoin);
                remove(xi, nodesSplit);

                const auto fr = xi;
                const auto to = xj;
                assert(fr < nv&& to < nv);
                addArc(fr, to);

                if (nodesSplit[xj].next.size() + nodesJoin[xj].prev.size() == 1) {
                    q.push_back(xj);
                }
            }
        }
    }

    void ContourTree::output(const std::string& fileName) {
        std::cout << "removing deg-2 nodes and computing segmentation" << std::endl;

        // saving some memory
        nodesJoin.clear();
        nodesJoin.shrink_to_fit();
        nodesSplit.clear();
        nodesSplit.shrink_to_fit();

        std::vector<int64_t> nodeids;
        std::vector<scalar_t> nodefns;
        std::vector<char> nodeTypes;
        std::vector<int64_t> arcs;

        arcMap.resize(nv, -1);

        uint32_t arcNo = 0;
        for (int64_t i = 0; i < nv; i++) {
            // go in sorted order
            int64_t v = tree->sv[i];
            // process only regular vertices
            if (ctNodes[v].prev.size() == 1 && ctNodes[v].next.size() == 1) {
                continue;
            }
            nodeids.push_back(v);
            nodefns.push_back(tree->data->getFunctionValue(v));
            nodeTypes.push_back(tree->criticalPts[v]);

            // create an arc for which this critical point is the source of the arc
            // traverse up for each of its arcs
            int64_t from = v;
            for (size_t j = 0; j < ctNodes[v].next.size(); j++) {
                int64_t vv = ctNodes[v].next[j];
                while (ctNodes[vv].prev.size() == 1 && ctNodes[vv].next.size() == 1) {
                    // regular
                    arcMap[vv] = arcNo;
                    vv = ctNodes[vv].next[0];
                }
                arcMap[v] = arcNo;
                arcMap[vv] = arcNo;
                int64_t to = vv;
                // create arc (from, to)
                arcs.push_back(from);
                arcs.push_back(to);
                arcNo++;
            }
        }

        // write meta data
        std::cout << "Writing meta data" << std::endl;
        {
            std::ofstream pr(fileName + ".rg.dat");
            pr << nodeids.size() << "\n";
            pr << arcNo << "\n";
            pr.close();
        }

        std::cout << "writing tree output" << std::endl;
        std::string rgFile = fileName + ".rg.bin";
        std::ofstream of(rgFile, std::ios::binary);
        of.write(reinterpret_cast<char*>(nodeids.data()), nodeids.size() * sizeof(int64_t));
        of.write(reinterpret_cast<char*>(nodefns.data()), nodeids.size() * sizeof(scalar_t));
        of.write(reinterpret_cast<char*>(nodeTypes.data()), nodeids.size());
        of.write(reinterpret_cast<char*>(arcs.data()), arcs.size() * sizeof(int64_t));
        of.close();

        std::cout << "writing partition" << std::endl;
        std::string rawFile = fileName + ".part.raw";
        of.open(rawFile, std::ios::binary);
        of.write(reinterpret_cast<char*>(arcMap.data()), arcMap.size() * sizeof(uint32_t));
        of.close();
    }

    void ContourTree::remove(int64_t xi, std::vector<ContourTree::Node>& nodeArray) const {
        if (auto [jn_v, jn_next, jn_prev] = nodeArray[xi]; jn_prev.size() == 1 && jn_next.size() == 1) {
            const auto p = jn_prev[0];
            const auto n = jn_next[0];
            auto [pn_v, pn_next, pn_prev] = nodeArray[p];
            auto [nn_v, nn_next, nn_prev] = nodeArray[n];

            removeAndAdd(pn_next, xi, nn_v);
            removeAndAdd(nn_prev, xi, pn_v);
        }
        else if (jn_prev.empty() && jn_next.size() == 1) {
            const auto n = jn_next[0];
            auto [nn_v, nn_next, nn_prev] = nodeArray[n];
            remove(nn_prev, xi);
        }
        else if (jn_prev.size() == 1 && jn_next.empty()) {
            const auto p = jn_prev[0];
            auto [pn_v, pn_next, pn_prev] = nodeArray[p];
            remove(pn_next, xi);
        }
        else {
            assert(false);
        }
    }

    void ContourTree::removeAndAdd(std::vector<int64_t>& arr, int64_t rem, int64_t add) {
        for (auto& val : arr) {
            if (val == rem) {
                val = add;
                return;
            }
        }
        assert(false);
    }

    void ContourTree::remove(std::vector<int64_t>& arr, int64_t xi) {
        for (int64_t i = 0; i < static_cast<int64_t>(arr.size()); i++) {
            if (arr[i] == xi) {
                if (i != static_cast<int64_t>(arr.size()) - 1) {
                    arr[i] = arr[arr.size() - 1];
                }
                arr.pop_back();
                return;
            }
        }
        assert(false);
    }

    void ContourTree::addArc(int64_t from, int64_t to) {
        ctNodes[from].next.push_back(to);
        ctNodes[to].prev.push_back(from);
    }


}
