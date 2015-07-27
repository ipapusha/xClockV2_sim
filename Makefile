CC := gcc
TARGET := xClockV2-sim
SRCDIR := src
OBJDIR := obj
CFILES := $(wildcard $(SRCDIR)/*.c)
OBJFILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(CFILES))
LDFLAGS := -lpthread -lm 
CFLAGS := -Wall -pedantic -ansi
CFLAGS += -std=c99
#CFLAGS += -O3 -DNDEBUG=1

# OSX uses frameworks instead
OS := $(shell uname)
ifeq ($(OS),Darwin)
	LDFLAGS += -framework GLUT -framework OpenGL
	# ignore the fact that OSX does not like glut
	CFLAGS += -Wno-deprecated-declarations
else
	LDFLAGS += -lglut
endif

default: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR): 
	mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJFILES) $(OBJFILES:.o=.d)
	rmdir $(OBJDIR)

CFLAGS += -MMD
-include $(OBJFILES:.o=.d)
