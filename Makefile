DBUS_CFLAGS=`pkg-config --cflags dbus-1` 
DBUS_LIBS=`pkg-config --libs dbus-1`

CC=gcc

CFLAGS=-g $(DBUS_CFLAGS)
LFLAGS=-L/usr/lib $(DBUS_LIBS)

TARGET=test_dbus

OBJECTS = test_dbus.o handlers.o error_replies.o

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $(TARGET)

test_dbus.o: test_dbus.c test_dbus.h
	$(CC) -c $(CFLAGS) test_dbus.c -o test_dbus.o

handlers.o: handlers.c test_dbus.h
	$(CC) -c $(CFLAGS) handlers.c -o handlers.o

error_replies.o: error_replies.c test_dbus.h
	$(CC) -c $(CFLAGS) error_replies.c -o error_replies.o
