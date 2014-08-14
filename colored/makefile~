CC = g++
CFLAGS = -Ofast
LDFLAGS = -lrt

OBJECTS = raytracer.o
SOURCE = raytracer.cpp
EXECUTABLE = Tracer

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

all : $(EXECUTABLE)

clean :
	rm -rf *~ *.o $(EXECUTABLE)
