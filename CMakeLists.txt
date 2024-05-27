cmake_minimum_required(VERSION 3.13)

project (libinternal_power VERSION 0.0.1)


if(NOT INTERNAL_POWER_POSITION_INDEPENDENT_CODE)
	set(INTERNAL_POWER_POSITION_INDEPENDENT_CODE OFF)
endif()
option(INTERNAL_POWER_POSITION_INDEPENDENT_CODE "Whether to build libinternal_power with position independent code" ${INTERNAL_POWER_POSITION_INDEPENDENT_CODE})

set(LIB_SRCS
	src/power.cpp
	src/intel/intel.cpp
)


add_library(internal_power ${LIB_SRCS})
target_include_directories(internal_power
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set_target_properties(internal_power PROPERTIES
	POSITION_INDEPENDENT_CODE ${INTERNAL_POWER_POSITION_INDEPENDENT_CODE}
)

install(
	TARGETS
	internal_power
)
