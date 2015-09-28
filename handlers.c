#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include "test_dbus.h"


static const char *_introspect_xml_main_empty = "\
<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\
<node name=\"/\">\
  <interface name=\"org.freedesktop.DBus.Introspectable\">\
    <method name=\"Introspect\">\
      <arg name=\"xml_data\" type=\"s\" direction=\"out\"/>\
    </method>\
  </interface>\
</node>";


static const char *_introspect_xml_tmpl = "\
<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\
<node name=\"/\">\
  <interface name=\"org.freedesktop.DBus.Introspectable\">\
    <method name=\"Introspect\">\
      <arg name=\"xml_data\" type=\"s\" direction=\"out\"/>\
    </method>\
  </interface>\
  <interface name=\"org.freedesktop.DBus.Properties\">\
    <method name=\"Get\">\
      <arg name=\"interface_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"property_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"value\" type=\"v\" direction=\"out\"/>\
    </method>\
    <method name=\"Set\">\
      <arg name=\"interface_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"property_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"value\" type=\"v\" direction=\"in\"/>\
    </method>\
    <method name=\"GetAll\">\
      <arg name=\"interface_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"props\" type=\"{sv}\" direction=\"out\"/>\
    </method>\
  </interface>\
  <node name=\"StatusNotifierItem\"/>\
</node>";


// DONE: all
static const char *_introspect_xml_tmpl_statusnotifier = "\
<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\
<node name=\"/\">\
  <interface name=\"org.kde.StatusNotifierItem\">\
    <property name=\"Category\" type=\"s\" access=\"read\"/>\
    <property name=\"Id\" type=\"s\" access=\"read\"/>\
    <property name=\"Title\" type=\"s\" access=\"read\"/>\
    <property name=\"Status\" type=\"s\" access=\"read\"/>\
    <property name=\"WindowId\" type=\"u\" access=\"read\"/>\
    <property name=\"IconName\" type=\"s\" access=\"read\"/>\
    <property name=\"IconPixmap\" type=\"a(iiay)\" access=\"read\"/>\
    <property name=\"OverlayIconName\" type=\"s\" access=\"read\"/>\
    <property name=\"OverlayIconPixmap\" type=\"a(iiay)\" access=\"read\"/>\
    <property name=\"AttentionIconName\" type=\"s\" access=\"read\"/>\
    <property name=\"AttentionIconPixmap\" type=\"a(iiay)\" access=\"read\"/>\
    <property name=\"AttentionMovieName\" type=\"s\" access=\"read\"/>\
    <property name=\"ToolTip\" type=\"(sa(iiay)ss)\" access=\"read\"/>\
    <method name=\"ContextMenu\">\
      <arg name=\"x\" type=\"i\" direction=\"in\"/>\
      <arg name=\"y\" type=\"i\" direction=\"in\"/>\
    </method>\
    <method name=\"Activate\">\
      <arg name=\"x\" type=\"i\" direction=\"in\"/>\
      <arg name=\"y\" type=\"i\" direction=\"in\"/>\
    </method>\
    <method name=\"SecondaryActivate\">\
      <arg name=\"x\" type=\"i\" direction=\"in\"/>\
      <arg name=\"y\" type=\"i\" direction=\"in\"/>\
    </method>\
    <method name=\"Scroll\">\
      <arg name=\"delta\" type=\"i\" direction=\"in\"/>\
      <arg name=\"orientation\" type=\"s\" direction=\"in\"/>\
    </method>\
    <signal name=\"NewTitle\"></signal>\
    <signal name=\"NewIcon\"></signal>\
    <signal name=\"NewAttentionIcon\"></signal>\
    <signal name=\"NewOverlayIcon\"></signal>\
    <signal name=\"NewToolTip\"></signal>\
    <signal name=\"NewStatus\">\
      <arg name=\"status\" type=\"s\"/>\
    </signal>\
  </interface>\
</node>";


/*
typedef struct _StatusNotifierItem_C {
    char category[64];
    char id[32];
    char title[64];
    char status[64];
    dbus_uint32_t window_id;
    char icon_name[64];
} StatusNotifierItem;

static StatusNotifierItem g_trayIcon;
*/


