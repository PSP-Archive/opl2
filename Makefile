include objects.mak

CC = gcc
CFLAGS += -Wall -g

LIBS += -llua -lglut -lGLU -lGL -lalut -lopenal
TARGET = opl2

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)

rebuild: clean $(TARGET)

GEOM = -geometry 640x480+0+0
test: $(TARGET)
	$(TARGET) $(GEOM)
debug: $(TARGET)
	gdb -ex run --args $(TARGET) $(GEOM)

