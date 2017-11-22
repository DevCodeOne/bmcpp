# comment

CXX = avr-g++
CPPFLAGS += -DF_CPU=16000000 -Datmega328p
CPPFLAGS += -I./include
CXXFLAGS = -mmcu=atmega328p -Wall -Wextra -Wconversion -std=c++17 -Os -fconcepts

CCSRC = $(wildcard *.cc)
CCOBJ = $(patsubst %.cc, %.o, $(CCSRC))
CCDEP = $(patsubst %.cc, %.d, $(CCSRC))
CCASM = $(patsubst %.cc, %.s, $(CCSRC))

DEP += $(CCDEP)

all: $(CCDEP) main.elf main.s

main.elf: main.cc

%.elf: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.s: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(ASMFLAGS) -S -o $@ $<

%.d: %.cc
	$(CXX) -MM -MG $(CPPFLAGS) $(CXXFLAGS) $< | sed 's/$*\.o/& $@/g' > $@

clean:
	$(RM) *.elf
	$(RM) *.o
	$(RM) *.s
	$(RM) *.d

-include $(DEP)