static int is_for_my_path( DBusMessage *message, const char *path ) {
    const char *mdest = dbus_message_get_destination(message);
    const char *mpath = dbus_message_get_path(message);
    if( (strcmp(mdest, MY_NAME_STR) == 0) && (strcmp(mpath, path) == 0) ) {
        return 1;
    }
    return 0;
}


static int is_for_main_path( DBusConnection *conn, DBusMessage *message, const char *path ) {
    const char *unique_name = dbus_bus_get_unique_name(conn);
    const char *mdest = dbus_message_get_destination(message);
    const char *mpath = dbus_message_get_path(message);
    if( (strcmp(mdest, unique_name) == 0) && (strcmp(mpath, path) == 0) ) {
        return 1;
    }
    return 0;
}


/* Message from org.freedesktop.DBus type = 4, no_reply = 1: 
  dest   = :1.482
  path   = /org/freedesktop/DBus
  iface  = org.freedesktop.DBus
  member = NameAcquired
  SIGNAL: marking as handled! */
DBusHandlerResult handle_signal(DBusConnection *connection, DBusMessage *message) {
    const char *mpath = dbus_message_get_path(message);
    const char *mdest = dbus_message_get_destination(message);
    const char *mmemb = dbus_message_get_member(message);
    const char *mintf = dbus_message_get_interface(message);
    DBusError derr;
    dbus_error_init(&derr);
    if( (strcmp(mpath, "/org/freedesktop/DBus") == 0) &&
        (strcmp(mintf, "org.freedesktop.DBus") == 0) &&
        (strcmp(mmemb, "NameAcquired") == 0) ) {
        char *v_STRING = NULL;
        dbus_message_get_args( message, &derr,
                              DBUS_TYPE_STRING, &v_STRING,
                              DBUS_TYPE_INVALID );
        printf("  SIGNAL NameAcquired(): [%s]\n", v_STRING);
        dbus_message_unref(message);
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    // enum DBusHandlerResult
    // DBUS_HANDLER_RESULT_HANDLED -Message has had its effect - no need to run more handlers. 
    // DBUS_HANDLER_RESULT_NOT_YET_HANDLED - Message has not had any effect 
    //                                    - see if other handlers want it. 
    // DBUS_HANDLER_RESULT_NEED_MEMORY - Please try again later with more memory.
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}














static void _send_introspect_reply(DBusConnection *connection, DBusMessage *message, const char *reply_string) {
    // this responds simlple string value
    dbus_bool_t retb = FALSE;
    DBusMessage *reply = dbus_message_new_method_return(message);
    if( reply ) {
        char *v_STRING = strdup(reply_string);
        retb = dbus_message_append_args(reply,
            DBUS_TYPE_STRING, &v_STRING,
            DBUS_TYPE_INVALID);
        if( retb == FALSE ) {
            printf("ERROR: _send_introspect_reply: dbus_message_append_args() failed!\n");
        }
        // printf("  sending sig [%s]\n", dbus_message_get_signature(reply) );
        retb = dbus_connection_send(connection, reply, NULL);
        if( retb == FALSE ) {
            printf("ERROR: _send_introspect_reply: could not send reply!\n");
        }
        dbus_message_unref(reply);
        free(v_STRING);
    } else {
        printf("ERROR: _send_introspect_reply: dbus_message_new_method_return() failed!\n");
    }
}


static void _send_propget_reply_s(DBusConnection *connection, DBusMessage *message, const char *reply_string) {
    // this should reply DBUS_VARIANT type with internal string value
    dbus_bool_t retb = FALSE;
    DBusMessage *reply = dbus_message_new_method_return(message);
    if( reply ) {
        char *v_STRING = strdup(reply_string);
        // we can send VARIANT only with iterator
        DBusMessageIter iter, sub_iter;
        dbus_message_iter_init_append(reply, &iter);
        dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "s", &sub_iter);
        dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_STRING, &v_STRING);
        dbus_message_iter_close_container(&iter, &sub_iter);
        //
        printf("  sending sig [%s]\n", dbus_message_get_signature(reply) );
        retb = dbus_connection_send(connection, reply, NULL);
        if( retb == FALSE ) {
            printf("ERROR: _send_propget_reply: could not send reply!\n");
        }
        dbus_message_unref(reply);
        free(v_STRING);
    } else {
        printf("ERROR: _send_propget_reply: dbus_message_new_method_return() failed!\n");
    }
}


