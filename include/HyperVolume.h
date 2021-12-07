#ifndef HYPERVOLUME_HPP
#define HYPERVOLUME_HPP

#include "SimFunction.h"
#include "ContourTreeData.h"

namespace contourtree {

class HyperVolume : public SimFunction
{
public:
    HyperVolume(std::shared_ptr<const ContourTreeData> ctData, const std::string partFile);
    HyperVolume(std::shared_ptr < const ContourTreeData> ctData, const std::vector<uint32_t>& cols);

    virtual void init(std::shared_ptr<std::vector<float>> fn, std::vector<Branch>& br) final;
    virtual void update(const std::vector<Branch>& br, uint32_t brNo) final;
    virtual void branchRemoved(std::vector<Branch>& br, uint32_t brNo,
                               std::vector<bool>& invalid) final;
    virtual float getBranchWeight(uint32_t brNo) final;

private:
    float volume(const std::vector<Branch> &br, int brNo);
    void initVolumes(const std::vector<uint32_t> &cols);

public:
    std::shared_ptr<const std::vector<float>>fnVals_;
    std::shared_ptr<std::vector<float>> fn_;
    std::vector<uint32_t> vol;
    std::vector<uint32_t> brVol;
};

}

#endif // HYPERVOLUME_HPP
