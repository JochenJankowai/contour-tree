#pragma once

#include <string>
#include <vector>

namespace contourtree {

    class MergeTree;

    class ContourTree
    {
    public:
        struct Node {
            int64_t v;
            std::vector<int64_t> next;
            std::vector<int64_t> prev;
        };

        ContourTree() = default;

        void setup(const MergeTree* tree);
        void computeCT();
        void output(const std::string& fileName);

    private:
        void remove(int64_t xi, std::vector<Node>& nodeArray)const;
        static void removeAndAdd(std::vector<int64_t>& arr, int64_t rem, int64_t add);
        static void remove(std::vector<int64_t>& arr, int64_t xi);
        void addArc(int64_t from, int64_t to);

    public:
        const MergeTree* tree;
        std::vector<Node> nodesJoin;
        std::vector<Node> nodesSplit;
        std::vector<Node> ctNodes;
        std::vector<uint32_t> arcMap;

        int64_t nv;
    };

}