static void _send_propget_reply_u(DBusConnection *connection, DBusMessage *message, dbus_uint32_t value) {
    // this should reply DBUS_VARIANT type with internal int value
    dbus_bool_t retb = FALSE;
    DBusMessage *reply = dbus_message_new_method_return(message);
    if( reply ) {
        // we can send VARIANT only with iterator
        DBusMessageIter iter, sub_iter;
        dbus_message_iter_init_append(reply, &iter);
        dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "u", &sub_iter);
        dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_UINT32, &value);
        dbus_message_iter_close_container(&iter, &sub_iter);
        //
        printf("  sending sig [%s]\n", dbus_message_get_signature(reply) );
        retb = dbus_connection_send(connection, reply, NULL);
        if( retb == FALSE ) {
            printf("ERROR: _send_propget_reply: could not send reply!\n");
        }
        dbus_message_unref(reply);
    } else {
        printf("ERROR: _send_propget_reply: dbus_message_new_method_return() failed!\n");
    }
}


static void _send_propget_reply_empty_pixmap(DBusConnection *connection, DBusMessage *message) {
    dbus_bool_t retb = FALSE;
    DBusMessage *reply = dbus_message_new_method_return(message);
    if( reply ) {
        // variant, containing array: a(iiay)
        // array of structures: (int, int, byte array)
        // we should output just 0-length array, for now
        // qdbusviewer report following reply for exmpty pixmaps:
        //   Arguments: [Variant: [Argument: a(iiay) {}]]
        DBusMessageIter iter, v_iter, a_iter;
        dbus_message_iter_init_append(reply, &iter);
        dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "a(iiay)", &v_iter);
        dbus_message_iter_open_container(&v_iter, DBUS_TYPE_ARRAY, "(iiay)", &a_iter);
        // no contents in array
        dbus_message_iter_close_container(&v_iter, &a_iter);
        dbus_message_iter_close_container(&iter, &v_iter);
        //
        printf("  sending sig [%s]\n", dbus_message_get_signature(reply) );
        retb = dbus_connection_send(connection, reply, NULL);
        if( retb == FALSE ) {
            printf("ERROR: _send_propget_reply_empty_pixmap: could not send reply!\n");
        }
        dbus_message_unref(reply);
    } else {
        printf("ERROR: _send_propget_reply_empty_pixmap: dbus_message_new_method_return() failed!\n");
    }
}


static void _send_propget_reply_empty_tooltip(DBusConnection *connection, DBusMessage *message,
                                              const char *icon_name,
                                              const char *tip_title,
                                              const char *tip_text
                                             ) {
    // (sa(iiay)ss)
    // Components are:
    // STRING: Freedesktop-compliant name for an icon.
    // ARRAY(INT, INT, ARRAY BYTE): icon data
    // STRING: title for this tooltip
    // STRING: descriptive text for this tooltip.
    dbus_bool_t retb = FALSE;
    DBusMessage *reply = dbus_message_new_method_return(message);
    if( reply ) {
        DBusMessageIter iter, v_iter, st_iter, arr_iter;
        dbus_message_iter_init_append(reply, &iter);
        dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "(sa(iiay)ss)", &v_iter); // open variant
        dbus_message_iter_open_container(&v_iter, DBUS_TYPE_STRUCT, NULL, &st_iter); // open struct
        dbus_message_iter_append_basic(&st_iter, DBUS_TYPE_STRING, &icon_name);
        dbus_message_iter_open_container(&st_iter, DBUS_TYPE_ARRAY, "(iiay)", &arr_iter); // open array
        // no contents in icon array
        dbus_message_iter_close_container(&st_iter, &arr_iter); // close array
        dbus_message_iter_append_basic(&st_iter, DBUS_TYPE_STRING, &tip_title);
        dbus_message_iter_append_basic(&st_iter, DBUS_TYPE_STRING, &tip_text);
        dbus_message_iter_close_container(&v_iter, &st_iter); // close struct
        dbus_message_iter_close_container(&iter, &v_iter); // close variant
        //
        printf("  sending sig %s\n", dbus_message_get_signature(reply) );
        retb = dbus_connection_send(connection, reply, NULL);
        if( retb == FALSE ) {
            printf("ERROR: _send_propget_reply_empty_pixmap: could not send reply!\n");
        }
        dbus_message_unref(reply);
    } else {
        printf("ERROR: _send_propget_reply_empty_pixmap: dbus_message_new_method_return() failed!\n");
    }
}



