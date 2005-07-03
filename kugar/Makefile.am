#SUBDIRS = lib part shell samples kudesigner
SUBDIRS = lib part samples kudesigner_lib kudesigner

KDE_ICON = kugar kugardata

messages: 
	$(EXTRACTRC) lib/*.ui part/*.ui kudesigner/*.ui */*.rc > rc.cpp
	$(XGETTEXT) -ktranslate lib/*.cpp part/*.cpp kudesigner/*.cpp kudesigner_lib/*.cpp kudesigner_lib/*.h rc.cpp -o $(podir)/kugar.pot
