#include <internal_energy/energy.hpp>
#include <internal_energy/hwmon/hwmon.hpp>
#include <internal_energy/perf.hpp>
#ifdef HAVE_NEC
#include <internal_energy/nec/nec_sensors.hpp>
#endif

#ifdef HAVE_NVML
#include <internal_energy/nvml/nvml.hpp>
#endif

#ifdef HAVE_ROCM_SMI
#include <internal_energy/rocm_smi/rocm_smi.hpp>
#endif

#include <perf-cpp/topology.hpp>
namespace internal_energy
{

std::vector<std::unique_ptr<EventSource>> get_energy_counters()
{
    std::vector<std::unique_ptr<EventSource>> res;
    auto perf_events = perf::EventSource::get_all_events();

    for (auto perf_event : perf_events)
    {
        res.emplace_back(std::make_unique<perf::EventSource>(perf_event));
    }
#ifdef HAVE_NVML
    auto cuda_events = cuda::EventSource::get_all_events();

    for (auto cuda_event : cuda_events)
    {
        res.emplace_back(std::make_unique<cuda::EventSource>(cuda_event));
    }
#endif

#ifdef HAVE_NEC
    auto nec_events = nec::EventSource::get_all_events();

    for (auto nec_event : nec_events)
    {
        res.emplace_back(
            std::make_unique<EnergyFromPowerSource>(std::make_unique<nec::EventSource>(nec_event)));
    }
#endif

#ifdef HAVE_ROCM_SMI
    auto rocm_events = rocm::EventSource::get_all_events();

    for (auto rocm_event : rocm_events)
    {
        res.emplace_back(std::make_unique<rocm::EventSource>(rocm_event));
    }
#endif
    auto hwmon_events = hwmon::EventSource::get_all_events();

    for (auto hwmon_event : hwmon_events)
    {
        res.emplace_back(std::make_unique<EnergyFromPowerSource>(
            std::make_unique<hwmon::EventSource>(hwmon_event)));
    }
    return res;
}
} // namespace internal_energy
