# $Id: CMakeLists.txt 68058 2013-03-13 14:47:43Z gcosmo $
set(useROOT true)


#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(EEShash)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
if(useROOT)
find_package(ROOT REQUIRED)
	EXECUTE_PROCESS(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")
	include_directories(${ROOT_INCLUDE_DIR})
endif(useROOT) 

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(runEEShashlik runEEShashlik.cc ${sources} ${headers})
target_link_libraries(runEEShashlik ${Geant4_LIBRARIES})
if(useROOT)
	EXECUTE_PROCESS(COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
	set(CMAKE_EXE_LINKER_FLAGS ${ROOT_LD_FLAGS})
	target_link_libraries(runEEShashlik ${ROOT_LIBRARIES})
endif(useROOT)
#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build EEShash. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(runEEShashlik_SCRIPTS
  runEEShashlik.out
  runEEShashlik.in
  icons.mac
  gui.mac
  run.png
  init.mac
  init_vis.mac
  run1.mac
  run2.mac
  vis.mac
  )

foreach(_script ${runEEShashlik_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS runEEShashlik DESTINATION bin)
