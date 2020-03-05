CFLAGS = -Wall -g
LDLIBS = -lm
OBJS = inst.o reservation_station.o buffer.o circ_buffer.o main.o
EXE = program
$(EXE) : $(OBJS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean: 
	rm *.o $(EXE)
