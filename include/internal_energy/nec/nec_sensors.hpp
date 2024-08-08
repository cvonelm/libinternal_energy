//  SPDX-FileCopyrightText: 2024 Technische Universität Dresden
//  SPDX-License-Identifier: MIT
#pragma once

#include <internal_energy/topology.hpp>
#include <internal_energy/util.hpp>

#include <fmt/core.h>
#include <veda/api.h>

namespace internal_energy
{

namespace nec
{
class EventInstance : public internal_energy::EventInstance
{
public:
    EventInstance(NecDevice dev, std::unique_ptr<internal_energy::EventSource>&& src)
    : internal_energy::EventInstance(std::move(src)), dev_(dev)
    {
        vedaInit(0);
    }

    ~EventInstance()
    {
        vedaExit();
    }
    double read()
    {
        float value;
        vedaDeviceGetPower(&value, dev_.as_device_t());
        return value;
    }

protected:
    NecDevice dev_;
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

        std::visit(overloaded{ [&](NecDevice dev)
                               { res = std::make_unique<nec::EventInstance>(dev, this->clone()); },
                               [](auto arg) {} },
                   location);
        return res;
    }
    static std::vector<nec::EventSource> get_all_events()
    {
        return { nec::EventSource() };
    }
    Subsystem get_subsystem() const override
    {
        return Subsystem::NEC;
    }

    std::string name() const
    {
        return "NEC::power";
    }
    std::unique_ptr<internal_energy::EventSource> clone() override
    {
        return std::unique_ptr<nec::EventSource>();
    }
};

} // namespace nec
} // namespace internal_energy
