# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build

# Include any dependencies generated for this target.
include apps/annotation/CMakeFiles/opencv_annotation.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include apps/annotation/CMakeFiles/opencv_annotation.dir/compiler_depend.make

# Include the progress variables for this target.
include apps/annotation/CMakeFiles/opencv_annotation.dir/progress.make

# Include the compile flags for this target's objects.
include apps/annotation/CMakeFiles/opencv_annotation.dir/flags.make

apps/annotation/CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.o: apps/annotation/CMakeFiles/opencv_annotation.dir/flags.make
apps/annotation/CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.o: /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/apps/annotation/opencv_annotation.cpp
apps/annotation/CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.o: apps/annotation/CMakeFiles/opencv_annotation.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object apps/annotation/CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.o"
	cd /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build/apps/annotation && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT apps/annotation/CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.o -MF CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.o.d -o CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.o -c /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/apps/annotation/opencv_annotation.cpp

apps/annotation/CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.i"
	cd /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build/apps/annotation && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/apps/annotation/opencv_annotation.cpp > CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.i

apps/annotation/CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.s"
	cd /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build/apps/annotation && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/apps/annotation/opencv_annotation.cpp -o CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.s

# Object files for target opencv_annotation
opencv_annotation_OBJECTS = \
"CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.o"

# External object files for target opencv_annotation
opencv_annotation_EXTERNAL_OBJECTS =

bin/opencv_annotation: apps/annotation/CMakeFiles/opencv_annotation.dir/opencv_annotation.cpp.o
bin/opencv_annotation: apps/annotation/CMakeFiles/opencv_annotation.dir/build.make
bin/opencv_annotation: lib/libopencv_world.4.7.0.dylib
bin/opencv_annotation: apps/annotation/CMakeFiles/opencv_annotation.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/opencv_annotation"
	cd /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build/apps/annotation && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/opencv_annotation.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
apps/annotation/CMakeFiles/opencv_annotation.dir/build: bin/opencv_annotation
.PHONY : apps/annotation/CMakeFiles/opencv_annotation.dir/build

apps/annotation/CMakeFiles/opencv_annotation.dir/clean:
	cd /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build/apps/annotation && $(CMAKE_COMMAND) -P CMakeFiles/opencv_annotation.dir/cmake_clean.cmake
.PHONY : apps/annotation/CMakeFiles/opencv_annotation.dir/clean

apps/annotation/CMakeFiles/opencv_annotation.dir/depend:
	cd /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/apps/annotation /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build/apps/annotation /Users/tanabetakumi/ANTAM_Q_for_Mac/ANTAM_Q/ANTAM_Q/opencv/build/apps/annotation/CMakeFiles/opencv_annotation.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : apps/annotation/CMakeFiles/opencv_annotation.dir/depend

