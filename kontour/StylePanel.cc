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
#include "GStyle.h"

#include <qlayout.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qvgroupbox.h>
#include <qgrid.h>
#include <qlabel.h>

#include <koColorChooser.h>
#include <klocale.h>

PaintPanel::PaintPanel(QWidget *parent, const char *name):
QTabWidget(parent, name)
{
  QGroupBox *paintColor = new QGroupBox(2, Qt::Vertical, this);
  QBoxLayout *box = new QBoxLayout(paintColor, QBoxLayout::Down);
  mFilled = new QCheckBox(i18n("filled"), paintColor);
  connect(mFilled, SIGNAL(toggled(bool)), this, SIGNAL(changeFilled(bool)));
  box->addWidget(mFilled);
  KoColorChooser *mPaintPanel = new KoColorChooser(paintColor);
  connect(mPaintPanel, SIGNAL(colorChanged(const KoColor &)), this, SIGNAL(changePaintColor(const KoColor &)));
  box->addWidget(mPaintPanel);
  insertTab(paintColor, "Color");
  // TODO : add some content here :)
  insertTab(new QWidget(this), "Gradient");
  insertTab(new QWidget(this), "Pattern");

}

void PaintPanel::slotStyleChanged(const GStyle &style)
{
  mFilled->setChecked(style.filled());
}

OutlinePanel::OutlinePanel(QWidget *parent, const char *name):
QTabWidget(parent, name)
{
  QGroupBox *outlineColor = new QGroupBox(2, Qt::Vertical, this);
  mStroked = new QCheckBox(i18n("stroked"), outlineColor);
  connect(mStroked, SIGNAL(toggled(bool)), this, SIGNAL(changeStroked(bool)));
  KoColorChooser *mOutlinePanel = new KoColorChooser(outlineColor);
  connect(mOutlinePanel, SIGNAL(colorChanged(const KoColor &)), this, SIGNAL(changeOutlineColor(const KoColor &)));
  insertTab(outlineColor, "Color");

  QVGroupBox *outlineStyle = new QVGroupBox(this);
  QWidget *lwidth = new QWidget(outlineStyle);
  QHBoxLayout *lay = new QHBoxLayout(lwidth);
  QLabel *lwidthText = new QLabel(i18n("Width"), lwidth);
  lay->addWidget(lwidthText);
  mlwidthBox = new QSpinBox(0, 100, 1, lwidth);
  connect(mlwidthBox, SIGNAL(valueChanged(int)), this, SLOT(slotChangeLinewidth(int)));
  lay->addWidget(mlwidthBox);
  insertTab(outlineStyle, "Style");

/*QGrid *join = new QGrid(4, outlineStyle);
QLabel *joinText = new QLabel(i18n("Join"), join);
KToolBarButton *round = new KToolBarButton("eye", 0, join);
KToolBarButton *miter = new KToolBarButton("miter", 1, join);
KToolBarButton *bevel = new KToolBarButton("bevel", 2, join);

QGrid *cap = new QGrid(4, outlineStyle);
QLabel *capText = new QLabel(i18n("Cap"), cap);
KToolBarButton *cround = new KToolBarButton("cround", 0, cap);
KToolBarButton *square = new KToolBarButton("square", 1, cap);
KToolBarButton *flat   = new KToolBarButton("flat", 2, cap);*/
}

void OutlinePanel::slotChangeLinewidth(int lwidth)
{
  emit changeLinewidth((unsigned int)lwidth);
}

void OutlinePanel::slotStyleChanged(const GStyle &style)
{
  mStroked->setChecked(style.stroked());
  mlwidthBox->blockSignals(true);
  mlwidthBox->setValue(style.outlineWidth());
  mlwidthBox->blockSignals(false);
}

#include "StylePanel.moc"
