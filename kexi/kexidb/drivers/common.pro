CONFIG += kde3lib

TEMPLATE	= lib
include( $(KEXI)/common.pro )

win32:LIBS += \
$$QKWLIB/kexicore$$KEXILIB_SUFFIX \
$$QKWLIB/kexidb$$KEXILIB_SUFFIX

#$$QKWLIB/kexidatatable$$KEXILIB_SUFFIX \