/* Typical reply looks like:
 [Argument: a{sv} {"AttentionIconName" = [Variant(QString): "konv_message"], "AttentionIconPixmap" = [Variant: [Argument: a(iiay) {}]], "AttentionMovieName" = [Variant(QString): ""], "Category" = [Variant(QString): "Communications"], "IconName" = [Variant(QString): "konversation"], "IconPixmap" = [Variant: [Argument: a(iiay) {}]], "IconThemePath" = [Variant(QString): ""], "Id" = [Variant(QString): "Konversation"], "ItemIsMenu" = [Variant(bool): false], "Menu" = [Variant: [ObjectPath: /MenuBar]], "OverlayIconName" = [Variant(QString): ""], "OverlayIconPixmap" = [Variant: [Argument: a(iiay) {}]], "Status" = [Variant(QString): "Passive"], "Title" = [Variant(QString): "Konversation"], "ToolTip" = [Variant: [Argument: (sa(iiay)ss) "konversation", [Argument: a(iiay) {}], "Konversation", "Konversation - IRC Client"]], "WindowId" = [Variant(int): 67108892]}] */
static void _send_reply_properties_getall_append_attr_s(DBusMessageIter *piter, const char *name, const char *value) {
    DBusMessageIter dict_iter, var_iter;
    dbus_message_iter_open_container(piter, DBUS_TYPE_DICT_ENTRY, NULL, &dict_iter); // open dict
    dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &name); // write attr name as dict key
    dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_VARIANT, "s", &var_iter); // open variant
    dbus_message_iter_append_basic(&var_iter, DBUS_TYPE_STRING, &value); // write attr value as Variant(string)
    dbus_message_iter_close_container(&dict_iter, &var_iter); // close variant
    dbus_message_iter_close_container(piter, &dict_iter); // close dict
}
static void _send_reply_properties_getall_append_attr_u(DBusMessageIter *piter, const char *name, dbus_uint32_t value) {
    DBusMessageIter dict_iter, var_iter;
    dbus_message_iter_open_container(piter, DBUS_TYPE_DICT_ENTRY, NULL, &dict_iter); // open dict
    dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &name); // write attr name as dict key
    dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_VARIANT, "u", &var_iter); // open variant
    dbus_message_iter_append_basic(&var_iter, DBUS_TYPE_UINT32, &value); // write attr value as Variant(uint32)
    dbus_message_iter_close_container(&dict_iter, &var_iter); // close variant
    dbus_message_iter_close_container(piter, &dict_iter); // close dict
}
static void _send_reply_properties_getall_append_attr_pixmap(DBusMessageIter *piter, const char *name) {
    DBusMessageIter dict_iter, var_iter, arr_iter;
    dbus_message_iter_open_container(piter, DBUS_TYPE_DICT_ENTRY, NULL, &dict_iter); // open dict
    dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &name); // write attr name as dict key
    dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_VARIANT, "a(iiay)", &var_iter); // open variant
    // key value - empty pixmap array inside Variant
    // "AttentionIconPixmap" = [Variant: [Argument: a(iiay) {}]]
    dbus_message_iter_open_container(&var_iter, DBUS_TYPE_ARRAY, "(iiay)", &arr_iter);
    dbus_message_iter_close_container(&var_iter, &arr_iter);
    //
    dbus_message_iter_close_container(&dict_iter, &var_iter); // close variant
    dbus_message_iter_close_container(piter, &dict_iter); // close dict
}
static void _send_reply_properties_getall_append_attr_tooltip(DBusMessageIter *piter,
                                                              const char *name,
                                                              const char *icon_name,
                                                              const char *tip_title,
                                                              const char *tip_text
                                                             ) {
    DBusMessageIter dict_iter, var_iter, st_iter, arr_iter;
    dbus_message_iter_open_container(piter, DBUS_TYPE_DICT_ENTRY, NULL, &dict_iter); // open dict
    dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &name); // write attr name as dict key
    dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_VARIANT, "(sa(iiay)ss)", &var_iter); // open variant
    // key value - struct (sa(iiay)ss)
    dbus_message_iter_open_container(&var_iter, DBUS_TYPE_STRUCT, NULL, &st_iter); // open struct
    dbus_message_iter_append_basic(&st_iter, DBUS_TYPE_STRING, &icon_name);
    dbus_message_iter_open_container(&st_iter, DBUS_TYPE_ARRAY, "(iiay)", &arr_iter); // open array
    // no contents in icon array
    dbus_message_iter_close_container(&st_iter, &arr_iter); // close array
    dbus_message_iter_append_basic(&st_iter, DBUS_TYPE_STRING, &tip_title);
    dbus_message_iter_append_basic(&st_iter, DBUS_TYPE_STRING, &tip_text);
    dbus_message_iter_close_container(&var_iter, &st_iter); // close struct
    //
    dbus_message_iter_close_container(&dict_iter, &var_iter); // close variant
    dbus_message_iter_close_container(piter, &dict_iter); // close dict
}

