# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /boot/system/bin/cmake

# The command to remove a file.
RM = /boot/system/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /boot/home/Desktop/p/node-master/deps/uv

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /boot/home/Desktop/p/node-master/deps/uv/test_haiku

# Utility rule file for NightlyTest.

# Include the progress variables for this target.
include CMakeFiles/NightlyTest.dir/progress.make

CMakeFiles/NightlyTest:
	/boot/system/bin/ctest -D NightlyTest

NightlyTest: CMakeFiles/NightlyTest
NightlyTest: CMakeFiles/NightlyTest.dir/build.make

.PHONY : NightlyTest

# Rule to build all files generated by this target.
CMakeFiles/NightlyTest.dir/build: NightlyTest

.PHONY : CMakeFiles/NightlyTest.dir/build

CMakeFiles/NightlyTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/NightlyTest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/NightlyTest.dir/clean

CMakeFiles/NightlyTest.dir/depend:
	cd /boot/home/Desktop/p/node-master/deps/uv/test_haiku && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /boot/home/Desktop/p/node-master/deps/uv /boot/home/Desktop/p/node-master/deps/uv /boot/home/Desktop/p/node-master/deps/uv/test_haiku /boot/home/Desktop/p/node-master/deps/uv/test_haiku /boot/home/Desktop/p/node-master/deps/uv/test_haiku/CMakeFiles/NightlyTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/NightlyTest.dir/depend

