/*
 *  channelview.h - part of KImageShop
 *
 *  Copyright (c) 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
 *                1999 Michael Koch    <mkoch@kde.org>
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

#ifndef __channelview_h__
#define __channelview_h__

#include <qdialog.h>
#include <qtableview.h>
#include <qrect.h>

class KImageShopDoc;
class QPopupMenu;

class ChannelView : public QTableView
{
  Q_OBJECT

public:

  enum action { VISIBLE, ADDCHANNEL, REMOVECHANNEL, RAISECHANNEL, LOWERCHANNEL };

  ChannelView( QWidget *_parent = 0, const char *_name = 0 );
  ChannelView( KImageShopDoc *_doc, QWidget *_parent = 0, const char *_name = 0 );

  void updateTable();
  void updateAllCells();
  void update_contextmenu( int _index );

  void swapChannels( int a, int b );
  void selectChannel( int _index );
  void slotInverseVisibility( int _index );

  virtual QSize sizeHint() const;

public slots:

  void slotMenuAction( int );
  void slotAddChannel();
  void slotRemoveChannel();
  void slotRaiseChannel();
  void slotLowerChannel();

protected:

  virtual void paintCell( QPainter*, int _row, int _col );
  virtual void mousePressEvent( QMouseEvent *_event );

private:

  void init(KImageShopDoc* doc);

  KImageShopDoc* m_doc;
  int m_items, m_selected;
  QPopupMenu* m_contextmenu;
};

#endif


