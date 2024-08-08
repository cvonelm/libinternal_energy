#pragma once
namespace internal_energy
{

// whoever is responsible for this _not_ being in the standard
// belongs in the Hague

// helper type for the visitor #4
template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
} // namespace internal_energy
