/* This file is part of the KDE libraries
    Copyright (C) 1998 Jörg Habenicht (j.habenicht@europemail.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/


#include "kruler.h"

#include <QFont>
#include <QPolygon>
#include <QStylePainter>

#define INIT_VALUE 0
#define INIT_MIN_VALUE 0
#define INIT_MAX_VALUE 100
#define INIT_TINY_MARK_DISTANCE 1
#define INIT_LITTLE_MARK_DISTANCE 5
#define INIT_MIDDLE_MARK_DISTANCE (INIT_LITTLE_MARK_DISTANCE * 2)
#define INIT_BIG_MARK_DISTANCE (INIT_LITTLE_MARK_DISTANCE * 10)
#define INIT_SHOW_TINY_MARK false
#define INIT_SHOW_LITTLE_MARK true
#define INIT_SHOW_MEDIUM_MARK true
#define INIT_SHOW_BIG_MARK true
#define INIT_SHOW_END_MARK true
#define INIT_SHOW_POINTER true
#define INIT_SHOW_END_LABEL true

#define INIT_PIXEL_PER_MARK (double)10.0 /* distance between 2 base marks in pixel */
#define INIT_OFFSET (-20)
#define INIT_LENGTH_FIX true
#define INIT_END_OFFSET 0

#define FIX_WIDTH 20 /* widget width in pixel */
#define LINE_END (FIX_WIDTH - 3)
#define END_MARK_LENGTH (FIX_WIDTH - 6)
#define END_MARK_X2 LINE_END
#define END_MARK_X1 (END_MARK_X2 - END_MARK_LENGTH)
#define BIG_MARK_LENGTH (END_MARK_LENGTH*3/4)
#define BIG_MARK_X2 LINE_END
#define BIG_MARK_X1 (BIG_MARK_X2 - BIG_MARK_LENGTH)
#define MIDDLE_MARK_LENGTH (END_MARK_LENGTH/2)
#define MIDDLE_MARK_X2 LINE_END
#define MIDDLE_MARK_X1 (MIDDLE_MARK_X2 - MIDDLE_MARK_LENGTH)
#define LITTLE_MARK_LENGTH (MIDDLE_MARK_LENGTH/2)
#define LITTLE_MARK_X2 LINE_END
#define LITTLE_MARK_X1 (LITTLE_MARK_X2 - LITTLE_MARK_LENGTH)
#define BASE_MARK_LENGTH (LITTLE_MARK_LENGTH/2)
#define BASE_MARK_X2 LINE_END
#define BASE_MARK_X1 (BASE_MARK_X2 - BASE_MARK_LENGTH)

#define LABEL_SIZE 8
#define END_LABEL_X 4
#define END_LABEL_Y (END_LABEL_X + LABEL_SIZE - 2)

#undef PROFILING

#ifdef PROFILING
# include <qdatetime.h>
#endif

class KRuler::KRulerPrivate
{
public:
  int  endOffset_length;  /* marks the offset at the end of the ruler
                           * i.e. right side at horizontal and down side
                           * at vertical rulers.
                           * the ruler end mark is moved endOffset_length
                           * ticks away from the widget end.
                           * positive offset moves end mark inside the ruler.
                           * if lengthFix is true, endOffset_length holds the
                           * length of the ruler.
                           */
  int fontWidth; // ONLY valid for vertical rulers

  QAbstractSlider range;
  Qt::Orientation dir;
  int tmDist;
  int lmDist;
  int mmDist;
  int bmDist;
  int offset;
  bool showtm : 1; /* show tiny, little, medium, big, endmarks */
  bool showlm : 1;
  bool showmm : 1;
  bool showbm : 1;
  bool showem : 1;

  bool showpointer : 1;
  bool showEndL : 1;
  bool lengthFix : 1;

  double ppm; /* pixel per mark */

  QString endlabel;
};



