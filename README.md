# test_dbus
Just me messing with raw libdbus API.
Want some time to get working StatusNotifier tray icon example, with *only* dbus dependency

Well, it's complete now. It is a working exapmle except some features:
 * can use only predefined well-known icon names, not any pixmaps. See http://standards.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 * does not emit signals about changed text/icon/tooltip

Signals emitting is trivial, but sending raw pixmap data is a bit more complex operation :)
But I will test it some day.
