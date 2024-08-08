#pragma once

#include <cstdint>

extern "C"
{
#include <nvml.h>
}

namespace internal_energy
{
namespace cuda
{
class EventInstance : public internal_energy::EventInstance
{
public:
    EventInstance(CudaDevice dev, std::unique_ptr<internal_energy::EventSource>&& src)
    : internal_energy::EventInstance(std::move(src)), dev_(dev)
    {
    }
    double read() override
    {
        long long unsigned int energy;
        nvmlDeviceGetTotalEnergyConsumption(dev_.as_device_t(), &energy);
        return energy / (1000.0d);
    }

private:
    CudaDevice dev_;
};

class EventSource : public internal_energy::EventSource
{
public:
    EventSource()
    {
    }

    std::unique_ptr<internal_energy::EventInstance> open(location_t location) override
    {
        std::unique_ptr<internal_energy::EventInstance> res;

        std::visit(overloaded{ [&](CudaDevice dev)
                               { res = std::make_unique<cuda::EventInstance>(dev, this->clone()); },
                               [](auto arg) {} },
                   location);
        return res;
    }

    static std::vector<cuda::EventSource> get_all_events()
    {
        return { cuda::EventSource() };
    }

    Subsystem get_subsystem() const override
    {
        return Subsystem::CUDA;
    }

    std::string name() const override
    {
        return "CUDA::energy";
    }

    std::unique_ptr<internal_energy::EventSource> clone()
    {
        return std::make_unique<cuda::EventSource>();
    }
};

} // namespace cuda
} // namespace internal_energy
