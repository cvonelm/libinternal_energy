#pragma once

#include <internal_energy/metric.hpp>

#include <cstdint>
#include <memory>
#include <rocm_smi/rocm_smi.h>
#include <string>
#include <vector>

#include <fmt/core.h>

namespace internal_energy
{
namespace rocm
{
class MetricInstance : public internal_energy::MetricInstance
{
public:
    MetricInstance(const MetricSource* source) : internal_energy::MetricInstance(source)
    {
    }

    double read() override
    {
        uint64_t val;
        float counter_resolution;
        uint64_t timestamp;
        auto ret = rsmi_dev_energy_count_get(0, &val, &counter_resolution, &timestamp);
        return val * counter_resolution;
    }
};

class MetricSource : public internal_energy::MetricSource
{
public:
    MetricSource(Location l) : internal_energy::MetricSource(l)
    {
        rsmi_init(0);
    }

    std::unique_ptr<internal_energy::MetricInstance> open() const override;

    Subsystem get_subsystem() const override
    {
        return Subsystem::ROCM;
    }

    std::string name() const override
    {
        return "energy";
    }

    std::string str() const override
    {
        return fmt::format("ROCM::{}::{}", location_.str(), name());
    }

    Unit unit() const override
    {
        return Unit::ENERGY;
    }
};

std::set<rocm::MetricSource> get_all_metrics();
} // namespace rocm
} // namespace internal_energy
