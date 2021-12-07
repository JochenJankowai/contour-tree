#ifndef CONTOURTREE_HPP
#define CONTOURTREE_HPP

#include <string>
#include <vector>
#include "constants.h"
#include <memory>

namespace contourtree {

class MergeTree;

class ContourTree
{
public:
    struct Node {
        int64_t v;
        std::vector<int64_t> next; //< Vertex id
        std::vector<int64_t> prev;//< Vertex id
    };

public:
    ContourTree();

    void setup(std::shared_ptr<const MergeTree> tree);
    void computeCT();
    void generateArrays();
    void writeToFile(const std::string fileName);

private:
    void remove(int64_t xi, std::vector<Node>& nodeArray);
    void removeAndAdd(std::vector<int64_t> &arr, int64_t rem, int64_t add);
    void remove(std::vector<int64_t> &arr, int64_t xi);
    void addArc(int64_t from, int64_t to);

public:
    std::shared_ptr<const MergeTree> tree;
    std::vector<Node> nodesJoin;
    std::vector<Node> nodesSplit;
    std::vector<Node> ctNodes;
    std::vector<uint32_t> arcMap;
    
    std::vector<int64_t> nodeids;
    std::vector<scalar_t> nodefns;
    std::vector<char> nodeTypes;
    std::vector<int64_t> arcs;

    uint32_t arcNo;
    int64_t nv;
};

}

#endif // CONTOURTREE_HPP
