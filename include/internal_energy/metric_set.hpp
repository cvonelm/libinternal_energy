#pragma once

#include "internal_energy/operators/combinator.hpp"
#include <fmt/ranges.h>
#include <internal_energy/metric.hpp>
#include <internal_energy/msr/metric.hpp>
#include <internal_energy/operators/level_integrator.hpp>
#include <internal_energy/perf/metric.hpp>
#include <internal_energy/topology.hpp>

#ifdef HAVE_NVML
#include <internal_energy/nvml/metric.hpp>
#endif

#ifdef HAVE_ROCM_SMI
#include <internal_energy/rocm_smi/metric.hpp>
#endif

#include <memory>
#include <stdexcept>

namespace internal_energy
{

class MetricProvider
{
public:
    static const MetricProvider& instance()
    {
        static MetricProvider m;
        return m;
    }

    std::set<const MetricSource*> get_all_metrics() const
    {
        return metrics_;
    }

private:
    MetricProvider()
    {
        perf_sources_ = perf::get_all_metrics();

        msr_sources_ = msr::get_all_metrics();

#ifdef HAVE_NVML
        nvidia_sources_ = cuda::get_all_metrics();

        for (auto& nvidia_source : nvidia_sources_)
        {
            metrics_.emplace(&nvidia_source);
        }
#endif
#ifdef HAVE_ROCM_SMI
        amd_sources_ = rocm::get_all_metrics();
        for (auto& amd_source : amd_sources_)
        {
            metrics_.emplace(&amd_source);
        }
#endif
        for (auto& perf_source : perf_sources_)
        {
            metrics_.emplace(&perf_source);
        }

        for (auto& msr_metric : msr_sources_)
        {
            metrics_.emplace(msr_metric.get());
        }
    }
    std::set<perf::MetricSource> perf_sources_;
#ifdef HAVE_NVML
    std::set<cuda::MetricSource> nvidia_sources_;
#endif
#ifdef HAVE_ROCM_SMI
    std::set<rocm::MetricSource> amd_sources_;
#endif
    std::set<std::unique_ptr<msr::MetricSource>> msr_sources_;
    std::set<const MetricSource*> metrics_;
};
class MetricSet
{
public:
    MetricSet()
    {
        metrics_ = MetricProvider::instance().get_all_metrics();
    }
    void filter_location(Location l)
    {
        std::set<const MetricSource*> res;

        for (const auto* source : metrics_)
        {
            if (source->get_location() == l)
            {
                res.emplace(source);
            }
        }
        metrics_ = res;
    }

    void filter_unit(Unit unit)
    {
        std::set<const MetricSource*> res;
        for (const auto* source : metrics_)
        {
            if (source->unit() == unit)
            {
                res.emplace(source);
            }
        }
        metrics_ = res;
    }

    void filter_subsytem(Subsystem sub)
    {
        if (sub == Subsystem::ANY)
        {
            return;
        }
        std::set<const MetricSource*> res;

        for (const auto* source : metrics_)
        {
            if (source->get_subsystem() == sub)
            {
                res.emplace(source);
            }
        }
        metrics_ = res;
    }

    void filter_name(std::string name)
    {
        if (name == "*")
        {
            return;
        }
        std::set<const MetricSource*> res;

        for (const auto* source : metrics_)
        {
            if (source->name() == name)
            {
                res.emplace(source);
            }
        }
        metrics_ = res;
    }

    const std::set<const MetricSource*> metrics()
    {
        return metrics_;
    }

    CombinatorSource combine()
    {
        if (metrics_.size() == 0)
        {
            throw std::runtime_error("Empty MetricSet, can not combine!");
        }
        return CombinatorSource(metrics_);
    }

    void convert_to_energy()
    {
        std::set<const MetricSource*> res;

        metrics_ = res;
    }

    std::set<std::unique_ptr<MetricInstance>> open()
    {
        std::set<std::unique_ptr<MetricInstance>> res;

        for (const auto* metric : metrics_)
        {
            res.emplace(metric->open());
        }
        return res;
    }

    void deduplicate()
    {
        std::set<const MetricSource*> res = metrics_;
        std::set<const MetricSource*> delete_list;
        for (const auto* source : res)
        {
            for (const auto* other_source : res)
            {
                if (other_source == source)
                {
                    continue;
                }

                if (source->get_location().contains(other_source->get_location()))
                {
                    if (source->get_location() == source->get_location())
                    {
                        /*
                         * PERF energy counters are not always available (they sometimes require
                         * very new kernels. However, if both are available prefer perf. perf is
                         * usually based on the MSR and can possibly hide some very nasty MSR
                         * implementation details from us
                         */
                        if (source->get_subsystem() == Subsystem::PERF &&
                            other_source->get_subsystem() == Subsystem::MSR)
                        {
                            delete_list.emplace(other_source);
                        }
                        else if (source->get_subsystem() == Subsystem::MSR &&
                                 other_source->get_subsystem() == Subsystem::PERF)
                        {
                        }
                        else
                        {
                            delete_list.emplace(other_source);
                        }
                    }
                    else
                    {
                        delete_list.emplace(other_source);
                    }
                }
            }
        }
        for (auto* del : delete_list)
        {
            res.erase(del);
        }
        metrics_ = res;
    }

    friend MetricSet operator+(const MetricSet& lhs, const MetricSet& rhs)
    {
        MetricSet new_set = MetricSet();

        std::set<const MetricSource*> res;
        for (const auto* metric : lhs.metrics_)
        {
            if (rhs.metrics_.count(metric))
            {
                res.emplace(metric);
            }
        }
        return new_set;
    }

private:
    std::set<const MetricSource*> metrics_;
};

} // namespace internal_energy
