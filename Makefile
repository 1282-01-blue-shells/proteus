# The Proteus Makefile for Team D (name subject to change).
#
# This replaces both the *Proteus_Project_Template* and *fehproteusfirmware* Makefiles.
# Features include:
#
# * support for co-existing source trees of multiple Proteus programs ("applications") that can be
#   built individually or simultaneously
# * suppressed warnings when compiling the Proteus firmware
# * Doxygen integration
#
# Example Usage
# -------------
#
# ```
# # Build all applications.
# make
# ```
#
# ```
# # Clean the build directory.
# make clean
# ```
#
# ```
# # Build Doxygen documentation and then view it in a browser.
# make doc open-doc
# ```
#
# Applications
# ------------
#
# To create a new Proteus application, make a new directory at `$(APPS_DIR)/<app-name>`, where
# `$(APPS_DIR)` is *Apps* by default and `<app-name>` is the name of the appplication. This
# directory will contain your *main.cpp* and any accompanying files. To selectively compile your
# application, pass the argument `APPS=<app-name>` to Make.
#
# To selectively compile multiple applications simultaneously, pass a comma-separated string of
# application names to Make with the `APPS` variable, e.g., `APPS=App1,App2`. Alternatively, to
# compile all applications at once, call Make without any arguments.
#
# Pitfalls
# --------
#
# Unlike the *Proteus_Project_Template* Makefile, the Proteus firmware repository is not
# automatically cloned when you run `mingw32-make`. You can obtain it from
# <https://code.osu.edu/fehelectronics/proteus_software/fehproteusfirmware.git>. By default, the
# directory containing the firmware repository is expected to be located at
# *Vendor/fehproteusfirmware*. See the `REPO_DIR` variable for details.
#
# Source-level Documentation
# --------------------------
#
# For the convenience of the reader, many variables have been annotated with a *quot*[1] `::`
# followed by a tag that categorizes the information contained within. The full list of variable
# tags used in this Makefile is given below:
#
# * text
#     A generic string that does not contain whitespace.
# * rel-path
#     A type of `text` that represents a filesystem path relative to the current directory.
# * exe
#     A type of `text` that represents the name of an executable program accessible from a shell.
# * A -> B
#     A Makefile function that receives an argument of type `A` and returns type `B`.
# * [A]
#     A list of type `A`.
#
# [1]: `::` is read *has type*.

# :: rel-path
# The relative path to the directory containing Proteus applications.
APPS_DIR := Apps
# :: rel-path
# The relative path to the directory containing Proteus application libraries.
LIBS_DIR := Libs
# :: rel-path
# The relative path to the directory in which object files and generated documentation are stored.
BUILD_DIR := Build
# :: rel-path
# The relative path to the directory containing vendored repositories (e.g., the Proteus firmware
# repo).
VENDOR_DIR := Vendor
# :: text
# The name of the Proteus firmware repo.
REPO_NAME := fehproteusfirmware
# :: rel-path
# The relative path to the local firmware repo directory.
REPO_DIR := $(VENDOR_DIR)/$(REPO_NAME)

# :: text
# The comma token.
COMMA := ,
# :: text
# The empty string.
EMPTY :=
# :: text
# The space token.
SPACE := $(EMPTY) $(EMPTY)

# APPS :: [text]
# The names of all applications to be built.
ifdef APPS
APPS := $(subst $(COMMA),$(SPACE),$(APPS))
else
APPS := $(notdir $(patsubst %/.,%,$(wildcard $(APPS_DIR)/*/.)))
endif

-include $(foreach app,$(APPS),$(APPS_DIR)/$(app)/libs.mk)
LIBS := $(sort $(foreach app,$(APPS),$($(app)_LIBS)))

# :: [text]
# The basenames of all products to be built.
PRODUCTS := $(addprefix $(BUILD_DIR)/,$(APPS))
PRODUCT_S19S := $(addsuffix .s19,$(PRODUCTS))

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
_recurse_dirs = $(foreach dir,$(patsubst %/.,%,$(wildcard $1/*/.)), \
                  $(dir) $(call _recurse_dirs,$(dir)))
