#include "perf-cpp/topology.hpp"
#include <internal_energy/perf/metric.hpp>
namespace internal_energy
{
namespace perf
{
std::unique_ptr<internal_energy::MetricInstance> perf::MetricSource::open() const
{
    return std::make_unique<perf::MetricInstance>(this);
}

std::set<perf::MetricSource> get_all_metrics()
{
    std::set<perf::MetricSource> res;

    try
    {
        auto ev = perf_cpp::EventResolver::instance().get_event_by_name("power/energy-pkg");

        int package_id = 0;
        for (auto cpu : ev.cpus())
        {
            Package pkg(Package(perf_cpp::Topology::instance().package_of(cpu).as_int()));
            res.emplace(MetricSource(ev, Location::from(pkg)));
            package_id++;
        }
    }
    catch (perf_cpp::EventAttr::InvalidEvent& e)
    {
        std::cout << "Ignoring power/energy-pkg, not available" << std::endl;
    }

    try
    {
        auto ev = perf_cpp::EventResolver::instance().get_event_by_name("power_core/energy-core");
        for (auto cpu : ev.cpus())
        {
            Core core(Core(perf_cpp::Topology::instance().core_of(cpu).as_int()));
            res.emplace(MetricSource(ev, Location::from(core)));
        }
    }
    catch (perf_cpp::EventAttr::InvalidEvent& e)
    {
        std::cout << "Ignoring power/energy-pkg, not available" << std::endl;
    }

    return res;
}
} // namespace perf
} // namespace internal_energy
