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

#include "canvas.h"

class LayerList : public QTableView
{
  Q_OBJECT

public:

  LayerList(QWidget* _parent = 0, const char* _name = 0 );
  LayerList(Canvas* _canvas, QWidget* _parent = 0, const char* name = 0 );

  void updateTable();
  void updateList();

  virtual QSize sizeHint() const;

protected:

  virtual void paintCell( QPainter*, int _row, int _col );
  virtual void mousePressEvent( QMouseEvent* _event );

private:

  void init( Canvas* _canvas );

  Canvas* m_canvas;
  int m_items, m_selected;
  static QPixmap *m_eyeIcon, *m_linkIcon;
  static QRect m_eyeRect, m_linkRect;
};

#endif // __layerlist_h__

