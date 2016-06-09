# -----------------------------------------------------------------------------
#  Generic Makefile for SystemC projects
#
#  Author: Jens Rudolf (Uni Rostock)
#  Date:   2015-24-04
#
#
#  Changelog:
#	 2015-24-04 - initial version
#	 2015-29-04 - create named pipes (FIFOs)
#
#  Report bugs to:
# 	 jens.rudolf@uni-rostock.de
#
# -----------------------------------------------------------------------------

# Binary names (names of generated executables)
TBENCH		= tbench
PLAYER		= player

# The basic directory layout
SRCDIR		= ./src
OBJDIR		= ./obj
INCDIR		= ./include

# The named pipes (FIFOs)
INFILE		= ./DATA_IN
OUTFILE		= ./DATA_OUT

# Location of systemc library and includes
SYSTEMC_HOME = /usr/local/systemc-2.3.1
SYSTEMC_INC = $(SYSTEMC_HOME)/include
SYSTEMC_LIB = $(SYSTEMC_HOME)/lib-linux64

# The C++ compiler and its flags
CXX			= g++
CXXFLAGS	= -Wall -O2 -I. -I$(INCDIR) -I$(SYSTEMC_INC)

# The C++ linker and its flags
LD			= $(CXX)
LDFLAGS		= -L$(SYSTEMC_LIB) -lsystemc

# The particular files
TBSRC		:= $(wildcard $(SRCDIR)/tbench/*.cpp)
PLSRC		:= $(wildcard $(SRCDIR)/player/*.cpp)

TBOBJ		:= $(TBSRC:$(SRCDIR)/tbench/%.cpp=$(OBJDIR)/%.o)
PLOBJ		:= $(PLSRC:$(SRCDIR)/player/%.cpp=$(OBJDIR)/%.o)


# The phony targets
.PHONY: all
all: sim


.PHONY: sim
sim: $(TBENCH) $(PLAYER) $(INFILE) $(OUTFILE)
	@echo "Starting Testbench in background"
	@./$(TBENCH) $(INFILE) $(OUTFILE) &
	@echo "Starting Player in foreground"
	@./$(PLAYER) $(OUTFILE) $(INFILE)


.PHONY: clean
clean:
	@rm -f $(PRJ) $(OBJ)


# Create ./objdir
$(OBJDIR):
	@mkdir $@

# Create named pipes
$(INFILE) $(OUTFILE) :
	@mkfifo $@


# The build rules
$(TBENCH): $(TBOBJ)
	$(LD) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build of $@ complete!"

$(PLAYER): $(PLOBJ)
	$(LD) $(CXXFLAGS) -o $@ $^
	@echo "Build of $@ complete!"

$(TBOBJ): $(OBJDIR)/%.o : $(SRCDIR)/tbench/%.cpp $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PLOBJ): $(OBJDIR)/%.o : $(SRCDIR)/player/%.cpp $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@
