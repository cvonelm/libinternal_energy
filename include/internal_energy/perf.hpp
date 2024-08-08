#pragma once

#include <internal_energy/event.hpp>
#include <internal_energy/util.hpp>

namespace internal_energy
{
namespace perf
{

class EventInstance : public internal_energy::EventInstance
{
public:
    EventInstance(perf_cpp::PerfEventInstance&& ev, std::unique_ptr<EventSource> src)
    : internal_energy::EventInstance(std::move(src)), ev_(std::move(ev))
    {
    }
    double read() override
    {
        return ev_.read<double>();
    }

private:
    perf_cpp::PerfEventInstance ev_;
};

class EventSource : public internal_energy::EventSource
{
public:
    EventSource(perf_cpp::PerfEvent ev) : ev_(ev)
    {
    }

    std::unique_ptr<internal_energy::EventInstance> open(location_t location) override;

    static std::vector<perf::EventSource> get_all_events()
    {
        std::vector<perf::EventSource> res;
        res.emplace_back(EventSource(perf_cpp::SysfsPerfEvent("power", "energy-pkg")));

        return res;
    }

    Subsystem get_subsystem() const override
    {
        return Subsystem::PERF;
    }

    std::unique_ptr<internal_energy::EventSource> clone()
    {
        return std::make_unique<perf::EventSource>(ev_);
    }
    std::string name() const override
    {
        return "PERF";
    }

private:
    perf_cpp::PerfEvent ev_;
};
} // namespace perf
} // namespace internal_energy