static void _send_reply_properties_getall(DBusConnection *connection, DBusMessage *message) {
    dbus_bool_t retb = FALSE;
    DBusMessage *reply = dbus_message_new_method_return(message);
    if( !reply ) return;
    //
    DBusMessageIter iter, arr_iter;
    dbus_message_iter_init_append(reply, &iter);
    retb = dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sv}", &arr_iter); // open array
    // if( retb == FALSE ) { printf("ERROR: cannot open container for array!\n"); }
    // else { printf("  OK: opened container for array\n"); } // all ok till now
    _send_reply_properties_getall_append_attr_s(&arr_iter, "Category", "ApplicationStatus");
    _send_reply_properties_getall_append_attr_s(&arr_iter, "Id", "1");
    _send_reply_properties_getall_append_attr_s(&arr_iter, "Title", "test_dbus");
    _send_reply_properties_getall_append_attr_s(&arr_iter, "Status", "Active");
    _send_reply_properties_getall_append_attr_u(&arr_iter, "WindowId", 0);
    // ^^ KDE sets this as int32, not uint32... what?
    _send_reply_properties_getall_append_attr_s(&arr_iter, "IconName", "applications-development");
    _send_reply_properties_getall_append_attr_s(&arr_iter, "OverlayIconName", "");
    _send_reply_properties_getall_append_attr_s(&arr_iter, "AttentionIconName", "");
    _send_reply_properties_getall_append_attr_s(&arr_iter, "AttentionMovieName", "");
    _send_reply_properties_getall_append_attr_pixmap(&arr_iter, "IconPixmap");
    _send_reply_properties_getall_append_attr_pixmap(&arr_iter, "OverlayIconPixmap");
    _send_reply_properties_getall_append_attr_pixmap(&arr_iter, "AttentionIconPixmap");
    _send_reply_properties_getall_append_attr_tooltip(&arr_iter,
                                                      "ToolTip", // attribute name
                                                      "applications-development", // icon name
                                                      "Test DBus",  // tooltip title
                                                      "sample text to test tray icon" // tooltip text
                                                     );
    dbus_message_iter_close_container(&iter, &arr_iter); // close array
    //
    printf("  sending sig %s\n", dbus_message_get_signature(reply) );
    retb = dbus_connection_send(connection, reply, NULL);
    if( retb == FALSE ) {
        printf("ERROR: _send_reply_properties_getall: could not send reply!\n");
    }
    dbus_message_unref(reply);
}












