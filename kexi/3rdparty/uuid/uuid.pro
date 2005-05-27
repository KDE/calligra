
TEMPLATE = lib

win32:DEFINES += MAKE_KEXIUUID_LIB

include( $(KEXI)/common.pro )

TARGET = kexiuuid$$KDELIBDEBUG


SOURCES = \
clear.c \
compare.c \
copy.c \
gen_uuid.c \
isnull.c \
pack.c \
parse.c \
unpack.c \
unparse.c \
uuid_time.c

#tst_uuid.c \
#gen_uuid_nt.c \
