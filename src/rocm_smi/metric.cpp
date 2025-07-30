#include <internal_energy/rocm_smi/amd_device.hpp>
#include <internal_energy/rocm_smi/metric.hpp>

namespace internal_energy
{
namespace rocm
{
std::unique_ptr<internal_energy::MetricInstance> MetricSource::open() const
{
    return std::make_unique<rocm::MetricInstance>(this);
}

std::vector<rocm::MetricSource> get_all_metrics()
{
    std::vector<MetricSource> sources_;

    for (auto& device : AMDDevice::get_all_devices())
    {
        sources_.emplace_back(rocm::MetricSource(Location::from<AMDDevice>(device)));
    }
    return sources_;
}
} // namespace rocm
} // namespace internal_energy
