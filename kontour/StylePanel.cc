/* -*- C++ -*-

  This file is part of Kontour.
  Copyright (C) 2001 Rob Buis (rwlbuis@wanadoo..nl)

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

#include "StylePanel.h"

#include <qlayout.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qgrid.h>
#include <qlabel.h>
#include <qvgroupbox.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qcombobox.h>

#include <koColorChooser.h>
#include <klocale.h>
#include <ktoolbarbutton.h>
#include <kdebug.h>

#include "GStyle.h"
#include "BrushCells.h"

#define JOIN_WIDTH  30
#define JOIN_HEIGHT 30
#define CAP_WIDTH   30
#define CAP_HEIGHT  30

PaintPanel::PaintPanel(QWidget *parent, const char *name):
QTabWidget(parent, name)
{
  setTabShape(Triangular);

  QWidget *mPainting = new QWidget(this);
  QGridLayout *mPaintingLayout = new QGridLayout(mPainting, 3, 2);
  mPaintingBox = new QComboBox(mPainting);
  mPaintingBox->insertItem(i18n("No"));
  mPaintingBox->insertItem(i18n("Color"));
  mPaintingBox->insertItem(i18n("Gradient"));
  mPaintingBox->insertItem(i18n("Pattern"));
  mPaintingBox->setCurrentItem(0);
  mPaintingLayout->addWidget(mPaintingBox, 1, 1);
  insertTab(mPainting, i18n("Painting"));

  KoColorChooser *mPaintPanel = new KoColorChooser(this);
  connect(mPaintPanel, SIGNAL(colorChanged(const KoColor &)), this, SIGNAL(changePaintColor(const KoColor &)));
  connect(this, SIGNAL(colorChanged(const KoColor &)), mPaintPanel, SLOT(slotChangeColor(const KoColor &)));
  insertTab(mPaintPanel, i18n("Color"));

  QGroupBox *pattern = new QGroupBox(1, Qt::Vertical, this);
  QBoxLayout *box2 = new QBoxLayout(pattern, QBoxLayout::Down);
  BrushCells *brushCells = new BrushCells(pattern);
  connect(brushCells, SIGNAL(brushChanged(Qt::BrushStyle)), this, SIGNAL(changeBrushStyle(Qt::BrushStyle)));
  box2->addWidget(brushCells);
  insertTab(pattern, i18n("Pattern"));


  connect(mPaintingBox, SIGNAL(activated(int)), this, SLOT(activate(int)));
}

void PaintPanel::slotStyleChanged(const GStyle &style)
{
  // TODO : I fear the color chooser will send a signal back
  //        blocking doesnt work, disconnect maybe ?
  //mPaintPanel->blockSignals(true);
  emit colorChanged(style.fillColor());
  //mPaintPanel->blockSignals(false);
}

void PaintPanel::activate(int f)
{

}

OutlinePanel::OutlinePanel(QWidget *parent, const char *name):
QTabWidget(parent, name)
{
  setTabShape(Triangular);
  /* Color tab */
  QGroupBox *outlineColor = new QGroupBox(2, Qt::Vertical, this);
  mStroked = new QCheckBox(i18n("stroked"), outlineColor);
  connect(mStroked, SIGNAL(toggled(bool)), this, SIGNAL(changeStroked(bool)));
  mOutlinePanel = new KoColorChooser(outlineColor);
  connect(mOutlinePanel, SIGNAL(colorChanged(const KoColor &)), this, SIGNAL(changeOutlineColor(const KoColor &)));
  connect(this, SIGNAL(colorChanged(const KoColor &)), mOutlinePanel, SLOT(slotChangeColor(const KoColor &)));

  /* Style tab */
  QVGroupBox *outlineStyle = new QVGroupBox(this);

  /* Outline width */
  QWidget *lwidth = new QWidget(outlineStyle);
  QHBoxLayout *lay = new QHBoxLayout(lwidth);
  QLabel *lwidthText = new QLabel(i18n("Width"), lwidth);
  lay->addWidget(lwidthText);
  mlwidthBox = new QSpinBox(0, 100, 1, lwidth);
  connect(mlwidthBox, SIGNAL(valueChanged(int)), this, SLOT(slotChangeLinewidth(int)));
  lay->addWidget(mlwidthBox);

  /* Join style selection */
  QGrid *join = new QGrid(4, outlineStyle);
  join->setSpacing(70);
  join->setMargin(0);
  QLabel *joinText = new QLabel(i18n("Join"), join);
  joinText->setMargin(0);
  mJoinBox = new QButtonGroup(3, Qt::Horizontal, join);
  mJoinBox->setFrameStyle(QFrame::NoFrame);
  mJoinBox->setExclusive(true);
  QPushButton *round = new QPushButton(mJoinBox);
  round->setToggleButton(true);
  round->setMaximumWidth(JOIN_WIDTH);
  round->setMaximumHeight(JOIN_HEIGHT);
  /*round->setMinimumWidth(JOIN_WIDTH);
  round->setMinimumHeight(JOIN_HEIGHT);*/
  QPixmap pix(JOIN_WIDTH, JOIN_HEIGHT);
  QBitmap bmap(JOIN_WIDTH, JOIN_HEIGHT);
  pix.fill();
  QPainter p(&pix);
  QPen pen;
  pen.setColor(Qt::black);
  pen.setWidth(8);
  pen.setJoinStyle(Qt::RoundJoin);
  p.setPen(pen);
  QPointArray pa;
  pa.setPoints(3, JOIN_WIDTH / 3, JOIN_HEIGHT - 1, JOIN_WIDTH / 3, JOIN_HEIGHT / 3, JOIN_WIDTH - 1, JOIN_HEIGHT / 3);
  p.drawPolyline(pa);
  bmap = pix;
  pix.setMask(bmap);
  round->setPixmap(pix);

  QPushButton *miter = new QPushButton(mJoinBox);
  miter->setToggleButton(true);
  miter->setMaximumWidth(JOIN_WIDTH);
  miter->setMaximumHeight(JOIN_HEIGHT);
  pix.fill();
  pen.setJoinStyle(Qt::MiterJoin);
  p.setPen(pen);
  p.drawPolyline(pa);
  bmap = pix;
  pix.setMask(bmap);
  miter->setPixmap(pix);

  QPushButton *bevel = new QPushButton(mJoinBox);
  bevel->setToggleButton(true);
  bevel->setMaximumWidth(JOIN_WIDTH);
  bevel->setMaximumHeight(JOIN_HEIGHT);
  pix.fill();
  pen.setJoinStyle(Qt::BevelJoin);
  p.setPen(pen);
  p.drawPolyline(pa);
  bmap = pix;
  pix.setMask(bmap);
  bevel->setPixmap(pix);

  connect(mJoinBox, SIGNAL(pressed(int)), this, SLOT(slotJoinPressed(int)));

  /* Cap style selection */
  QGrid *cap = new QGrid(4, outlineStyle);
  cap->setSpacing(70);
  cap->setMargin(0);
  QLabel *capText = new QLabel(i18n("Cap"), cap);
  mCapBox = new QButtonGroup(3, Qt::Horizontal, cap);
  mCapBox->setFrameStyle(QFrame::NoFrame);
  mCapBox->setExclusive(true);
  QPushButton *cround = new QPushButton(mCapBox);
  cround->setToggleButton(true);
  cround->setMaximumWidth(CAP_WIDTH);
  cround->setMaximumHeight(CAP_HEIGHT);
  pix.fill();
  pen.setCapStyle(Qt::RoundCap);
  p.setPen(pen);
  pa.setPoints(2, CAP_WIDTH / 3, CAP_HEIGHT / 2, CAP_WIDTH - 1, CAP_HEIGHT / 2);
  p.drawPolyline(pa);
  bmap = pix;
  pix.setMask(bmap);
  cround->setPixmap(pix);

  QPushButton *square = new QPushButton(mCapBox);
  square->setToggleButton(true);
  square->setMaximumWidth(CAP_WIDTH);
  square->setMaximumHeight(CAP_HEIGHT);
  pix.fill();
  pen.setCapStyle(Qt::SquareCap);
  p.setPen(pen);
  p.drawPolyline(pa);
  bmap = pix;
  pix.setMask(bmap);
  square->setPixmap(pix);

  QPushButton *flat = new QPushButton(mCapBox);
  flat->setToggleButton(true);
  flat->setMaximumWidth(CAP_WIDTH);
  flat->setMaximumHeight(CAP_HEIGHT);
  pix.fill();
  pen.setCapStyle(Qt::FlatCap);
  p.setPen(pen);
  p.drawPolyline(pa);
  bmap = pix;
  pix.setMask(bmap);
  flat->setPixmap(pix);

  connect(mCapBox, SIGNAL(pressed(int)), this, SLOT(slotCapPressed(int)));

  insertTab(outlineColor, i18n("Color"));
  insertTab(outlineStyle, i18n("Style"));
}

