#SUBDIRS = lib part shell samples kudesigner
SUBDIRS = lib part samples kudesigner


KDE_ICON = kugar kugardata

messages: 
	$(EXTRACTRC) lib/*.ui part/*.ui kudesigner/*.ui shell/*.ui */*.rc > rc.cpp
	$(XGETTEXT) lib/*.cpp part/*.cpp kudesigner/*.cpp shell/*.cpp rc.cpp -o $(podir)/kugar.pot
