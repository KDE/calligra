/*
 *  layerview.h - part of KImageShop
 *
 *  Copyright (c) 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
 *                1999 Michael Koch    <koch@kde.org>
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

#ifndef __layerview_h__
#define __layerview_h__

#include <qdialog.h>
#include <qtableview.h>
#include <qrect.h>

class KisDoc;
class QPopupMenu;
class QLineEdit;
class KIntSpinBox;

class KisLayer;

class LayerView : public QTableView
{
  Q_OBJECT

public:

  enum action { VISIBLE, SELECTION, LINKING, PROPERTIES, ADDLAYER, REMOVELAYER, ADDMASK, REMOVEMASK, UPPERLAYER, LOWERLAYER, FRONTLAYER, BACKLAYER };

  LayerView(QWidget* _parent = 0, const char* _name = 0 );
  LayerView(KisDoc* doc, QWidget* _parent = 0, const char* name = 0 );

  void updateTable();
  void updateAllCells();
  void update_contextmenu( int _index );

  void swapLayers( int a, int b );
  void selectLayer( int _index );
  void slotInverseVisibility( int _index );
  void slotInverseLinking( int _index );
  void slotProperties();

  virtual QSize sizeHint() const;

public slots:

  void slotMenuAction( int );
  void slotAddLayer();
  void slotRemoveLayer();
  void slotRaiseLayer();
  void slotLowerLayer();
  void slotFrontLayer();
  void slotBackgroundLayer();

  void slotDocUpdated();

protected:

  virtual void paintCell( QPainter*, int _row, int _col );
  virtual void mousePressEvent( QMouseEvent* _event );
  virtual void mouseDoubleClickEvent( QMouseEvent* _event );

private:

  void init(KisDoc* doc);

  KisDoc* m_doc;
  int m_items, m_selected;
  QPopupMenu* m_contextmenu;
};

class LayerPropertyDialog : QDialog
{
  Q_OBJECT

public:

  static bool editProperties( KisLayer& _layer );

protected:

  LayerPropertyDialog( QString _layername, uchar _opacity, QWidget *_parent, const char *_name );

  QLineEdit *m_name;
  KIntSpinBox *m_spin;
};

#endif // __layerview_h__


