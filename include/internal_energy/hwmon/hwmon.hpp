#pragma once

#include <internal_energy/event.hpp>
#include <internal_energy/topology.hpp>
#include <internal_energy/util.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <variant>

namespace internal_energy
{
namespace hwmon
{

class EventInstance : public internal_energy::EventInstance
{
public:
    EventInstance(std::unique_ptr<EventSource>&& ev, std::filesystem::path path)
    : internal_energy::EventInstance(std::move(ev))
    {
        sensor.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        sensor.open(path);
    }

    double read()
    {
        double output;
        sensor >> output;
        sensor.seekg(0);
        // hwmon sensors are in microWatt
        return output / 1000000;
    }

private:
    std::ifstream sensor;
};

class EventSource : public internal_energy::EventSource
{
public:
    EventSource(std::filesystem::path path) : path_(path)
    {
    }

    std::unique_ptr<internal_energy::EventInstance> open(location_t location)
    {
        std::unique_ptr<EventInstance> res;

        std::visit(overloaded{ [&](HwmonDevice dev)
                               {
                                   res = std::make_unique<hwmon::EventInstance>(
                                       std::make_unique<hwmon::EventSource>(*this), path_);
                               },
                               [&](auto dev) {} },
                   location);
        return res;
    }

    Subsystem get_subsystem() const override
    {
        return Subsystem::HWMON;
    }

    std::string name() const override
    {
        return "HWMON::" + path_.string();
    }

    std::unique_ptr<internal_energy::EventSource> clone()
    {
        return std::make_unique<hwmon::EventSource>(path_);
    }

    static std::vector<hwmon::EventSource> get_all_events();

private:
    std::filesystem::path path_;
};
} // namespace hwmon
} // namespace internal_energy
