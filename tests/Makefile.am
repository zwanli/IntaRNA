
# ensure serial test build and run
AUTOMAKE_OPTIONS = serial-tests

# list of tests to run
TESTS = runTests


# the program to build
check_PROGRAMS = runTests 

# test sources
runTests_SOURCES =	catch.hpp \
					AccessibilityConstraint_test.cpp \
					AccessibilityFromStream_test.cpp \
					IndexRange_test.cpp  \
					IndexRangeList_test.cpp  \
					Interaction_test.cpp  \
					InteractionEnergyBasePair_test.cpp  \
					InteractionRange_test.cpp  \
					RnaSequence_test.cpp \
					OutputRangeOnly_test.cpp \
					runTests.cpp

# add source objects for linking
runTests_LDFLAGS = `ls ../src/*.o | tr " " "\n" | grep -v "intaRNA" | grep -v "Command" | tr "\n" " "` @AM_LDFLAGS@ @LDFLAGS@

# add source include for compilation
runTests_CXXFLAGS =	-I$(top_srcdir)/src @AM_CXXFLAGS@ @CXXFLAGS@
