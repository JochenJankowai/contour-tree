#ifndef CONTOURTREEDATA_HPP
#define CONTOURTREEDATA_HPP

#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <constants.h>
#include "MergeTree.h"

namespace contourtree {

/**
 * Stores indices to next and prev branches. Indices' target is SimplifyCT::branches
 */
struct Node {
    std::vector<uint32_t>
        next; /**< Arcs with the other end node having function value higher than the node. */
    std::vector<uint32_t>
        prev; /**< Arcs with the other end node having function value lower  than the node. */
};

struct Arc {
    uint32_t from; /**< Index of node with lower  value*/
    uint32_t to;   /**< Index of node with higher value*/
    uint32_t id;
};

class ContourTreeData {
public:
    ContourTreeData();
    ContourTreeData(std::shared_ptr<const ContourTree> CT);
    ContourTreeData(std::shared_ptr<const MergeTree> CT);

    void loadBinFile(std::string fileName);
    void loadTxtFile(std::string fileName);

protected:
    void loadData(const std::vector<int64_t>& nodeids, const std::vector<scalar_t>& nodefns,
                  const std::vector<char>& nodeTypes, const std::vector<int64_t>& iarcs);

public:
    uint32_t noNodes;
    uint32_t noArcs;

    std::vector<Node> nodes; /**< List of nodes sorted by function value. */

    std::vector<Arc> arcs; /**< List of arcs. */

    std::shared_ptr<std::vector<float>> fnVals; /**< Sorted function values. */

    std::vector<char> type; /**< Node types, i.e., minimum, maximum, saddle. */

    std::vector<int64_t>
        nodeVerts; /**< nodeVerts[i] = data point corresponding to the i^th node. */

    std::unordered_map<int64_t, uint32_t> nodeMap; /**< Maps the vertex ids (points in the input
                                                      data) to the node ids of the contour tree. */
};

}  // namespace contourtree

#endif  // CONTOURTREEDATA_HPP
