#include <internal_energy/msr/amd/metric.hpp>

namespace internal_energy
{
namespace msr
{
namespace amd
{

std::unique_ptr<internal_energy::MetricInstance> AMDPkgMetricSource::open() const
{
    return std::make_unique<AMDPkgMetricInstance>(this);
}

std::unique_ptr<internal_energy::MetricInstance> AMDCoreMetricSource::open() const
{
    return std::make_unique<AMDCoreMetricInstance>(this);
}
std::set<std::unique_ptr<msr::MetricSource>> get_all_metrics()
{
    std::set<std::unique_ptr<msr::MetricSource>> res;
    for (auto package : perf_cpp::Topology::instance().packages())
    {
        res.emplace(std::make_unique<AMDPkgMetricSource>(Location::from<Package>(
            Package(perf_cpp::Topology::instance().measuring_cpu_for_package(package).as_int()))));
    }

    for (auto core : perf_cpp::Topology::instance().cores())
    {
        res.emplace(
            std::make_unique<AMDCoreMetricSource>(Location::from<Core>(Core(core.as_int()))));
    }

    return res;
}
} // namespace amd
} // namespace msr
} // namespace internal_energy