KRuler::KRuler(QWidget *parent)
  : QAbstractSlider(parent)
  , d( new KRulerPrivate )
{
  setRange(INIT_MIN_VALUE, INIT_MAX_VALUE);
  setPageStep(10);
  setValue(INIT_VALUE);
  initWidget(Qt::Horizontal);
  setFixedHeight(FIX_WIDTH);
}


KRuler::KRuler(Qt::Orientation orient,
               QWidget *parent, Qt::WindowFlags f)
  : QAbstractSlider(parent)
  , d( new KRulerPrivate )
{
  setRange(INIT_MIN_VALUE, INIT_MAX_VALUE);
  setPageStep(10);
  setValue(INIT_VALUE);
  setWindowFlags(f);
  initWidget(orient);
  if (orient == Qt::Horizontal)
    setFixedHeight(FIX_WIDTH);
  else
    setFixedWidth(FIX_WIDTH);
}


KRuler::KRuler(Qt::Orientation orient, int widgetWidth,
               QWidget *parent, Qt::WindowFlags f)
  : QAbstractSlider(parent)
  , d( new KRulerPrivate )
{
  setRange(INIT_MIN_VALUE, INIT_MAX_VALUE);
  setPageStep(10);
  setValue(INIT_VALUE);
  setWindowFlags(f);
  initWidget(orient);
  if (orient == Qt::Horizontal)
    setFixedHeight(widgetWidth);
  else
    setFixedWidth(widgetWidth);
}


void KRuler::initWidget(Qt::Orientation orientation)
{
#ifdef __GNUC__
  #warning FIXME setFrameStyle(WinPanel | Raised);
#endif

  d->showpointer = INIT_SHOW_POINTER;
  d->showEndL = INIT_SHOW_END_LABEL;
  d->lengthFix = INIT_LENGTH_FIX;
  d->endOffset_length = INIT_END_OFFSET;

  d->tmDist = INIT_TINY_MARK_DISTANCE;
  d->lmDist = INIT_LITTLE_MARK_DISTANCE;
  d->mmDist = INIT_MIDDLE_MARK_DISTANCE;
  d->bmDist = INIT_BIG_MARK_DISTANCE;
  d->offset= INIT_OFFSET;
  d->showtm = INIT_SHOW_TINY_MARK;
  d->showlm = INIT_SHOW_LITTLE_MARK;
  d->showmm = INIT_SHOW_MEDIUM_MARK;
  d->showbm = INIT_SHOW_BIG_MARK;
  d->showem = INIT_SHOW_END_MARK;
  d->ppm = INIT_PIXEL_PER_MARK;
  d->dir = orientation;
}


KRuler::~KRuler()
{
  delete d;
}

#ifndef KDE_NO_DEPRECATED
void
KRuler::setMinValue(int value)
{
  setMinimum(value);
}
#endif

#ifndef KDE_NO_DEPRECATED
int
KRuler::minValue() const
{ return minimum(); }
#endif

#ifndef KDE_NO_DEPRECATED
void
KRuler::setMaxValue(int value)
{
  setMaximum(value);
}
#endif

#ifndef KDE_NO_DEPRECATED
int
KRuler::maxValue() const
{ return maximum(); }
#endif

void
KRuler::setTinyMarkDistance(int dist)
{
  if (dist != d->tmDist) {
    d->tmDist = dist;
    update(contentsRect());
  }
}

int
KRuler::tinyMarkDistance() const
{ return d->tmDist; }

void
KRuler::setLittleMarkDistance(int dist)
{
  if (dist != d->lmDist) {
    d->lmDist = dist;
    update(contentsRect());
  }
}

int
KRuler::littleMarkDistance() const
{ return d->lmDist; }

void
KRuler::setMediumMarkDistance(int dist)
{
  if (dist != d->mmDist) {
    d->mmDist = dist;
    update(contentsRect());
  }
}

int
KRuler::mediumMarkDistance() const
{ return d->mmDist; }

void
KRuler::setBigMarkDistance(int dist)
{
  if (dist != d->bmDist) {
    d->bmDist = dist;
    update(contentsRect());
  }
}

int
KRuler::bigMarkDistance() const
{ return d->bmDist; }

