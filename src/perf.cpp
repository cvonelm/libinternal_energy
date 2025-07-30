#include <internal_energy/perf/metric.hpp>

namespace internal_energy
{
namespace perf
{

std::unique_ptr<internal_energy::EventInstance> EventSource::open(location_t location)
{
    std::unique_ptr<internal_energy::EventInstance> res;
    std::visit(overloaded{ [&](Cpu cpu)
                           {
                               if (ev_.is_supported_in(perf_cpp::Cpu(cpu.as_int())))
                               {
                                   res = std::make_unique<perf::EventInstance>(
                                       ev_.open(perf_cpp::Cpu(cpu.as_int())),
                                       std::move(std::make_unique<perf::EventSource>(*this)));
                               }
                           },
                           [&](Process process)
                           {
                               res = std::make_unique<perf::EventInstance>(
                                   ev_.open(perf_cpp::Thread(process.as_pid_t())),
                                   std::move(std::make_unique<perf::EventSource>(*this)));
                           },
                           [&](auto arg) {} },
               location);
    return res;
}
} // namespace perf
} // namespace internal_energy
