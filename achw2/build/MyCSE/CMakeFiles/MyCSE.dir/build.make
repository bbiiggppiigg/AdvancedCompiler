# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bbiiggppiigg/AdvancedCompiler/achw2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bbiiggppiigg/AdvancedCompiler/achw2/build

# Include any dependencies generated for this target.
include MyCSE/CMakeFiles/MyCSE.dir/depend.make

# Include the progress variables for this target.
include MyCSE/CMakeFiles/MyCSE.dir/progress.make

# Include the compile flags for this target's objects.
include MyCSE/CMakeFiles/MyCSE.dir/flags.make

MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.o: MyCSE/CMakeFiles/MyCSE.dir/flags.make
MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.o: ../MyCSE/MyCSE.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/bbiiggppiigg/AdvancedCompiler/achw2/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.o"
	cd /home/bbiiggppiigg/AdvancedCompiler/achw2/build/MyCSE && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/MyCSE.dir/MyCSE.cpp.o -c /home/bbiiggppiigg/AdvancedCompiler/achw2/MyCSE/MyCSE.cpp

MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MyCSE.dir/MyCSE.cpp.i"
	cd /home/bbiiggppiigg/AdvancedCompiler/achw2/build/MyCSE && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/bbiiggppiigg/AdvancedCompiler/achw2/MyCSE/MyCSE.cpp > CMakeFiles/MyCSE.dir/MyCSE.cpp.i

MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MyCSE.dir/MyCSE.cpp.s"
	cd /home/bbiiggppiigg/AdvancedCompiler/achw2/build/MyCSE && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/bbiiggppiigg/AdvancedCompiler/achw2/MyCSE/MyCSE.cpp -o CMakeFiles/MyCSE.dir/MyCSE.cpp.s

MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.o.requires:
.PHONY : MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.o.requires

MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.o.provides: MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.o.requires
	$(MAKE) -f MyCSE/CMakeFiles/MyCSE.dir/build.make MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.o.provides.build
.PHONY : MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.o.provides

MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.o.provides.build: MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.o

# Object files for target MyCSE
MyCSE_OBJECTS = \
"CMakeFiles/MyCSE.dir/MyCSE.cpp.o"

# External object files for target MyCSE
MyCSE_EXTERNAL_OBJECTS =

MyCSE/libMyCSE.so: MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.o
MyCSE/libMyCSE.so: MyCSE/CMakeFiles/MyCSE.dir/build.make
MyCSE/libMyCSE.so: MyCSE/CMakeFiles/MyCSE.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared module libMyCSE.so"
	cd /home/bbiiggppiigg/AdvancedCompiler/achw2/build/MyCSE && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MyCSE.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
MyCSE/CMakeFiles/MyCSE.dir/build: MyCSE/libMyCSE.so
.PHONY : MyCSE/CMakeFiles/MyCSE.dir/build

MyCSE/CMakeFiles/MyCSE.dir/requires: MyCSE/CMakeFiles/MyCSE.dir/MyCSE.cpp.o.requires
.PHONY : MyCSE/CMakeFiles/MyCSE.dir/requires

MyCSE/CMakeFiles/MyCSE.dir/clean:
	cd /home/bbiiggppiigg/AdvancedCompiler/achw2/build/MyCSE && $(CMAKE_COMMAND) -P CMakeFiles/MyCSE.dir/cmake_clean.cmake
.PHONY : MyCSE/CMakeFiles/MyCSE.dir/clean

MyCSE/CMakeFiles/MyCSE.dir/depend:
	cd /home/bbiiggppiigg/AdvancedCompiler/achw2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bbiiggppiigg/AdvancedCompiler/achw2 /home/bbiiggppiigg/AdvancedCompiler/achw2/MyCSE /home/bbiiggppiigg/AdvancedCompiler/achw2/build /home/bbiiggppiigg/AdvancedCompiler/achw2/build/MyCSE /home/bbiiggppiigg/AdvancedCompiler/achw2/build/MyCSE/CMakeFiles/MyCSE.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : MyCSE/CMakeFiles/MyCSE.dir/depend

