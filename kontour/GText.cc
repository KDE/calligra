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

#include <assert.h>
#include <GText.h>
#include <GDocument.h>
#include "GPage.h"

#include <qdom.h>
#include <klocale.h>
#include <kdebug.h>

float seg_angle (const Coord& c1, const Coord& c2) {
  float dx = c2.x () - c1.x ();
  float dy = c2.y () - c1.y ();
  if (dx != 0)
    return atan (dy / dx) * RAD_FACTOR;
  else
    return 0.0;
}

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

GText::GText (GDocument *doc )
:GObject(doc)
{
  textInfo = defaultTextInfo;
  fm = new QFontMetrics (textInfo.font);
  cursx = cursy = 0;
  max_width = 0;
  cursorActive = false;
  text.append(QString (""));
  pathObj = 0L;
  cmatrices.setAutoDelete(true);
}

GText::GText (GDocument *doc, const QDomElement &element)
:GObject(doc, element.namedItem("gobject").toElement())
{
    textInfo = defaultTextInfo;
    fm = new QFontMetrics (textInfo.font);
    cursx = cursy = 0;
    max_width = 0;
    cursorActive = false;
    text.append(QString (""));
    pathObj = 0L;
    float x = 0, y = 0;

    refid=element.attribute("ref");
    x=element.attribute("x").toFloat();
    y=element.attribute("y").toFloat();
    textInfo.align=(TextInfo::Alignment)element.attribute("align").toInt();

    QDomElement f = element.namedItem("font").toElement();
    QFont font = QFont::defaultFont ();
    font.setFamily(f.attribute("face"));
    font.setPointSize(f.attribute("point-size").toInt());
    font.setWeight(f.attribute("weight").toInt());
    font.setItalic(f.attribute("italic").toInt());
    setFont (font);
    setText (element.text()); // Did I already say that I love QDom? :)

    if (x != 0.0 || y != 0.0) {
        tMatrix.translate (x, y);
        iMatrix = tMatrix.invert ();
        initTmpMatrix ();
    }
  cmatrices.setAutoDelete(true);
}

GText::GText (const GText& obj) : GObject (obj) {
  textInfo = obj.textInfo;
  fm = new QFontMetrics (textInfo.font);
  cursx = cursy = 0;
  cursorActive = false;
  pathObj = 0L;
  if (obj.pathObj)
    setPathObject (obj.pathObj);
  setText(obj.getText());
  calcBoundingBox ();
  cmatrices.setAutoDelete(true);
}

GText::~GText () {
  if (pathObj)
    pathObj->unref ();
  delete fm;
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

QString GText::typeName () const {
  return i18n("Text");
}

void GText::draw (QPainter& p, bool , bool, bool withEditMarks) {
  QPen pen (outlineInfo.color, (uint) outlineInfo.width,
            outlineInfo.style);
  p.save ();
  p.setPen (pen);
  p.setFont (textInfo.font);
  p.setWorldMatrix (tmpMatrix, true);

  if (pathObj)
    drawPathText (p,withEditMarks);
  else
    drawSimpleText (p,withEditMarks);
  p.restore ();
}


void GText::drawSimpleText (QPainter& p, bool drawCursor) {
  QStringList::Iterator it = text.begin ();
  float y = fm->ascent ();
  for (; it != text.end (); ++it) {
    int ws = fm->width(*it);
    int xoff = 0;
    if (textInfo.align == TextInfo::AlignCenter)
      xoff = -ws / 2;
    else if (textInfo.align == TextInfo::AlignRight)
      xoff = -ws;
    QPoint pos (xoff, (int) y);
    p.drawText (pos, *it);
    y += fm->height ();
  }
  if (cursorActive&&drawCursor) {
    float x1, y1, y2;
    y1 = cursy * fm->height () - 1;
    y2 = y1 + fm->height () + 2;
    QString s = *(text.at(cursy));
    int ws = fm->width (s);
    x1 = 0;
    for (int i = 0; i < cursx; i++)
      x1 += fm->width (s[i]);
    if (textInfo.align == TextInfo::AlignCenter)
      x1 += (-ws / 2);
    else if (textInfo.align == TextInfo::AlignRight)
      x1 += -ws;

    p.setPen (black);
    p.drawLine (qRound (x1), qRound (y1), qRound (x1), qRound (y2));

    QPoint pos = p.pos();
    global_posX = pos.x();
    global_posY = pos.y();
  }
}

void GText::drawPathText (QPainter& p, bool drawCursor) {
  QStringList::Iterator it = text.begin ();
  int idx = 0;

  QWMatrix m = p.worldMatrix ();
  for (; it != text.end (); it++) {
    QString s = *it;
    int slen = s.length();

    for (int i = 0; i < slen; i++) {
      p.setWorldMatrix (m);
      p.setWorldMatrix (*(cmatrices[idx++]), true);
      p.drawText (0, 0, (QChar)s[i], 1);
    }
  }
  if (cursorActive&&drawCursor) {
    idx = 0;
    int line = 0;
    QStringList::Iterator it = text.begin ();
    while (line < cursy) {
      idx += (*it).length ();
      it++;
      line++;
    }
    idx += cursx;

    p.setWorldMatrix (m);
    p.setWorldMatrix (*(cmatrices[idx]), true);
    p.setPen (black);
    p.drawLine (0, 2, 0, -fm->height () - 1);
  }
}

void GText::setCursor (int x, int y) {
  if (y >= 0 && y < (int) text.count())
    cursy = y;
  else
    cursy = text.count() - 1;

  if (x >= 0 && x <= (int) (*text.at(y)).length ())
    cursx = x;
  else
    cursx = (*text.at(y)).length () - 1;
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
      text.append(sub);
      pos1 = pos2 + 1;
    }
    else {
      if (s.length () - pos1 == 0)
        break;

      QString sub = s.mid (pos1, s.length () - pos1);
      text.append(sub);
    }
  } while (pos2 != -1);
  updateMatricesForPath ();
  updateRegion ();
}