DBusHandlerResult handle_introspect(DBusConnection *connection, DBusMessage *message) {
    const char *mpath = dbus_message_get_path(message);
    const char *mdest = dbus_message_get_destination(message);
    const char *msig = dbus_message_get_signature(message);
    printf("  Introspect() for dest \"%s\", path \"%s\", signature [%s]\n", mdest, mpath, msig);
    if( is_for_my_path(message, "/") ) {
        _send_introspect_reply( connection, message, _introspect_xml_tmpl );
        dbus_message_unref(message);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if( is_for_my_path(message, "/StatusNotifierItem") ) {
        _send_introspect_reply( connection, message, _introspect_xml_tmpl_statusnotifier );
        dbus_message_unref(message);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if( is_for_main_path(connection, message, "/" ) ) {
        _send_introspect_reply( connection, message, _introspect_xml_main_empty );
        dbus_message_unref(message);
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    // enum DBusHandlerResult
    // DBUS_HANDLER_RESULT_HANDLED -Message has had its effect - no need to run more handlers. 
    // DBUS_HANDLER_RESULT_NOT_YET_HANDLED - Message has not had any effect 
    //                                    - see if other handlers want it. 
    // DBUS_HANDLER_RESULT_NEED_MEMORY - Please try again later with more memory.
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


DBusHandlerResult handle_property_get(DBusConnection *connection, DBusMessage *message) {
    DBusError derr;
    const char *mpath = dbus_message_get_path(message);
    const char *mdest = dbus_message_get_destination(message);
    const char *msig = dbus_message_get_signature(message);
    dbus_error_init(&derr);
    printf("  Properties.Get( \"%s\", \"%s\" ) signature [%s]\n", mdest, mpath, msig);
    // handle_property_get() for [ru.minlexx.TestDbus, /StatusNotifierItem], signature [ss]
    if( is_for_my_path(message, "/StatusNotifierItem") ) {
        // get message arguments
        //DBusMessageIter miter;
        //dbus_message_iter_init(message, &miter);
        //while( (current_type = dbus_message_iter_get_arg_type(&miter)) != DBUS_TYPE_INVALID ) {
        //	dbus_message_iter_next(&iter);
        //}
        if( strcmp(msig, "ss") == 0 ) {
            char *v_STRING1 = NULL;
            char *v_STRING2 = NULL;
            dbus_bool_t resb = dbus_message_get_args(message, &derr,
                DBUS_TYPE_STRING, &v_STRING1,
                DBUS_TYPE_STRING, &v_STRING2,
                DBUS_TYPE_INVALID );
            if( resb == FALSE ) {
                printf("ERROR: Failed to get message args!\n");
            } else {
                printf("  call args: [%s] [%s]\n", v_STRING1, v_STRING2);
                // Call args: [org.freedesktop.StatusNotifierItem] [Category]
                if(strcmp(v_STRING2, "Category") == 0) {
                    _send_propget_reply_s(connection, message, "ApplicationStatus");
                    // ApplicationStatus, Communications, SystemServices, Hardware
                } else if(strcmp(v_STRING2, "Id") == 0) {
                    _send_propget_reply_s(connection, message, "1");
                } else if(strcmp(v_STRING2, "Title") == 0) {
                    _send_propget_reply_s(connection, message, "test_dbus");
                } else if(strcmp(v_STRING2, "Status") == 0) {
                    _send_propget_reply_s(connection, message, "Active");
                    // Active, Passive, NeedsAttention
                } else if(strcmp(v_STRING2, "WindowId") == 0) {
                    _send_propget_reply_u(connection, message, 0); // 0 - not interested
                } else if(strcmp(v_STRING2, "IconName") == 0) {
                    _send_propget_reply_s(connection, message, "applications-development"); // std icon name
                } else if(strcmp(v_STRING2, "OverlayIconName") == 0) {
                    _send_propget_reply_s(connection, message, "");
                } else if(strcmp(v_STRING2, "AttentionIconName") == 0) {
                    _send_propget_reply_s(connection, message, "");
                } else if(strcmp(v_STRING2, "AttentionMovieName") == 0) {
                    _send_propget_reply_s(connection, message, "");
                } else if(strcmp(v_STRING2, "IconPixmap") == 0) {
                    _send_propget_reply_empty_pixmap(connection, message);
                } else if(strcmp(v_STRING2, "OverlayIconPixmap") == 0) {
                    _send_propget_reply_empty_pixmap(connection, message);
                } else if(strcmp(v_STRING2, "AttentionIconPixmap") == 0) {
                    _send_propget_reply_empty_pixmap(connection, message);
                } else if(strcmp(v_STRING2, "ToolTip") == 0) {
                    _send_propget_reply_empty_tooltip(connection, message,
                                                      "applications-development",
                                                      "Tooltip title",
                                                      "sample text");
                } else {
                    printf("ERROR: unknown property requested: [%s]\n", v_STRING2);
                    error_reply_unknown_property(connection, message, v_STRING2);
                }
                dbus_message_unref(message);
                return DBUS_HANDLER_RESULT_HANDLED;
            }
        } else {
            printf("ERROR: incorrect signature: [%s]\n", msig);
        }
    }
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


DBusHandlerResult handle_property_set(DBusConnection *connection, DBusMessage *message) {
    // for now, all properties are read-only
    const char *msig = dbus_message_get_signature(message);
    DBusError derr;
    dbus_error_init(&derr);
    if( strcmp(msig, "ssv") == 0 ) {
        char *v_IFACE_NAME = NULL;
        char *v_PROP_NAME = NULL;
        dbus_bool_t resb = dbus_message_get_args(message, &derr,
            DBUS_TYPE_STRING, &v_IFACE_NAME,
            DBUS_TYPE_STRING, &v_PROP_NAME,
            DBUS_TYPE_INVALID );
        if( resb == FALSE ) {
            printf("ERROR: handle_property_set(): Failed to get message args!\n");
            error_reply_read_only_prop(connection, message, "whatever");
        } else {
            printf("  Property Set \"%s\": replying is read-only\n", v_PROP_NAME);
            error_reply_read_only_prop(connection, message, v_PROP_NAME);
        }
    } else {
        printf("ERROR: handle_property_set(): incorrect signature: [%s]\n", msig);
        error_reply_read_only_prop(connection, message, "whatever");
    }
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_HANDLED;
}


//org.freedesktop.DBus.Properties.GetAll (in STRING interface_name, out DICT<STRING,VARIANT> props);
DBusHandlerResult handle_property_get_all(DBusConnection *connection, DBusMessage *message) {
    const char *msig = dbus_message_get_signature(message);
    DBusError derr;
    dbus_error_init(&derr);
    if( strcmp(msig, "s") == 0 ) {
        char *v_IFACE_NAME = NULL;
        dbus_bool_t resb = dbus_message_get_args(message, &derr,
            DBUS_TYPE_STRING, &v_IFACE_NAME,
            DBUS_TYPE_INVALID );
        if( resb == FALSE ) {
            printf("ERROR: Properties.GetAll(): Failed to get message args!\n");
            error_reply_unknown_property(connection, message, "whatever");
        } else {
            printf("  Properties.GetAll( \"%s\" ): \n", v_IFACE_NAME);
            if( strcmp(v_IFACE_NAME, ORG_KDE_STATUSNOTIFIER_INTERFACE) == 0 ) {
                // okay, respond with all our "properties"
                printf("  OK, replying with all our properties\n");
                _send_reply_properties_getall(connection, message);
            } else {
                printf( "  ERROR: unknown interface requested: [%s]\n", v_IFACE_NAME );
                error_reply_unknown_interface(connection, message, v_IFACE_NAME);
            }
        }
    } else {
        printf("ERROR: Properties.GetAll(): incorrect message signature!\n");
    }
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_HANDLED;
}






// VOID org.freedesktop.StatusNotifierItem.Activate (INT x, INT y);
DBusHandlerResult handle_icon_activate(DBusConnection *connection, DBusMessage *message) {
    const char *msig = NULL;
    DBusError derr;
    dbus_error_init(&derr);
    msig = dbus_message_get_signature(message);
    if( strcmp(msig, "ii") == 0 ) {
        dbus_int32_t x = 0;
        dbus_int32_t y = 0;
        dbus_bool_t resb = dbus_message_get_args(message, &derr,
            DBUS_TYPE_INT32, &x,
            DBUS_TYPE_INT32, &y,
            DBUS_TYPE_INVALID );
        if( resb == FALSE ) {
            printf("ERROR: StatusNotifierItem.Activate(): Failed to get message args!\n");
            error_reply_invalid_args(connection, message);
        } else {
            printf("  StatusNotifierItem.Activate( %d, %d ): \n", x, y);
            // does not need reply
        }
    } else {
        printf("ERROR: StatusNotifierItem.Activate(): incorrect message signature!\n");
    }
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_HANDLED;
}


// VOID org.freedesktop.StatusNotifierItem.SecondaryActivate (INT x, INT y);
DBusHandlerResult handle_icon_activate_secondry(DBusConnection *connection, DBusMessage *message) {
    const char *msig = NULL;
    DBusError derr;
    dbus_error_init(&derr);
    msig = dbus_message_get_signature(message);
    if( strcmp(msig, "ii") == 0 ) {
        dbus_int32_t x = 0;
        dbus_int32_t y = 0;
        dbus_bool_t resb = dbus_message_get_args(message, &derr,
            DBUS_TYPE_INT32, &x,
            DBUS_TYPE_INT32, &y,
            DBUS_TYPE_INVALID );
        if( resb == FALSE ) {
            printf("ERROR: StatusNotifierItem.SecondaryActivate(): Failed to get message args!\n");
            error_reply_invalid_args(connection, message);
        } else {
            printf("  StatusNotifierItem.SecondaryActivate( %d, %d ): \n", x, y);
            // does not need reply
        }
    } else {
        printf("ERROR: StatusNotifierItem.SecondaryActivate(): incorrect message signature!\n");
    }
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_HANDLED;
}


// VOID org.freedesktop.StatusNotifierItem.ContextMenu (INT x, INT y);
DBusHandlerResult handle_icon_context_menu(DBusConnection *connection, DBusMessage *message) {
    const char *msig = NULL;
    DBusError derr;
    dbus_error_init(&derr);
    msig = dbus_message_get_signature(message);
    if( strcmp(msig, "ii") == 0 ) {
        dbus_int32_t x = 0;
        dbus_int32_t y = 0;
        dbus_bool_t resb = dbus_message_get_args(message, &derr,
            DBUS_TYPE_INT32, &x,
            DBUS_TYPE_INT32, &y,
            DBUS_TYPE_INVALID );
        if( resb == FALSE ) {
            printf("ERROR: StatusNotifierItem.ContextMenu(): Failed to get message args!\n");
            error_reply_invalid_args(connection, message);
        } else {
            printf("  StatusNotifierItem.ContextMenu( %d, %d ): \n", x, y);
            // does not need reply
        }
    } else {
        printf("ERROR: StatusNotifierItem.ContextMenu(): incorrect message signature!\n");
    }
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_HANDLED;
}


// VOID org.freedesktop.StatusNotifierItem.Scroll (INT delta, STRING orientation);
DBusHandlerResult handle_icon_scroll(DBusConnection *connection, DBusMessage *message) {
    const char *msig = NULL;
    DBusError derr;
    dbus_error_init(&derr);
    msig = dbus_message_get_signature(message);
    if( strcmp(msig, "is") == 0 ) {
        dbus_int32_t delta = 0;
        char *orientation = NULL;
        dbus_bool_t resb = dbus_message_get_args(message, &derr,
            DBUS_TYPE_INT32, &delta,
            DBUS_TYPE_STRING, &orientation,
            DBUS_TYPE_INVALID );
        if( resb == FALSE ) {
            printf("ERROR: StatusNotifierItem.Scroll(): Failed to get message args!\n");
            error_reply_invalid_args(connection, message);
        } else {
            printf("  StatusNotifierItem.Scroll( %d, \"%s\" ): \n", delta, orientation);
            // does not need reply
        }
    } else {
        printf("ERROR: StatusNotifierItem.Scroll(): incorrect message signature!\n");
    }
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_HANDLED;
}
