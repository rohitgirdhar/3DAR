CC = g++
PROG = a.out

SRCS = main.cpp CameraNVMParser.cpp glm/*.c

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut -lGL -lGLU
endif

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

run:
	./a.out 

clean:
	rm -f $(PROG)