QString GText::getText () const {
  QString s;
  int line = 1;
  for (QStringList::ConstIterator it = text.begin (); it != text.end ();
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
    if (cursy < (int) text.count() - 1) {
      s.append (text[cursy + 1]);
      text.remove(text.at(cursy + 1));
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
    text.remove(text.at(cursy));
    cursy--;
    cursx = oldpos;
  }
  updateMatricesForPath ();
  updateRegion ();
}

void GText::insertChar(const QString &string) {
  QString& s = text[cursy];
  if (string[0] == '\n') {
    // newline
    QString rest (s.right (s.length () - cursx));
    s.truncate (cursx);
    text.insert (text.at(++cursy), rest);
    cursx = 0;
  }
  else {
    s.insert(cursx, string);
    cursx+=string.length();
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
    if (pathObj) {
      cursx = cursy = 0;
    }
    else {
      QPoint pp = iMatrix.map (QPoint ((int) p.x (), (int) p.y ()));

      cursy = (int) (pp.y () / fm->height ());
      int x = (int) pp.x ();
      QString s = line (cursy);
      int n = s.length();
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
}

void GText::setFont (const QFont& f) {
  textInfo.font = f;
  if (fm) delete fm;
  fm = new QFontMetrics (textInfo.font);
  updateRegion ();
}

void GText::calcBoundingBox () {
  QStringList::Iterator it;
  if (pathObj) {
    // compute bounding box for path text -> union of character boxes
    int idx = 0;
    QRect rect;

    for (it = text.begin (); it != text.end (); it++) {
      QString s = *it;
      int slen = s.length();
      for (int i = 0; i < slen; i++) {
        QRect r = fm->boundingRect (s[i]);
        r = (*cmatrices[idx]).map (r);
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
    int xp = 0;
    for (it = text.begin (); it != text.end (); it++) {
      QString s = *it;
      int ws = fm->width (s);
      width = width > ws ? width : ws;
      height += fm->height ();
    }
    max_width = width;
    if (cursorActive) {
      height = QMAX(height, fm->height () + 2);
      width += 2;
    }
    if (textInfo.align == TextInfo::AlignCenter)
      xp = -max_width / 2;
    else if (textInfo.align == TextInfo::AlignRight)
      xp = -max_width;
    width += xp;
    calcUntransformedBoundingBox (Coord (xp, 0),
                                  Coord (width, cursorActive ? -1 : 0),
                                  Coord (width,
                                         height + (cursorActive ? 2 : 0)),
                                  Coord (xp, height + (cursorActive ? 2 : 0)));
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

/*GObject* GText::clone (const QDomElement &element) {
  return new GText (element);
}*/

QDomElement GText::writeToXml (QDomDocument &document) {

    QDomElement element=document.createElement("text");
    element.setAttribute ("align", (int) textInfo.align);
    if (pathObj)
        element.setAttribute ("ref", pathObj->getId ());

    QDomElement font=document.createElement("font");
    font.setAttribute ("face", textInfo.font.family());
    font.setAttribute ("point-size", textInfo.font.pointSize());
    font.setAttribute ("weight", textInfo.font.weight());
    if (textInfo.font.italic())
        font.setAttribute("italic", 1);
    element.appendChild(font);

    int i = 0;
    QString t;
    for (QStringList::Iterator it = text.begin (); it != text.end ();
         it++, i++)
        t+=(*it) + '\n';
    element.appendChild(document.createCDATASection(t));
    element.appendChild(GObject::writeToXml(document));
    return element;
}

void GText::updateMatricesForPath ()
{
   if (pathObj)
   {
      QStringList::Iterator it;
      int i;
      int num_chars = 0;

      // initialize transformation matrices for characters
      for (it = text.begin (); it != text.end (); it++)
         num_chars += (*it).length ();
      cmatrices.clear ();
      //kdDebug(38000)<<"updateMatrices...   num_chars: "<<num_chars<<endl;

      cmatrices.resize (num_chars);
      //kdDebug(38000)<<"cmatrices size(): "<<cmatrices.size()<<" count(): "<<cmatrices.count()<<endl;
      for (int i=0; i<num_chars; i++)
         cmatrices.insert(i,new QWMatrix);
      //kdDebug(38000)<<"cmatrices size(): "<<cmatrices.size()<<" count(): "<<cmatrices.count()<<endl;

      if (pathObj->inherits ("GPolyline") || pathObj->isA ("GOval"))
      {
         // get path for aligning
         QValueList<Coord> path;
         pathObj->getPath (path);

         // map path from world coordinates to object coordinates
         for (i = 0; i < (int) path.count(); i++)
            path[i] = path[i].transform (iMatrix);

         // now compute character matrices according the path
         float len, angle;
         float lpos = 0; // width of string at current segment

         int max_pos = path.count() - 1;
         int s_pos = 0, e_pos = 1;
         len = seg_length (path[s_pos], path[e_pos]);
         angle = seg_angle (path[s_pos], path[e_pos]);
         if (path[e_pos].x () - path[s_pos].x () < 0)
            angle += 180;

         for (it = text.begin (); it != text.end (); it++)
         {
            QString s = *it;
            int slen = s.length();
            i = 0;
            while (i < slen)
            {
               int cwidth = fm->width (s[i]);
               if (e_pos < max_pos && lpos + cwidth > len)
               {
                  e_pos += 1;
                  float nlen = seg_length (path[e_pos - 1], path[e_pos]);
                  angle = seg_angle (path[s_pos], path[e_pos]);
                  if (path[e_pos].x () - path[s_pos].x () < 0)
                     angle += 180;
                  len += nlen;
               }
               else
               {
                  if (e_pos - s_pos > 1)
                  {
                     lpos = 0;
                     int pidx = (e_pos + s_pos) / 2;
                     //kdDebug(38000)<<"updateMatricesForPath() pidx: "<<pidx<<" pathlength: "<<path.count()-1<<endl;
                     cmatrices[i]->translate (path[pidx].x (), path[pidx].y ());
                     cmatrices[i]->rotate (angle);
                     cmatrices[i]->translate (-cwidth/2, 0);
                     s_pos = e_pos - 1;
                     len = seg_length (path[s_pos], path[e_pos]);
                     angle = seg_angle (path[s_pos], path[e_pos]);
                     if (path[e_pos].x () - path[s_pos].x () < 0)
                        angle += 180;
                  }
                  else
                  {
                     //kdDebug(38000)<<"updateMatricesForPath() s_pos: "<<s_pos<<" pathlength: "<<path.count()-1<<" x: "<<path[s_pos].x ()<<" y: "<<path[s_pos].y ()<<endl;
                     //kdDebug(38000)<<"updateMatricesForPath() i: "<<i<<" vector size: "<<cmatrices.count()-1<<endl;
                     cmatrices[i]->translate (path[s_pos].x (), path[s_pos].y ());
                     //kdDebug(38000)<<"updateMatricesForPath() succeeded"<<endl;
                     cmatrices[i]->rotate (angle);
                     cmatrices[i]->translate (lpos, 0);
                     lpos += cwidth;
                  }
                  i++;
               }
            }
         }
      }
      updateRegion ();
   }
}

void GText::deletePathObject () {
  setPathObject (0L);
}

void GText::setPathObject (GObject* obj)
{
   if (pathObj != 0L)
   {
      disconnect (pathObj, SIGNAL(changed(const Rect&)),
                  this, SLOT(updateMatricesForPath ()));
      disconnect (pathObj, SIGNAL(deleted ()),
                  this, SLOT(deletePathObject ()));
      pathObj->unref ();
   }
   pathObj = obj;
   if (pathObj != 0L)
   {
      pathObj->ref ();
      // force generation of id for refering in XML
      (void) pathObj->getId ();

      connect (obj, SIGNAL(changed (const Rect&)),this, SLOT(updateMatricesForPath ()));

      connect (obj, SIGNAL(deleted ()),this, SLOT(deletePathObject ()));
      updateMatricesForPath ();
   }
   else
   {
      updateRegion (true);
   }
}

bool GText::isEmpty () const {
  return (text.isEmpty () || (lines () == 1 && text[0] == ""));
}

#include <GText.moc>
