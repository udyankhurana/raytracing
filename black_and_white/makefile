CC = g++
CFLAGS = -Ofast
LDFLAGS = -lrt

OBJECTS = Source.o raytracer.o
SOURCE = Source.cpp raytracer.cpp
EXECUTABLE = Tracer

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

all : $(EXECUTABLE)

clean :
	rm -rf *~ *.o $(EXECUTABLE)
