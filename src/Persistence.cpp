#include "Persistence.h"

namespace contourtree {

Persistence::Persistence(std::shared_ptr<const ContourTreeData> ctData)
    : SimFunction(SimType::Persistence) {
    fnVals_ = ctData->fnVals;
}

void Persistence::init(std::shared_ptr<std::vector<float>> fn, std::vector<Branch>& br) {
    /*I don't know what fn is supposed to be but I know that at first, fn and br have the same size. It could be persistence?*/
    fn_ = fn;
    for (size_t i = 0; i < fn->size(); i++) {
        this->fn_->at(i) = fnVals_->at(br[i].to) - fnVals_->at(br[i].from);
    }
}

void Persistence::init(std::vector<Branch>& br) {
    /*I don't know what fn is supposed to be but I know that at first, fn and br have the same size.
     * It could be persistence?*/
    fn_ = std::make_shared<std::vector<float>>();
    fn_->resize(br.size());
    for (size_t i = 0; i < br.size(); i++) {
        this->fn_->at(i) = fnVals_->at(br[i].to) - fnVals_->at(br[i].from);
    }
}

void Persistence::update(const std::vector<Branch>& br, uint32_t brNo) {
    fn_->at(brNo) = fnVals_->at(br[brNo].to) - fnVals_->at(br[brNo].from);
}

void Persistence::branchRemoved(std::vector<Branch>&, uint32_t, std::vector<bool>&) {
    // not required for persistence
}

float Persistence::getBranchWeight(uint32_t brNo) { return fn_->at(brNo); }

}  // namespace contourtree
