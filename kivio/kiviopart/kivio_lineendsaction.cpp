/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
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
#include "kivio_lineendsaction.h"
#include "kivio_lineendspix.h"
#include "tkcombobox.h"

#include <qbitmap.h>
#include <qpixmap.h>
#include <qpainter.h>

LineEndsAction::LineEndsAction( bool isRightEnds, QObject* parent, const char* name )
: TKBaseSelectAction(parent,name)
{
  m_rightends = isRightEnds;
}

LineEndsAction::~LineEndsAction()
{
}

void LineEndsAction::initComboBox(TKComboBox* combo)
{
  TKBaseSelectAction::initComboBox(combo);

  int cindex = 0;

  int tw = combo->fontMetrics().width(" 99:");

  QBitmap mask;
  QPixmap pixAll(lineedns);
  if (!m_rightends) {
    QPixmap tpix(pixAll.width(),pixAll.height());
    QPainter tp(&tpix);
    tp.scale(-1,1);
    tp.drawPixmap(-pixAll.width()+1,0,pixAll);
    tp.end();
    pixAll = tpix;
  }

  QPixmap pix(pixAll.width()+tw+3,17);
  QPainter p(&pix,combo);

  // insert item "0: None"
  pix.fill(white);
  p.drawText(0,0,tw,pix.height(),AlignRight|AlignVCenter,QString("%1:").arg(cindex));
  p.drawText(tw+3,0,pix.width()-tw-3,pix.height(),AlignLeft|AlignVCenter,"None");
  mask = pix;
  pix.setMask(mask);
  combo->insertItem(pix,cindex++);

  for (int y=0; y<pixAll.height(); y+= 17 ) {
    pix.fill(white);
    p.drawText(0,0,tw,pix.height(),AlignRight|AlignVCenter,QString("%1:").arg(cindex));
    p.drawPixmap(tw+3,0,pixAll,0,y,pix.width(),pix.height());

    mask = pix;
    pix.setMask(mask);
    combo->insertItem(pix,cindex++);
  }
  p.end();

}
#include "kivio_lineendsaction.moc"
