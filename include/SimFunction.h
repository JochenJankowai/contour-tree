#ifndef SIMFUNCTION_HPP
#define SIMFUNCTION_HPP

#include <vector>
#include <stdint.h>
#include <memory>

namespace contourtree {

struct Branch {
    uint32_t from; /**< Node id */
    uint32_t to;   /**< Node id */
    std::vector<uint32_t> arcs;

    uint32_t parent;                 //<  Parent branch
    std::vector<uint32_t> children;  // Child branches
};

class SimFunction {
public:
    enum class SimType { Persistence, HyperVolume };

    SimFunction() = delete;
    SimFunction(SimType simType) : simType_(simType) {}

    virtual void init(std::shared_ptr<std::vector<float>> fn, std::vector<Branch>& br) = 0;
    virtual void update(const std::vector<Branch>& br, uint32_t brNo) = 0;
    virtual void branchRemoved(std::vector<Branch>& br, uint32_t brNo,
                               std::vector<bool>& invalid) = 0;
    virtual float getBranchWeight(uint32_t brNo) = 0;

    virtual float getMinValue() const = 0;
    virtual float getMaxValue() const = 0;

    SimType simType_;
    std::shared_ptr<const std::vector<float>> fnVals_; /**< Function values of input data.*/
};

}  // namespace contourtree

#endif  // SIMFUNCTION_HPP
