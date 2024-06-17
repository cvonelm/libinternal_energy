#include <internal_energy/energy.hpp>
#include <perf-cpp/perf_event_instance.hpp>
#include <perf-cpp/sysfs_event.hpp>

#ifdef HAS_NEC
#include <internal_energy/nec/nec_sensors.hpp>
#include <memory>
#include <vector>
#endif

namespace internal_energy
{

std::vector<std::unique_ptr<Energy>> get_devices()
{
    std::vector<std::unique_ptr<Energy>> res;

    perf_cpp::SysfsPerfEvent ev("power", "energy-pkg");
    res.emplace_back(std::make_unique<EnergyFromEnergyCounter<perf_cpp::PerfEventInstance>>(
        ev.open(perf_cpp::Cpu(0))));

#ifdef HAS_NEC
    res.emplace_back(std::make_unique<EnergyFromPowerCounter<NecSensor>>(SensorType::POWER, 0));
#endif
    return res;
}

} // namespace internal_energy