void OutlinePanel::slotChangeLinewidth(int lwidth)
{
  emit changeLinewidth((unsigned int)lwidth);
}

void OutlinePanel::slotJoinPressed(int which)
{
  Qt::PenJoinStyle style;
  switch(which)
  {
    case 0: style = Qt::RoundJoin; break;
	case 1: style = Qt::MiterJoin; break;
	case 2: style = Qt::BevelJoin;
  }
  emit changeJoinStyle(style);
}

void OutlinePanel::slotCapPressed(int which)
{
  Qt::PenCapStyle style;
  switch(which)
  {
    case 0: style = Qt::RoundCap; break;
	case 1: style = Qt::SquareCap; break;
	case 2: style = Qt::FlatCap;
  }
  emit changeCapStyle(style);
}

void OutlinePanel::slotStyleChanged(const GStyle &style)
{
  mStroked->setChecked(style.stroked());
  mlwidthBox->blockSignals(true);
  mlwidthBox->setValue(style.outlineWidth());
  mlwidthBox->blockSignals(false);
  //mOutlinePanel->blockSignals(true);
  emit colorChanged(style.outlineColor());
  //mOutlinePanel->blockSignals(false);
  switch(style.joinStyle())
  {
    case Qt::RoundJoin: mJoinBox->setButton(0); break;
	case Qt::MiterJoin: mJoinBox->setButton(1); break;
	case Qt::BevelJoin: mJoinBox->setButton(2);
  }
  switch(style.capStyle())
  {
    case Qt::RoundCap: mCapBox->setButton(0); break;
	case Qt::SquareCap: mCapBox->setButton(1); break;
	case Qt::FlatCap: mCapBox->setButton(2);
  }
}

#include "StylePanel.moc"
