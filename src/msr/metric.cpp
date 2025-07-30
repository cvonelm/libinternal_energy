#include <internal_energy/msr/amd/metric.hpp>
#include <internal_energy/msr/cpuinfo.hpp>
#include <internal_energy/msr/intel/metric.hpp>
#include <internal_energy/msr/metric.hpp>

namespace internal_energy
{
namespace msr
{

std::set<std::unique_ptr<msr::MetricSource>> get_all_metrics()
{
    if (open("/dev/cpu/0/msr", O_RDONLY) == -1)
    {
        std::cout << "Cannot access MSRs: " << strerror(errno) << " ignoring!\n";
        return {};
    }
    if (CpuInfo::instance().vendor() == Vendor::AMD)
    {
        return amd::get_all_metrics();
    }
    else if (CpuInfo::instance().vendor() == Vendor::INTEL)
    {
        return intel::get_all_metrics();
    }
    else
    {
        std::cout << "Not implemented!\n";
    }
    return {};
}
} // namespace msr
} // namespace internal_energy
