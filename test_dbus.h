#ifndef H_TESTDBUS
#define H_TESTDBUS


#define MY_NAME_STR "ru.minlexx.TestDbus"

DBusHandlerResult handle_signal(DBusConnection *connection, DBusMessage *message);

DBusHandlerResult handle_introspect(DBusConnection *connection, DBusMessage *message);

DBusHandlerResult handle_property_get(DBusConnection *connection, DBusMessage *message);
DBusHandlerResult handle_property_set(DBusConnection *connection, DBusMessage *message);
DBusHandlerResult handle_property_get_all(DBusConnection *connection, DBusMessage *message);


void error_reply_unknown_property(DBusConnection *connection, DBusMessage *message, const char *prop_name);
void error_reply_unknown_interface(DBusConnection *connection, DBusMessage *message, const char *iface);

#endif
