/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef LayerView_h_
#define LayerView_h_

#include <qtableview.h>
#include <qpixmap.h>
#include <qlist.h>

class QLineEdit;
class GDocument;
class GLayer;

class LayerView : public QTableView {

    Q_OBJECT

public:
    LayerView (QWidget *parent = 0L, const char *name = 0);
    ~LayerView ();

    virtual QSize sizeHint() const { return QSize(280, 100); }
    virtual QSize minimumSizeHint() const { return QSize(80, 60); }
    void setActiveDocument (GDocument* doc);

protected:
    void showLayers (const QList<GLayer>& lvec);

    virtual int cellWidth (int col);
    virtual int cellHeight (int row);
    virtual void paintCell (QPainter *, int row, int col);

    virtual void mousePressEvent (QMouseEvent *event);
    virtual void mouseDoubleClickEvent (QMouseEvent *event);

private slots:
    void lineEditorSlot ();

private:
    GDocument* document;
    QList<GLayer> layers;
    QPixmap pixmaps[3];
    QLineEdit* lineEditor;
    int editorRow;
};

#endif
