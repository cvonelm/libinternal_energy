cmake_minimum_required(VERSION 3.13)
project (libinternal_energy VERSION 0.0.1)

include(FeatureSummary)
include(CMakeDependentOption)

add_subdirectory(lib/perf-cpp)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

find_package(VEDA)
find_package(CUDAToolkit)

if(NOT INTERNAL_POWER_POSITION_INDEPENDENT_CODE)
	set(INTERNAL_POWER_POSITION_INDEPENDENT_CODE OFF)
endif()
option(INTERNAL_POWER_POSITION_INDEPENDENT_CODE "Whether to build libinternal_energy with position independent code" ${INTERNAL_POWER_POSITION_INDEPENDENT_CODE})

CMAKE_DEPENDENT_OPTION(USE_NVML "Use nvml to retrieve Nvidia GPU energy values" ON "CUDAToolkit_FOUND" OFF)
add_feature_info("USE_NVML" USE_NVML "Use nvml to retrieve Nvidia GPU energy values")
CMAKE_DEPENDENT_OPTION(USE_NEC "Use libveda to retrieve NEC Vector Engine energy values" ON "VEDA_FOUND" OFF)
add_feature_info("USE_NEC" USE_NEC "Use libveda to retrieve NEC Vector Engine energy values")


set(LIB_SRCS
	src/energy.cpp)
add_library(internal_energy ${LIB_SRCS})

target_link_libraries(internal_energy perf-cpp)
target_include_directories(internal_energy
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set_target_properties(internal_energy PROPERTIES
	POSITION_INDEPENDENT_CODE ${INTERNAL_POWER_POSITION_INDEPENDENT_CODE}
)

if (USE_NVML)
	if(CUDAToolkit_FOUND)
		target_compile_definitions(internal_energy PUBLIC HAVE_NVML)
		target_link_libraries(internal_energy PRIVATE CUDA::nvml)
	else()
		message(SEND_ERROR "NVML not found but requested")
	endif()
endif()

if (USE_NEC)
	if(VEDA_FOUND)
		target_compile_definitions(internal_energy PUBLIC HAVE_NVML)
		target_link_libraries(internal_energy PRIVATE VEDA::veda)
	else()
		message(SEND_ERROR "NVML not found but requested")
	endif()
endif()
		

add_executable(amd_test examples/amd_test.cpp)
target_link_libraries(amd_test PRIVATE internal_energy)

install(
	TARGETS
	internal_energy
)
