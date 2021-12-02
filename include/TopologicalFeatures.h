#pragma once

#include "SimplifyCT.h"
#include <string>
#include <set>

namespace contourtree {

struct Feature {
    std::vector<uint32_t> arcs;
    uint32_t from, to;
};

class TopologicalFeatures {
public:
    TopologicalFeatures();

    void loadDataFromArrays(std::shared_ptr<const ContourTreeData> input_ctdata,
                            const std::vector<uint32_t>& input_order,
                            const std::vector<float>& input_weights, bool partition = false);
    std::vector<Feature> getArcFeatures(int topk = -1, float th = 0) const;
    std::vector<Feature> getPartitionedExtremaFeatures(int topk = -1, float th = 0) const;

public:
    std::shared_ptr<const ContourTreeData> ctdata;
    std::vector<uint32_t> order;
    std::vector<float> weights;

    // when completely partitioning branch decomposition
    std::vector<std::vector<uint32_t>> featureArcs;
    std::shared_ptr<SimplifyCT> sim;

    bool isPartitioned;

private:
    void addFeature(const SimplifyCT& sim, uint32_t bno, std::vector<Feature>& features,
                    std::set<size_t>& featureSet);
};

}  // namespace contourtree
