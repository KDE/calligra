/* -*- C++ -*-

  This file is part of Kontour.
  Copyright (C) 2001 Rob Buis (rwlbuis@wanadoo.nl)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

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
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qcombobox.h>

#include <koColorChooser.h>
#include <klocale.h>
#include <ktoolbarbutton.h>
#include <kdebug.h>

#include "GStyle.h"
#include "BrushCells.h"

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

/*  QGroupBox *pattern = new QGroupBox(1, Qt::Vertical, this);
  QBoxLayout *box2 = new QBoxLayout(pattern, QBoxLayout::Down);
  BrushCells *brushCells = new BrushCells(pattern);
  connect(brushCells, SIGNAL(brushChanged(Qt::BrushStyle)), this, SIGNAL(changeBrushStyle(Qt::BrushStyle)));
  box2->addWidget(brushCells);
  insertTab(pattern, i18n("Pattern"));*/
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

  QWidget *mOutline = new QWidget(this);
  QGridLayout *mOutlineLayout = new QGridLayout(mOutline, 4, 2);

  mStroked = new QCheckBox(i18n("stroked"), mOutline);
  connect(mStroked, SIGNAL(toggled(bool)), this, SIGNAL(changeStroked(bool)));

  QLabel *mStartArrowText = new QLabel(i18n("Start Arrow"), mOutline);
  mStartArrowBox = new QComboBox(mOutline); // It's only temporary
  mStartArrowBox->insertItem("no");
  mStartArrowBox->insertItem("<-");
  mStartArrowBox->insertItem("<<-");
  mStartArrowBox->setCurrentItem(0);

  QLabel *mEndArrowText = new QLabel(i18n("End Arrow"), mOutline);
  mEndArrowBox = new QComboBox(mOutline); // It's only temporary
  mEndArrowBox->insertItem("no");
  mEndArrowBox->insertItem("->");
  mEndArrowBox->insertItem("->>");
  mEndArrowBox->setCurrentItem(0);

  QLabel *mOpacityText = new QLabel(i18n("Opacity"), mOutline);
  mOpacityBox = new QSpinBox(0, 100, 5, mOutline);
  mOpacityBox->setSuffix("%");

  mOutlineLayout->addMultiCellWidget(mStroked, 0, 0, 0, 1);
  mOutlineLayout->addWidget(mStartArrowText, 1, 0);
  mOutlineLayout->addWidget(mStartArrowBox, 1, 1);
  mOutlineLayout->addWidget(mEndArrowText, 2, 0);
  mOutlineLayout->addWidget(mEndArrowBox, 2, 1);
  mOutlineLayout->addWidget(mOpacityText, 3, 0);
  mOutlineLayout->addWidget(mOpacityBox, 3, 1);

  insertTab(mOutline, i18n("Stroking"));


  /* Color tab */
  mOutlinePanel = new KoColorChooser(this);
  connect(mOutlinePanel, SIGNAL(colorChanged(const KoColor &)), this, SIGNAL(changeOutlineColor(const KoColor &)));
  connect(this, SIGNAL(colorChanged(const KoColor &)), mOutlinePanel, SLOT(slotChangeColor(const KoColor &)));
  insertTab(mOutlinePanel, i18n("Color"));

  /* Style tab */

  QWidget *mOutlineStyle = new QWidget(this);
  QGridLayout *mOutlineStyleLayout = new QGridLayout(mOutlineStyle, 4, 2);

  /* Outline width */
  QLabel *mWidthText = new QLabel(i18n("Width"), mOutlineStyle);
  mWidthBox = new QSpinBox(0, 100, 1, mOutlineStyle);
  connect(mWidthBox, SIGNAL(valueChanged(int)), this, SLOT(slotChangeLineWidth(int)));

  QLabel *mLineStyleText = new QLabel(i18n("Line Style"), mOutlineStyle);
  mLineStyleBox = new QComboBox(mOutlineStyle); // It's only temporary
  mLineStyleBox->insertItem("----------");
  mLineStyleBox->insertItem("- - - - - ");
  mLineStyleBox->insertItem("-  -  -  -");
  mLineStyleBox->setCurrentItem(0);

  /* Join style selection */
  mJoinBox = new QButtonGroup(3, Qt::Horizontal, mOutlineStyle);
  mJoinBox->setFrameStyle(QFrame::NoFrame);
//  mJoinBox->setMargin(1);
  mJoinBox->setFixedHeight(22); // remove it
  mJoinBox->setExclusive(true);
  QPushButton *mRoundBtn = new QPushButton(mJoinBox);
  mRoundBtn->setToggleButton(true);
  mRoundBtn->setFixedWidth(20);
  mRoundBtn->setFixedHeight(20);
  QPushButton *mMiterBtn = new QPushButton(mJoinBox);
  mMiterBtn->setToggleButton(true);
  mMiterBtn->setFixedWidth(20);
  mMiterBtn->setFixedHeight(20);
  QPushButton *mBevelBtn = new QPushButton(mJoinBox);
  mBevelBtn->setToggleButton(true);
  mBevelBtn->setFixedWidth(20);
  mBevelBtn->setFixedHeight(20);
  connect(mJoinBox, SIGNAL(pressed(int)), this, SLOT(slotJoinPressed(int)));

  QLabel *mJoinText = new QLabel(i18n("Join"), mOutlineStyle);
  
/*  QPixmap pix(JOIN_WIDTH, JOIN_HEIGHT);
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
  bevel->setPixmap(pix);*/

  /* Cap style selection */
  mCapBox = new QButtonGroup(3, Qt::Horizontal, mOutlineStyle);
  mCapBox->setFrameStyle(QFrame::NoFrame);
  mCapBox->setFixedHeight(22); // remove it
  mCapBox->setExclusive(true);
  QPushButton *mCRoundBtn = new QPushButton(mCapBox);
  mCRoundBtn->setToggleButton(true);
  mCRoundBtn->setFixedWidth(20);
  mCRoundBtn->setFixedHeight(20);
  QPushButton *mSquareBtn = new QPushButton(mCapBox);
  mSquareBtn->setToggleButton(true);
  mSquareBtn->setFixedWidth(20);
  mSquareBtn->setFixedHeight(20);
  QPushButton *mFlatBtn = new QPushButton(mCapBox);
  mFlatBtn->setToggleButton(true);
  mFlatBtn->setFixedWidth(20);
  mFlatBtn->setFixedHeight(20);
  connect(mCapBox, SIGNAL(pressed(int)), this, SLOT(slotJoinPressed(int)));

  QLabel *mCapText = new QLabel(i18n("Cap"), mOutlineStyle);

  mOutlineStyleLayout->addWidget(mWidthText, 0, 0);
  mOutlineStyleLayout->addWidget(mWidthBox, 0, 1);
  mOutlineStyleLayout->addWidget(mLineStyleText, 1, 0);
  mOutlineStyleLayout->addWidget(mLineStyleBox, 1, 1);
  mOutlineStyleLayout->addWidget(mJoinText, 2, 0);
  mOutlineStyleLayout->addWidget(mJoinBox, 2, 1);
  mOutlineStyleLayout->addWidget(mCapText, 3, 0);
  mOutlineStyleLayout->addWidget(mCapBox, 3, 1);

/*  mCapBox = new QButtonGroup(3, Qt::Horizontal, cap);
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
  flat->setPixmap(pix);*/

  insertTab(mOutlineStyle, i18n("Style"));
}

void OutlinePanel::slotChangeLineWidth(int lwidth)
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
  mWidthBox->blockSignals(true);
  mWidthBox->setValue(style.outlineWidth());
  mWidthBox->blockSignals(false);
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