void
KRuler::setShowTinyMarks(bool show)
{
  if (show != d->showtm) {
    d->showtm = show;
    update(contentsRect());
  }
}

bool
KRuler::showTinyMarks() const
{
  return d->showtm;
}

void
KRuler::setShowLittleMarks(bool show)
{
  if (show != d->showlm) {
    d->showlm = show;
    update(contentsRect());
  }
}

bool
KRuler::showLittleMarks() const
{
  return d->showlm;
}

void
KRuler::setShowMediumMarks(bool show)
{
  if (show != d->showmm) {
    d->showmm = show;
    update(contentsRect());
  }
}

bool
KRuler::showMediumMarks() const
{
  return d->showmm;
}

void
KRuler::setShowBigMarks(bool show)
{
  if (show != d->showbm) {
    d->showbm = show;
    update(contentsRect());
  }
}


bool
KRuler::showBigMarks() const
{
  return d->showbm;
}

void
KRuler::setShowEndMarks(bool show)
{
  if (show != d->showem) {
    d->showem = show;
    update(contentsRect());
  }
}

bool
KRuler::showEndMarks() const
{
  return d->showem;
}

void
KRuler::setShowPointer(bool show)
{
  if (show != d->showpointer) {
    d->showpointer = show;
    update(contentsRect());
  }
}


bool
KRuler::showPointer() const
{
  return d->showpointer;
}

void
KRuler::setFrameStyle(int)
{
#ifdef __GNUC__
	#warning implement me (jowenn)
#endif
}

void
KRuler::setShowEndLabel(bool show)
{
  if (d->showEndL != show) {
    d->showEndL = show;
    update(contentsRect());
  }
}


bool
KRuler::showEndLabel() const
{
    return d->showEndL;
}


void
KRuler::setEndLabel(const QString& label)
{
  d->endlabel = label;

  // premeasure the fontwidth and save it
  if (d->dir == Qt::Vertical) {
    QFont font = this->font();
    font.setPointSize(LABEL_SIZE);
    QFontMetrics fm(font);
    d->fontWidth = fm.width(d->endlabel);
  }
  update(contentsRect());
}

QString KRuler::endLabel() const
{
  return d->endlabel;
}

void
KRuler::setRulerMetricStyle(KRuler::MetricStyle style)
{
  switch (style) {
  default: /* fall through */
  case Custom:
    return;
  case Pixel:
    setLittleMarkDistance(1);
    setMediumMarkDistance(5);
    setBigMarkDistance(10);

    setShowTinyMarks(false);
    setShowLittleMarks(true);
    setShowMediumMarks(true);
    setShowBigMarks(true);
    setShowEndMarks(true);

    update(contentsRect());
    setPixelPerMark(10.0);

    break;
  case Inch:
    setTinyMarkDistance(1);
    setLittleMarkDistance(2);
    setMediumMarkDistance(4);
    setBigMarkDistance(8);

    setShowTinyMarks(true);
    setShowLittleMarks(true);
    setShowMediumMarks(true);
    setShowBigMarks(true);
    setShowEndMarks(true);

    update(contentsRect());
    setPixelPerMark(9.0);

    break;
  case Millimetres: /* fall through */
  case Centimetres: /* fall through */
  case Metres:
    setLittleMarkDistance(1);
    setMediumMarkDistance(5);
    setBigMarkDistance(10);

    setShowTinyMarks(false);
    setShowLittleMarks(true);
    setShowMediumMarks(true);
    setShowBigMarks(true);
    setShowEndMarks(true);

    update(contentsRect());
    setPixelPerMark(3.0);
  }
  switch (style) {
  case Pixel:
    setEndLabel(QLatin1String("pixel"));
    break;
  case Inch:
    setEndLabel(QLatin1String("inch"));
    break;
  case Millimetres:
    setEndLabel(QLatin1String("mm"));
    break;
  case Centimetres:
    setEndLabel(QLatin1String("cm"));
    break;
  case Metres:
    setEndLabel(QLatin1String("m"));
  default: /* never reached, see above switch */
    /* empty command */;
  }
  // if the style changes one of the values,
  // update would have been called inside the methods
  // -> no update() call needed here !
}

