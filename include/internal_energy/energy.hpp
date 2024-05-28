#pragma once

namespace internal_energy
{
class Device
{
public:
};

class EnergyCounter
{
	public:
    EnergyCounter() {}
    // reads momentary energy in joules
    virtual double read_energy() {return 0;}
};

} // namespace internal_energy
