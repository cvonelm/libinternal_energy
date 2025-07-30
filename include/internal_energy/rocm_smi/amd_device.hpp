#pragma once

#include <string>
#include <vector>

#include <fmt/core.h>

extern "C"
{
#include <rocm_smi/rocm_smi.h>
}
namespace internal_energy
{
namespace rocm
{
class AMDDevice
{
public:
    AMDDevice(uint32_t idx) : device_index_(idx)
    {
    }
    uint32_t as_int() const
    {
        return device_index_;
    }

    std::string str() const
    {
        return fmt::format("AMDGPU{}", device_index_);
    }
    static std::vector<AMDDevice> get_all_devices()
    {
        std::vector<AMDDevice> res;
        rsmi_init(0);
        uint32_t num_devices;

        rsmi_num_monitor_devices(&num_devices);
        for (int i = 0; i < num_devices; i++)
        {
            res.emplace_back(AMDDevice(i));
        }
        return res;
    }

    friend bool operator==(const AMDDevice& lhs, const AMDDevice& rhs)
    {
        return lhs.device_index_ == rhs.device_index_;
    }

    friend bool operator<(const AMDDevice& lhs, const AMDDevice& rhs)
    {
        return lhs.device_index_ < rhs.device_index_;
    }

private:
    uint32_t device_index_;
};
} // namespace rocm
} // namespace internal_energy
