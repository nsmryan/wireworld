CFLAGS += -I../..
ifeq ($(OS),Windows_NT)
	LDFLAGS += -s -lopengl32 -lgdi32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LDFLAGS += -framework OpenGL -framework Cocoa
	else ifeq ($(UNAME_S),Linux)
		LDFLAGS += -s -lGLU -lGL -lX11
	endif
endif

wireworld: wireworld.c tigr.c
	g++ $^ -O2 -o $@ $(CFLAGS) $(LDFLAGS)

.PHONY: run
run:
	./wireworld
