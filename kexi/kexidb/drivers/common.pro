CONFIG += kde3lib

TEMPLATE	= lib
include( $(KEXI)/kexidb/common.pro )

win32:LIBS += \
$$QKWLIB/kexidb$$KEXILIB_SUFFIX

#$$QKWLIB/kexicore$$KEXILIB_SUFFIX \
#$$QKWLIB/kexidatatable$$KEXILIB_SUFFIX \

