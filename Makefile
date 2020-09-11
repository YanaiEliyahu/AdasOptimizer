PROJECT_NAME = $(shell basename $(dir $(MAKEFILE_PATH)))
SRCDIR = .
BASEDIR ?= /tmp/$(PROJECT_NAME)
OBJDIR = $(BASEDIR)/obj
BUILDDIR = $(BASEDIR)/bin
PCHDIR = $(BASEDIR)/pch
SRCENVDIR = $(SRCDIR)/env
DSTENVDIR = $(BUILDDIR)/env
HASHDIR = $(BASEDIR)/hash
CORES ?= $(shell cat /proc/cpuinfo | grep processor | wc -l)

RELEASE ?= 0
ifeq ($(RELEASE),1)
	DEBUG ?= 0
	SYMBOLS ?= 0
	NATIVE ?= 0
	OPT ?= 1
else
	DEBUG ?= 1
	SYMBOLS ?= 1
	NATIVE ?= 1
	OPT ?= 0
endif
ifeq ($(DEBUG),1)
	SANE ?= 1
else
	SANE ?= 0
endif

ARCH64 ?= 1
BASE_WAE ?= 1
SYNTAX ?= 0
TRAP ?= 1
EXFLAGS ?=
DEFINES = -D_debug=$(DEBUG) -D_optimizations=$(OPT) -D_release=$(RELEASE) -D_arch64=$(ARCH64)
ifeq ($(SANE),1)
	OPTIMIZATION += -fbounds-check -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=float-divide-by-zero -fno-sanitize=alignment -D_GLIBCXX_DEBUG
	ifeq ($(TRAP),1)
		OPTIMIZATION +=	-ftrapv -fsanitize-undefined-trap-on-error
	endif
endif
ifeq ($(OPT),1)
	OPTIMIZATION += -O3 -flto=$(CORES) # -ftree-parallelize-loops=$(CORES) -floop-parallelize-all
else ifeq ($(OPT),2)
	OPTIMIZATION += -Os -flto=$(CORES)
else ifeq ($(OPT),3)
	OPTIMIZATION += -Og -flto=$(CORES)
else ifeq ($(OPT),4)
	OPTIMIZATION += -O2 -flto=$(CORES)
else
	OPTIMIZATION += -O0 -fmerge-constants
endif
ifeq ($(NATIVE),1)
	OPTIMIZATION += -march=native -mtune=native
endif
ifeq ($(ARCH64),0)
	ARCH = -m32
else
	ARCH = -m64
endif
ifeq ($(SYMBOLS),0)
	DEBUG_SYMBOLS = -g0 -s -fomit-frame-pointer
else
	DEBUG_SYMBOLS = -g3 -fno-omit-frame-pointer -rdynamic -fPIC -no-pie
endif
ifeq ($(BASE_WAE),1)
	WARNINGS_AS_ERRORS = -Werror=all -Werror=extra -Werror=attributes \
						 $(shell $(CXX) -Q --help=warning | grep enabled | sed -e 's/^\s*\(\-\S*\)\s*\[\w*\]/\1 /gp;d' | sed -e "s/-W/-Werror=/g" | grep "Werror") \
						 -Wno-format-security -Wno-format-zero-length \
						 -Wno-long-long -Wno-pointer-arith -Wno-error=unused-function -Wno-error=vla \
						 -Wno-switch -Wno-dangling-else -Wno-empty-body -Wno-reorder
else
	WARNINGS_AS_ERRORS =
endif
ifeq ($(SYNTAX),1)
	SYNTAX_ONLY = -fsyntax-only
endif

MKDIR_P = @mkdir -p
MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))

SOURCES = $(shell grep `find $(SRCDIR) -name '*.cpp' -type f` -Le "^//\\\#disable-compilation")
HEADERS = $(shell find $(SRCDIR) -name '*.hpp' -type f)
OBJECTS = $(SOURCES:$(SRCDIR)/%=$(OBJDIR)/%.o)
PCHS = $(PCHDIR)/utility.hpp.gch
PCH_PATH = $(shell realpath $(PCHDIR)/utility.hpp)
SRCENVS = $(shell find $(SRCENVDIR) -type f 2> /dev/null)
DSTENVS = $(SRCENVS:$(SRCENVDIR)%=$(DSTENVDIR)%)

