#pragma once

#include <internal_energy/metric.hpp>
#include <internal_energy/topology.hpp>

#include <chrono>
#include <ratio>
#include <thread>

namespace internal_energy
{

class LevelIntegratorSource : public MetricSource
{
public:
    LevelIntegratorSource(const MetricSource* base_source)
    : MetricSource(base_source->get_location()), base_source_(base_source)
    {
        if (base_source->unit() != Unit::POWER)
        {
            throw std::runtime_error("Can not integrate counters that are not Power!");
        }
    }

    std::unique_ptr<MetricInstance> open() const override;

    std::string name() const override
    {
        return "INTEGRATOR_OF::{}" + base_source_->name();
    }

    Subsystem get_subsystem() const override
    {
        return base_source_->get_subsystem();
    }

    std::string str() const override
    {
        return name();
    }

    Unit unit() const override
    {
        return Unit::ENERGY;
    }

    const MetricSource* base_source_;
};

class LevelIntegratorInstance : public MetricInstance
{

public:
    LevelIntegratorInstance(const LevelIntegratorSource* src)
    : MetricInstance(src),
      counter_(reinterpret_cast<const LevelIntegratorSource*>(source_)->base_source_->open())
    {
        main_thread = std::thread(&LevelIntegratorInstance::main_loop, this);
    }

    ~LevelIntegratorInstance()
    {
        stop_ = true;
        main_thread.join();
    }

    //                 e-measurement
    // <-------|<------|
    // |-------|-------|-------|
private:
    void main_loop()
    {
        auto start = std::chrono::high_resolution_clock::now();
        while (!stop_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
    std::unique_ptr<MetricInstance> counter_;
    double energy_ = 0;
    std::thread main_thread;
    bool stop_ = false;
};

} // namespace internal_energy
