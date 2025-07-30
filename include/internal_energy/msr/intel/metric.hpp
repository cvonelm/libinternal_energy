#pragma once

#include <internal_energy/msr/metric.hpp>

namespace internal_energy
{
namespace msr
{
namespace intel
{
class IntelPkgMetricSource : public msr::MetricSource
{
public:
    IntelPkgMetricSource(Location l) : msr::MetricSource(l)
    {
    }
    std::unique_ptr<internal_energy::MetricInstance> open() const override;

    std::string name() const override
    {
        return "IntelPkgEnergy";
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

class IntelCoreMetricSource : public msr::MetricSource
{
public:
    IntelCoreMetricSource(Location l) : msr::MetricSource(l)
    {
    }
    std::unique_ptr<internal_energy::MetricInstance> open() const override;

    std::string name() const override
    {
        return "IntelCoreEnergy";
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
class IntelPkgMetricInstance : public msr::MetricInstance
{
public:
    IntelPkgMetricInstance(const IntelPkgMetricSource* src) : msr::MetricInstance(src)
    {
        // PPR for Intel Family 17h Model 31h B0 page 170;
        uint64_t unit_raw = rdmsr(0x611, 12, 8);
        unit = 1 / pow(2, unit_raw);
    }

    double read() override
    {
        // PPR for Intel Family 17h Model 31h B0 page 171;
        uint64_t energy = rdmsr(0x611, 31, 0, true);
        return energy * unit;
    }

private:
    double unit;
    uint64_t last_reading_ = 0;
};
class IntelCoreMetricInstance : public msr::MetricInstance
{
public:
    IntelCoreMetricInstance(const IntelCoreMetricSource* src) : msr::MetricInstance(src)
    {
        uint64_t unit_raw = rdmsr(0x639, 12, 8);
        unit = pow(2, unit_raw) * 0.001;
    }

    double read() override
    {
        uint64_t energy = rdmsr(0x639, 31, 0, true);
        return energy * unit;
    }

private:
    double unit;
    uint64_t last_reading_ = 0;
};

std::vector<std::unique_ptr<msr::MetricSource>> get_all_metrics();
} // namespace intel

} // namespace msr
} // namespace internal_energy
