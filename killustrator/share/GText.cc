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

#include <stdlib.h>
#include <iostream.h>
#include <math.h>
#include <assert.h>
#include "GText.h"
#include "GText.moc"
#include "GDocument.h"

#include <klocale.h>
#include <kapp.h>

GText::TextInfo GText::defaultTextInfo; 

void GText::setDefaultTextInfo (const TextInfo& ti) {
  if (ti.mask & TextInfo::Font)
    defaultTextInfo.font = ti.font;
  if (ti.mask & TextInfo::Align)
    defaultTextInfo.align = ti.align;
}

GText::TextInfo GText::getDefaultTextInfo () {
  return defaultTextInfo;
}

GText::GText () {
  textInfo = defaultTextInfo;
  fm = new QFontMetrics (textInfo.font);
  cursx = cursy = 0;
  max_width = 0;
  cursorActive = false;
  text.push_back (QString (""));
  pathObj = 0L;
}

GText::GText (const list<XmlAttribute>& attribs) : GObject (attribs) {
  textInfo = defaultTextInfo;
  fm = new QFontMetrics (textInfo.font);
  cursx = cursy = 0;
  max_width = 0;
  cursorActive = false;
  text.push_back (QString (""));
  pathObj = 0L;
  float x = 0, y = 0;

  list<XmlAttribute>::const_iterator first = attribs.begin ();
	
  while (first != attribs.end ()) {
    const string& attr = (*first).name ();
    if (attr == "x")
      x = (*first).floatValue ();
    else if (attr == "y")
      y = (*first).floatValue ();
    else if (attr == "align")
      textInfo.align = (TextInfo::Alignment) ((*first).intValue ());
    first++;
  }
  if (x != 0.0 || y != 0.0) {
    tMatrix.translate (x, y);
    iMatrix = tMatrix.invert ();
    initTmpMatrix ();
  }
}

GText::GText (const GText& obj) : GObject (obj) {
  textInfo = obj.textInfo;
  fm = new QFontMetrics (textInfo.font);
  cursx = cursy = 0;
  cursorActive = false;
  vector<QString>::const_iterator it = obj.text.begin ();
  while (it != obj.text.end ()) {
    text.push_back (*it);
    it++;
  }
  pathObj = obj.pathObj;
  if (pathObj)
    pathObj->ref ();
  calcBoundingBox ();
}

GText::~GText () {
  if (pathObj)
    pathObj->unref ();
}

void GText::setTextInfo (const TextInfo& tinfo) {
  if (tinfo.mask & TextInfo::Align)
    textInfo.align = tinfo.align;
  if (tinfo.mask & TextInfo::Font)
    setFont (tinfo.font);
}

GText::TextInfo GText::getTextInfo () const {
  TextInfo tinfo;
  tinfo.mask = TextInfo::Font | TextInfo::Align;
  tinfo.font = textInfo.font;
  tinfo.align = textInfo.align;
  return tinfo;
}

const char* GText::typeName () {
  return i18n ("Text");
}

void GText::draw (Painter& p, bool, bool) {
  QPen pen (outlineInfo.color, (uint) outlineInfo.width, 
            outlineInfo.style);
  p.save ();
  p.setPen (pen);
  p.setFont (textInfo.font);
  p.setWorldMatrix (tmpMatrix, true);

  if (pathObj)
    drawPathText (p);
  else
    drawSimpleText (p);
  p.restore ();
}


void GText::drawSimpleText (Painter& p) {
  vector<QString>::iterator it = text.begin ();
  float y = fm->ascent ();
  for (; it != text.end (); it++) {
    const char* s = (const char *) *it;
    int ws = fm->width (s);
    int xoff = 0;
    if (textInfo.align == TextInfo::AlignCenter)
      xoff = (max_width - ws) / 2;
    else if (textInfo.align == TextInfo::AlignRight)
      xoff = max_width - ws;
    QPoint pos (xoff, (int) y);
    p.drawText (pos, s);
    y += fm->height ();
  }
  if (cursorActive) {
    float x1, x2, y1, y2;
    y1 = cursy * fm->height () - 1;
    y2 = y1 + fm->height () + 2;
    const char* s = text[cursy];
    x1 = 0;
    for (int i = 0; i < cursx; i++) 
	x1 += fm->width (s[i]);
    x2 = x1;
    p.setPen (black);
    p.drawLine (x1, y1, x2, y2);
  }
}

void GText::drawPathText (Painter& p) {
  vector<QString>::iterator it = text.begin ();
  int idx = 0;

  QWMatrix m = p.worldMatrix ();
  for (; it != text.end (); it++) {
    const char* s = (const char *) *it;
    int slen = strlen (s);

    for (int i = 0; i < slen; i++) {
      p.setWorldMatrix (m);
      p.setWorldMatrix (cmatrices[idx++], true);
      p.drawText (0, 0, &s[i], 1);
    }
  }
  /*
  if (cursorActive) {
    float x1, x2, y1, y2;
    y1 = cursy * fm->height () - 1 + opos.y ();
    y2 = y1 + fm->height () + 2;
    const char* s = text[cursy];
    x1 = 0;
    for (int i = 0; i < cursx; i++) 
	x1 += fm->width (s[i]);
    x1 += opos.x ();
    x2 = x1;
    p.setPen (black);
    p.drawLine (x1, y1, x2, y2);
  }
  */
}

