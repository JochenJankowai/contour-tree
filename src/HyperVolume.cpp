#include "HyperVolume.h"

#include <fstream>
#include <cassert>
#include <iostream>

namespace contourtree {

HyperVolume::HyperVolume(std::shared_ptr<const ContourTreeData> ctData, const std::string partFile)
    : SimFunction(SimType::HyperVolume) {
    fnVals_ = ctData->fnVals;

    std::ifstream bin(partFile, std::ios::binary| std::ios::ate);
    uint32_t size = bin.tellg();
    std::cout << "part size: " << size << std::endl;
    bin.close();

    std::vector<uint32_t> cols(size / sizeof(uint32_t));
    bin.open(partFile, std::ios::binary);
    bin.read((char *)cols.data(),size);
    bin.close();

    vol.resize(ctData->noArcs,0);
    brVol.resize(ctData->noArcs,0);
    initVolumes(cols);
}

HyperVolume::HyperVolume(std::shared_ptr<const ContourTreeData> ctData,
                         const std::vector<uint32_t>& cols)
    : SimFunction(SimType::HyperVolume) {
    fnVals_ = ctData->fnVals;
    vol.resize(ctData->noArcs,0);
    brVol.resize(ctData->noArcs,0);
    initVolumes(cols);
}

void HyperVolume::initVolumes(const std::vector<uint32_t> &cols) {
    for(size_t i = 0;i < cols.size();i ++) {
        if(cols[i] == -1) {
            continue;
        }
        vol[cols[i]] ++;
    }
}

void HyperVolume::init(std::shared_ptr<std::vector<float>>fn, std::vector<Branch> &br) {
    this->fn_ = fn;
    for(int i = 0;i < fn->size();i ++) {
        this->update(br,i);
    }
}

void HyperVolume::update(const std::vector<Branch> &br, uint32_t brNo) {
    brVol[brNo] = 0;
    for(int i = 0;i < br[brNo].arcs.size();i ++) {
        brVol[brNo] += vol[br[brNo].arcs.at(i)];
    }
    for(int i = 0;i < br[brNo].children.size();i ++) {
        int child = br[brNo].children.at(i);
        brVol[brNo] += volume(br,child);
    }
    float fnDiff = fnVals_->at(br[brNo].to) - fnVals_->at(br[brNo].from);
    fn_->at(brNo) = fnDiff * brVol[brNo];
}

float HyperVolume::volume(const std::vector<Branch> &br, int brNo) {
    float val = 0;
    for(int i = 0;i < br[brNo].arcs.size();i ++) {
        val += vol[br[brNo].arcs.at(i)];
    }
    for(int i = 0;i < br[brNo].children.size();i ++) {
        int child = br[brNo].children.at(i);
        val = val + volume(br,child);
    }
    return val;
}

void HyperVolume::branchRemoved(std::vector<Branch>&, uint32_t, std::vector<bool>&) {

}

float HyperVolume::getBranchWeight(uint32_t brNo) {
    return fn_->at(brNo);
}

}
