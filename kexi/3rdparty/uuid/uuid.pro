
TEMPLATE = lib

TARGET = kexiuuid

include( $(KEXI)/common.pro )

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
