#-----------------------------------------------------------------------------
# GNU Makefile: common part for all targets
#
# $Id: Makefile.com.inc,v 1.4 2006/11/01 23:34:14 nathanst Exp $
# $Source: /usr/cvsroot/project_hog/build/gmake/Makefile.com.inc,v $
#-----------------------------------------------------------------------------

CXX = g++ # clang++
COMMON_CXXFLAGS += -w -std=c++11 # -stdlib=libc++

#COMMON_CXXFLAGS += -ansi -pedantic

CC = gcc
COMMON_CFLAGS += -w 

LN = $(CXX)
#COMMON_LNFLAGS += -stdlib=libc++

AR = ar

# Use -Wextra only for GCC version > 3
#ifneq ($(shell echo __GNUC__ | $(CXX) -E -xc - | tail -n 1), 3)
#COMMON_CXXFLAGS += -Wextra
#endif

ifeq ($(findstring "Darwin", "$(shell uname -s)"), "Darwin")

ifeq ("$(OPENGL)", "STUB")
COMMON_CXXFLAGS += -I$(ROOT)/gui/ -I$(ROOT)/gui/GL/ -DNO_OPENGL
else
COMMON_LNFLAGS += -framework Foundation -framework AppKit
COMMON_CXXFLAGS += -DOS_MAC  

COMMON_LNFLAGS += -framework GLUT -framework OpenGL
COMMON_CXXFLAGS += -I/System/Library/Frameworks/GLUT.framework/Versions/A/Headers/
COMMON_CXXFLAGS += -I/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers/
COMMON_CXXFLAGS += -I/System/Library/Frameworks/AGL.framework/Versions/A/Headers/
endif

else # not darwin

# if Linux need some other pthread call 
ifeq ($(findstring "Linux", "$(shell uname -s)"), "Linux")
COMMON_LNFLAGS += -Lapps/libs -Ldemos/libs -L/usr/X11R6/lib64 -L/usr/X11R6/lib -pthread #--stdlib=libstdc++ # -lXi -lXmu
else
COMMON_LNFLAGS += -Lapps/libs -Ldemos/libs -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lpthread #--stdlib=libstdc++ # -lXi -lXmu 
endif

ifeq ("$(OPENGL)", "STUB")
COMMON_CXXFLAGS += -I$(ROOT)/gui/ -I$(ROOT)/gui/GL/ -DNO_OPENGL
else
COMMON_CXXFLAGS += -I/usr/include/GL
COMMON_LNFLAGS +=  -lGL -lGLU -lglut
endif

endif

ifeq ("$(CPU)", "G5")
COMMON_CXXFLAGS += -mcpu=970 -mpowerpc64 -mtune=970
COMMON_CXXFLAGS += -mpowerpc-gpopt -force_cpusubtype_ALL
endif

DBG_CXXFLAGS = $(PROJ_DBG_CXXFLAGS) $(COMMON_CXXFLAGS) -g
REL_CXXFLAGS = $(PROJ_REL_CXXFLAGS) $(COMMON_CXXFLAGS) -g -O3 #-DNDEBUG

DBG_CFLAGS = $(PROJ_DBG_CFLAGS) $(COMMON_CFLAGS) -g
REL_CFLAGS = $(PROJ_REL_CFLAGS) $(COMMON_CFLAGS) -g -O3 #-DNDEBUG

DBG_LNFLAGS = $(PROJ_DBG_LNFLAGS) $(COMMON_LNFLAGS) -g
REL_LNFLAGS = $(PROJ_REL_LNFLAGS) $(COMMON_LNFLAGS) -g

DBG_OBJS = \
  $(patsubst %.cpp, $(DBG_OBJDIR)/%.o, $(SRC_CPP)) \
  $(patsubst %.c, $(DBG_OBJDIR)/%.o, $(SRC_C))
REL_OBJS = \
  $(patsubst %.cpp, $(REL_OBJDIR)/%.o, $(SRC_CPP)) \
  $(patsubst %.c, $(REL_OBJDIR)/%.o, $(SRC_C))

