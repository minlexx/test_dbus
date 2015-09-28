#ifndef H_TESTDBUS
#define H_TESTDBUS


#define MY_NAME_STR "ru.minlexx.TestDbus"
#define ORG_KDE_STATUSNOTIFIER_INTERFACE "org.kde.StatusNotifierItem"


DBusHandlerResult handle_signal(DBusConnection *connection, DBusMessage *message);


DBusHandlerResult handle_introspect(DBusConnection *connection, DBusMessage *message);


DBusHandlerResult handle_property_get(DBusConnection *connection, DBusMessage *message);
DBusHandlerResult handle_property_set(DBusConnection *connection, DBusMessage *message);
DBusHandlerResult handle_property_get_all(DBusConnection *connection, DBusMessage *message);


DBusHandlerResult handle_icon_activate(DBusConnection *connection, DBusMessage *message);
DBusHandlerResult handle_icon_activate_secondry(DBusConnection *connection, DBusMessage *message);
DBusHandlerResult handle_icon_context_menu(DBusConnection *connection, DBusMessage *message);
DBusHandlerResult handle_icon_scroll(DBusConnection *connection, DBusMessage *message);


void error_reply_unknown_property(DBusConnection *connection, DBusMessage *message, const char *prop_name);
void error_reply_unknown_interface(DBusConnection *connection, DBusMessage *message, const char *iface);
void error_reply_read_only_prop(DBusConnection *connection, DBusMessage *message, const char *prop_name);
void error_reply_invalid_args(DBusConnection *connection, DBusMessage *message);

#endif
