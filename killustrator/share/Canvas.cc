/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by  
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <iostream.h>
#include <fstream.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qprintdialog.h>
#include <qcolor.h>    
#include <qdatetime.h>    
#include <kmsgbox.h>
#include "Canvas.h"
#include "Canvas.moc"
#include "GDocument.h"
#include "Handle.h"
#include "ToolController.h"
#include "QwViewport.h"
#include "version.h"

QArray<float> Canvas::zoomFactors;
QString Canvas::psPrologPath = 
     kapp->kde_datadir () + "/killustrator/prolog.ps";
QDict<QString> Canvas::fontMap;

Canvas::Canvas (GDocument* doc, float res, QwViewport* vp, QWidget* parent, 
		const char* name) : QWidget (parent, name) {
  document = doc;
  resolution = res;
  zoomFactor = 1.0;
  drawBasePoints = false;
  viewport = vp;

  connect (document, SIGNAL (changed ()), this, SLOT (updateView ()));
  connect (document, SIGNAL (sizeChanged ()), this, SLOT (calculateSize ()));
  connect (&(document->handle ()), SIGNAL (handleChanged ()),
	   this, SLOT (updateView ()));

  pixmap = 0L;

  vGridDistance = hGridDistance = 50;
  gridIsOn = false;

  calculateSize ();
  setFocusPolicy (ClickFocus);
  setMouseTracking (true);
  setBackgroundMode (NoBackground);

  dragging = false;
  ensureVisibilityFlag = false;
}

Canvas::~Canvas () {
  delete pixmap;
}

void Canvas::ensureVisibility (bool flag) {
  ensureVisibilityFlag = flag;
}

void Canvas::calculateSize () {
  width = (int) (document->getPaperWidth () * resolution * 
		 zoomFactor / 72.0);
  height = (int) (document->getPaperHeight () * resolution * 
		  zoomFactor / 72.0);
  resize (width, height);

  if (pixmap != 0L)
    delete pixmap;
  pixmap = new QPixmap (width, height);
  pixmap->fill (white);
  updateView ();
  emit sizeChanged ();
}

void Canvas::initZoomFactors (QArray<float>& factors) {
  zoomFactors.duplicate (factors);
}


void Canvas::setZoomFactor (float factor) {
  zoomFactor = factor;
  calculateSize ();
  updateView ();
  emit sizeChanged ();
  emit zoomFactorChanged (zoomFactor);
}

float Canvas::getZoomFactor () const {
  return zoomFactor;
}

void Canvas::showGrid (bool flag) {
  gridIsOn = flag;
  updateView ();
}

void Canvas::snapToGrid (bool flag) {
  gridSnapIsOn = flag;
}

void Canvas::setGridDistance (int hdist, int vdist) {
  hGridDistance = hdist;
  vGridDistance = vdist;
}

void Canvas::snapPositionToGrid (int& x, int& y) {
  if (gridSnapIsOn) {
    int p, m, n;

    p = x / hGridDistance;
    m = x % hGridDistance;
    n = p * hGridDistance;
    if (m > hGridDistance / 2)
      n += hGridDistance;
    x = n;

    p = y / vGridDistance;
    m = y % vGridDistance;
    n = p * vGridDistance;
    if (m > vGridDistance / 2)
      n += hGridDistance;
    y = n;
  }
}


void Canvas::setToolController (ToolController* tc) {
  toolController = tc;
}

