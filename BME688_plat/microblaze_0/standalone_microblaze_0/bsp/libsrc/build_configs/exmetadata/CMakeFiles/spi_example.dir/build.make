# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = C:/Xilinx/Vitis/2023.2/tps/win64/cmake-3.24.2/bin/cmake.exe

# The command to remove a file.
RM = C:/Xilinx/Vitis/2023.2/tps/win64/cmake-3.24.2/bin/cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:/Users/cloud/OneDrive/_Arty_Spartan_7/Projects/BME688-ARTYS7-50-SPI-MAIN/BME688_plat/microblaze_0/standalone_microblaze_0/bsp/libsrc/build_configs/exmetadata

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:/Users/cloud/OneDrive/_Arty_Spartan_7/Projects/BME688-ARTYS7-50-SPI-MAIN/BME688_plat/microblaze_0/standalone_microblaze_0/bsp/libsrc/build_configs/exmetadata

# Utility rule file for spi_example.

# Include any custom commands dependencies for this target.
include CMakeFiles/spi_example.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/spi_example.dir/progress.make

CMakeFiles/spi_example:
	lopper -f -O C:/Users/cloud/OneDrive/_Arty_Spartan_7/Projects/BME688-ARTYS7-50-SPI-MAIN/BME688_plat/microblaze_0/standalone_microblaze_0/bsp/libsrc/spi C:/Users/cloud/OneDrive/_Arty_Spartan_7/Projects/BME688-ARTYS7-50-SPI-MAIN/BME688_plat/microblaze_0/standalone_microblaze_0/bsp/hw_artifacts/microblaze_0_baremetal.dts -- bmcmake_metadata_xlnx microblaze_0 C:/Xilinx/Vitis/2023.2/data/embeddedsw/XilinxProcessorIPLib/drivers/spi_v4_11/examples drvcmake_metadata

spi_example: CMakeFiles/spi_example
spi_example: CMakeFiles/spi_example.dir/build.make
.PHONY : spi_example

# Rule to build all files generated by this target.
CMakeFiles/spi_example.dir/build: spi_example
.PHONY : CMakeFiles/spi_example.dir/build

CMakeFiles/spi_example.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/spi_example.dir/cmake_clean.cmake
.PHONY : CMakeFiles/spi_example.dir/clean

CMakeFiles/spi_example.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" C:/Users/cloud/OneDrive/_Arty_Spartan_7/Projects/BME688-ARTYS7-50-SPI-MAIN/BME688_plat/microblaze_0/standalone_microblaze_0/bsp/libsrc/build_configs/exmetadata C:/Users/cloud/OneDrive/_Arty_Spartan_7/Projects/BME688-ARTYS7-50-SPI-MAIN/BME688_plat/microblaze_0/standalone_microblaze_0/bsp/libsrc/build_configs/exmetadata C:/Users/cloud/OneDrive/_Arty_Spartan_7/Projects/BME688-ARTYS7-50-SPI-MAIN/BME688_plat/microblaze_0/standalone_microblaze_0/bsp/libsrc/build_configs/exmetadata C:/Users/cloud/OneDrive/_Arty_Spartan_7/Projects/BME688-ARTYS7-50-SPI-MAIN/BME688_plat/microblaze_0/standalone_microblaze_0/bsp/libsrc/build_configs/exmetadata C:/Users/cloud/OneDrive/_Arty_Spartan_7/Projects/BME688-ARTYS7-50-SPI-MAIN/BME688_plat/microblaze_0/standalone_microblaze_0/bsp/libsrc/build_configs/exmetadata/CMakeFiles/spi_example.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/spi_example.dir/depend

