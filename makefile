#Compile flags 
#
#   If EXCEPTIONCOREDUMP is defined then exceptions does abort() to dump a core for debugging.
#                       not defined then exceptions are thrown and should be handled by the user.
#                         NB remember to enable coredumps  ulimit -c unlimited
#
#   If SPECIALFUNCTIONSERIES is defined, then special functions are replaced by series expansions around 0
#                            which allows sub-unit validate to check the series expansions of some of the
#                            scattering terms, which is otherwise prevented by a division by zero.
#                            This should not be used for production code!
#
# Chose one of the lines below
flags=-std=c++11   -O2     

#flags=-std=c++11  -O2     -DEXCEPTIONCOREDUMP
#flags=-std=c++11  -ggdb -DEXCEPTIONCOREDUMP
#flags=-std=c++11  -ggdb -DEXCEPTIONCOREDUMP  -DSPECIALFUNCTIONSERIES


# Detect number of cores
CORES := $(shell nproc)

# Build on multiple cores
$(info Building on $(CORES) cores out of $(shell nproc --all) cores available)

# Set default make options
MAKEFLAGS += -j$(CORES) 


#folders
SRC=SEB
SRCS=SEB/Subunits
OBJ=build/obj

#target LIBrary to build
TARGETLIB=build/libseb.a

# SEB depends on GiNaC and GNU Scientific library  (order matters here!)
LIB= -lgsl -lgslcblas -lm  -lginac
INC=

# Code Examples should be linked against the SEB library using
LIBLIB=-lseb ${LIB} -L./build 
INCINC=${INC} -I$(SRC) 

#
#  Compile library
#
#  The entire library is recompiled from scratch, if any of the header files changes, since 
#  it only involves three source files, this is a small price to pay compared to tracking dependencies.
#

SEBSOURCE  = $(wildcard $(SRC)/*.cpp)
SEBHEADERS = $(wildcard $(SRC)/*.hpp) $(wildcard $(SRCS)/*.hpp)
SEBOBJ     = $(SEBSOURCE:$(SRC)/%.cpp=$(OBJ)/%.o)

#Generate object files for each SEB source file
${OBJ}/%.o : ${SRC}/%.cpp $(SEBHEADERS)
	mkdir -p ${OBJ}
	c++ ${flags} -c ${LIB} ${INC}  $< -o $@

#Merge object files into library
$(TARGETLIB) : $(SEBOBJ)
	ar -rcs $@  $^


#
#  Compile all examples
#

EXDIR=Examples
EXSOURCE = $(wildcard $(EXDIR)/*.cpp)
EXTARGET = $(EXSOURCE:%.cpp=%)
EXOBJ = $(EXSOURCE:%.cpp=%.o)

$(EXDIR)/%.o : $(EXDIR)/%.cpp $(TARGETLIB)
	c++ ${flags} -c ${LIBLIB} ${INCINC}  $< -o $@

$(EXDIR)/% : $(EXDIR)/%.o $(TARGETLIB)
	c++ ${flags}  $< ${LIBLIB} ${INCINC}  -o $@
	rm -f $<


#
#  Compile all PaperFigs
#

PAPERDIR=PaperFigs
PAPERSOURCE = $(wildcard $(PAPERDIR)/*.cpp)
PAPERTARGET = $(PAPERSOURCE:%.cpp=%)
PAPEROBJ = $(PAPERSOURCE:%.cpp=%.o)

$(PAPERDIR)/%.o : $(PAPERDIR)/%.cpp $(TARGETLIB)
	c++ ${flags} -c ${LIBLIB} ${INCINC}  $< -o $@

$(PAPERDIR)/% : $(PAPERDIR)/%.o $(TARGETLIB)
	c++ ${flags}  $< ${LIBLIB} ${INCINC}  -o $@
	rm -f $<



#
#  Compile work  (useful for working with the library)
#

WORKDIR=work
WORKSOURCE = $(wildcard $(WORKDIR)/*.cpp)
WORKTARGET = $(WORKSOURCE:%.cpp=%)
WORKOBJ = $(WORKSOURCE:%.cpp=%.o)

$(WORKDIR)/%.o : $(WORKDIR)/%.cpp $(TARGETLIB)
	c++ ${flags} -c ${LIBLIB} ${INCINC}  $< -o $@

$(WORKDIR)/% : $(WORKDIR)/%.o $(TARGETLIB)
	c++ ${flags}  $< ${LIBLIB} ${INCINC}  -o $@
	rm -f $<



#
#  What to compile, default everything, optionally examples or clean.
#


all : ${TARGETLIB} $(EXTARGET)  ${PAPERTARGET} ${WORKTARGET}
seb : ${TARGETLIB}
work : $(WORKTARGET)
examples : $(EXTARGET)
paperfigs : ${PAPERTARGET}



.PHONY : all
.DEFAULT_GOAL := all


#Clean up by removing all object files and example executables
clean: 
	rm -f $(OBJ)/*.o  $(EXTARGET)  ${PAPERTARGET} ${WORKTARGET} ${VALIDATIONTARGET} ${TARGETLIB}

cleanexamples: 
	rm -f  ${EXTARGET}

cleanpaperfigs: 
	rm -f  ${PAPERTARGET} PaperFigs/*.q
	
cleanwork: 
	rm -f  ${WORKTARGET} ${WORKOBJ}
