#include <internal_energy/perf/perf.hpp>
#include <iostream>
#include <thread>
#include <chrono>
int main(void)
{
    internal_energy::EnergyFromEnergyCounter<internal_energy::perf::RaplAmdPkgCounter> dev;
    
    dev.start();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    dev.end();

    std::cout << dev.get_total_energy();
    return 0;
}