LIBRARYS = -static-libasan -lpthread
EXECUTABLE = $(BUILDDIR)/$(PROJECT_NAME)
INCLUDE_DIRS = -I$(SRCDIR)
BASE_FLAGS = $(ARCH) $(OPTIMIZATION) $(DEBUG_SYMBOLS) $(EXFLAGS) -ftemplate-backtrace-limit=0 -pipe -Winvalid-pch -mbmi2 -msse -msse2 -msse3 -msse4.1 -fconcepts -mavx -mavx2 -ftree-vectorize -funroll-loops
# this is for the hashing purposes, when some flags wants not to be included there.
CFLAGS_CORE = -c -std=c++17 $(WARNINGS_AS_ERRORS) $(DEFINES) $(INCLUDE_DIRS) $(BASE_FLAGS)
CFLAGS = $(CFLAGS_CORE) $(SYNTAX_ONLY)
LDFLAGS = $(LIBRARYS) $(BASE_FLAGS)

PARAMS_HASH_TARGET = $(HASHDIR)/params/$(shell echo $(CFLAGS_CORE) $(LDFLAGS) | /usr/bin/md5sum | head -c 32)
FILES_LINK_HASH_TARGET = $(HASHDIR)/files_link/$(shell echo $(SOURCES) | /usr/bin/md5sum | head -c 32)
GLOBAL_DEPS = $(PARAMS_HASH_TARGET) $(MAKEFILE_PATH)

all: $(DSTENVS) $(PCHS) $(OBJECTS) $(EXECUTABLE)

ifneq ($(MAKECMDGOALS),clean)
    include $(shell find $(OBJDIR) -name '*.d' -type f 2> /dev/null)
endif

$(PARAMS_HASH_TARGET) $(FILES_LINK_HASH_TARGET): $(MAKEFILE_PATH)
	@echo DEPS $(shell basename $(dir $@))
	rm -rf $(dir $@)
	$(MKDIR_P) $(dir $@)
	touch $@

$(PCHS): $(GLOBAL_DEPS) $(@:$(PCHDIR)/%.gch=$(SRCDIR)/%)
	@echo PCH $(@:$(PCHDIR)/%.gch=%)
	$(MKDIR_P) $(dir $@)
	$(MKDIR_P) $(dir $(@:$(PCHDIR)/%.gch=$(OBJDIR)/%.d))
	$(CXX) $(CFLAGS) -x c++-header $(realpath $(@:$(PCHDIR)/%.gch=$(SRCDIR)/%)) -MMD -MT "$(@:$(PCHDIR)/%.gch=$(OBJDIR)/%.d) $@" -MF $(@:$(PCHDIR)/%.gch=$(OBJDIR)/%.d) -o $@

$(OBJECTS): $(PCHS) $(GLOBAL_DEPS) $(@:$(OBJDIR)/%.o=$(SRCDIR)/%)
	@echo CXX $(@:$(OBJDIR)/%.o=%)
	$(MKDIR_P) $(dir $@)
	@HEADER_IMPORT='';\
	if [ -e $(@:$(OBJDIR)/%.cpp.o=$(SRCDIR)/%.hpp) ] && [ "utility.hpp" != "$(@:$(OBJDIR)/%.cpp.o=%.hpp)" ]; then\
		HEADER_IMPORT='-include $(@:$(OBJDIR)/%.cpp.o=$(SRCDIR)/%.hpp)';\
	else\
		HEADER_IMPORT='';\
	fi;\
	echo $(CXX) -I$(PCHDIR) $(CFLAGS) -include $(PCH_PATH) $$HEADER_IMPORT $(realpath $(@:$(OBJDIR)/%.o=$(SRCDIR)/%)) -MMD -MT "$(@:$(OBJDIR)/%.o=$(OBJDIR)/%.d) $@" -MF $(@:$(OBJDIR)/%.o=$(OBJDIR)/%.d) -o $@;\
	$(CXX) -I$(PCHDIR) $(CFLAGS) -include $(PCH_PATH) $$HEADER_IMPORT $(realpath $(@:$(OBJDIR)/%.o=$(SRCDIR)/%)) -MMD -MT "$(@:$(OBJDIR)/%.o=$(OBJDIR)/%.d) $@" -MF $(@:$(OBJDIR)/%.o=$(OBJDIR)/%.d) -o $@;\

$(DSTENVDIR)%: $(GLOBAL_DEPS) $(SRCENVDIR)%
	@echo ENV $(@:$(DSTENVDIR)/%=env/%)
	$(MKDIR_P) $(dir $@)
	cp -r $(@:$(DSTENVDIR)/%=$(SRCENVDIR)/%) $@

$(EXECUTABLE): $(GLOBAL_DEPS) $(FILES_LINK_HASH_TARGET) $(OBJECTS)
	@echo LINK $@
	$(MKDIR_P) $(dir $@)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@.2
	mv $@.2 $@

.PHONY: run
run: $(EXECUTABLE)
	$(EXECUTABLE)

.PHONY: clean
clean:
	rm -rf $(PCHDIR) $(OBJDIR) $(BUILDDIR) $(DSTENVDIR) $(HASHDIR)
