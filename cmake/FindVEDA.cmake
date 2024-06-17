
find_path(VEDA_INCLUDE_DIRS veda/api.h PATHS ENV C_INCLUDE_PATH ENV CPATH PATH_SUFFIXES include)

find_library(VEDA_LIBRARIES veda HINT ENV LIBRARY_PATH ENV LD_LIBRARY_PATH)


include (FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(VEDA DEFAULT_MSG VEDA_LIBRARIES VEDA_INCLUDE_DIRS)

if(VEDA_FOUND)
    add_library(libveda INTERFACE)
    target_link_libraries(libveda INTERFACE ${VEDA_LIBRARIES})
    target_include_directories(libveda SYSTEM INTERFACE ${VEDA_INCLUDE_DIRS})
    add_library(VEDA::veda ALIAS libveda)
endif()

mark_as_advanced(VEDA_LIBRARIES VEDA_INCLUDE_DIRS)
