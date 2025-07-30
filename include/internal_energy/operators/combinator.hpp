#pragma once

#include <internal_energy/metric.hpp>
#include <internal_energy/topology.hpp>

#include <fmt/ranges.h>

#include <stdexcept>

namespace internal_energy
{

class CombinatorSource : public MetricSource
{
public:
    CombinatorSource(std::vector<const MetricSource*> sources)
    : MetricSource(sources[0]->get_location()), sources_(sources)
    {
        for (auto* source : sources)
        {
            for (auto* other_source : sources)
            {
                if (source == other_source)
                {
                    continue;
                }
                if (source->get_location().contains(other_source->get_location()))
                {
                    throw std::runtime_error(
                        fmt::format("source {} contains the location of {}, cannot combine them!",
                                    source->name(), other_source->name()));
                }
            }
        }
        if (sources.size() == 0)
        {
            throw std::runtime_error("Must combine at least one Source!");
        }

        Unit u = sources[0]->unit();

        for (auto* source : sources)
        {
            if (source->unit() != u)
            {
                throw std::runtime_error(fmt::format("Cannot combine {} and {}, different units!",
                                                     sources[0]->name(), source->name()));
            }
        }
        unit_ = u;
    }

    Unit unit() const override
    {
        return unit_;
    }

    std::unique_ptr<MetricInstance> open() const override;

    std::string name() const override
    {
        std::vector<std::string> names;

        for (auto* source : sources_)
        {
            names.emplace_back(source->name());
        }
        return fmt::format("COMBINE_OF::({})", fmt::join(names, ", "));
    }

    std::string str() const override
    {
        return name();
    }

    Subsystem get_subsystem() const override
    {
        return Subsystem::ANY;
    }

    std::vector<const MetricSource*> sources_;
    Unit unit_;
};

class CombinatorInstance : public MetricInstance
{

public:
    CombinatorInstance(const CombinatorSource* src) : MetricInstance(src)
    {
        for (auto* counter : reinterpret_cast<const CombinatorSource*>(source_)->sources_)
        {
            counters_.emplace_back(counter->open());
        }
    }

private:
    double read() override
    {
        double res = 0;

        for (auto& counter : counters_)
        {
            res += counter->read();
        }
        return res;
    }

private:
    std::vector<std::unique_ptr<MetricInstance>> counters_;
};

} // namespace internal_energy