void GText::setCursor (int x, int y) {
  if (y >= 0 && y < (int) text.size ())
    cursy = y;
  else 
    cursy = text.size () - 1;

  if (x >= 0 && x <= (int) text[y].length ())
    cursx = x;
  else
    cursx = text[y].length () - 1;
  updateRegion ();
}

void GText::setOrigin (const Coord& p) {
  tMatrix.translate (p.x () - tMatrix.dx (), 
		     p.y () - tMatrix.dy ());
  iMatrix = tMatrix.invert ();
  initTmpMatrix ();
  updateRegion ();
}

void GText::setText (const QString& s) {
  text.clear ();
  int pos1 = 0, pos2;

  do {
    pos2 = s.find ('\n', pos1);
    if (pos2 != -1) {
      QString sub = s.mid (pos1, pos2 - pos1);
      text.push_back (sub);
      pos1 = pos2 + 1;
    }
    else {
      QString sub = s.mid (pos1, s.length () - pos1);
      text.push_back (sub);
    }
  } while (pos2 != -1);
  updateMatricesForPath ();
  updateRegion ();
}

QString GText::getText () const {
  QString s;
  int line = 1;
  for (vector<QString>::const_iterator it = text.begin (); it != text.end (); 
       it++, line++) {
    s += *it;
    if (line < lines ())
      s+= "\n";
  }
  return s;
}

void GText::deleteChar () {
  QString& s = text[cursy];
  if (cursx == (int) s.length ()) {
    // end of line
    if (cursy < (int) text.size () - 1) {
      s.append (text[cursy + 1]);
      vector<QString>::iterator it = text.begin ();
      advance (it, cursy + 1);
      text.erase (it); 
    }
  }
  else
    s.remove (cursx, 1);
  updateMatricesForPath ();
  updateRegion ();
}

void GText::deleteBackward () {
  if (cursx == 0 && cursy == 0)
    return;

  QString& s = text[cursy];

  if (cursx > 0) {
    s.remove (cursx - 1, 1);
    cursx--;
  }
  else if (cursy > 0) {
    // begin of line
    QString& prev = text[cursy - 1];
    int oldpos = prev.length ();
    prev.append (s);
    vector<QString>::iterator it = text.begin ();
    advance (it, cursy);
    text.erase (it); 
    cursy--;
    cursx = oldpos;
  }
  updateMatricesForPath ();
  updateRegion ();
}

void GText::insertChar (char c) {
  QString& s = text[cursy];
  if (c == '\n') {
    // newline
    QString rest (s.right (s.length () - cursx));
    s.truncate (cursx);
    vector<QString>::iterator it = text.begin ();
    advance (it, ++cursy);
    text.insert (it, rest);
    cursx = 0;
  } 
  else {
    s.insert (cursx, c);
    cursx++;
  }
  updateMatricesForPath ();
  updateRegion ();
}

void GText::showCursor (bool flag) {
  cursorActive = flag;
  updateRegion (false);
}

void GText::updateCursor (const Coord& p) {
  if (box.contains (p)) {
    QPoint pp = iMatrix.map (QPoint ((int) p.x (), (int) p.y ()));

    cursy = (int) (pp.y () / fm->height ());
    int x = (int) pp.x ();
    const char *s = (const char *)line (cursy);
    int n = ::strlen (s);
    int width = 0;
    for (int i = 0; i < n; i++) {
      width += fm->width (s[i]);
      if (x <= width) {
	cursx = i;
	break;
      }
    }
  }
}

void GText::setFont (const QFont& f) {
  textInfo.font = f;
  if (fm) delete fm;
  fm = new QFontMetrics (textInfo.font);
  updateRegion ();
}

void GText::calcBoundingBox () {
  vector<QString>::iterator it;
  if (pathObj) {
    // compute bounding box for path text -> union of character boxes
    int idx = 0;
    QRect rect;

    for (it = text.begin (); it != text.end (); it++) {
      const char *s = *it;
      int slen = strlen (s);
      for (int i = 0; i < slen; i++) {
	QRect r = fm->boundingRect (s[i]);
	r = cmatrices[idx].map (r);
	r = tmpMatrix.map (r);
	if (idx == 0)
	  rect = r;
	else 
	  rect = rect.unite (r);
	idx++;
      }
    }
    updateBoundingBox (Rect (rect.left () - 1, rect.top () - 1, 
			     rect.width () + 2, rect.height () + 2));
  }
  else {
    int width = 0, height = 0;
    for (it = text.begin (); it != text.end (); it++) {
      const char* s = *it;
      int ws = fm->width (s);
      width = width > ws ? width : ws;
      height += fm->height ();
    }
    max_width = width;
    if (cursorActive) {
      height = QMAX(height, fm->height () + 2);
      width += 2;
    }
    calcUntransformedBoundingBox (Coord (0, 0), 
				  Coord (width, cursorActive ? -1 : 0),
				  Coord (width, 
					 height + (cursorActive ? 2 : 0)),
				  Coord (0, height + (cursorActive ? 2 : 0)));
  }
}

