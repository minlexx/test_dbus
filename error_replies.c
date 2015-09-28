#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include "test_dbus.h"


void error_reply_unknown_property(DBusConnection *connection, DBusMessage *message, const char *prop_name) {
    // other interesting
    // DBUS_ERROR_ACCESS_DENIED
    // DBUS_ERROR_FILE_NOT_FOUND
    // DBUS_ERROR_INVALID_ARGS
    // DBUS_ERROR_INVALID_SIGNATURE
    // DBUS_ERROR_IO_ERROR
    // DBUS_ERROR_NO_MEMORY
    // DBUS_ERROR_NOT_SUPPORTED
    // DBUS_ERROR_PROPERTY_READ_ONLY
    // DBUS_ERROR_UNKNOWN_INTERFACE
    // DBUS_ERROR_FAILED (generic)
    // DBUS_ERROR_UNKNOWN_PROPERTY
    char error_message[256] = {0};
    snprintf(error_message, sizeof(error_message)-1, "Unknown property: [%s]", prop_name);
    DBusMessage *reply = dbus_message_new_error(message, DBUS_ERROR_UNKNOWN_PROPERTY, error_message);
    if( !reply ) return;
    dbus_bool_t ret = dbus_connection_send(connection, reply, NULL);
    if( ret == FALSE ) {
        printf("ERROR: error_reply_not_found: could not send reply!\n");
    }
    dbus_message_unref(reply);
}


void error_reply_unknown_interface(DBusConnection *connection, DBusMessage *message, const char *iface) {
    char error_message[256] = {0};
    snprintf(error_message, sizeof(error_message)-1, "Unknown interface: [%s]", iface);
    DBusMessage *reply = dbus_message_new_error(message, DBUS_ERROR_UNKNOWN_INTERFACE, error_message);
    if( !reply ) return;
    dbus_bool_t ret = dbus_connection_send(connection, reply, NULL);
    if( ret == FALSE ) {
        printf("ERROR: error_reply_unknown_interface: could not send reply!\n");
    }
    dbus_message_unref(reply);
}


void error_reply_invalid_args(DBusConnection *connection, DBusMessage *message) {
    DBusMessage *reply = dbus_message_new_error(message, DBUS_ERROR_INVALID_ARGS, "Invalid arguments!");
    if( !reply ) return;
    dbus_bool_t ret = dbus_connection_send(connection, reply, NULL);
    if( ret == FALSE ) {
        printf("ERROR: error_reply_unknown_interface: could not send reply!\n");
    }
    dbus_message_unref(reply);
}


void error_reply_read_only_prop(DBusConnection *connection, DBusMessage *message, const char *prop_name) {
    char error_message[256] = {0};
    snprintf(error_message, sizeof(error_message)-1, "property: %s", prop_name);
    DBusMessage *reply = dbus_message_new_error(message, DBUS_ERROR_PROPERTY_READ_ONLY, error_message);
    if( !reply ) return;
    dbus_bool_t ret = dbus_connection_send(connection, reply, NULL);
    if( ret == FALSE ) {
        printf("ERROR: error_reply_unknown_interface: could not send reply!\n");
    }
    dbus_message_unref(reply);
}
