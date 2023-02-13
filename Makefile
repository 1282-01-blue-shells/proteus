# Note: I have only tested this on Windows so it may fail on *nix-based platforms.
#
# Note: For the convenience of the reader, all variables have been annotated with a *quot*[1] `::`
#       followed by a tag that categorizes the information contained within. The full list of
#       variable tags used in this Makefile is given below:
#		  - text
#				A generic string that does not contain whitespace.
#   	  - rel-path
#				A type of `text` that represents a filesystem path relative to the current
#				directory.
#   	  - exe
#				A type of `text` that represents the name of an executable program accessible from a
#				shell.
#  	      - A -> B
#				A Makefile function that receives an argument of type `A` and returns type `B`.
#         - [A]
#				A list of type `A`.
#
# [1]: `::` is read *has type*.

# :: rel-path
# The relative path to the directory containing Proteus applications.
APPS_DIR := Apps
# :: rel-path
# The relative path to the directory in which object files and generated documentation are stored.
BUILD_DIR := Build
# :: rel-path
# The relative path to the directory containing vendored repositories.
VENDOR_DIR := Vendor
# :: text
# The name of the Proteus firmware repo.
REPO_NAME := fehproteusfirmware
# :: rel-path
# The relative path to the local firmware repo directory.
REPO_DIR := $(VENDOR_DIR)/$(REPO_NAME)

# :: rel-path
# The relative path to the target executable.
TARGET := $(BUILD_DIR)/Proteus

# :: text
# The prefix for the target platform toolchain.
TOOLCHAIN_PREFIX := arm-none-eabi-
# :: exe
# The name of the system Git executable.
GIT := git
# :: exe
# The name of the system C++ compiler.
CXX := $(TOOLCHAIN_PREFIX)g++
# :: exe
# The name of the system C compiler.
CC := $(TOOLCHAIN_PREFIX)gcc
# :: exe
# The name of the system Doxygen executable.
DOXYGEN := doxygen
# :: exe
# The name of the system `objcopy` executable.
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy

# :: rel-path -> [rel-path]
# Returns a list of relative paths to all nested subdirectories of the given directory, *excluding*
# the given directory itself.
#
# This function is an implementation detail of `recurse_dirs`, which should be used instead.
_recurse_dirs = $(foreach dir,$(dir $(wildcard $1*/.)),$(dir) $(call _recurse_dirs,$(dir)))
# :: rel-path -> [rel-path]
# Returns a list of relative paths to the given directory and all nested subdirectories therein.
#
# Each returned path is prefixed with the path of the given directory. Unlike `_recurse_dirs`, the
# returned list includes the given directory itself.
recurse_dirs = $1 $(call _recurse_dirs,$1)
# :: [rel-path]
# The list of relative paths to all directories that may contain source files.
SRC_DIRS := $(foreach dir,Drivers Libraries Startup,$(call recurse_dirs,$(REPO_DIR)/$(dir))) \
            $(call recurse_dirs,$(APPS_DIR))

# :: [rel-path]
# The list of relative paths to all source files.
SRCS := $(foreach suffix,.cpp .c,$(wildcard $(addsuffix *$(suffix),$(SRC_DIRS)))) \
        $(REPO_DIR)/FEHProteus.cpp

# :: text -> [rel-path]
# Returns a list of relative paths to all object files with source file names ending in the given
# suffix.
filter_objs = $(addprefix $(BUILD_DIR)/,$(patsubst %$1,%.o,$(filter %$1,$(SRCS))))
# :: [rel-path]
# The list of relative paths to all object files compiled from C++ source files.
CXX_OBJS := $(call filter_objs,.cpp)
# :: [rel-path]
# The list of relative paths to all object files compiled from C source files.
C_OBJS := $(call filter_objs,.c)
# :: [rel-path]
# The list of relative paths to all object files.
OBJS := $(CXX_OBJS) $(C_OBJS)
# :: [rel-path]
# The list of relative paths to all dependency Makefiles.
DEPS := $(OBJS:.o=d)

# :: [rel-path]
# The list of relative paths to directories containing C/C++ header files that should be marked as
# include directories with the GCC `-I` option.
INC_DIRS := $(VENDOR_DIR) \
            $(REPO_DIR) \
            $(foreach dir,Drivers Libraries Startup,$(REPO_DIR)/$(dir))
# :: [text]
# The list of GCC `-I` arguments that should be passed to all compiler invocations.
INCFLAGS := $(foreach dir,$(INC_DIRS),-I$(dir))
# :: [text]
# The list of GCC `-W` arguments that should be passed to all compiler invocations.
WARNFLAGS := $(foreach name,all extra pedantic conversion float-equal no-psabi,-W$(name))
# :: text
# The revision of the C++ standard to compile C++ source files with.
CXX_STD := 17
# :: text
# The revision of the C standard to compile C source files with.
C_STD := 17

