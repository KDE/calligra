//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#ifndef __layerlist_h__
#define __layerlist_h__

#include <qtableview.h>
#include <qrect.h>

class KImageShopDoc;
class QPopupMenu;

class LayerView : public QTableView
{
  Q_OBJECT

public:

  enum action { VISIBLE, SELECT, LINKING, OPACITY, RENAME, ADDLAYER, REMOVELAYER, ADDMASK, REMOVEMASK, UPPERLAYER, LOWERLAYER, FRONTLAYER, BACKLAYER };

  LayerView(QWidget* _parent = 0, const char* _name = 0 );
  LayerView(KImageShopDoc* doc, QWidget* _parent = 0, const char* name = 0 );

  void updateTable();
  void updateList();
  void update_contextmenu( int _index );

  void swapLayers( int a, int b );
  void selectLayer( int _index );
  void slotInverseVisibility( int _index );
  void slotInverseLinking( int _index );
  void slotRenameLayer( int _index );

  virtual QSize sizeHint() const;

public slots:

  void slotMenuAction( int );
  void slotAddLayer();
  void slotRemoveLayer();
  void slotUpperLayer();
  void slotLowerLayer();
  void slotFrontLayer();
  void slotBackgroundLayer();

protected:

  virtual void paintCell( QPainter*, int _row, int _col );
  virtual void mousePressEvent( QMouseEvent* _event );

private:

  void init(KImageShopDoc* doc);

  KImageShopDoc* m_doc;
  int m_items, m_selected;
  static QPixmap *m_eyeIcon, *m_linkIcon;
  static QRect m_eyeRect, m_linkRect;
  QPopupMenu* m_contextmenu;
};

#endif // __layerlist_h__

