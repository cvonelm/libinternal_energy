#include "internal_energy/nvml/nvidia_device.hpp"
#include <internal_energy/nvml/metric.hpp>

namespace internal_energy
{
namespace cuda
{
std::unique_ptr<internal_energy::MetricInstance> MetricSource::open() const
{
    return std::make_unique<cuda::MetricInstance>(this);
}

std::set<cuda::MetricSource> get_all_metrics()
{
    std::set<MetricSource> sources_;

    for (auto& device : NVidiaDevice::get_cuda_devices())
    {
        sources_.emplace(cuda::MetricSource(Location::from<NVidiaDevice>(device)));
    }
    return sources_;
}
} // namespace cuda
} // namespace internal_energy
