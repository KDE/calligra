CONFIG += kde3lib

TEMPLATE	= lib
include( $(KEXI)/kexidb/common.pro )

win32:LIBS += \
$$KDELIBDESTDIR/kexidb$$KEXILIB_SUFFIX

#$$KDELIBDESTDIR/kexicore$$KEXILIB_SUFFIX \
#$$KDELIBDESTDIR/kexidatatable$$KEXILIB_SUFFIX \

