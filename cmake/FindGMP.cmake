# FindGMP.cmake
# Find the GNU Multiple Precision Arithmetic Library (GMP)
#
# This module defines the following variables:
#  GMP_FOUND - System has GMP
#  GMP_INCLUDE_DIRS - GMP include directories
#  GMP_LIBRARIES - Libraries needed to use GMP
#
# This module also defines the following imported targets:
#  GMP::GMP - The GMP library

# Find include directory
find_path(GMP_INCLUDE_DIR
  NAMES gmp.h
  PATHS
    ${GMP_ROOT}/include
    /usr/include
    /usr/local/include
    /opt/local/include
    /sw/include
)

# Find library
find_library(GMP_LIBRARY
  NAMES gmp
  PATHS
    ${GMP_ROOT}/lib
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
)

# Handle standard arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP
  DEFAULT_MSG
  GMP_LIBRARY GMP_INCLUDE_DIR
)

# Set output variables
if(GMP_FOUND)
  set(GMP_LIBRARIES ${GMP_LIBRARY})
  set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR})
  
  # Create imported target
  if(NOT TARGET GMP::GMP)
    add_library(GMP::GMP UNKNOWN IMPORTED)
    set_target_properties(GMP::GMP PROPERTIES
      IMPORTED_LOCATION "${GMP_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${GMP_INCLUDE_DIR}"
    )
  endif()
endif()

# Hide internal variables
mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARY)
