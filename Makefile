obj_dir := obj
src_dir := src
bin_dir := bin
cc := clang++

ifeq ($(OS),Windows_NT)

	assembly := learningOpengl
	extension := .exe
	defines := -D_DEBUG -DPLATFORM_WINDOWS
	includes := -Isrc -I$(vulkan_sdk)\Include
	linker_flags := -luser32 -lvulkan-1 -L$(vulkan_sdk)\Lib 
	compiler_flags := -Wall -Wextra -g3 -Wconversion -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -fsanitize=undefined -fsanitize-trap
	build_platform := windows
	
	rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
	src_files := $(shell find $(src_dir) -type f -name '*[.cpp,.c]')
	directories := $(shell dir src /b /a:d)
	obj_files := $(patsubst %.cpp, $(obj_dir)/%.o, $(src_files))

else

	is_linux := $(shell uname -s)

	ifeq ($(is_linux),Linux)

	assembly := learningOpengl
	extension := 
	defines := -D_DEBUG -DPLATFORM_LINUX_WAYLAND
	includes := -Isrc 
	linker_flags := -lxcb -lEGL 
	compiler_flags := -Wall -Wextra -g -Wconversion -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -fsanitize=undefined -fsanitize-trap 

	src_files_c := $(shell find src -type f -name '*.c')
	src_files_cpp := $(shell find src -type f -name '*.cpp')
	dependencies := $(shell find $(src_dir) -type d)
	obj_files_cpp := $(patsubst %.cpp, $(obj_dir)/%.o, $(src_files_cpp)) 
	obj_files_c := $(patsubst %.c, $(obj_dir)/%.o, $(src_files_c)) 
endif 

endif 

all: scaffold link 

scaffold: 
ifeq ($(build_platform),windows)
	@echo scaffolding project structure 
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(obj_dir) 2>NUL || cd .
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(addsuffix \$(src_dir),$(obj_dir)) 2>NUL || cd .
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(addprefix $(obj_dir)\$(src_dir)\,$(directories)) 2>NUL || cd .
else
	@mkdir -p $(bin_dir)
	@mkdir -p $(obj_dir)
	@mkdir -p $(dir $(obj_files_c))
	@mkdir -p $(dir $(obj_files_cpp))
endif

$(obj_dir)/%.o : %.cpp
	@echo $<...
	@$(cc) $< $(compiler_flags) -c  -o $@ $(defines) $(includes) 

$(obj_dir)/%.o : %.c
	@echo $<...
	@clang $< $(compiler_flags) -c -o $@ $(defines) $(includes)


link: $(obj_files_c) $(obj_files_cpp)
	@echo Linking 
	@$(cc) $(compiler_flags) $^ -o $(bin_dir)/$(assembly)$(extension) $(includes) $(defines) $(linker_flags) 
