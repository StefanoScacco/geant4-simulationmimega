#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.16...3.21)

# Name of the project
project(Sim)

# To make everything work
# ui_all for execution
# vis_all for visualization
find_package(Geant4 REQUIRED ui_all vis_all)

# Setup include directory for this project
include(${Geant4_USE_FILE})

# Locate sources and headers for this project
file(GLOB sources ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/*.hh)

# Needs	to include macros with extension .mac, or extension .dat
file(GLOB MACRO_FILES "*.mac")
file(GLOB DATA_FILES "*.dat")

# Needs to copy every macro file or every data file in the folder Sim into the build folder
file(COPY ${MACRO_FILES} DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${DATA_FILES} DESTINATION ${PROJECT_BINARY_DIR})

# Add the executable, and link it to the Geant4 libraries
add_executable(Sim Sim.cc ${sources} ${headers})
target_link_libraries(Sim ${Geant4_LIBRARIES}) #debug purposes

# For internal Geant4 use - but has no effect if you build this
add_custom_target(B1 DEPENDS Sim)