# :: rel-path -> [rel-path]
# Returns a list of relative paths to the given directory and all nested subdirectories therein.
#
# Each returned path is prefixed with the path of the given directory. Unlike `_recurse_dirs`, the
# returned list includes the given directory itself.
recurse_dirs = $1 $(call _recurse_dirs,$1)
# :: [rel-path]
# The list of relative paths to all directories that may contain source files.
SRC_DIRS := $(foreach dir,Drivers Libraries Startup,$(call recurse_dirs,$(REPO_DIR)/$(dir))) \
            $(foreach app,$(APPS),$(call recurse_dirs,$(APPS_DIR)/$(app)))

# :: [rel-path]
# The list of relative paths to all source files.
SRCS := $(foreach suffix,.cpp .c,$(wildcard $(addsuffix /*$(suffix),$(SRC_DIRS)))) \
        $(foreach lib,$(LIBS),$(LIBS_DIR)/$(lib).cpp) \
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
INC_DIRS := $(LIBS_DIR) \
            $(VENDOR_DIR) \
            $(REPO_DIR) \
            $(foreach dir,Drivers Libraries Startup,$(REPO_DIR)/$(dir))
# :: [text]
# The list of GCC `-I` arguments that should be passed to all compiler invocations.
INCFLAGS := $(foreach dir,$(INC_DIRS),-I$(dir))
# :: text
# The revision of the C++ standard to compile C++ source files with.
CXX_STD := 17
# :: text
# The revision of the C standard to compile C source files with.
C_STD := 17

# :: [text]
# The list of arguments that should be passed to all compiler invocations.
COMMON_FLAGS := $(INCFLAGS) \
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
# :: text -> [text]
# Returns the list of arguments that should be passed to all linker invocations for the given build
# product.
ldflags = -u _printf_float \
          -u _scanf_float \
          -T$(REPO_DIR)/Linker/MK60DN512Z_flash.ld \
          -Xlinker --gc-sections \
          -Wl,-Map,$1.map \
          -n \
          -specs=nosys.specs
# :: text -> [text]
# Returns the list of GCC `-W` arguments that should be passed to all compiler compilations for the
# given object file.
warnflags = $(if $(filter $(BUILD_DIR)/$(REPO_DIR)/%.o,$1), \
              -w, \
              $(foreach name,all extra pedantic conversion float-equal no-psabi,-W$(name)))

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
.SECONDEXPANSION:
# This allows us to omit the `@` before shell commands in recipes.
.SILENT:

all: $(PRODUCTS)

$(PRODUCTS): $(PRODUCT_S19S)

$(PRODUCT_S19S): %.s19: %.elf
	echo [S19] $@
	$(OBJCOPY) -O srec --srec-len=40 --srec-forceS3 $< $(@)

%.elf:
	echo [ELF] $@
	$(CXX) -o $@ $^ $(call ldflags,$(basename $@))

# :: text -> [text]
# Returns a sequence of Makefile statements that define app-specific recipes for the given
# application.
define def_app_recipes
    __$1_OBJS := $(filter-out \
                   $(foreach app,$(filter-out $1,$(APPS)),$(BUILD_DIR)/$(APPS_DIR)/$(app)/%.o), \
                   $(OBJS)) \
                 $(filter-out \
                   $(foreach lib,$(filter-out $1_LIBS,$(LIBS)),$(BUILD_DIR)/$(LIBS_DIR)/$(lib).o), \
                   $(OBJS))

    $(BUILD_DIR)/$1.elf: $$(__$1_OBJS)
endef
# Define all app-specific recipes.
$(foreach app,$(APPS),$(eval $(call def_app_recipes,$(app))))

# Compiles all C++ source files.
$(CXX_OBJS): $(BUILD_DIR)/%.o: %.cpp | $$(@D)/.
	echo [CXX] $@
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(call warnflags,$@)

# Compiles all C source files.
$(C_OBJS): $(BUILD_DIR)/%.o: %.c | $$(@D)/.
	echo [CC ] $@
	$(CC) -o $@ -c $< $(CFLAGS) $(call warnflags,$@)

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
