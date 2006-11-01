#SUBDIRS = lib part shell samples kudesigner
SUBDIRS = lib part samples kudesigner_lib kudesigner

KDE_ICON = kugar kugardata

messages: 
	$(EXTRACTRC) lib/*.ui part/*.ui kudesigner/*.ui */*.rc > rc.cpp
	$(XGETTEXT) -ktr lib/*.cpp part/*.cpp kudesigner/*.cpp kudesigner_lib/*.cpp kudesigner_lib/*.h part/kugar_about.h kudesigner/kudesigner_aboutdata.h rc.cpp -o $(podir)/kugar.pot
