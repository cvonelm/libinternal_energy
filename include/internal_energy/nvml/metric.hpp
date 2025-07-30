#pragma once

#include "internal_energy/nvml/nvidia_device.hpp"

extern "C"
{
#include <nvml.h>
}
#include <internal_energy/metric.hpp>

namespace internal_energy
{
namespace cuda
{

class MetricInstance : public internal_energy::MetricInstance
{
public:
    MetricInstance(const MetricSource* source) : internal_energy::MetricInstance(source)
    {
        nvmlDeviceGetHandleByIndex_v2(source_->get_location().as<NVidiaDevice>().as_int(), &dev_);
    }

    double read() override
    {
        long long unsigned int energy;
        nvmlDeviceGetTotalEnergyConsumption(dev_, &energy);
        return energy / (1000.0);
    }
    nvmlDevice_t dev_;
};

class MetricSource : public internal_energy::MetricSource
{
public:
    MetricSource(Location l) : internal_energy::MetricSource(l)
    {
    }

    std::unique_ptr<internal_energy::MetricInstance> open() const override;

    Subsystem get_subsystem() const override
    {
        return Subsystem::CUDA;
    }

    std::string name() const override
    {
        return "energy";
    }

    std::string str() const override
    {
        return fmt::format("CUDA::{}::{}", location_.str(), name());
    }

    Unit unit() const override
    {
        return Unit::ENERGY;
    }
};

std::vector<cuda::MetricSource> get_all_metrics();

} // namespace cuda
} // namespace internal_energy
