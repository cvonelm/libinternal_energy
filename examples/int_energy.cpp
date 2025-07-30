#include "internal_energy/metric.hpp"
#include "internal_energy/topology.hpp"
#include <internal_energy/helpers.hpp>
#include <internal_energy/metric_set.hpp>
#include <internal_energy/operators/combinator.hpp>
#include <internal_energy/perf/metric.hpp>
#include <iostream>
#include <ostream>
#include <thread>

#include <internal_energy/msr/cpuinfo.hpp>

void print_help(std::string prog_name)
{
    std::cerr << prog_name << " [COMMAND] [[ARGS]...]" << std::endl;
    std::cerr << "\t" << prog_name << " list" << std::endl;
    std::cerr << "\t" << prog_name << " read [EVENT]" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        std::cerr << argv[0] << " needs at least one argument!" << std::endl;
        print_help(argv[0]);
        return 1;
    }

    if (std::string(argv[1]) == "list")
    {
        auto set = internal_energy::MetricSet();

        std::cout << "Available Counters: " << std::endl;

        for (const auto* metric : set.metrics())
        {
            std::cout << "\t" << metric->str() << std::endl;
        }

        return 0;
    }
    if (std::string(argv[1]) == "read")
    {
        if (argc != 3)
        {
            std::cerr << "'read' requires exactly one argument!" << std::endl;
            print_help(argv[0]);
            return 1;
        }

        auto set = internal_energy::MetricSet();
        auto res = internal_energy::split_metric_str(argv[2]);
        auto s = internal_energy::parse_subsystem(std::get<0>(res));
        auto l = internal_energy::Location::from_str(std::get<1>(res));
        auto name = std::get<2>(res);

        std::cout << l.str();
        set.filter_subsytem(s);
        set.filter_location(l);
        set.filter_name(name);

        set.deduplicate();
        if (set.metrics().size() == 0)
        {
            std::cerr << "No metric matches: " << argv[2] << "!" << std::endl;
            return 1;
        }

        std::cout << "Selected Counters: " << std::endl;

        for (const auto* metric : set.metrics())
        {
            std::cout << "\t" << metric->str() << std::endl;
        }

        std::cout << "Reading counters every second, Press Ctrl+C to exit" << std::endl;

        auto inst = set.open();
        while (1)
        {
            for (auto& inst : inst)
            {
                double new_val = inst->read();

                std::cout << inst->metric_source().str() << ": " << inst->read() << std::endl;
            }
            std::cout << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        return 0;
    }
    else
    {
        std::cerr << "Unknown command " << argv[1] << " !" << std::endl;
        print_help(argv[0]);
        return 1;
    }
}
