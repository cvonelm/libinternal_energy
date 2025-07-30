
#include <internal_energy/metric.hpp>

#include <internal_energy/perf/metric.hpp>
#include <numeric>
#include <optional>
#include <vector>
namespace internal_energy
{
std::vector<const MetricSource*> get_all_metrics(std::vector<Location> l)
{
    std::vector<const MetricSource*> res;
    static std::optional<std::vector<perf::MetricSource>> perf_metrics;

    if (!perf_metrics.has_value())
    {
        perf_metrics = std::make_optional(perf::get_all_metrics());
    }

    for (auto& perf_metric : perf_metrics.value())
    {
        res.emplace_back(&perf_metric);
    }

    auto foo [[maybe_unused]] =
        std::remove_if(res.begin(), res.end(),
                       [&l](auto& elem)
                       {
                           return std::accumulate(l.begin(), l.end(), true,
                                                  [&l, elem](const bool& prev, const auto& loc)
                                                  { return prev && elem->get_location() != loc; });
                       });

    return res;
}
} // namespace internal_energy