#-----------------------------------------------------------------------------

$(DBG_OBJDIR)/%.o: %.cpp
	@echo "$(notdir $@) (dbg)"
	mkdir -p $(dir $@)
	$(CXX) -c $(DBG_CXXFLAGS) -o $@ $<

$(REL_OBJDIR)/%.o: %.cpp
	@echo "$(notdir $@) (rel)"
	mkdir -p $(dir $@)
	$(CXX) -c $(REL_CXXFLAGS) -o $@ $<

$(DBG_OBJDIR)/%.o: %.c
	@echo "$(notdir $@) (dbg)"
	mkdir -p $(dir $@)
	$(CC) -c $(DBG_CFLAGS) -o $@ $<

$(REL_OBJDIR)/%.o: %.c
	@echo "$(notdir $@) (rel)"
	mkdir -p $(dir $@)
	$(CC) -c $(REL_CFLAGS) -o $@ $<

# Creation of dependency files using the compiler with -MM option and
# sed for postprocessing of its output (including the dependency file in
# the target section and adding the correct path to the object file). 
# Dependency files are separated for each project, even if the
# object files are shared, since the compiler generated dependency lines
# contain relative paths.

$(DBG_OBJDIR)/%.d: %.cpp
	@echo "$(notdir $@) (dbg)"
	mkdir -p $(dir $@)
	$(CXX) -MM $(DBG_CXXFLAGS) $< | sed 's#\([-A-Za-z0-9_,]*\)\.o[ :]*#$(patsubst %.d,%.o,$@) $@ : #g' >$@

$(REL_OBJDIR)/%.d: %.cpp
	@echo "$(notdir $@) (rel)"
	mkdir -p $(dir $@)
	$(CXX) -MM $(REL_CXXFLAGS) $< | sed 's#\([-A-Za-z0-9_,]*\)\.o[ :]*#$(patsubst %.d,%.o,$@) $@ : #g' >$@

$(DBG_OBJDIR)/%.d: %.c
	@echo "$(notdir $@) (dbg)"
	mkdir -p $(dir $@)
	$(CC) -MM $(DBG_CXXFLAGS) $< | sed 's#\([-A-Za-z0-9_,]*\)\.o[ :]*#$(patsubst %.d,%.o,$@) $@ : #g' >$@

$(REL_OBJDIR)/%.d: %.c
	@echo "$(notdir $@) (rel)"
	mkdir -p $(dir $@)
	$(CC) -MM $(REL_CXXFLAGS) $< | sed 's#\([-A-Za-z0-9_,]*\)\.o[ :]*#$(patsubst %.d,%.o,$@) $@ : #g' >$@

debug: $(DBG_BINDIR)/$(DBG_NAME) debug_post_build

release: $(REL_BINDIR)/$(REL_NAME) release_post_build

all: debug release

# Define post build steps in Makefile.prj.inc if needed.

post_build:

debug_post_build: post_build

release_post_build: post_build

clean:
	@rm -f `find $(DBG_OBJDIR) -name "*.o" -o -name "*.d"`
	@rm -f `find $(REL_OBJDIR) -name "*.o" -o -name "*.d"`
	@rm -f $(DBG_BINDIR)/$(DBG_NAME) $(REL_BINDIR)/$(REL_NAME)

#-----------------------------------------------------------------------------

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(SRC_CPP)),)
-include $(patsubst %.cpp, $(DBG_OBJDIR)/%.d, $(SRC_CPP))
-include $(patsubst %.cpp, $(REL_OBJDIR)/%.d, $(SRC_CPP))
endif
ifneq ($(strip $(SRC_C)),)
-include $(patsubst %.c, $(DBG_OBJDIR)/%.d, $(SRC_C))
-include $(patsubst %.c, $(REL_OBJDIR)/%.d, $(SRC_C))
endif
endif

#-----------------------------------------------------------------------------
