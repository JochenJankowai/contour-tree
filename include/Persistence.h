#ifndef PERSISTENCE_HPP
#define PERSISTENCE_HPP

#include "SimFunction.h"
#include "ContourTreeData.h"
#include <algorithm>

namespace contourtree {

class Persistence final : public SimFunction {
public:
    Persistence(std::shared_ptr<const ContourTreeData> ctData);
    virtual ~Persistence() = default;

    virtual void init(std::shared_ptr<std::vector<float>> fn, std::vector<Branch>& br)override;
    virtual void update(const std::vector<Branch>& br, uint32_t brNo) override;
    virtual void branchRemoved(std::vector<Branch>& br, uint32_t brNo,
                               std::vector<bool>& invalid) override;
    /**
     * @param brNo Branch index.
     * @return Persistence value for branch.
     */
    virtual float getBranchWeight(uint32_t brNo) final;

    float getMinPersistence() const { return *std::min_element(std::begin(*fn_), std::end(*fn_)); }
    float getMaxPersistence() const { return *std::max_element(std::begin(*fn_), std::end(*fn_)); }

public:
    std::shared_ptr<const std::vector<float>> fnVals_; /**< Function values if input data.*/

    std::shared_ptr<std::vector<float>> fn_; /**< Persistence values per arc.*/
};

}  // namespace contourtree

#endif  // PERSISTENCE_HPP
