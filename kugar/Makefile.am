#SUBDIRS = lib part shell samples kudesigner
SUBDIRS = lib part samples kudesigner


KDE_ICON = kugar kugardata

messages: 
	$(EXTRACTRC) */*.ui */*.rc > rc.cpp
	$(XGETTEXT) */*.cpp rc.cpp -o $(podir)/kugar.pot
