#pragma once

#include <internal_energy/msr/metric.hpp>

namespace internal_energy
{
namespace msr
{
namespace amd
{
class AMDPkgMetricSource : public msr::MetricSource
{
public:
    AMDPkgMetricSource(Location l) : msr::MetricSource(l)
    {
    }
    std::unique_ptr<internal_energy::MetricInstance> open() const override;

    std::string name() const override
    {
        return "AMDPkgEnergy";
    }

    std::string str() const override
    {
        return fmt::format("MSR::{}::{}", location_.str(), name());
    }

    Unit unit() const override
    {
        return Unit::ENERGY;
    }
};

class AMDCoreMetricSource : public msr::MetricSource
{
public:
    AMDCoreMetricSource(Location l) : msr::MetricSource(l)
    {
    }
    std::unique_ptr<internal_energy::MetricInstance> open() const override;

    std::string name() const override
    {
        return "AMDCoreEnergy";
    }

    std::string str() const override
    {
        return fmt::format("MSR::{}::{}", location_.str(), name());
    }

    Unit unit() const override
    {
        return Unit::ENERGY;
    }
};
class AMDPkgMetricInstance : public msr::MetricInstance
{
public:
    AMDPkgMetricInstance(const AMDPkgMetricSource* src) : msr::MetricInstance(src)
    {
        // PPR for AMD Family 17h Model 31h B0 page 170;
        uint64_t unit_raw = rdmsr(0xC0010299, 12, 8);
        unit = 1 / pow(2, unit_raw);
    }

    double read() override
    {
        // PPR for AMD Family 17h Model 31h B0 page 171;
        uint64_t energy = rdmsr(0xC001029B, 31, 0, true);
        return energy * unit;
    }

private:
    double unit;
    uint64_t last_reading_ = 0;
};
class AMDCoreMetricInstance : public msr::MetricInstance
{
public:
    AMDCoreMetricInstance(const AMDCoreMetricSource* src) : msr::MetricInstance(src)
    {
        // PPR for AMD Family 17h Model 31h B0 page 170;
        uint64_t unit_raw = rdmsr(0xC0010299, 12, 8);
        unit = 1 / pow(2, unit_raw);
    }

    double read() override
    {
        // PPR for AMD Family 17h Model 31h B0 page 171;
        uint64_t energy = rdmsr(0xC001029A, 31, 0, true);
        return energy * unit;
    }

private:
    double unit;
    uint64_t last_reading_ = 0;
};

std::set<std::unique_ptr<msr::MetricSource>> get_all_metrics();
} // namespace amd

} // namespace msr
} // namespace internal_energy
