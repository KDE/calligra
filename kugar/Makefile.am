SUBDIRS = lib part shell samples kudesigner


KDE_ICON = kugar kugardata

messages: 
	$(EXTRACTRC) */*.ui */*.rc > rc.cpp
	$(XGETTEXT) */*.cpp rc.cpp -o $(podir)/kugar.pot
