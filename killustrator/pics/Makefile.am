data_DATA =             \
ahcenter.png \
aleft.png \
aright.png \
atop.png \
abottom.png \
avcenter.png \
dleft.png \
dright.png \
dhcenter.png \
dhdist.png \
dtop.png \
dbottom.png \
dvcenter.png \
dvdist.png \
ellipsetool.png \
linetool.png \
freehandtool.png \
recttool.png \
selecttool.png \
texttool.png \
beziertool.png \
zoomtool.png \
polygontool.png \
pointtool.png \
ellipse1.png \
ellipse2.png \
ellipse3.png \
hmirror.png \
vmirror.png \
tleftalign.png \
tcenteralign.png \
trightalign.png \
killustrator.png \
logo.png \
deletelayer.png \
lowerlayer.png \
newlayer.png \
raiselayer.png \
eye.png \
deleteNode.png \
joinNodes.png \
moveNode.png \
newNode.png \
split.png \
mini-killustrator.png  \
killustrator-intro.gif

EXTRA_DIST = $(data_DATA) 

# directory for pixmaps
datadir = $(kde_datadir)/killustrator/pics

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-killustrator.png $(kde_minidir)/killustrator.png

uninstall-local:
	-rm -f $(kde_minidir)/killustrator.png

