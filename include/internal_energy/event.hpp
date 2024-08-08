#pragma once

#include <memory>

#include <internal_energy/topology.hpp>

namespace internal_energy
{
class EventInstance;
class EventSource
{
public:
    EventSource()
    {
    }
    virtual std::unique_ptr<EventInstance> open(location_t locations) = 0;

    virtual std::unique_ptr<EventSource> clone() = 0;

    virtual Subsystem get_subsystem() const = 0;

    virtual std::string name() const = 0;
};
class EventInstance
{
public:
    EventInstance(std::unique_ptr<EventSource>&& counter) : counter_(std::move(counter))
    {
    }
    virtual double read() = 0;

    std::chrono::seconds get_readout_interval()
    {
        return std::chrono::seconds(1);
    }

    const EventSource& event_source() const
    {
        return *counter_;
    }

private:
    std::unique_ptr<EventSource> counter_;
};
} // namespace internal_energy
