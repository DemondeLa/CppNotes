# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/will/lesson/gTest/01.test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/will/lesson/gTest/01.test/build

# Include any dependencies generated for this target.
include CMakeFiles/test_example.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/test_example.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/test_example.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_example.dir/flags.make

CMakeFiles/test_example.dir/test.cpp.o: CMakeFiles/test_example.dir/flags.make
CMakeFiles/test_example.dir/test.cpp.o: ../test.cpp
CMakeFiles/test_example.dir/test.cpp.o: CMakeFiles/test_example.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/will/lesson/gTest/01.test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_example.dir/test.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/test_example.dir/test.cpp.o -MF CMakeFiles/test_example.dir/test.cpp.o.d -o CMakeFiles/test_example.dir/test.cpp.o -c /home/will/lesson/gTest/01.test/test.cpp

CMakeFiles/test_example.dir/test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_example.dir/test.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/will/lesson/gTest/01.test/test.cpp > CMakeFiles/test_example.dir/test.cpp.i

CMakeFiles/test_example.dir/test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_example.dir/test.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/will/lesson/gTest/01.test/test.cpp -o CMakeFiles/test_example.dir/test.cpp.s

CMakeFiles/test_example.dir/test_add.cpp.o: CMakeFiles/test_example.dir/flags.make
CMakeFiles/test_example.dir/test_add.cpp.o: ../test_add.cpp
CMakeFiles/test_example.dir/test_add.cpp.o: CMakeFiles/test_example.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/will/lesson/gTest/01.test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/test_example.dir/test_add.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/test_example.dir/test_add.cpp.o -MF CMakeFiles/test_example.dir/test_add.cpp.o.d -o CMakeFiles/test_example.dir/test_add.cpp.o -c /home/will/lesson/gTest/01.test/test_add.cpp

CMakeFiles/test_example.dir/test_add.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_example.dir/test_add.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/will/lesson/gTest/01.test/test_add.cpp > CMakeFiles/test_example.dir/test_add.cpp.i

CMakeFiles/test_example.dir/test_add.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_example.dir/test_add.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/will/lesson/gTest/01.test/test_add.cpp -o CMakeFiles/test_example.dir/test_add.cpp.s

# Object files for target test_example
test_example_OBJECTS = \
"CMakeFiles/test_example.dir/test.cpp.o" \
"CMakeFiles/test_example.dir/test_add.cpp.o"

# External object files for target test_example
test_example_EXTERNAL_OBJECTS =

test_example: CMakeFiles/test_example.dir/test.cpp.o
test_example: CMakeFiles/test_example.dir/test_add.cpp.o
test_example: CMakeFiles/test_example.dir/build.make
test_example: /usr/local/lib/libgtest_main.a
test_example: /usr/local/lib/libgtest.a
test_example: CMakeFiles/test_example.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/will/lesson/gTest/01.test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable test_example"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_example.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_example.dir/build: test_example
.PHONY : CMakeFiles/test_example.dir/build

CMakeFiles/test_example.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_example.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_example.dir/clean

CMakeFiles/test_example.dir/depend:
	cd /home/will/lesson/gTest/01.test/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/will/lesson/gTest/01.test /home/will/lesson/gTest/01.test /home/will/lesson/gTest/01.test/build /home/will/lesson/gTest/01.test/build /home/will/lesson/gTest/01.test/build/CMakeFiles/test_example.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_example.dir/depend

