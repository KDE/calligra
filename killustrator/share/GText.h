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

#ifndef GText_h_
#define GText_h_

#include <qobject.h>
#include <qcolor.h>
#include <qfont.h>
#include <qwmatrix.h>
#include <qpainter.h>
#include <qdstream.h>
#include <qdict.h>
#include <qlist.h>

#include "Coord.h"
#include "GObject.h"

class GTextState;

class GText : public GObject {
  Q_OBJECT
public:

  struct TextInfo {
    enum { Font = 1 };
    unsigned int mask;
    QFont font;
  };

  static void setDefaultTextInfo (const TextInfo& ti);
  static TextInfo getDefaultTextInfo ();

  GText ();
  GText (const list<XmlAttribute>& attribs);
  GText (const GText& obj);
  ~GText () {}
  
  virtual void draw (Painter& p, bool withBasePoints = false);

  void setOrigin (const Coord& p);
  const Coord& origin () const { return opos; }

  void setTextInfo (const TextInfo& tinfo);
  TextInfo getTextInfo () const;

  void insertChar (char c);
  void deleteChar ();
  void deleteBackward ();
  void showCursor (bool flag);
  void updateCursor (const Coord& p);
  
  void setCursor (int x, int y);
  int cursorX () const { return cursx; }
  int cursorY () const { return cursy; }
  
  int lines () const { return text.count (); }
  const QString& line (int n) { return *text.at (n); }
  void setText (const QString& s);
  QString getText () const;
  
  const QFont& getFont () const { return font; }
  void setFont (const QFont& f);

  virtual const char* typeName ();

  virtual GOState* saveState ();
  virtual void restoreState (GOState* state);

  virtual GObject* copy ();

  virtual void writeToXml (XmlWriter&);
  
protected:
  void calcBoundingBox ();

  void initState (GOState* state);
  
private:
  Coord opos;
  int cursx, cursy;
  QList<QString> text;
  QFont font;
  QFontMetrics *fm;
  bool cursorActive;

  static TextInfo defaultTextInfo;
};

class GTextState : public GOState {
  friend class GText;
protected:
  GTextState () {}

public:
  virtual ~GTextState () {}

private:
  GText::TextInfo info;
  QString tstring;
};

#endif

