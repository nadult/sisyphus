MINGW_PREFIX=i686-w64-mingw32.static-
BUILD_DIR=build

-include Makefile.local

_dummy := $(shell [ -d output ] || mkdir -p output)
_dummy := $(shell [ -d $(BUILD_DIR) ] || mkdir -p $(BUILD_DIR))
_dummy := $(shell [ -d $(BUILD_DIR)/game ] || mkdir -p $(BUILD_DIR)/game)
_dummy := $(shell [ -d $(BUILD_DIR)/game/actions ] || mkdir -p $(BUILD_DIR)/game/actions)
_dummy := $(shell [ -d $(BUILD_DIR)/game/orders ] || mkdir -p $(BUILD_DIR)/game/orders)
_dummy := $(shell [ -d $(BUILD_DIR)/io ] || mkdir -p $(BUILD_DIR)/io)
_dummy := $(shell [ -d $(BUILD_DIR)/audio ] || mkdir -p $(BUILD_DIR)/audio)

SHARED_SRC=game_renderer camera base sprite physics \
		   game/base game/entity game/animator game/world game/human_control \
		   game/model_entity io/game_controller io/camera_controller \
		   audio/device audio/sound 

PROGRAM_SRC=game assets_convert 

ALL_SRC=$(PROGRAM_SRC) $(SHARED_SRC)

DEPS:=$(ALL_SRC:%=$(BUILD_DIR)/%.dep)

LINUX_OBJECTS:=$(ALL_SRC:%=$(BUILD_DIR)/%.o)
MINGW_OBJECTS:=$(ALL_SRC:%=$(BUILD_DIR)/%_.o)

LINUX_LIB_OBJECTS:=$(LIBS_SRC:%=$(BUILD_DIR)/%.o)
MINGW_LIB_OBJECTS:=$(LIBS_SRC:%=$(BUILD_DIR)/%_.o)

LINUX_SHARED_OBJECTS:=$(SHARED_SRC:%=$(BUILD_DIR)/%.o)  $(LIBS_SRC:%=$(BUILD_DIR)/%.o)
MINGW_SHARED_OBJECTS:=$(SHARED_SRC:%=$(BUILD_DIR)/%_.o) $(LIBS_SRC:%=$(BUILD_DIR)/%_.o)

LINUX_PROGRAMS:=$(PROGRAM_SRC:%=%)
MINGW_PROGRAMS:=$(PROGRAM_SRC:%=%.exe)
HTML5_PROGRAMS:=$(PROGRAM_SRC:%=output/%.html)
HTML5_PROGRAMS_SRC:=$(PROGRAM_SRC:%=output/%.html.cpp)

LINUX_PROGRAMS_OPT:=$(PROGRAM_SRC:%=%_opt)

HTML5_PROGRAM_FILES:=$(HTML5_PROGRAMS_SRC) $(HTML5_PROGRAMS) $(PROGRAM_SRC:%=output/%.html.mem) $(PROGRAM_SRC:%=output/%.js)

all: $(LINUX_PROGRAMS) $(MINGW_PROGRAMS)

FWK_DIR=libfwk
include $(FWK_DIR)/Makefile.include


LIBS=

LINUX_LIBS=$(LIBS) $(LINUX_FWK_LIBS) `$(LINUX_PKG_CONFIG) --libs bullet`
MINGW_LIBS=$(LIBS) $(MINGW_FWK_LIBS) `$(MINGW_PKG_CONFIG) --libs bullet`

INCLUDES=-Isrc/ $(FWK_INCLUDES)

NICE_FLAGS=-Woverloaded-virtual -Wnon-virtual-dtor -Werror=return-type -Wno-reorder \
		   -Wparentheses -Wno-unused-variable #-Werror -Wno-unused-but-set-variable

FLAGS+=-std=c++14 -ggdb -pthread -Wall $(NICE_FLAGS) $(INCLUDES)

LINUX_FLAGS=$(FLAGS) `$(LINUX_PKG_CONFIG) --cflags bullet` 
MINGW_FLAGS=$(FLAGS) `$(MINGW_PKG_CONFIG) --cflags bullet`

LINUX_CXX=clang++
LINUX_STRIP=strip

MINGW_CXX=$(MINGW_PREFIX)g++
MINGW_STRIP=$(MINGW_PREFIX)strip

$(DEPS): $(BUILD_DIR)/%.dep: src/%.cpp
	-$(LINUX_CXX) $(LINUX_FLAGS) -MM $< -MT $(BUILD_DIR)/$*.o   > $@
	-$(MINGW_CXX) $(MINGW_FLAGS) -MM $< -MT $(BUILD_DIR)/$*_.o >> $@

$(LINUX_OBJECTS): $(BUILD_DIR)/%.o:  src/%.cpp
	$(LINUX_CXX) $(LINUX_FLAGS) $(FLAGS_$*) -c src/$*.cpp -o $@

$(MINGW_OBJECTS): $(BUILD_DIR)/%_.o: src/%.cpp
	$(MINGW_CXX) $(MINGW_FLAGS) $(FLAGS_$*) -c src/$*.cpp -o $@

$(LINUX_PROGRAMS): %:     $(LINUX_SHARED_OBJECTS) $(BUILD_DIR)/%.o $(LINUX_FWK_LIB)
	    $(LINUX_CXX) -o $@ $^ -Wl,--export-dynamic $(LIBS_$@) $(LINUX_LIBS)

$(MINGW_PROGRAMS): %.exe: $(MINGW_SHARED_OBJECTS) $(BUILD_DIR)/%_.o $(MINGW_FWK_LIB) 
	$(MINGW_CXX) -o $@ $^ $(LIBS_$*) $(MINGW_LIBS)
	$(MINGW_STRIP) $@

HTML5_PRELOADS=data/
HTML5_FLAGS=$(HTML5_FWK_FLAGS) $(INCLUDES) -s TOTAL_MEMORY=64000000

$(HTML5_PROGRAMS_SRC): output/%.html.cpp: src/%.cpp $(SHARED_SRC:%=src/%.cpp) $(HTML5_FWK_LIB)
	cat $^ > $@

$(HTML5_PROGRAMS): output/%.html: output/%.html.cpp
	emcc $(HTML5_FLAGS) $^ -o $@ $(HTML5_PRELOADS:%=--embed-file %)

$(BUILD_DIR)/full_source.cpp: $(SHARED_SRC:%=src/%.cpp)
	cat $^ > $@

$(LINUX_PROGRAMS_OPT): %_opt: $(BUILD_DIR)/full_source.cpp $(FWK_FULL_SOURCE) src/%.cpp
	$(LINUX_CXX) $(LINUX_FLAGS) $(FULL_FWK_FLAGS) -O3 -DNDEBUG $^ -o $@

clean:
	$(MAKE) -C $(FWK_DIR) clean
	-rm -f $(LINUX_OBJECTS) $(MINGW_OBJECTS) $(LINUX_PROGRAMS) $(MINGW_PROGRAMS) $(HTML5_PROGRAM_FILES) \
		   $(BUILD_DIR)/full_source.cpp $(DEPS) $(BUILD_DIR)/.depend
	-rmdir output
	find $(BUILD_DIR) -type d -empty -delete

$(BUILD_DIR)/.depend: $(DEPS)
	cat $(DEPS) > $(BUILD_DIR)/.depend

depend: $(BUILD_DIR)/.depend
	$(MAKE) -C $(FWK_DIR) depend

.PHONY: clean depend

DEPEND_FILE=$(BUILD_DIR)/.depend
DEP=$(wildcard $(DEPEND_FILE))
ifneq "$(DEP)" ""
include $(DEP)
endif

