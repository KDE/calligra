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

class LayerList : public QTableView
{
  Q_OBJECT

public:

  LayerList(QWidget* _parent = 0, const char* _name = 0 );
  LayerList(KImageShopDoc* doc, QWidget* _parent = 0, const char* name = 0 );

  void updateTable();
  void updateList();
  void update_contextmenu( int _index );

  void selectLayer( int _index );
  void inverseVisibility( int _index );
  void inverseLinking( int _index );
  void renameLayer( int _index );
  void addLayer( int _index );
  void removeLayer( int _index );

  virtual QSize sizeHint() const;

public slots:

  void slotMenuAction( int );

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

