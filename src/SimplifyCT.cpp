#include "SimplifyCT.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <execution>
#include <numeric>

namespace contourtree {

bool BranchCompare::operator()(uint32_t v1, uint32_t v2) { return sim->compare(v1, v2); }

SimplifyCT::SimplifyCT() : fn_(std::make_shared<std::vector<float>>()), simFn(nullptr) {
    queue =
        std::priority_queue<uint32_t, std::vector<uint32_t>, BranchCompare>(BranchCompare(this));
    order_.clear();
}

SimplifyCT::SimplifyCT(std::shared_ptr<const SimplifyCT> simplifyCt)
    : data(simplifyCt->data)
    , branches(simplifyCt->branches)
    , nodes(simplifyCt->nodes)
    , fn_(simplifyCt->fn_)
    , fnv(simplifyCt->fnv)
    , invalid(simplifyCt->invalid)
    , removed(simplifyCt->removed)
    , inq(simplifyCt->inq)
    , simFn(simplifyCt->simFn)
    , queue(simplifyCt->queue)
    , order_(simplifyCt->order_)
    , weights_(simplifyCt->weights_)
    , vArray(simplifyCt->vArray) {}

void SimplifyCT::setInput(std::shared_ptr<const ContourTreeData> data) { this->data = data; }

void SimplifyCT::addToQueue(uint32_t ano) {
    if (isCandidate(branches[ano])) {
        queue.push(ano);
        inq[ano] = true;
    }
}

bool SimplifyCT::isCandidate(const Branch& br) {
    uint32_t from = br.from;
    uint32_t to = br.to;
    if (nodes[from].prev.empty()) {
        // minimum
        if (nodes[to].prev.size() > 1) {
            return true;
        }
            return false;
    }
    if (nodes[to].next.empty()) {
        // maximum
        if (nodes[from].next.size() > 1) {
            return true;
        }
        return false;
    }
    return false;
}

void SimplifyCT::init() {
    branches.resize(data->noArcs);
    nodes.resize(data->noNodes);
    for (uint32_t i = 0; i < branches.size(); i++) {
        branches[i].from = data->arcs[i].from;
        branches[i].to = data->arcs[i].to;
        branches[i].parent = -1;
        branches[i].arcs_.push_back(i);

        nodes[branches[i].from].next.push_back(i);
        nodes[branches[i].to].prev.push_back(i);
    }

    fn_->resize(branches.size());
    removed.resize(branches.size(), false);
    invalid.resize(branches.size(), false);
    inq.resize(branches.size(), false);

    vArray.resize(nodes.size());
}

void SimplifyCT::setSimplificationFunction(std::shared_ptr<SimFunction> f) {
    simFn = f;
    if (simFn->simType_ == SimFunction::SimType::Persistence) {
        fn_ = std::dynamic_pointer_cast<Persistence>(f)->fn_;
    }
}

void SimplifyCT::initSimplification(std::shared_ptr<SimFunction> f) {
    branches.resize(data->noArcs);
    nodes.resize(data->noNodes);
    for (uint32_t i = 0; i < branches.size(); i++) {
        branches[i].from = data->arcs[i].from;
        branches[i].to = data->arcs[i].to;
        branches[i].parent = -1;
        branches[i].arcs_.push_back(i);

        nodes[branches[i].from].next.push_back(i);
        nodes[branches[i].to].prev.push_back(i);
    }

    fn_->resize(branches.size());
    removed.resize(branches.size(), false);
    invalid.resize(branches.size(), false);
    inq.resize(branches.size(), false);

    vArray.resize(nodes.size());

    simFn = f;
    if (f) {
        simFn->init(fn_, branches);

        for (uint32_t i = 0; i < branches.size(); i++) {
            addToQueue(i);
        }
    }
}

bool SimplifyCT::compare(uint32_t b1, uint32_t b2) const {
    // If I want smallest weight on top, I need to return true if b1 > b2 (sort in descending
    // order)
    if (fn_->at(b1) > fn_->at(b2)) {
        return true;
    }
    if (fn_->at(b1) < fn_->at(b2)) {
        return false;
    }
    float p1 = data->fnVals->at(branches[b1].to) - data->fnVals->at(branches[b1].from);
    float p2 = data->fnVals->at(branches[b2].to) - data->fnVals->at(branches[b2].from);
    if (p1 > p2) {
        return true;
    }
    if (p1 < p2) {
        return false;
    }
    int diff1 = branches[b1].to - branches[b1].from;
    int diff2 = branches[b2].to - branches[b2].from;
    if (diff1 > diff2) {
        return true;
    }
    if (diff1 < diff2) {
        return false;
    }
    return (branches[b2].from > branches[b1].from);
}

void SimplifyCT::removeArc(uint32_t ano) {
    Branch br = branches[ano];
    uint32_t from = br.from;
    uint32_t to = br.to;
    uint32_t mergedVertex = -1;
    if (nodes[from].prev.empty()) {
        // minimum
        mergedVertex = to;
    }
    if (nodes[to].next.empty()) {
        // maximum
        mergedVertex = from;
    }
    nodes[from].next.erase(std::remove(nodes[from].next.begin(), nodes[from].next.end(), ano),
                           nodes[from].next.end());
    nodes[to].prev.erase(std::remove(nodes[to].prev.begin(), nodes[to].prev.end(), ano),
                         nodes[to].prev.end());
    removed[ano] = true;

    vArray[mergedVertex].push_back(ano);
    if (nodes[mergedVertex].prev.size() == 1 && nodes[mergedVertex].next.size() == 1) {
        mergeVertex(mergedVertex);
    }
    if (simFn) simFn->branchRemoved(branches, ano, invalid);
}

void SimplifyCT::mergeVertex(uint32_t v) {
    uint32_t prev = nodes[v].prev.at(0);
    uint32_t next = nodes[v].next.at(0);
    int a = -1;
    int rem = -1;
    if (inq[prev]) {
        invalid[prev] = true;
        removed[next] = true;
        branches[prev].to =
            branches[next].to; /**< This connects the first child node to the parent node. */
        a = prev;
        rem = next;

        /**
         * Loop over all child arcs and check whether one of them was the incident arc? Doesn't make
         * sense to me...
         */
        for (int i = 0; i < nodes[branches[prev].to].prev.size(); i++) {
            if (nodes[branches[prev].to].prev[i] == next) { /**< When would that ever happen? */
                nodes[branches[prev].to].prev[i] = prev;
            }
        }
    } else {
        invalid[next] = true;
        removed[prev] = true;
        branches[next].from = branches[prev].from;
        a = next;
        rem = prev;

        for (int i = 0; i < nodes[branches[next].from].next.size(); i++) {
            if (nodes[branches[next].from].next[i] == prev) {
                nodes[branches[next].from].next[i] = next;
            }
        }
        if (simFn && !inq[next]) {
            addToQueue(next);
        }
    }
    for (const auto ch : branches[rem].children) {
        // for (int i = 0; i < branches[rem].children.size(); i++) {
        // int ch = branches[rem].children.at(i);
        branches[a].children.push_back(ch);
        assert(branches[ch].parent == rem);
        branches[ch].parent = a;
    }
    //    branches[a].arcs_ << branches[rem].arcs_;
    branches[a].arcs_.insert(branches[a].arcs_.end(), branches[rem].arcs_.begin(),
                            branches[rem].arcs_.end());
    for (int i = 0; i < vArray[v].size(); i++) {
        uint32_t aa = vArray[v].at(i);
        branches[a].children.push_back(aa);
        branches[aa].parent = a;
    }
    branches[rem].parent = -2;
}

void SimplifyCT::simplify(std::shared_ptr<SimFunction> simFn) {
    std::cout << "init" << std::endl;
    initSimplification(simFn);

    std::cout << "going over priority queue" << std::endl;
    while (queue.size() > 0) {
        uint32_t ano = queue.top();
        queue.pop();
        inq[ano] = false;
        if (!removed[ano]) {
            if (invalid[ano]) {
                simFn->update(branches, ano);
                invalid[ano] = false;
                addToQueue(ano);
            } else {
                if (isCandidate(branches[ano])) {
                    removeArc(ano);
                    order_.push_back(ano);
                }
            }
        }
    }
    std::cout << "pass over removed" << std::endl;
    int root = 0;
    for (int i = 0; i < removed.size(); i++) {
        if (!removed[i]) {
            //            assert(root == 0);
            order_.push_back(i);
            root++;
        }
    }

    std::cout << "Simplification done." << std::endl;
}

void SimplifyCT::simplify(const std::vector<uint32_t>& order, int topk, float threshold,
                          const std::vector<float>& weights) {
    std::cout << "init" << std::endl;
    initSimplification(NULL);

    std::cout << "going over order queue" << std::endl;
    for (int i = 0; i < order.size(); i++) {
        inq[order.at(i)] = true;
    }
    
#define NEWCODE 0

#if( NEWCODE != 1)
    if (topk > 0) {
        const auto ct = order.size() - topk;
        for (int i = 0; i < ct; i++) {
            uint32_t ano = order.at(i);
            if (!isCandidate(branches[ano])) {
                std::cout << "failing candidate test" << std::endl;
                assert(false);
            }
            inq[ano] = false;
            removeArc(ano);
        }
    } else if (threshold != 0) {
        for (int i = 0; i < order.size() - 1; i++) {
            uint32_t ano = order.at(i);
            if (!isCandidate(branches[ano])) {
                std::cout << "failing candidate test" << std::endl;
                assert(false);
            }
            float fn = weights.at(i);
            if (fn > threshold) {
                break;
            }
            inq[ano] = false;
            removeArc(ano);
        }
    }

#else
    if (topk > 0) {
        const auto ct = order.size() - topk;
        std::vector indices(ct, 0);
        std::iota(std::begin(indices), std::end(indices), 0);
        std::for_each(std::execution::par_unseq, std::begin(indices), std::end(indices),
                      [&](const auto i) {
                          const auto ano = order.at(i);
                          if (!isCandidate(branches[ano])) {
                              std::cout << "failing candidate test" << std::endl;
                              assert(false);
                          }
                          inq[ano] = false;
                          removeArc(ano);
                      });
    } else if (threshold > 0.0f) {
        std::vector indices(order.size()-1, 0);
        std::iota(std::begin(indices), std::end(indices), 0);
        std::for_each(std::execution::par_unseq, std::begin(indices), std::end(indices),
                      [&](const auto i) {
                          const auto ano = order.at(i);
                          if (!isCandidate(branches[ano])) {
                              std::cout << "failing candidate test" << std::endl;
                              assert(false);
                          }
                          float fn = weights.at(i);
                          if (fn > threshold) {
                              return;
                          }
                          inq[ano] = false;
                          removeArc(ano);
                      });
    }
        
#endif
    std::cout << "Simplification done." << std::endl;
}

void SimplifyCT::simplify(const std::vector<uint32_t>& order, const float threshold,
                          const std::vector<float>& weights) {
    std::cout << "init" << std::endl;
    initSimplification(nullptr);

    std::cout << "going over order_ queue" << std::endl;
    for (const auto index : order) {
        inq[index] = true;
    }
    for (int i = 0; i < order.size() - 1; i++) {
        const uint32_t ano = order.at(i);
        if (!isCandidate(branches[ano])) {
            std::cout << "failing candidate test" << std::endl;
            assert(false);
        }

        if (const auto fn = weights.at(i); fn > threshold) {
            break;
        }

        inq[ano] = false;
        removeArc(ano);
    }

    std::cout << "Simplification done." << std::endl;
}

void SimplifyCT::computeWeights() {
    weights_.clear();
    for (size_t i = 0; i < order_.size(); i++) {
        uint32_t ano = order_.at(i);
        float val = this->simFn->getBranchWeight(ano);
        weights_.push_back(val);
    }

    // normalize weights_
    float maxWt = weights_.at(weights_.size() - 1);
    if (maxWt == 0) maxWt = 1;
    for (int i = 0; i < weights_.size(); i++) {
        weights_[i] /= maxWt;
    }
}

void SimplifyCT::writeToFile(const std::string fileName) {
    std::cout << "Writing meta data" << std::endl;
    {
        std::ofstream pr(fileName + ".order_.dat");
        pr << order_.size() << "\n";
        pr.close();
    }
    std::cout << "writing tree output" << std::endl;
    std::string binFile = fileName + ".order_.bin";
    std::ofstream of(binFile, std::ios::binary);
    of.write((char*)order_.data(), order_.size() * sizeof(uint32_t));
    of.write((char*)weights_.data(), weights_.size() * sizeof(float));
    of.close();
}

}  // namespace contourtree
