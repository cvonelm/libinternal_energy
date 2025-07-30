cmake_minimum_required(VERSION 3.13)
project (libinternal_energy VERSION 0.0.1)

include(FeatureSummary)
include(CMakeDependentOption)
include(FindPackageHandleStandardArgs)

add_subdirectory(lib/perf-cpp)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

find_package(VEDA)
find_package(CUDAToolkit)
find_package(Threads)
find_package(PkgConfig)


find_library(ROCM_SMI_LIBRARY NAMES rocm_smi64
    HINTS "/opt/rocm/lib")
find_path(ROCM_SMI_INCLUDE_DIR NAMES rocm_smi/rocm_smi.h
    HINTS "/opt/rocm/include")

find_package_handle_standard_args(ROCM_SMI REQUIRED_VARS ROCM_SMI_LIBRARY)


if (ROCM_SMI_FOUND)
    add_library(ROCM_SMI::ROCM_SMI UNKNOWN IMPORTED)
    set_property(TARGET ROCM_SMI::ROCM_SMI PROPERTY IMPORTED_LOCATION ${ROCM_SMI_LIBRARY})
    target_include_directories(ROCM_SMI::ROCM_SMI INTERFACE ${ROCM_SMI_INCLUDE_DIR})
endif()

option(BUILD_SHARED_LIBS "Whether to build shared libraries" ON)

CMAKE_DEPENDENT_OPTION(USE_NVML "Use nvml to retrieve Nvidia GPU energy values" ON "CUDAToolkit_FOUND" OFF)
add_feature_info("USE_NVML" USE_NVML "Use nvml to retrieve Nvidia GPU energy values")
CMAKE_DEPENDENT_OPTION(USE_NEC "Use libveda to retrieve NEC Vector Engine energy values" ON "VEDA_FOUND" OFF)
add_feature_info("USE_NEC" USE_NEC "Use libveda to retrieve NEC Vector Engine energy values")
CMAKE_DEPENDENT_OPTION(USE_ROCM_SMI "Use rocm-smi to retrive AMDGPU energy values" ON "ROCM_SMI_FOUND" OFF)
add_feature_info("USE_ROCM_SMI" USE_ROCM_SMI "Use rocm-smi to retrieve AMDGPU energy values")


set(LIB_SRCS
    src/topology.cpp
    src/metric.cpp
    src/perf/metric.cpp
    src/msr/metric.cpp
    src/msr/amd/metric.cpp
    src/msr/intel/metric.cpp
    src/operators/level_integrator.cpp
    src/operators/combinator.cpp)

add_library(internal_energy ${LIB_SRCS})

target_link_libraries(internal_energy PUBLIC perf-cpp)
target_link_libraries(internal_energy PUBLIC Threads::Threads)

if(PKG_CONFIG_FOUND)
    pkg_check_modules(fmt
        IMPORTED_TARGET
        fmt)
    if(fmt_FOUND)
        target_link_libraries(internal_energy PUBLIC PkgConfig::fmt)
    else()
        target_link_libraries(internal_energy PUBLIC fmt::fmt)
    endif()
else()
    target_link_libraries(internal_energy PUBLIC fmt::fmt)
endif()


target_link_libraries(internal_energy PUBLIC stdc++fs)
target_include_directories(internal_energy
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
set_property(TARGET internal_energy PROPERTY CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

if (${INTERNAL_ENERGY_POSITION_INDEPENDENT_CODE})
    set(PERF_CPP_POSITION_INDEPENDENT_CODE)
endif()

if (USE_NVML)
	if(CUDAToolkit_FOUND)
		target_compile_definitions(internal_energy PUBLIC HAVE_NVML)
		target_link_libraries(internal_energy PUBLIC CUDA::nvml)
        target_sources(internal_energy  PRIVATE src/nvml/metric.cpp)
	else()
		message(SEND_ERROR "NVML not found but requested")
	endif()
endif()

if (USE_NEC)
	if(VEDA_FOUND)
        target_compile_definitions(internal_energy PUBLIC HAVE_NEC)
	target_link_libraries(internal_energy PUBLIC VEDA::veda)
	else()
		message(SEND_ERROR "VEDA not found but requested")
	endif()
endif()


if(USE_ROCM_SMI)
    if(ROCM_SMI_FOUND)
        target_compile_definitions(internal_energy PUBLIC HAVE_ROCM_SMI)
        target_link_libraries(internal_energy PUBLIC ROCM_SMI::ROCM_SMI)
        target_sources(internal_energy  PRIVATE src/rocm_smi/metric.cpp)
    else()
        message(SEND_ERROR "ROCM_SMI not found but requested")
    endif()
endif()


add_executable(int_energy examples/int_energy.cpp)
target_link_libraries(int_energy PRIVATE internal_energy)
set_property(TARGET int_energy PROPERTY CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/include/internal_energy
    DESTINATION include)
install(
	TARGETS
	internal_energy
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