# :: [text]
# The list of arguments that should be passed to all compiler invocations.
COMMON_FLAGS := $(INCFLAGS) \
                $(WARNFLAGS) \
                -Os \
                -MMD \
                -MP \
                -mcpu=cortex-m4 \
                -mfloat-abi=soft \
                -mthumb \
                -g3 \
                -gdwarf-2 \
                -gstrict-dwarf \
                -ffunction-sections \
                -fdata-sections \
                -fno-exceptions \
                -fmessage-length=0
# :: [text]
# The list of arguments that should be passed to all C++ compiler invocations.
CXXFLAGS := $(COMMON_FLAGS) -std=c++$(CXX_STD)
# :: [text]
# The list of arguments that should be passed to all C compiler invocations.
CFLAGS := $(COMMON_FLAGS) -std=c$(C_STD)
# :: [text]
# The list of arguments that should be passed to all linker invocations.
LDFLAGS := -u _printf_float \
           -u _scanf_float \
           -T$(REPO_DIR)/Linker/MK60DN512Z_flash.ld \
           -Xlinker --gc-sections \
           -Wl,-Map,$(TARGET).map \
           -n \
           -specs=nosys.specs

# :: [text]
# A shell command 'epilogue' that causes the output of the preceding command to be discarded.
ifeq ($(OS),Windows_NT)
	SUPPRESS_SHELL := > nul 2>&1
else
	SUPPRESS_SHELL := 2> /dev/null
endif
# :: exe -> exe
# Returns a suppressed version of the given shell command that discards standard output and error.
quiet_shell = $1 $(SUPPRESS_SHELL)

# OPEN :: exe
# The name of the system `open` executable that opens a file with its default handler program.
#
# mkdir :: rel-path -> exe
# Returns a shell command that creates the given directory.
#
# rm :: rel-path -> exe
# Returns a shell command that removes the given file.
#
# rmdir :: rel-path -> exe
# Returns a shell command that removes the given directory, including all contained files and nested
# subdirectories.
ifeq ($(OS),Windows_NT)
# :: exe
# The name of the command prompt program.
	SHELL := cmd
	OPEN := start ""

# :: rel-path -> rel-path
# Replaces all occurences of `/` with `\` in the given filesystem path.
	flip_slashes = $(subst /,\,$1)

	mkdir = $(call quiet_shell,md $(call flip_slashes,$1))
	rm = $(call quiet_shell,del /q $(call flip_slashes,$1))
	rmdir = $(call quiet_shell,rd /s /q $(call flip_slashes,$1))
else
	mkdir = $(call quiet_shell,mkdir -p $1)
	rm = $(call quiet_shell,rm -f $1)
# The 'frfr' is essential for this to work.
	rmdir = $(call quiet_shell,rm -frfr $1)

	ifeq ($(shell uname),Darwin)
		OPEN := open
	else
		OPEN := xdg-open
	endif
endif

.PHONY: doc docs open-doc open-docs clean
# This allows us to omit the `@` before shell commands in recipes.
.SILENT:

# Builds the target executable.
all: $(TARGET).s19

$(TARGET).s19: $(TARGET).elf
	echo [S19] $@
	$(OBJCOPY) -O srec --srec-len=40 --srec-forceS3 $< $(@)

$(TARGET).elf: $(OBJS)
	echo [ELF] $@
	$(CXX) -o $@ $^ $(LDFLAGS)

.SECONDEXPANSION:

# Compiles all C++ source files.
$(CXX_OBJS): $(BUILD_DIR)/%.o: %.cpp | $$(@D)/.
	echo [CXX] $@
	$(CXX) -o $@ -c $< $(CXXFLAGS)

# Compiles all C source files.
$(C_OBJS): $(BUILD_DIR)/%.o: %.c | $$(@D)/.
	echo [CC ] $@
	$(CC) -o $@ -c $< $(CFLAGS)

# Creates the build directory.
$(BUILD_DIR)/.:
	$(call mkdir,$(dir $@))
# Creates all subdirectories of the build directory.
$(BUILD_DIR)/%/.:
	$(call mkdir,$(dir $@))

-include $(DEPS)

# Generates documentation with Doxygen.
#
# The generated webpage files are written to the build directory.
doc docs:
	$(DOXYGEN)

# Opens the generated documentation in the system browser.
open-doc open-docs:
	$(OPEN) $(BUILD_DIR)/html/index.html

# Deletes the target executable and build directory.
clean:
	-$(call rmdir,$(BUILD_DIR))
