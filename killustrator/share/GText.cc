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
  //  font = defaultTextInfo.font;
  textInfo = defaultTextInfo;
  fm = new QFontMetrics (textInfo.font);
  cursx = cursy = 0;
  max_width = 0;
  cursorActive = false;
  text.setAutoDelete (true);
  text.append (new QString (""));
  pathObj = 0L;
}

GText::GText (const list<XmlAttribute>& attribs) : GObject (attribs) {
  //  font = defaultTextInfo.font;
  textInfo = defaultTextInfo;
  fm = new QFontMetrics (textInfo.font);
  cursx = cursy = 0;
  max_width = 0;
  cursorActive = false;
  text.setAutoDelete (true);
  text.append (new QString (""));
  pathObj = 0L;

  list<XmlAttribute>::const_iterator first = attribs.begin ();
	
  while (first != attribs.end ()) {
    const string& attr = (*first).name ();
    if (attr == "x")
      opos.x ((*first).floatValue ());
    else if (attr == "y")
      opos.y ((*first).floatValue ());
    else if (attr == "align")
      textInfo.align = (TextInfo::Alignment) ((*first).intValue ());
    first++;
  }

}

GText::GText (const GText& obj) : GObject (obj) {
  //  font = obj.font;
  textInfo = obj.textInfo;
  opos = obj.opos;
  fm = new QFontMetrics (textInfo.font);
  cursx = cursy = 0;
  cursorActive = false;
  text.setAutoDelete (true);
  QListIterator<QString> it (obj.text);
  for (; it.current (); ++it) 
    text.append (new QString (it.current ()->copy ()));
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

  QListIterator<QString> it (text);
  float y = opos.y () + fm->ascent ();
  for (; it.current (); ++it) {
    const char* s = *(it.current ());
    int ws = fm->width (s);
    int xoff = 0;
    if (textInfo.align == TextInfo::AlignCenter)
      xoff = (max_width - ws) / 2;
    else if (textInfo.align == TextInfo::AlignRight)
      xoff = max_width - ws;
    QPoint pos ((int) opos.x () + xoff, (int) y);
    p.drawText (pos, s);
    y += fm->height ();
  }
  if (cursorActive) {
    float x1, x2, y1, y2;
    y1 = cursy * fm->height () - 1 + opos.y ();
    y2 = y1 + fm->height () + 2;
    const char* s = *(text.at (cursy));
    x1 = 0;
    for (int i = 0; i < cursx; i++) 
	x1 += fm->width (s[i]);
    x1 += opos.x ();
    x2 = x1;
    p.setPen (black);
    p.drawLine (x1, y1, x2, y2);
  }

  p.restore ();
}

void GText::setCursor (int x, int y) {
  if (y >= 0 && y < (int) text.count ())
    cursy = y;
  else 
    cursy = text.count () - 1;

  if (x >= 0 && x <= (int) text.at (y)->length ())
    cursx = x;
  else
    cursx = text.at (y)->length () - 1;
  updateRegion (false);
  //  emit changed ();
}

void GText::setOrigin (const Coord& p) {
  opos = p;
  updateRegion ();
}

void GText::setText (const QString& s) {
  text.clear ();
  int pos1 = 0, pos2;

  do {
    pos2 = s.find ('\n', pos1);
    if (pos2 != -1) {
      QString sub = s.mid (pos1, pos2 - pos1);
      text.append (new QString (sub));
      pos1 = pos2 + 1;
    }
    else {
      QString sub = s.mid (pos1, s.length () - pos1);
      text.append (new QString (sub));
    }
  } while (pos2 != -1);
  updateMatricesForPath ();
  updateRegion ();
}

QString GText::getText () const {
  QString s;
  int line = 1;

  QListIterator<QString> it (text);
  for (; it.current (); ++it, ++line) {
    s += *(it.current ());
    if (line < lines ())
      s+= "\n";
  }
  return s;
}

void GText::deleteChar () {
  //  printf ("del\n");
  QString& s = *(text.at (cursy));
  if (cursx == (int) s.length ()) {
    // end of line
    if (cursy < (int) text.count () - 1) {
      s.append (*text.at (cursy + 1));
      text.remove (cursy + 1);
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

  QString& s = *(text.at (cursy));

  if (cursx > 0) {
    s.remove (cursx - 1, 1);
    cursx--;
  }
  else if (cursy > 0) {
    // begin of line
    QString& prev = *(text.at (cursy - 1));
    int oldpos = prev.length ();
    prev.append (s);
    text.remove (cursy);
    cursy--;
    cursx = oldpos;
  }
  updateMatricesForPath ();
  updateRegion ();
}

void GText::insertChar (char c) {
  QString& s = *(text.at (cursy));
  if (c == '\n') {
    // newline
    QString* rest = new QString (s.right (s.length () - cursx));
    s.truncate (cursx);
    text.insert (++cursy, rest);
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

    Coord c = opos;
    cursy = (int) ((pp.y () - c.y ()) / fm->height ());
    int x = (int) (pp.x () - c.x ());
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
  int width = 0, height = 0;
  QListIterator<QString> it (text);
  for (; it.current (); ++it) {
    const char* s = *(it.current ());
    int ws = fm->width (s);
    width = width > ws ? width : ws;
    height += fm->height ();
  }
  max_width = width;
  calcUntransformedBoundingBox (opos, Coord (opos.x () + width, opos.y ()),
				Coord (opos.x () + width, opos.y () + height),
				Coord (opos.x (), opos.y () + height));
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
  xml.addAttribute ("x", opos.x ());
  xml.addAttribute ("y", opos.y ());
  xml.addAttribute ("align", (int) textInfo.align);
  xml.closeTag (false);

  xml.startTag ("font", false);
  xml.addAttribute ("face", textInfo.font.family ());
  xml.addAttribute ("point-size", textInfo.font.pointSize ());
  xml.addAttribute ("weight", textInfo.font.weight ());
  if (textInfo.font.italic ())
    xml.addAttribute ("italic", 1);

  xml.closeTag (false);

  QListIterator<QString> it (text);
  int i = 1;
  for (; it.current (); ++it, ++i) {
    xml.writeText ((const char *) *(it.current ()));
    if (i < lines ())
      xml.writeTag ("br");
  }

  xml.endTag ();
  xml.endTag ();
}

void GText::updateMatricesForPath () {
  if (pathObj) {
    cout << "compute character matrices" << endl;
    emit changed ();
  }
}

void GText::setPathObject (GObject* obj) {
  if (pathObj != 0L) {
    pathObj->unref ();
    disconnect (obj, SIGNAL(changed(const Rect&)), 
		this, SLOT(updateMatricesForPath ()));
  }
  pathObj = obj;
  if (pathObj != 0L) {
    pathObj->ref ();
    connect (obj, SIGNAL(changed(const Rect&)), 
	     this, SLOT(updateMatricesForPath ()));
    updateMatricesForPath ();
  }
}
