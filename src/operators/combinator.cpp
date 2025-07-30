#include <internal_energy/operators/combinator.hpp>

namespace internal_energy
{
std::unique_ptr<MetricInstance> CombinatorSource::open() const
{
    return std::make_unique<CombinatorInstance>(this);
}
} // namespace internal_energy
