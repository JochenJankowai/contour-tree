#pragma once

#include "SimplifyCT.h"
#include <string>
#include <set>
#include <stdio.h>

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
    [[nodiscard]] std::pair<contourtree::SimplifyCT, std::vector<Feature>> getArcFeatures(
        int topk = -1, float th = 0) const;
    [[nodiscard]] std::pair<contourtree::SimplifyCT, std::vector<Feature>>
    getPartitionedExtremaFeatures(int topk = -1, float th = 0) const;

    /**
     * Performs a persistence simplification and extracts the arcs incident to the n lowest
     * minima/highest maxima.
     */
    template <contourtree::TreeType T, typename ValueType>
    [[nodiscard]] std::pair<contourtree::SimplifyCT, std::vector<Feature>>
    getNExtremalArcFeatures(int n, float thresold,
                            const std::vector<ValueType>& functionValues) const;

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

template <contourtree::TreeType T, typename ValueType>
std::pair<contourtree::SimplifyCT, std::vector<Feature>>
TopologicalFeatures::getNExtremalArcFeatures(int n, float thresold,
                                             const std::vector<ValueType>& functionValues) const {
    auto [simplifyCT, features] = getArcFeatures(-1, thresold);

    /**
     * For split trees we look at feature.from, for join trees we look at feature.to
     */

    if constexpr (T == contourtree::TreeType::SplitTree) {
        std::sort(std::begin(features), std::end(features),
                  [&functionValues](const auto& feature1, const auto& feature2) {
                      return functionValues[feature1.from] < functionValues[feature2.from];
                  });
    }
    if constexpr (T == contourtree::TreeType::JoinTree) {
        std::sort(std::begin(features), std::end(features),
                  [&functionValues](const auto& feature1, const auto& feature2) {
                      return functionValues[feature1.to] > functionValues[feature2.to];
                  });
    }

    if (features.size() > n) {
        features.resize(n);
    }

    return {simplifyCT, features};
}

}  // namespace contourtree
