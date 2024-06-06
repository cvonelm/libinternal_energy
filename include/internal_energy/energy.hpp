#pragma once

#include <thread>
#include <chrono>

namespace internal_energy
{
class Device
{
public:
};

class EnergyCounter
{
public:
    EnergyCounter()
    {
    }
    // reads momentary energy in joules
    virtual double read_energy()
    {
        return 0;
    }
};

class Energy
{
	public:
		Energy()
		{
		}
		double get_total_energy()
		{
			return energy_;
		}
	
		virtual void start() = 0;
		virtual void end() = 0;
	protected:
		double energy_;
		
};

template<class EnergyCounterT>
class EnergyFromEnergyCounter : public Energy
{
	public:
		void start() override
	{
		start_ = energy_counter_.read_energy(); 
	}

	void end() override
	{
		energy_ = energy_counter_.read_energy() - start_;
	}
	private:
	EnergyCounterT energy_counter_;
	double start_;
};

template<class PowerCounterT>
class EnergyFromPowerCounter : public Energy
{
	public:
		EnergyFromPowerCounter(PowerCounterT power_counter)
			: power_counter_(power_counter)
		{
		}
		void start() override
		{
			main_thread = t1(&EnergyFromPowerCounter::main_loop, this);
		}
		void end() override
		{
			stop_ = true;
		}

		void main_loop()
		{
			auto start = std::chrono::high_resolution_clock::now();
			while(!stop_)
			{
				std::this_thread::sleep_for(power_counter_.get_readout_interval());
				auto end = std::chrono::high_resolution_clock::now();
				std::chrono::seconds interval = std::chrono::duration_cast<std::chrono::seconds>(end - start);
				
				energy_ += power_counter_.get_power()/interval.count();
				start = end;
			}

		}
	private:
		PowerCounterT power_counter_;
		std::thread main_thread;
		bool stop_ = false;
};

} // namespace internal_energy
