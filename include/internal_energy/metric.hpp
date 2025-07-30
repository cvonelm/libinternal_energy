#pragma once

#include <memory>

#include <internal_energy/topology.hpp>

namespace internal_energy
{

class MetricInstance;

enum class Unit
{
    POWER,
    ENERGY
};

//
class MetricSource
{
public:
    MetricSource(Location location) : location_(location)
    {
    }

    virtual std::unique_ptr<MetricInstance> open() const = 0;

    virtual Subsystem get_subsystem() const = 0;

    virtual std::string name() const = 0;

    virtual std::string str() const = 0;

    virtual Unit unit() const = 0;

    Location get_location() const
    {
        return location_;
    }

    friend bool operator<(const MetricSource& lhs, const MetricSource& rhs)
    {
        if (lhs.get_subsystem() == rhs.get_subsystem())
        {
            if (lhs.location_ == rhs.location_)
            {
                return lhs.name() < rhs.name();
            }
            return lhs.location_ < rhs.location_;
        }
        return lhs.get_subsystem() < rhs.get_subsystem();
    }

    friend bool operator==(const MetricSource& lhs, const MetricSource& rhs)
    {
        return lhs.get_subsystem() == rhs.get_subsystem() && lhs.location_ == rhs.location_ &&
               lhs.name() == rhs.name();
    }

protected:
    Location location_;
};

class MetricInstance
{
public:
    MetricInstance(const MetricSource* source) : source_(source)
    {
    }
    virtual double read() = 0;

    // backlink to the used energy sensor. We try to keep the
    // metadata solely in the Metric
    const MetricSource& metric_source() const
    {
        return *source_;
    }

protected:
    const MetricSource* source_;
};

std::vector<const MetricSource*> get_all_metrics(std::vector<Location> l);

} // namespace internal_energy