void
KRuler::setPixelPerMark(double rate)
{ // never compare floats against each other :)
  d->ppm = rate;
  update(contentsRect());
}

double
KRuler::pixelPerMark() const
{ return d->ppm; }

void
KRuler::setLength(int length)
{
  int tmp;
  if (d->lengthFix) {
    tmp = length;
  }
  else {
    tmp = width() - length;
  }
  if (tmp != d->endOffset_length) {
    d->endOffset_length = tmp;
    update(contentsRect());
  }
}

int
KRuler::length() const
{
  if (d->lengthFix) {
    return d->endOffset_length;
  }
  return (width() - d->endOffset_length);
}


void
KRuler::setLengthFixed(bool fix)
{
  d->lengthFix = fix;
}

bool
KRuler::lengthFixed() const
{
  return d->lengthFix;
}

void
KRuler::setOffset(int _offset)
{// debug("set offset %i", _offset);
  if (d->offset != _offset) {
    d->offset = _offset;
    update(contentsRect());
  }
}

int
KRuler::offset() const
{ return d->offset; }

int
KRuler::endOffset() const
{
  if (d->lengthFix) {
    return (width() - d->endOffset_length);
  }
  return d->endOffset_length;
}


void
KRuler::slideUp(int count)
{
  if (count) {
    d->offset += count;
    update(contentsRect());
  }
}

void
KRuler::slideDown(int count)
{
  if (count) {
    d->offset -= count;
    update(contentsRect());
  }
}


void
KRuler::slotNewValue(int _value)
{
  int oldvalue = value();
  if (oldvalue == _value) {
    return;
  }
  //    setValue(_value);
  setValue(_value);
  if (value() == oldvalue) {
    return;
  }
  // get the rectangular of the old and the new ruler pointer
  // and repaint only him
  if (d->dir == Qt::Horizontal) {
    QRect oldrec(-5+oldvalue,10, 11,6);
    QRect newrec(-5+_value,10, 11,6);
    repaint( oldrec.united(newrec) );
  }
  else {
    QRect oldrec(10,-5+oldvalue, 6,11);
    QRect newrec(10,-5+_value, 6,11);
    repaint( oldrec.united(newrec) );
  }
}

void
KRuler::slotNewOffset(int _offset)
{
  if (d->offset != _offset) {
    //setOffset(_offset);
    d->offset = _offset;
    repaint(contentsRect());
  }
}


void
KRuler::slotEndOffset(int offset)
{
  int tmp;
  if (d->lengthFix) {
    tmp = width() - offset;
  }
  else {
    tmp = offset;
  }
  if (d->endOffset_length != tmp) {
    d->endOffset_length = tmp;
    repaint(contentsRect());
  }
}

