#include <internal_energy/operators/level_integrator.hpp>

namespace internal_energy
{
std::unique_ptr<MetricInstance> LevelIntegratorSource::open() const
{
    return std::make_unique<LevelIntegratorInstance>(this);
}
} // namespace internal_energy
