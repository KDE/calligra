/*
 *  layerdlg.h - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch    <koch@kde.org>
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

#ifndef __layerdlg_h__
#define __layerdlg_h__

#include <kfloatingtabdialog.h>

class KisDoc;

#define CELLWIDTH   200
#define CELLHEIGHT  40
#define MAXROWS	    8

class LayerTab : public QWidget
{
  Q_OBJECT

public:

  LayerTab( KisDoc* doc, QWidget* _parent = 0, const char* _name = 0, WFlags _flags = 0 );
};

class ChannelTab : public QWidget
{
  Q_OBJECT

public:

  ChannelTab( KisDoc* doc, QWidget* _parent = 0, const char* _name = 0, WFlags _flags = 0 );
};

class LayerDialog : public KFloatingTabDialog
{
  Q_OBJECT

public:

  LayerDialog( KisDoc* doc, QWidget* _parent = 0 );

  static QPixmap *m_eyeIcon, *m_linkIcon;
  static QRect m_eyeRect, m_linkRect, m_previewRect;

public slots:

  void slotTabSelected( int );

private:

  QWidget *m_pLayerTab, *m_pChannelTab;
};

#endif // __layerdlg_h__

