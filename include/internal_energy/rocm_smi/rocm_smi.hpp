#pragma once

#include <memory>
#include <rocm_smi/rocm_smi.h>
#include <string>
#include <vector>

namespace internal_energy
{
namespace rocm
{
class EventInstance : public internal_energy::EventInstance
{
public:
    EventInstance(RocmDevice dev, std::unique_ptr<internal_energy::EventSource>&& src)
    : internal_energy::EventInstance(std::move(src)), dev_(dev)
    {
    }

    double read() override
    {
        uint64_t val;
        float counter_resolution;
        uint64_t timestamp;
        rsmi_dev_energy_count_get(dev_.as_device_t(), &val, &counter_resolution, &timestamp);
        return val * counter_resolution * 0.000001;
    }

private:
    RocmDevice dev_;
};

class EventSource : public internal_energy::EventSource
{
public:
    EventSource()
    {
        rsmi_init(0);
    }

    std::unique_ptr<internal_energy::EventInstance> open(location_t location) override
    {
        std::unique_ptr<internal_energy::EventInstance> res;
        std::visit(overloaded{ [&](RocmDevice dev)
                               { res = std::make_unique<rocm::EventInstance>(dev, this->clone()); },
                               [](auto arg) {} },
                   location);
        return res;
    }

    static std::vector<rocm::EventSource> get_all_events()
    {
        uint32_t num_devices;
        rsmi_num_monitor_devices(&num_devices);

        if (num_devices == 0)
        {
            return {};
        }
        uint64_t val;
        float counter_resolution;
        uint64_t timestamp;
        auto ret = rsmi_dev_energy_count_get(0, &val, &counter_resolution, &timestamp);
        if (ret == RSMI_STATUS_SUCCESS)
        {
            return { rocm::EventSource() };
        }
        return {};
    }

    Subsystem get_subsystem() const override
    {
        return Subsystem::ROCM;
    }

    std::string name() const override
    {
        return "ROCM::energy";
    }

    std::unique_ptr<internal_energy::EventSource> clone()
    {
        return std::make_unique<rocm::EventSource>();
    }
};
} // namespace rocm
} // namespace internal_energy
