#include <internal_energy/energy.hpp>
#include <internal_energy/hwmon/hwmon.hpp>
#include <internal_energy/measurement.hpp>
int main(void)
{
    auto counters = internal_energy::get_energy_counters();
    auto devices = internal_energy::get_all_devices();

    std::cerr << "Available counters:" << std::endl;
    for (auto& counter : counters)
    {
        std::cerr << counter->name() << std::endl;
    }

    internal_energy::Measurement measure(devices, std::move(counters));

    measure.start();
    std::this_thread::sleep_for(std::chrono::seconds(20));
    measure.stop();
    std::cerr << measure.get_total_energy() << std::endl;

    return 0;
}
