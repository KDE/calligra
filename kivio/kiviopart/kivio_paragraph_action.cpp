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
#include "kivio_paragraph_action.h"
#include "tkcombobox.h"

#include <qlistbox.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include <qpainter.h>
#include "qdrawutil.h"

#include <ktoolbar.h>
#include <kapp.h>

/**********************************************************************************************/

static const char * align_left_xpm[] = {
"16 16 2 1",
" 	c None",
".	c #000000",
"                ",
" ........       ",
"                ",
" ............   ",
"                ",
" ..........     ",
"                ",
" ........       ",
"                ",
" ..........     ",
"                ",
" ....           ",
"                ",
" ..........     ",
"                ",
"                "};

static const char * align_center_xpm[] = {
"16 16 2 1",
" 	c None",
".	c #000000",
"                ",
"   ..........   ",
"                ",
" .............. ",
"                ",
"    ........    ",
"                ",
"  ............  ",
"                ",
"  ............  ",
"                ",
"   ..........   ",
"                ",
" .............. ",
"                ",
"                "};

static const char * align_right_xpm[] = {
"16 16 2 1",
" 	c None",
".	c #000000",
"                ",
"       ........ ",
"                ",
"   ............ ",
"                ",
"     .......... ",
"                ",
"       ........ ",
"                ",
"     .......... ",
"                ",
"           .... ",
"                ",
"     .......... ",
"                ",
"                "};

static const char * align_bottom_xpm[] = {
"16 16 2 1",
" 	c None",
".	c #000000",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
" ............   ",
" ............   ",
"                "};

static const char * align_middle_xpm[] = {
"16 16 2 1",
" 	c None",
".	c #000000",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
" ............   ",
" ............   ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                "};

static const char * align_top_xpm[] = {
"16 16 2 1",
" 	c None",
".	c #000000",
"                ",
" .............  ",
" .............  ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                "};


KivioParagraphAction::KivioParagraphAction( bool vertical, QObject* parent, const char* name )
: TKBaseSelectAction(parent,name)
{
  m_isVertical = vertical;
  setCurrentItem(m_isVertical ? Qt::AlignVCenter : Qt::AlignHCenter);
}

KivioParagraphAction::~KivioParagraphAction()
{
}

void KivioParagraphAction::initComboBox(TKComboBox* combo)
{
  TKBaseSelectAction::initComboBox(combo);

  if( m_isVertical==true )
  {
    combo->insertItem( QPixmap((const char **)align_top_xpm ));
    combo->insertItem( QPixmap((const char **)align_middle_xpm ));
    combo->insertItem( QPixmap((const char **)align_bottom_xpm ));
  }
  else
  {
    combo->insertItem( QPixmap((const char **)align_left_xpm ));
    combo->insertItem( QPixmap((const char **)align_center_xpm ));
    combo->insertItem( QPixmap((const char **)align_right_xpm ));
  }
}

void KivioParagraphAction::slotActivated(int id)
{
  int align;
  if( m_isVertical==true )
  {
    switch (id) {
      case 0:
        align = Qt::AlignTop;
        break;
      case 1:
        align = Qt::AlignVCenter;
        break;
      case 2:
        align = Qt::AlignBottom;
        break;
      default:
        align = Qt::AlignVCenter;
        break;
    }
  }
  else
  {
    switch (id) {
      case 0:
        align = Qt::AlignLeft;
        break;
      case 1:
        align = Qt::AlignHCenter;
        break;
      case 2:
        align = Qt::AlignRight;
        break;
      default:
        align = Qt::AlignHCenter;
        break;
    }
  }
  emit activated(align);
}

void KivioParagraphAction::setCurrentItem(int index)
{
  if( m_isVertical==true )
  {
    switch (index) {
      case Qt::AlignTop:
        index = 0;
        break;
      case Qt::AlignVCenter:
        index = 1;
        break;
      case Qt::AlignBottom:
        index = 2;
        break;
      default:
        index = 1;
        break;
    }
  }
  else
  {
    switch (index) {
      case Qt::AlignLeft:
        index = 0;
        break;
      case Qt::AlignHCenter:
        index = 1;
        break;
      case Qt::AlignRight:
        index = 2;
        break;
      default:
        index = 1;
        break;
    }
  }
  TKBaseSelectAction::setCurrentItem(index);
}
#include "kivio_paragraph_action.moc"
