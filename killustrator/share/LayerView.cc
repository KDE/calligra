/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include "LayerView.h"
#include "LayerView.moc"

#include <kapp.h>
#include <kiconloader.h>

#include <iostream.h>

#define CELL_HEIGHT 25
#define CELL1_WIDTH 25
#define CELL2_WIDTH 200

LayerView::LayerView (QWidget *parent, const char *name) 
  : QTableView (parent, name) {
  setNumCols (4);
  setBackgroundColor (white);
  
  document = 0L;
  KIconLoader* loader = kapp->getIconLoader ();
  
  pixmaps[0] = loader->loadIcon ("eye.xpm");
  pixmaps[1] = loader->loadIcon ("freehandtool.xpm");
  pixmaps[2] = loader->loadIcon ("fileprint.xpm");
  setMinimumSize (3 * CELL1_WIDTH + CELL2_WIDTH, 4 * CELL_HEIGHT);
  setTableFlags (Tbl_autoScrollBars | Tbl_smoothScrolling);
  setFrameStyle (QFrame::Panel | QFrame::Sunken);
  setLineWidth (2);
}

LayerView::~LayerView () {
}

void LayerView::setActiveDocument (GDocument* doc) {
  document = doc;
  showLayers (document->getLayers ());
}

void LayerView::showLayers (const vector<GLayer*>& lvec) {
  layers = lvec;
  setNumRows (layers.size ());
  updateTableSize ();
  repaint ();
}

int LayerView::cellWidth (int col) {
  return (col == 3 ? CELL2_WIDTH : CELL1_WIDTH);
}

int LayerView::cellHeight (int row) {
  return QMAX (CELL_HEIGHT, fontMetrics ().lineSpacing () + 1);
}

void LayerView::paintCell (QPainter *p, int row, int col) {
  GLayer* layer = layers[numRows () - 1 - row];
  bool rowIsActive = (document->activeLayer () == layer);

  p->save ();
  p->setPen (rowIsActive ? white : black);
  if (col < 3)
    p->fillRect (0, 0, CELL1_WIDTH, cellHeight (row), 
		 QBrush (rowIsActive ? darkBlue : white));

  switch (col) {
  case 0:
    // visible
    if (layer->isVisible ())
      p->drawPixmap (2, 2, pixmaps[col]);
    break;
  case 1:
    // editable
    if (layer->isEditable ())
      p->drawPixmap (2, 2, pixmaps[col]);
    break;
  case 2:
    // printable
    if (layer->isPrintable ())
      p->drawPixmap (2, 2, pixmaps[col]);
    break;
  case 3:
    {
      // name
      p->fillRect (0, 0, width (), cellHeight (row),
		   QBrush (rowIsActive ? darkBlue : white));
      QFontMetrics fm = p->fontMetrics ();
      int yPos;
      if (CELL_HEIGHT < fm.height ())
	yPos = fm.ascent () + fm.leading () / 2;
      else
	yPos = (CELL_HEIGHT - fm.height ()) / 2 + fm.ascent ();
      p->drawText (5, yPos, layer->name ());
      break;
    }
  default:
    break;
  }
  p->restore ();
}

void LayerView::mousePressEvent (QMouseEvent *event) {
  int row, col;
  
  row = findRow (event->y ());
  col = findCol (event->x ());
  
  if (row != -1 && col != -1) {
    GLayer* layer = layers[numRows () - 1 - row];
    
    switch (col) {
    case 0:
      layer->setVisible (! layer->isVisible ());
      break;
    case 1:
      layer->setEditable (! layer->isEditable ());
      break;
    case 2:
      layer->setPrintable (! layer->isPrintable ());
      break;
    case 3:
      document->setActiveLayer (layer);
      break;
    default:
      break;
    }
    repaint ();
  }
}