void Canvas::propagateMouseEvent (QMouseEvent *e) {
  // transform position of the mouse pointer according to current
  // zoom factor
  QPoint new_pos ((int) (e->x () * 72 / (resolution * zoomFactor)),
		  (int) (e->y () * 72 / (resolution * zoomFactor)));
  QMouseEvent new_ev (e->type (), new_pos, e->button (), e->state ());

  emit mousePositionChanged (new_pos.x (), new_pos.y ());

  // ensure visibility
  if (ensureVisibilityFlag) {
    if (e->type () == Event_MouseButtonPress && e->button () == LeftButton)
      dragging = true;
    else if (e->type () == Event_MouseButtonRelease && 
	     e->button () == LeftButton)
      dragging = false;
    else if (e->type () == Event_MouseMove && dragging) 
      viewport->ensureVisible (e->x (), e->y (), 10, 10);
  }

#if 0  
  if (e->button () == RightButton) {
    if (e->type () != Event_MouseButtonPress)
      return;
    if (document->selectionIsEmpty ()) {
      GObject* obj = document->findContainingObject (new_pos.x (), 
						     new_pos.y ());
      if (obj) {
	// pop up menu for the picked object
	emit rightButtonAtObjectClicked (e->x (), e->y (), obj);
      }
      else {
	emit rightButtonClicked (e->x (), e->y ());
      }
    }
    else {
      // pop up menu for the current selection
	emit rightButtonAtSelectionClicked (e->x (), e->y ());
    }
    return;
  }
  else
#endif
    if (toolController) {
      // the tool controller processes the event
      toolController->delegateEvent (&new_ev, document, this);
    }
}

void Canvas::propagateKeyEvent (QKeyEvent *e) {
  if (toolController) {
    toolController->delegateEvent (e, document, this);
  }
}

void Canvas::mousePressEvent (QMouseEvent* e) {
  propagateMouseEvent (e);
}

void Canvas::mouseReleaseEvent (QMouseEvent* e) {
  propagateMouseEvent (e);
}

void Canvas::mouseMoveEvent (QMouseEvent* e) {
  propagateMouseEvent (e);
}

void Canvas::keyPressEvent (QKeyEvent* e) {
  propagateKeyEvent (e);
}

void Canvas::paintEvent (QPaintEvent* e) {
  const QRect& rect = e->rect ();
  bitBlt (this, rect.x (), rect.y (), pixmap, 
	  rect.x (), rect.y (), rect.width (), rect.height ());
}

void Canvas::moveEvent (QMoveEvent *e) {
    emit visibleAreaChanged (e->pos ().x (), e->pos ().y ());
}

void Canvas::setDocument (GDocument* doc) {
  document = doc;
  connect (document, SIGNAL (changed ()), this, SLOT (updateView ()));
}

GDocument* Canvas::getDocument () {
  return document;
}

void Canvas::showBasePoints (bool flag) {
  drawBasePoints = flag;
  updateView ();
}

float Canvas::scaleFactor () const {
  return resolution * zoomFactor / 72.0;
}

void Canvas::updateView () {
  Painter p;
  
  p.begin (pixmap);
  p.setBackgroundColor (white);
  pixmap->fill (backgroundColor ());

  p.scale (scaleFactor (), scaleFactor ());
  p.eraseRect (0, 0, document->getPaperWidth (),
	       document->getPaperHeight ());

  if (gridIsOn)
    drawGrid (p);
  
  QListIterator<GObject> it = document->getObjects ();
  for (; it.current (); ++it) {
    it.current ()->draw (p, drawBasePoints);
  }
  if (! document->selectionIsEmpty ()) {
    document->handle ().draw (p);
  }
  p.end ();
  repaint ();
}

void Canvas::drawGrid (Painter& p) {
  int pw = document->getPaperWidth ();
  int ph = document->getPaperHeight ();
  int h, v;

  QPen pen1 (blue, 0, DotLine);

  p.save ();
  p.setPen (pen1);
  for (h = hGridDistance; h < pw; h += hGridDistance)
    p.drawLine (h, 0, h, ph);
  for (v = vGridDistance; v < ph; v += vGridDistance)
    p.drawLine (0, v, pw, v);
  p.restore ();
}

void Canvas::printDocument () {
  QPrinter printer;
  if (printer.setup (this)) {
    Painter paint;
    paint.begin (&printer);
    
    QListIterator<GObject> it = document->getObjects ();
    for (; it.current (); ++it) 
      it.current ()->draw (paint);

    paint.end ();
  }
}

