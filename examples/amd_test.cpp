#include <chrono>
#include <internal_energy/topology.hpp>

#include <iostream>
#include <thread>
int main(void)
{
    auto devices_ = internal_energy::get_devices();
    devices_[0]->start();
    std::this_thread::sleep_for(std::chrono::seconds(20));
    devices_[0]->end();
    std::cerr << devices_[0]->get_total_energy();
    return 0;
}
