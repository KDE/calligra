This library depends on libgsf >= 1.7.2, but it doesn't need the GNOME part
of it, so it's probably a good idea to configure libgsf with the
--without-gnome option (then you don't have to install bonobo et al.)
For newer libgsf versions this is detected automatically, so you don't have
to specify that anymore.

You can download libgsf at: ftp://ftp.gnome.org/pub/GNOME/sources/libgsf

libgsf depends on glib2 (www.gtk.org) and libxml2 (www.xmlsoft.org), in case
you use the --without-gnome option.

Additionally you need pkg-config (www.freedesktop.org/software/pkgconfig/)

In case you want to package wv2, please use the --disable-debug flag to get
rid of the debug output.
