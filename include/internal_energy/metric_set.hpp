#pragma once

#include "internal_energy/operators/combinator.hpp"
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
#include <ostream>
#include <stdexcept>

namespace internal_energy
{
class MetricSet
{
public:
    MetricSet()
    {
        perf_sources_ = perf::get_all_metrics();

        msr_sources_ = msr::get_all_metrics();

#ifdef HAVE_NVML
        nvidia_sources_ = cuda::get_all_metrics();

        for (auto& nvidia_source : nvidia_sources_)
        {
            metrics_.emplace_back(&nvidia_source);
        }
#endif
#ifdef HAVE_ROCM_SMI
        amd_sources_ = rocm::get_all_metrics();
        for (auto& amd_source : amd_sources_)
        {
            metrics_.emplace_back(&amd_source);
        }
#endif
        for (auto& perf_source : perf_sources_)
        {
            metrics_.emplace_back(&perf_source);
        }

        for (auto& msr_metric : msr_sources_)
        {
            metrics_.emplace_back(msr_metric.get());
        }
    }
    void filter_location(Location l)
    {
        std::vector<const MetricSource*> res;

        for (const auto* source : metrics_)
        {
            if (source->get_location() == l)
            {
                res.emplace_back(source);
            }
        }
        metrics_ = res;
    }

    void filter_unit(Unit unit)
    {
        std::vector<const MetricSource*> res;
        for (const auto* source : metrics_)
        {
            if (source->unit() == unit)
            {
                res.emplace_back(source);
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
        std::vector<const MetricSource*> res;

        for (const auto* source : metrics_)
        {
            if (source->get_subsystem() == sub)
            {
                res.emplace_back(source);
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
        std::vector<const MetricSource*> res;

        for (const auto* source : metrics_)
        {
            if (source->name() == name)
            {
                res.emplace_back(source);
            }
        }
        metrics_ = res;
    }

    const std::vector<const MetricSource*> metrics()
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
        std::vector<const MetricSource*> res;

        for (const auto* metric : metrics_)
        {
            if (metric->unit() == Unit::ENERGY)
            {
                res.emplace_back(metric);
            }

            else
            {
                integrators_.emplace_back(LevelIntegratorSource(metric));
                res.emplace_back(&integrators_.back());
            }
        }
        metrics_ = res;
    }

    std::vector<std::unique_ptr<MetricInstance>> open()
    {
        std::vector<std::unique_ptr<MetricInstance>> res;

        for (const auto* metric : metrics_)
        {
            res.emplace_back(metric->open());
        }
        return res;
    }

    void deduplicate()
    {
        std::vector<const MetricSource*> res = metrics_;
        std::vector<const MetricSource*> delete_list;
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
                    std::cout << "THIS: " << source->get_location().str() << std::endl;
                    std::cout << "THAT: " << other_source->get_location().str() << std::endl;
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
                            delete_list.emplace_back(other_source);
                        }
                        else if (source->get_subsystem() == Subsystem::MSR &&
                                 other_source->get_subsystem() == Subsystem::PERF)
                        {
                        }
                        else
                        {
                            delete_list.emplace_back(other_source);
                        }
                    }
                    else
                    {
                        delete_list.emplace_back(other_source);
                    }
                }
            }
        }
        for (auto* del : delete_list)
        {
            res.erase(std::remove(res.begin(), res.end(), del), res.end());
        }
        metrics_ = res;
    }

private:
    std::vector<perf::MetricSource> perf_sources_;
#ifdef HAVE_NVML
    std::vector<cuda::MetricSource> nvidia_sources_;
#endif
#ifdef HAVE_ROCM_SMI
    std::vector<rocm::MetricSource> amd_sources_;
#endif
    std::vector<std::unique_ptr<msr::MetricSource>> msr_sources_;
    std::vector<LevelIntegratorSource> integrators_;
    std::vector<const MetricSource*> metrics_;
};

} // namespace internal_energy
