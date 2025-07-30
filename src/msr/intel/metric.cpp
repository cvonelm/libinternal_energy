#include <internal_energy/msr/intel/metric.hpp>

namespace internal_energy
{
namespace msr
{
namespace intel
{

std::unique_ptr<internal_energy::MetricInstance> IntelPkgMetricSource::open() const
{
    return std::make_unique<IntelPkgMetricInstance>(this);
}

std::unique_ptr<internal_energy::MetricInstance> IntelCoreMetricSource::open() const
{
    return std::make_unique<IntelCoreMetricInstance>(this);
}
std::vector<std::unique_ptr<msr::MetricSource>> get_all_metrics()
{
    std::vector<std::unique_ptr<msr::MetricSource>> res;
    for (auto package : perf_cpp::Topology::instance().packages())
    {
        res.emplace_back(std::make_unique<IntelPkgMetricSource>(Location::from<Package>(
            Package(perf_cpp::Topology::instance().measuring_cpu_for_package(package).as_int()))));
    }

    for (auto core : perf_cpp::Topology::instance().cores())
    {
        res.emplace_back(
            std::make_unique<IntelCoreMetricSource>(Location::from<Core>(Core(core.as_int()))));
    }

    return res;
}
} // namespace intel
} // namespace msr
} // namespace internal_energy
