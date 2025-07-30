#pragma once

#include <internal_energy/metric.hpp>
#include <internal_energy/topology.hpp>

#include <iostream>
#include <memory>

#include <perf-cpp/event_attr.hpp>
#include <perf-cpp/event_resolver.hpp>
#include <stdexcept>
namespace internal_energy
{
namespace perf
{

class MetricSource : public internal_energy::MetricSource
{
public:
    MetricSource(perf_cpp::EventAttr ev, Location location)
    : internal_energy::MetricSource(location), ev_(ev)
    {
    }

    std::unique_ptr<internal_energy::MetricInstance> open() const override;

    Subsystem get_subsystem() const override
    {
        return Subsystem::PERF;
    }

    std::string name() const override
    {
        return ev_.name();
    }

    std::string str() const override
    {
        return fmt::format("PERF::{}::{}", location_.str(), ev_.name());
    }

    Unit unit() const override
    {
        if (ev_.unit() == std::string("Joules"))
        {
            return Unit::ENERGY;
        }
        return Unit::POWER;
    }

    perf_cpp::EventAttr get_perf_event_attr() const
    {
        return ev_;
    }

private:
    perf_cpp::EventAttr ev_;
};

class MetricInstance : public internal_energy::MetricInstance
{
public:
    MetricInstance(const perf::MetricSource* src) : internal_energy::MetricInstance(src)
    {
        if (src->get_location().tag == DeviceType::CORE)
        {
            guard_ = std::make_unique<perf_cpp::EventGuard>(src->get_perf_event_attr().open(
                src->get_location().as<Core>().measuring_cpu().as_perf_cpu()));
        }
        else if (src->get_location().tag == DeviceType::PACKAGE)
        {
            guard_ = std::make_unique<perf_cpp::EventGuard>(src->get_perf_event_attr().open(
                src->get_location().as<Package>().measuring_cpu().as_perf_cpu()));
        }
        else
        {
            throw std::runtime_error(
                fmt::format("Not a Core or Cpu: {}", src->get_location().str()));
        }
    }
    double read() override
    {
        return guard_->read<uint64_t>() *
               reinterpret_cast<const perf::MetricSource*>(source_)->get_perf_event_attr().scale();
    }

private:
    std::unique_ptr<perf_cpp::EventGuard> guard_;
};

std::set<perf::MetricSource> get_all_metrics();

} // namespace perf
} // namespace internal_energy
