# libinternal_energy USAGE Guide


In general try to have a look at examples/int_energy.cpp. It tries to use all the
user-facing features of the MetricSet

The central piece for measuring metrics is the MetricSet


```cpp
auto set = internal_energy::MetricSet();
```

A MetricSet contains by default all available Metrics of the system.

A MetricSet can be filtered using the following operations:

```cpp
// By Metric Location, e.g. Location.from(Cpu(42))
set.filter_location(Location l)
// By Metric Unit, e.g. Unit::POWER or Unit::Energy
set.filter_unit(Unit u)
// By Subsystem, e.g. Subsystem::PERF
sef.filter_subsystem(Subsystem s)
// By Name e.g. "energy_pkg"
set.filter_name(std::string name)
```

The following modifiers exist:
```cpp
// If a metric already has Unit::ENERGY, do nothing.
// If a metric is of Unit::Power, wrap it in a LevelIntegrator integration component
set.convert_to_energy()
```

```cpp
// A metric set can contain duplicate Locations, e.g. both PERF and MSR and Core(0)
// energy metrics, so that adding all the energy values from a MetricSet is not sound.
// deduplicate() deduplicates the Metrics in a set:
// - Deletes duplicate Locations, choosing the saner interface (e.g. PERF CORE0 over MSR CORE0)
// - Deletes overlapping Locations, chosoing the event with the greater coverage e.g. (PKG0 contains CORE0, so PKG0 is chosen over CORE0
set.deduplicate()
```

```cpp
// Combines the metric set into one MetricSource.
// This adds the energy values of all MetricSources in the MetricSet.
// So the MetricSet needs to be deduplicated before hand, otherwise an exception
// is thrown.
CombinatorSource set.combine()
```

The following accessors exist:

```cpp
// returns const references to all metrics in the MetricSource
const std::vector<const MetricSource*> set.metrics()
```

```cpp
// Opens all Metrics in the MetricSet and returns the opened MetricInstance's
std::vector<std::unique_ptr<MetricInstance>> set.open()
```

The MetricSet contains MetricSource classes. These can not be directly constructed by the user

They have the following accessors:

```cpp
Subsystem source.get_subsystem()
Unit source.unit()
// The name only e.g. 'energy'
std::string source.name()
// The full metric string e.g. 'PERF::CORE0::energy'
std::string source.str()
Location get_location()
```

The only relevant operation is opening the Metric Source:

```cpp
std::unique_ptr<MetricInstance> open()
```

Every MetricSource is associated by a Location. The location class is a thin wrapper around the subsystem specific location types:

```cpp
// A cpu in the Linux sense 
Cpu
// A core
Core
// A CPU package
Package
// A NEC accelerator card
NecDevice // only if HAVE_VEDA
// An NVidia NVML device, usually a GPU
NVidiaDevice // only if HAVE_NVML
// An AMD ROCM device, usually a GPU
AMDDevice // only if HAVE_ROCM_SMI
```

Methods:
```cpp
// as type T, exception if Location is not of type T
location.as<T>()
// true, if Location is of type T
location.is<T>()
// true if this location contains the location l (e.g. packages contain cores).
// Ever location contains itself so: Cpu0.contains(Cpu0) == true
location.contains(Location l)
// prints out the name of the Location 
std::string str()
```

Constructors:
```cpp
// Constructs a location from a specific location type, such as Cpu(0)
location.from<T> (T)
// parses a string representation, such as "CPU0" into a location
location.from_str(std::sting str)
// Constructs a special Location, that is equal to all locations of type t
// e.g. location.core_any() == Core(0), == Core(1), == Core(2) ... Core(N)
location.pkg_any()
location.core_any()
location.cpu_any()
location.nvidia_device_any()
location.amd_device_any();
// Constructs a location that is equal to all locations
location.any()
```

When a MetricSource is open()ed, a MetricInstance is created, which is a specific open handle to a MetricSource.

A MetricInstance can only be opened from the corresponding MetricSource.open method.

A MetricInstance has two operations:

```cpp
// Collects a reading in Watt if the unit is Unit::POWER or Joules if the unit is Unit::ENERGY
double instance.read()
// Returns the underlying metric source
double const MetricSource *instance.metric_source()
```

## Lifetimes

`MetricSource` are stored directly in the `MetricSet`, so their lifetime is bound to the MetricSet.

All MetricSource references and MetricInstances created from the Metrics in a MetricSet are only valid as long as the MetricSet exists

