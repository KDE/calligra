/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "tkcombobox.h"

#include <qlistbox.h>
#include <qpainter.h>
#include <qdrawutil.h>

#include <kapp.h>
#include <kstyle.h>

TKComboBox::TKComboBox(QWidget* parent, const char* name)
: QComboBox(false,parent,name)
{
}


TKComboBox::TKComboBox( bool isEditable, QWidget* parent, const char* name )
: QComboBox(isEditable,parent,name)
{
}

TKComboBox::~TKComboBox()
{
}

void TKComboBox::paintEvent(QPaintEvent*)
{
  QRect r;
  if (editable()){
    r = QRect( style().comboButtonRect( 0, 0, width(), height() ) );
  } else {
    r = QRect(4, 2, width()-height()-2, height()-4);
  }
  int by = 2;
  int bx = r.x() + r.width();
  int bw = width() - bx - 2;
  int bh = height()-4;

  QPainter p( this );
  const QColorGroup& g = colorGroup();

  QRect fr(2,2,width()-4,height()-4);

  if ( hasFocus()) {
    p.fillRect( fr, g.brush( QColorGroup::Highlight ) );
  } else {
    p.fillRect( fr, g.brush( QColorGroup::Base ) );
  }

  QRect r1(1,1,width()-1,height()-1);
  qDrawShadePanel( &p, r1, g, true, 1 );

  static const char* arrow_down[] = {
  "7 7 2 1",
  "X c Gray0",
  "  c None",
  "XXXXXXX",
  "XXXXXXX",
  "       ",
  "XXXXXXX",
  " XXXXX ",
  "  XXX  ",
  "   X   "};

  QPixmap pixmap(arrow_down);


  if (kapp->kstyle()) {
    QFont ref_font;
    kapp->kstyle()->drawKToolBarButton(&p, bx, by, bw, bh,
        isEnabled()? colorGroup() : palette().disabled(), false,
        true, isEnabled(), false, KStyle::Icon, QString::null,
        &pixmap, &ref_font, this);
  } else {
    style().drawButton( &p, bx, by, bw, bh, isEnabled() ? colorGroup() : palette().disabled(), false );
    style().drawItem( &p, bx, by, bw, bh, AlignCenter, colorGroup(), isEnabled(), &pixmap, QString::null );
  }


  if ( hasFocus()) {
    style().drawFocusRect(&p, fr, g, &g.highlight());
  }

  if (!editable()) {
    p.setClipRect(r);
    p.setPen( g.text() );
    p.setBackgroundColor( g.background() );

    if ( listBox()->item(currentItem()) ) {
      QListBoxItem * item = listBox()->item(currentItem());
      const QPixmap *pix = item->pixmap();
      QString text = item->text();
      int x = r.x();
      if ( pix ) {
        p.drawPixmap( x, r.y() + ( r.height() - pix->height() ) / 2 +1, *pix );
        x += pix->width()+3;
      }
      if (!text.isEmpty())
        p.drawText( x, r.y(), r.width()-x, r.height(), AlignLeft|AlignVCenter|SingleLine, text );
    }
  }
  p.end();
}

void TKComboBox::activate()
{
  emit activated(currentItem());
}

