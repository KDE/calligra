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

#include <LayerView.h>

#include <qlineedit.h>
#include <kiconloader.h>

#include <GLayer.h>
#include <GDocument.h>

#define CELL_HEIGHT 25
#define CELL1_WIDTH 25
#define CELL2_WIDTH 200

LayerView::LayerView (QWidget *parent, const char *name)
  : QTableView (parent, name) {

    setNumCols (4);
    setBackgroundColor(colorGroup().base());
    document = 0L;

    pixmaps[0] = SmallIcon ("eye");
    pixmaps[1] = SmallIcon ("freehandtool");
    pixmaps[2] = BarIcon ("fileprint");

    setTableFlags (Tbl_autoScrollBars | Tbl_smoothScrolling);
    setFrameStyle (QFrame::Panel | QFrame::Sunken);
    setLineWidth (2);
    lineEditor = 0L;
    editorRow = -1;
}

LayerView::~LayerView () {
}

void LayerView::setActiveDocument (GDocument* doc) {
  document = doc;
  showLayers(document->getLayers ());
}

void LayerView::showLayers (const QList<GLayer>& lvec) {
  layers = lvec;
  setNumRows (layers.count());
  updateTableSize ();
  repaint ();
}

int LayerView::cellWidth (int col) {
  return (col == 3 ? CELL2_WIDTH : CELL1_WIDTH);
}

int LayerView::cellHeight (int) {
  return QMAX (CELL_HEIGHT, fontMetrics().lineSpacing () + 1);
}

void LayerView::paintCell (QPainter *p, int row, int col) {
  GLayer* layer = layers.at(numRows () - 1 - row);
  bool rowIsActive = (document->activeLayer () == layer);

  p->save ();
  p->setPen (rowIsActive ? colorGroup().highlightedText() : colorGroup().text());
  if (col < 3)
    p->fillRect (0, 0, CELL1_WIDTH, cellHeight (row),
                 QBrush (rowIsActive ? colorGroup().highlight() : colorGroup().base()));

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
      QFontMetrics fm = p->fontMetrics ();
      int yPos;
      if (CELL_HEIGHT < fm.height ())
        yPos = fm.ascent () + fm.leading () / 2;
      else
        yPos = (CELL_HEIGHT - fm.height ()) / 2 + fm.ascent ();
      if (editorRow == row) {
        if (lineEditor == 0L) {
          lineEditor = new QLineEdit (this);
          lineEditor->setMaxLength (20);
          lineEditor->setFrame (false);
          connect (lineEditor, SIGNAL(returnPressed ()),
                   this, SLOT(lineEditorSlot ()));
        }
        lineEditor->setGeometry (3 * CELL1_WIDTH + 3,
                                 CELL_HEIGHT * editorRow + 1,
                                 CELL2_WIDTH, CELL_HEIGHT);
        lineEditor->setEnabled (true);
        lineEditor->show ();
        lineEditor->setFocus ();
        lineEditor->setText (layer->name ());
      }
      else {
        // name
          p->fillRect (0, 0, CELL2_WIDTH, cellHeight(row),
                       QBrush (rowIsActive ? colorGroup().highlight() : colorGroup().base()));
          p->drawText (5, yPos, layer->name ());
      }
      break;
  }
  default:
    break;
  }
  p->restore ();
}

void LayerView::mouseDoubleClickEvent (QMouseEvent *event) {
  int row, col;

  row = findRow (event->y ());
  col = findCol (event->x ());

  if (row != -1 && col == 3) {
    editorRow = row;
    repaint ();
  }
}

void LayerView::mousePressEvent (QMouseEvent *event) {
  int row, col;

  row = findRow (event->y ());
  col = findCol (event->x ());

  if (row != -1 && col != -1) {
    if (editorRow != -1) {
      editorRow = -1;
      lineEditor->setEnabled (false);
      lineEditor->hide ();
    }
    else {
      GLayer* layer = layers.at(numRows () - 1 - row);

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
    }
    repaint ();
  }
}

void LayerView::lineEditorSlot () {
  GLayer* layer = layers.at(numRows () - 1 - editorRow);
  layer->setName (lineEditor->text ());
  lineEditor->setEnabled (false);
  lineEditor->hide ();
  repaint ();
}

#undef CELL_HEIGHT
#undef CELL1_WIDTH
#undef CELL2_WIDTH

#include <LayerView.moc>