void
KRuler::paintEvent(QPaintEvent * /*e*/)
{
  //  debug ("KRuler::drawContents, %s",(horizontal==dir)?"horizontal":"vertical");

  QStylePainter p(this);
#ifdef PROFILING
  QTime time;
  time.start();
  for (int profile=0; profile<10; profile++) {
#endif

  int value  = this->value(),
    minval = minimum(),
    maxval;
    if (d->dir == Qt::Horizontal) {
    maxval = maximum()
    + d->offset
    - (d->lengthFix?(height()-d->endOffset_length):d->endOffset_length);
    }
    else
    {
    maxval = maximum()
    + d->offset
    - (d->lengthFix?(width()-d->endOffset_length):d->endOffset_length);
    }
    //ioffsetval = value-offset;
    //    pixelpm = (int)ppm;
  //    left  = clip.left(),
  //    right = clip.right();
  double f, fend,
    offsetmin=(double)(minval-d->offset),
    offsetmax=(double)(maxval-d->offset),
    fontOffset = (((double)minval)>offsetmin)?(double)minval:offsetmin;

  // draw labels
  QFont font = p.font();
  font.setPointSize(LABEL_SIZE);
  p.setFont( font );
  // draw littlemarklabel

  // draw mediummarklabel

  // draw bigmarklabel

  // draw endlabel
  if (d->showEndL) {
    if (d->dir == Qt::Horizontal) {
      p.translate( fontOffset, 0 );
      p.drawText( END_LABEL_X, END_LABEL_Y, d->endlabel );
    }
    else { // rotate text +pi/2 and move down a bit
      //QFontMetrics fm(font);
#ifdef KRULER_ROTATE_TEST
      p.rotate( -90.0 + rotate );
      p.translate( -8.0 - fontOffset - d->fontWidth + xtrans,
                    ytrans );
#else
      p.rotate( -90.0 );
      p.translate( -8.0 - fontOffset - d->fontWidth, 0.0 );
#endif
      p.drawText( END_LABEL_X, END_LABEL_Y, d->endlabel );
    }
    p.resetMatrix();
  }

  // draw the tiny marks
  if (d->showtm) {
    fend = d->ppm*d->tmDist;
    for ( f=offsetmin; f<offsetmax; f+=fend ) {
      if (d->dir == Qt::Horizontal) {
        p.drawLine((int)f, BASE_MARK_X1, (int)f, BASE_MARK_X2);
      }
      else {
        p.drawLine(BASE_MARK_X1, (int)f, BASE_MARK_X2, (int)f);
      }
    }
  }
  if (d->showlm) {
    // draw the little marks
    fend = d->ppm*d->lmDist;
    for ( f=offsetmin; f<offsetmax; f+=fend ) {
      if (d->dir == Qt::Horizontal) {
        p.drawLine((int)f, LITTLE_MARK_X1, (int)f, LITTLE_MARK_X2);
      }
      else {
        p.drawLine(LITTLE_MARK_X1, (int)f, LITTLE_MARK_X2, (int)f);
      }
    }
  }
  if (d->showmm) {
    // draw medium marks
    fend = d->ppm*d->mmDist;
    for ( f=offsetmin; f<offsetmax; f+=fend ) {
      if (d->dir == Qt::Horizontal) {
        p.drawLine((int)f, MIDDLE_MARK_X1, (int)f, MIDDLE_MARK_X2);
      }
      else {
        p.drawLine(MIDDLE_MARK_X1, (int)f, MIDDLE_MARK_X2, (int)f);
      }
    }
  }
  if (d->showbm) {
    // draw big marks
    fend = d->ppm*d->bmDist;
    for ( f=offsetmin; f<offsetmax; f+=fend ) {
      if (d->dir == Qt::Horizontal) {
        p.drawLine((int)f, BIG_MARK_X1, (int)f, BIG_MARK_X2);
      }
      else {
        p.drawLine(BIG_MARK_X1, (int)f, BIG_MARK_X2, (int)f);
      }
    }
  }
  if (d->showem) {
    // draw end marks
    if (d->dir == Qt::Horizontal) {
      p.drawLine(minval-d->offset, END_MARK_X1, minval-d->offset, END_MARK_X2);
      p.drawLine(maxval-d->offset, END_MARK_X1, maxval-d->offset, END_MARK_X2);
    }
    else {
      p.drawLine(END_MARK_X1, minval-d->offset, END_MARK_X2, minval-d->offset);
      p.drawLine(END_MARK_X1, maxval-d->offset, END_MARK_X2, maxval-d->offset);
    }
  }

  // draw pointer
  if (d->showpointer) {
    QPolygon pa(4);
    if (d->dir == Qt::Horizontal) {
      pa.setPoints(3, value-5, 10, value+5, 10, value/*+0*/,15);
    }
    else {
      pa.setPoints(3, 10, value-5, 10, value+5, 15, value/*+0*/);
    }
    p.setBrush( p.background().color() );
    p.drawPolygon( pa );
  }

#ifdef PROFILING
  }
  int elapsed = time.elapsed();
  debug("paint time %i",elapsed);
#endif

}

