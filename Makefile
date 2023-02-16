TARGETS= ringmaster

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

ringmaster: ringmaster.cpp
	g++ -g -o $@ $<