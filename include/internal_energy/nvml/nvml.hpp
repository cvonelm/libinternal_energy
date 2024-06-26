#pragma once

extern "C"
{
#include <nvml.h>
}

namespace internal_energy
{
namespace nvml
{
class NvmlEnergy
{
public:
    NvmlEnergy(nvmlDevice_t device) : device_(device)
    {
    }

    template <>
    double read<double>() override
    {
        uint64_t energy;
        nvmlDeviceGetTotalEnergyConsumption(device_, &energy);
        return energy;
    }

    static std::vector<NvmlEnergy> get_devices()
    {
        std::vector<NVmlEnergy> res;
        nvmlDevice_t handle;
        for (int i = 0; nvmlDeviceGetHandleByIndex_v2(i, &handle); i++)
        {
            res.emplace_back(handle);
        }
        return res;
    }

private:
    nvmlDevice_t device_;
}
} // namespace nvml
} // namespace internal_energy
