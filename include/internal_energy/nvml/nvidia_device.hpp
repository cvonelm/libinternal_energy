#pragma once

#include <fmt/format.h>
#include <nvml.h>
#include <vector>
namespace internal_energy
{
namespace cuda
{

class NVidiaDevice
{
public:
    NVidiaDevice(int dev) : dev_(dev)
    {
    }

    int as_int() const
    {
        return dev_;
    }

    std::string str() const
    {
        return fmt::format("NVIDIAGPU{}", dev_);
    }
    static std::vector<NVidiaDevice> get_cuda_devices()
    {
        nvmlInit();
        std::vector<NVidiaDevice> res;
        nvmlDevice_t handle;
        unsigned int count;
        if (nvmlDeviceGetCount_v2(&count) != NVML_SUCCESS)
        {
            return res;
        }
        for (int i = 0; i < count; i++)
        {
            res.emplace_back(i);
        }
        return res;
    }

    friend bool operator==(const NVidiaDevice& lhs, const NVidiaDevice& rhs)
    {
        return lhs.dev_ == rhs.dev_;
    }
    friend bool operator<(const NVidiaDevice& lhs, const NVidiaDevice& rhs)
    {
        return lhs.dev_ < rhs.dev_;
    }

private:
    int dev_;
};
} // namespace cuda
} // namespace internal_energy
