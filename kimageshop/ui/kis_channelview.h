/*
 *  kis_channelview.h - part of KImageShop
 *
 *  Copyright (c) 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
 *                1999 Michael Koch    <koch@kde.org>
 *                2000 Matthias Elter  <elter@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __kis_channelview_h__
#define __kis_channelview_h__

#define CELLWIDTH   200
#define CELLHEIGHT  40
#define MAXROWS	    8

#include <qdialog.h>
#include <qtableview.h>
#include <qrect.h>

class KisDoc;
class QPopupMenu;

class KisChannelView : public QWidget
{
  Q_OBJECT

public:
  KisChannelView( KisDoc* doc, QWidget* _parent = 0,  const char* _name = 0 );
};

class ChannelTable : public QTableView
{
  Q_OBJECT

public:

  enum action { VISIBLE, ADDCHANNEL, REMOVECHANNEL, RAISECHANNEL, LOWERCHANNEL };

  ChannelTable( QWidget *_parent = 0, const char *_name = 0 );
  ChannelTable( KisDoc *_doc, QWidget *_parent = 0, const char *_name = 0 );

  void updateTable();
  void updateAllCells();
  void update_contextmenu( int _index );

  void selectChannel( int _index );
  void slotInverseVisibility( int _index );

  virtual QSize sizeHint() const;

public slots:

  void slotMenuAction( int );
  void slotAddChannel();
  void slotRemoveChannel();

protected:

  virtual void paintCell( QPainter*, int _row, int _col );
  virtual void mousePressEvent( QMouseEvent *_event );

private:

  void init(KisDoc* doc);

  KisDoc* m_doc;
  int m_items, m_selected;
  QPopupMenu* m_contextmenu;
  QPixmap *m_eyeIcon, *m_linkIcon;
  QRect m_eyeRect, m_linkRect, m_previewRect;
};

#endif