void Canvas::writePSHeader (ostream& os) {
  os << "%!PS-Adobe-2.0\n"
     << "%%Title: " << (const char *) document->fileName () << '\n'
     << "%%Creator: KIllustrator " << APP_VERSION << '\n'
     << "%%CreationDate: " << QDateTime::currentDateTime ().toString () << '\n'
     << "%%EndComments" << endl;
}

bool Canvas::writePSProlog (ostream& os) {
  ifstream prolog (psPrologPath);
  if (!prolog) 
    return false;
  
  char buf[128];
  while (!prolog.eof ()) {
    prolog.getline (buf, 128);
    os << buf << '\n';
  }
  return true;
}

const char* Canvas::getPSFont (const QFont& qfont) {
  if (fontMap.isEmpty ()) {
    QString psFontmapPath = kapp->kde_datadir () + "/killustrator/fontmap";
    ifstream fin ((const char *) psFontmapPath);
    fin.ignore (INT_MAX, '\n');
    char key[128], value[128];
    while (! fin.eof ()) {
      fin >> key >> value;
      if (key[0] == '\0')
	break;
      fontMap.insert (key, new QString (value));
    }
  }
  QString family = qfont.family ();
  family = family.lower ();
  bool italic = qfont.italic ();
  int weight = qfont.weight ();
  
  QString key = family;
  if (italic)
    key += ".italic";
  if (weight >= QFont::Bold)
    key += ".bold";
  else if (weight >= QFont::DemiBold)
    key += ".demibold";
  else if (weight <= QFont::Light)
    key += ".light";
  
  QString* font = fontMap[key];
  if (font)
    return (const char *) *font;
  else
    return "/Times-Roman";
}

void Canvas::printPSDocument () {
  QPrinter pSetup;
  const char* tmpName;
 
  if (QPrintDialog::getPrinterSetup (&pSetup)) {
    if (! pSetup.outputToFile ())
     tmpName = tempnam (NULL, "kps");
    else 
      tmpName = pSetup.outputFileName ();

    ofstream psStream (tmpName);
    if (!psStream)
      return;

    // write header
    writePSHeader (psStream);
    if (! writePSProlog (psStream)) {
      KMsgBox::message (this, i18n ("Error"), i18n ("Cannot find PS prolog !"),
                        KMsgBox::STOP, i18n ("Abort"));
      return;
    }
    psStream << "%%BeginSetup\n"
	     << "/PaperWidth " << document->getPaperWidth () << " def\n"
             << "/PaperHeight " << document->getPaperHeight () << " def\n"
             << "InitTMatrix\n"
	     << "%%EndSetup\n";

    // write objects
    QListIterator<GObject> it = document->getObjects ();
    for (; it.current (); ++it) 
      it.current ()->writeToPS (psStream);
    psStream << "showpage\n%%EOF" << endl;
    psStream.close ();

    if (! pSetup.outputToFile ()) {
      const char* prog = pSetup.printProgram ();
      const char* printer = pSetup.printerName ();
      int num = pSetup.numCopies ();
      QString cmd;
      cmd.sprintf ("%s -# %d -P %s %s", prog, num, printer, tmpName);
      system ((const char *) cmd);
      unlink (tmpName);
    }
  }
}

void Canvas::zoomIn (int x, int y) {
  int pos = zoomFactors.find (getZoomFactor ());
  assert (pos != -1);
  if (pos < (int) zoomFactors.size () - 1) {
    setZoomFactor (zoomFactors[pos + 1]);
    viewport->centerOn (x, y);
    emit zoomFactorChanged (zoomFactors[pos + 1]);
  }
}

void Canvas::zoomOut () {
  int pos = zoomFactors.find (getZoomFactor ());
  assert (pos != -1);
  if (pos > 0)
    setZoomFactor (zoomFactors[pos - 1]);
}
