TEMPLATE	= lib

include( $(KEXI)/kexidb/common.pro )

# needed to export library classes:
DEFINES += MAKE_KEXI_DB_LIB

TARGET = kexidb$$KDEBUG

DEFINES += YYERROR_VERBOSE=1

system( bash kmoc )

SOURCES = \
object.cpp \
drivermanager.cpp \
driver.cpp \
driver_p.cpp \
admin.cpp \
connectiondata.cpp \
connection.cpp \
utils.cpp \
field.cpp \
schemadata.cpp \
tableschema.cpp \
queryschema.cpp \
queryschemaparameter.cpp \
transaction.cpp \
indexschema.cpp \
cursor.cpp \
fieldlist.cpp \
global.cpp \
relationship.cpp \
roweditbuffer.cpp \
msghandler.cpp \
dbobjectnamevalidator.cpp \
fieldvalidator.cpp \
dbproperties.cpp \
\
parser/parser.cpp \
parser/parser_p.cpp \
parser/sqlparser.cpp \
parser/sqlscanner.cpp \
expression.cpp \
keywords.cpp \
preparedstatement.cpp \
alter.cpp \
lookupfieldschema.cpp \
simplecommandlineapp.cpp

#HEADERS =
