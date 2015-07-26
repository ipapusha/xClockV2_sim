CC := gcc
TARGET := xClockV2-sim
SRCDIR := src
OBJDIR := obj
CFILES := $(wildcard $(SRCDIR)/*.c)
OBJFILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(CFILES))
LDFLAGS := -lpthread -lglut
CFLAGS := -Wall -pedantic -ansi
CFLAGS += -std=c99
#CFLAGS += -O3 -DNDEBUG=1

default: $(TARGET)

$(TARGET): $(OBJFILES)
	#$(CC) $(LDFLAGS) -o $@ $^
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