GOState* GText::saveState () {
  GTextState *state = new GTextState;
  GObject::initState (state);
  GText::initState (state);
  return state;
}

void GText::restoreState (GOState* state) {
#ifdef USE_RTTI
  GTextState *s = dynamic_cast<GTextState *> (state);
  assert (s != 0L);
#else
  GTextState *s = (GTextState *) state;
#endif
  setFont (s->info.font);
  textInfo.align = s->info.align;
  setText (s->tstring);
  GObject::restoreState (state);
}

void GText::initState (GOState* state) {
#ifdef USE_RTTI
  GTextState *s = dynamic_cast<GTextState *> (state);
  assert (s != 0L);
#else
  GTextState *s = (GTextState *) state;
#endif
  s->info.font = textInfo.font;
  s->info.align = textInfo.align;
  s->tstring = getText ();
}
  
GObject* GText::copy () {
  return new GText (*this);
}

void GText::writeToXml (XmlWriter& xml) {
  xml.startTag ("text", false);
  writePropertiesToXml (xml);
  xml.addAttribute ("align", (int) textInfo.align);
  xml.closeTag (false);

  xml.startTag ("font", false);
  xml.addAttribute ("face", textInfo.font.family ());
  xml.addAttribute ("point-size", textInfo.font.pointSize ());
  xml.addAttribute ("weight", textInfo.font.weight ());
  if (textInfo.font.italic ())
    xml.addAttribute ("italic", 1);

  xml.closeTag (false);

  int i = 1;
  for (vector<QString>::iterator it = text.begin (); it != text.end (); it++) {
    xml.writeText ((const char *) *it);
    if (i < lines ())
      xml.writeTag ("br");
  }

  xml.endTag ();
  xml.endTag ();
}

void GText::updateMatricesForPath () {
  if (pathObj) {
    vector<QString>::iterator it;
    int i;
    int num_chars = 0;

    // initialize transformation matrices for characters
    for (it = text.begin (); it != text.end (); it++) 
      num_chars += it->length ();
    cmatrices.clear ();
    cmatrices.resize (num_chars);

    if (pathObj->isA ("GPolyline") || pathObj->isA ("GPolygon")) {
      // get path for aligning
      vector<Coord> path;
      pathObj->getPath (path);

      // map path from world coordinates to object coordinates
      for (i = 0; i < (int) path.size (); i++)
	path[i] = path[i].transform (iMatrix);

      // now compute character matrices according the path
      float xp1, yp1, xp2, yp2;
      float dx, dy, len, angle;
      int lpos = 0; // width of string at current segment
      int seg = 0; // segment of path
      int idx = 0; // matrix index
      bool last_segment = (seg + 2 == (int) path.size ());

      xp1 = path[seg].x (); yp1 = path[seg].y ();
      xp2 = path[seg + 1].x (); yp2 = path[seg + 1].y ();
      dx = xp2 - xp1; dy = yp2 - yp1;
      len = sqrt (dx * dx + dy * dy);
      angle = atan (dy / dx) * RAD_FACTOR;
      if (dx < 0) angle = -angle;

      for (it = text.begin (); it != text.end (); it++) {
	const char *s = (const char *) *it;
	int slen = strlen (s);
	for (i = 0; i < slen; i++) {
	  int cwidth = fm->width (s[i]);
	  if (! last_segment && lpos + cwidth > len) {
	    // character doesn't fits to current segment
	    // --> draw on next segment
	    seg++;
	    xp1 = path[seg].x (); yp1 = path[seg].y ();
	    xp2 = path[seg + 1].x (); yp2 = path[seg + 1].y ();
	    dx = xp2 - xp1; dy = yp2 - yp1;
	    len = sqrt (dx * dx + dy * dy);
	    angle = atan (dy / dx) * RAD_FACTOR;
	    if (dx < 0) angle += 180;
	    lpos = 0;
	    last_segment = (seg + 2 == (int) path.size ());
	  }
	  cmatrices[idx].translate (path[seg].x (), path[seg].y ());
	  cmatrices[idx].rotate (angle);
	  cmatrices[idx].translate (lpos, 0);
	  lpos += cwidth;
	  idx++;
	} 
      }
    }
    //    emit changed ();
    updateRegion ();
  }
}

void GText::setPathObject (GObject* obj) {
  cout << "set path object" << endl;
  if (pathObj != 0L) {
    pathObj->unref ();
    disconnect (obj, SIGNAL(changed(const Rect&)), 
		this, SLOT(updateMatricesForPath ()));
  }
  pathObj = obj;
  if (pathObj != 0L) {
    cout << "update matrices" << endl;
    pathObj->ref ();
    connect (obj, SIGNAL(changed(const Rect&)), 
	     this, SLOT(updateMatricesForPath ()));
    updateMatricesForPath ();
  }
}
