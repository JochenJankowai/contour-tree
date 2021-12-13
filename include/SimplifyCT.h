#ifndef SIMPLIFYCT_HPP
#define SIMPLIFYCT_HPP

#include "ContourTreeData.h"
#include "Persistence.h"
#include <queue>
#include <vector>
#include <memory>

#if defined(WIN32)
#include <functional>
#endif

namespace contourtree {

class SimplifyCT;

struct BranchCompare {
    BranchCompare() {}
    BranchCompare(const SimplifyCT* simct) : sim(simct) {}
    bool operator()(uint32_t v1, uint32_t v2);

    const SimplifyCT* sim;
};

class SimplifyCT {
public:
    SimplifyCT();
    SimplifyCT(std::shared_ptr<const SimplifyCT> simplifyCt);

    void setInput(std::shared_ptr<const ContourTreeData> data);
    void simplify(std::shared_ptr<SimFunction> simFn);
    void simplify(const std::vector<uint32_t>& order, int topk = -1, float threshold = 0,
                  const std::vector<float>& weights = std::vector<float>());
    void simplify(const std::vector<uint32_t>& order, float threshold,
                  const std::vector<float>& weights);

    void computeWeights();
    void writeToFile(const std::string fileName);

    void init();
    void setSimplificationFunction(std::shared_ptr<SimFunction> f);
    void initSimplification(std::shared_ptr<SimFunction> f);

protected:
    
    void addToQueue(uint32_t ano);
    bool isCandidate(const Branch& br);
    void removeArc(uint32_t ano);
    void mergeVertex(uint32_t v);

public:
    bool compare(uint32_t b1, uint32_t b2) const;

public:
    std::shared_ptr<const ContourTreeData> data;
    std::vector<Branch> branches;
    std::vector<Node> nodes;

    std::shared_ptr<std::vector<float>> fn_;
    std::vector<float> fnv;
    std::vector<bool> invalid;
    std::vector<bool> removed;
    std::vector<bool> inq;
    std::shared_ptr<SimFunction> simFn;

    std::priority_queue<uint32_t, std::vector<uint32_t>, BranchCompare> queue;
    std::vector<uint32_t> order_;
    std::vector<float> weights_;
    std::vector<std::vector<uint32_t>> vArray;
};

}  // namespace contourtree

#endif  // SIMPLIFYCT_HPP
