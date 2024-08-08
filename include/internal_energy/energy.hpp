#pragma once

#include <internal_energy/event.hpp>
#include <internal_energy/topology.hpp>

#include <perf-cpp/perf_event_instance.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <type_traits>

namespace internal_energy
{

class EnergyFromPowerInstance : public EventInstance
{

public:
    EnergyFromPowerInstance(std::unique_ptr<EventSource> src, std::unique_ptr<EventInstance>&& inst)
    : EventInstance(std::move(src)), counter_(std::move(inst))
    {
        main_thread = std::thread(&EnergyFromPowerInstance::main_loop, this);
    }
    ~EnergyFromPowerInstance()
    {
        stop_ = true;
        main_thread.join();
    }

private:
    void main_loop()
    {
        auto start = std::chrono::high_resolution_clock::now();
        while (!stop_)
        {
            std::this_thread::sleep_for(counter_->get_readout_interval());
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::seconds interval =
                std::chrono::duration_cast<std::chrono::seconds>(end - start);

            energy_ += counter_->read() / interval.count();
            start = end;
        }
    }

    double read() override
    {
        return energy_;
    }

private:
    std::unique_ptr<EventInstance> counter_;
    double energy_ = 0;
    std::thread main_thread;
    bool stop_ = false;
};

class EnergyFromPowerSource : public EventSource
{
public:
    EnergyFromPowerSource(std::unique_ptr<EventSource>&& base_source)
    : base_source_(std::move(base_source))
    {
    }

    std::unique_ptr<EventInstance> open(location_t location)
    {
        auto base = base_source_->open(location);
        if (!base)
        {
            return std::unique_ptr<EventInstance>();
        }
        return std::make_unique<EnergyFromPowerInstance>(clone(), std::move(base));
    }
    std::unique_ptr<EventSource> clone()
    {
        return std::make_unique<EnergyFromPowerSource>(base_source_->clone());
    }

    std::string name() const
    {
        return "Energy derived from " + base_source_->name();
    }

    Subsystem get_subsystem() const override
    {
        return base_source_->get_subsystem();
    }

private:
    std::unique_ptr<EventSource> base_source_;
};

std::vector<std::unique_ptr<EventSource>> get_energy_counters();

} // namespace internal_energy
