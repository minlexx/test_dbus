#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <dbus/dbus.h>

#include "test_dbus.h"


void print_error_and_free(DBusError *perr) {
    printf("error name: %s\n", perr->name);
    printf("error message: %s\n", perr->message);
    dbus_error_free(perr);
}


void test_getpid() {
    int pid = getpid();
    printf("our pid= %d\n", pid);
}


// Function to unregister this handler.  (???)
void ObjectPathUnregisterFunction(DBusConnection *connection, void *user_data) {
    printf("ObjectPathUnregisterFunction() called\n");
}


// Function to handle messages.
DBusHandlerResult ObjectPathMessageFunction(
            DBusConnection *connection,
            DBusMessage *message,
            void *user_data ) {
    const char *mpath = dbus_message_get_path(message);
    const char *mintf = dbus_message_get_interface(message);
    const char *mmemb = dbus_message_get_member(message);
    const char *mdest = dbus_message_get_destination(message);
    const char *sender = dbus_message_get_sender(message);
    int mtype = dbus_message_get_type(message);
    int no_reply = dbus_message_get_no_reply(message);
    printf("Message from %s type = %d, no_reply = %d: \n", sender, mtype, no_reply);
    printf("  dest   = %s\n", mdest);
    printf("  path   = %s\n", mpath);
    printf("  iface  = %s\n", mintf);
    printf("  member = %s\n", mmemb);
    if( dbus_message_is_method_call(message, "org.freedesktop.DBus.Introspectable", "Introspect") ) {
        return handle_introspect(connection, message);
    } else if( dbus_message_is_method_call(message, "org.freedesktop.DBus.Properties", "Get") ) {
        return handle_property_get(connection, message);
    } else if( dbus_message_is_method_call(message, "org.freedesktop.DBus.Properties", "Set") ) {
        return handle_property_set(connection, message);
    } else if( dbus_message_is_method_call(message, "org.freedesktop.DBus.Properties", "GetAll") ) {
        return handle_property_get_all(connection, message);
    } else if( dbus_message_is_method_call(message, "org.kde.StatusNotifierItem", "Activate") ) {
        return handle_icon_activate(connection, message);
    } else if( dbus_message_is_method_call(message, "org.kde.StatusNotifierItem", "SecondaryActivate") ) {
        return handle_icon_activate_secondry(connection, message);
    } else if( dbus_message_is_method_call(message, "org.kde.StatusNotifierItem", "ContextMenu") ) {
        return handle_icon_context_menu(connection, message);
    } else if( dbus_message_is_method_call(message, "org.kde.StatusNotifierItem", "Scroll") ) {
        return handle_icon_scroll(connection, message);
    }
    
    // handle signals
    if( mtype == DBUS_MESSAGE_TYPE_SIGNAL ) {
        return handle_signal(connection, message);
    }
    
    if( no_reply == 0 ) {
        printf("  WARNING: unhandled message waiting for reply!\n");
    }
    
    // enum DBusHandlerResult
    // DBUS_HANDLER_RESULT_HANDLED -Message has had its effect - no need to run more handlers. 
    // DBUS_HANDLER_RESULT_NOT_YET_HANDLED - Message has not had any effect 
    //                                    - see if other handlers want it. 
    // DBUS_HANDLER_RESULT_NEED_MEMORY - Please try again later with more memory.
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


/* ObjectPathMessageFunction() called, message = 0x1615610
 message interface = org.freedesktop.DBus.Introspectable
 message path = /
 message member = Introspect
 message dest = ru.minlexx.TestDbus
 message sender = :1.363  */

/*
Message from :1.363: 
  dest   = ru.minlexx.TestDbus
  path   = /StatusNotifierItem
  iface  = org.freedesktop.DBus.Properties
  member = Get
*/


int main(int argc, char **argv) {
    DBusConnection *pconn = NULL;
    DBusError derr;
    DBusMessage *msg_in = NULL;
    DBusObjectPathVTable root_vtable;
    int res = 0;
    dbus_bool_t resb = FALSE;
    int loop_timeout = 5000;
    int loops_max = 100;
    int iloop = 0;
    char *v_STRING = NULL;

    test_getpid();

    dbus_error_init(&derr);
    printf("Linked with DBus version: %s\n", DBUS_VERSION_STRING);

    // also has DBUS_BUS_SYSTEM
    pconn = dbus_bus_get(DBUS_BUS_SESSION, &derr);
    printf("pconn = %p\n", pconn);

    if( !pconn ) {
        fprintf(stderr, "Failed to get DBUS session bus!\n");
        print_error_and_free(&derr);
        return EXIT_FAILURE;
    }
    
    memset(&root_vtable, 0, sizeof(root_vtable));
    root_vtable.unregister_function = ObjectPathUnregisterFunction;
    root_vtable.message_function = ObjectPathMessageFunction;
    //resb = dbus_connection_try_register_object_path(pconn, "/", &root_vtable, NULL, &derr);
    resb = dbus_connection_try_register_fallback( pconn, "/", &root_vtable, NULL, &derr);
    if (resb == FALSE) {
        printf("Failed to register fallback handler!\n");
        print_error_and_free(&derr);
    }
    printf("Root path fallback handler registered OK!\n");
    dbus_connection_read_write_dispatch(pconn, 1000);

    printf("Connected to session bus as unique name: [%s]\n", dbus_bus_get_unique_name(pconn));
    dbus_connection_set_exit_on_disconnect(pconn, FALSE);
    dbus_connection_read_write_dispatch(pconn, 1000);

    res = dbus_bus_request_name(pconn, MY_NAME_STR, DBUS_NAME_FLAG_DO_NOT_QUEUE, &derr);
    if( res != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER ) {
        printf("Request name failed! Not primary owner! %d\n", res);
        print_error_and_free(&derr);
    } else {
        printf("Request name [%s] OK!\n", MY_NAME_STR);
    }
    dbus_connection_read_write_dispatch(pconn, 1000);
    
    // try to communicate with StatusNotifierWatcher
    DBusMessage *req = dbus_message_new_method_call(
        "org.kde.StatusNotifierWatcher", // dest
        "/StatusNotifierWatcher", // path
        "org.kde.StatusNotifierWatcher", // iface
        "RegisterStatusNotifierItem" // method
    );
    v_STRING = "ru.minlexx.TestDbus";
    dbus_message_append_args(req,
                             DBUS_TYPE_STRING, &v_STRING,
                             DBUS_TYPE_INVALID);
    dbus_connection_send(pconn, req, NULL );
    printf("Request to register tray icon was sent\n");


    printf("Falling into loop\n");
    for(iloop = 0; iloop < loops_max; iloop++ ) {
        res = dbus_connection_read_write_dispatch(pconn, loop_timeout);
        printf(" ...  loop %d res = %d\n", iloop, res);
        //DBusDispatchStatus dispatch_status = dbus_connection_get_dispatch_status(pconn);
        //printf("      dispatch status: %d\n", (int)dispatch_status);
    }
    
    printf("Exited loop on my own? iloops = %d\n", iloop);

    dbus_bus_release_name(pconn, MY_NAME_STR, &derr);
    dbus_connection_unref(pconn);
    printf("Unreferenced pconn\n");

    return EXIT_SUCCESS;
}

