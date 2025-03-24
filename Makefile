obj_dir := obj
src_dir := src
bin_dir := bin
cc := clang++

ifeq ($(OS),Windows_NT)

	assembly := learningOpengl
	extension := .exe
	defines := -D_DEBUG -DPLATFORM_WINDOWS
	includes := -Isrc -Isrc/opengl/extern/ 
	linker_flags := -lgdi32 -lwinmm -lopengl32 -lshell32 -luser32 -llibcmtd -L"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\lib\x64\" 
	compiler_flags := -Wall -Wextra -g -Wconversion -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -fsanitize=undefined -fsanitize-trap
	build_platform := windows
	
	rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
	src_files_c := $(call rwildcard,$(src_dir)/,*.c)
	src_files_cpp := $(call rwildcard,$(src_dir)/,*.cpp)
	directories := $(shell dir src /b /s /a:d)
	obj_files_c := $(patsubst %.c, $(obj_dir)/%.o, $(src_files_c))
	obj_files_cpp := $(patsubst %.cpp, $(obj_dir)/%.o, $(src_files_cpp))

else

	is_linux := $(shell uname -s)

	ifeq ($(is_linux),Linux)

	assembly := learningOpengl
	extension := 
	defines := -D_DEBUG -DPLATFORM_LINUX
	includes := -Isrc -Isrc/opengl/extern/
	linker_flags := -lX11 -lxcb -lX11-xcb 
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
ifeq ($(OS),Windows_NT)
	@echo scaffolding project structure 
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(obj_dir) 2>NUL || cd .
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(bin_dir) 2>NUL || cd .
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(addsuffix \$(src_dir),$(obj_dir)) 2>NUL || cd .
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(addprefix $(obj_dir)\$(src_dir)\,$(subst D:\projects\learningOpengl\src\,,$(directories))) 2>NUL || cd .
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
