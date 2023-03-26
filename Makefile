PREFIX ?= /usr/X11R6
CFLAGS ?= -O3 -pedantic -Wall
CXX ?= g++

TARGET = winwm

all: $(TARGET)

$(TARGET): winwm.cpp functions.hpp
	$(CXX) $(CFLAGS) -I$(PREFIX)/include -L$(PREFIX)/lib -lX11 -ljsoncpp $< -o $@

install: $(TARGET)
	cp $(TARGET) $(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(TARGET)
