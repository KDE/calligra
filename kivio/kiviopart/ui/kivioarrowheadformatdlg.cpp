/* This file is part of the KDE project
   Copyright (C) 2003 Peter Simonsson <psn@linux.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kivioarrowheadformatdlg.h"

#include <qlabel.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qframe.h>

#include <kcombobox.h>
#include <klocale.h>
#include <kiconloader.h>

#include <KoUnitWidgets.h>

#include "kivio_view.h"
#include "kivioglobal.h"

KivioArrowHeadFormatDlg::KivioArrowHeadFormatDlg(KivioView* parent, const char* name)
  : KDialogBase(parent, name, true, i18n("Arrowhead Format"), Ok|Cancel|Default, Ok)
{
  m_unit = KoUnit::U_PT;
  init();
}

void KivioArrowHeadFormatDlg::init()
{
  QWidget* mainWidget = new QWidget(this);
  setMainWidget(mainWidget);
  QGridLayout* gl = new QGridLayout(mainWidget, 2, 1, 0, KDialog::spacingHint());

  QLabel* startAHHeaderIcon = new QLabel(mainWidget);
  startAHHeaderIcon->setPixmap(SmallIcon("start_arrowhead", 16));
  startAHHeaderIcon->setFixedSize(16, 16);
  QLabel* startAHHeaderLabel = new QLabel(i18n("<b>Arrowhead at Origin</b>"), mainWidget);
  QFont font = startAHHeaderLabel->font();
  QFrame* startAHHeaderLine = new QFrame(mainWidget);
  startAHHeaderLine->setFrameStyle(QFrame::HLine|QFrame::Plain);
  QLabel* startAHTypeLbl = new QLabel(i18n("&Type:"), mainWidget);
  m_startAHTypeCBox = new KComboBox(mainWidget);
  loadArrowHeads(m_startAHTypeCBox);
  startAHTypeLbl->setBuddy(m_startAHTypeCBox);
  QLabel* startAHWidthLbl = new QLabel(i18n("&Width:"), mainWidget);
  m_startAHWidthUSBox = new KoUnitDoubleSpinBox(mainWidget, 0.0, 1000.0, 0.1, 1.0, m_unit, 2);
  startAHWidthLbl->setBuddy(m_startAHWidthUSBox);
  QLabel* startAHHeightLbl = new QLabel(i18n("&Length:"), mainWidget);
  m_startAHHeightUSBox = new KoUnitDoubleSpinBox(mainWidget, 0.0, 1000.0, 0.1, 1.0, m_unit, 2);
  startAHHeightLbl->setBuddy(m_startAHHeightUSBox);

  QLabel* endAHHeaderIcon = new QLabel(mainWidget);
  endAHHeaderIcon->setPixmap(SmallIcon("end_arrowhead", 16));
  endAHHeaderIcon->setFixedSize(16, 16);
  QLabel* endAHHeaderLabel = new QLabel(i18n("<b>Arrowhead at End</b>"), mainWidget);
  QFrame* endAHHeaderLine = new QFrame(mainWidget);
  endAHHeaderLine->setFrameStyle(QFrame::HLine|QFrame::Plain);
  QLabel* endAHTypeLbl = new QLabel(i18n("T&ype:"), mainWidget);
  m_endAHTypeCBox = new KComboBox(mainWidget);
  loadArrowHeads(m_endAHTypeCBox);
  endAHTypeLbl->setBuddy(m_endAHTypeCBox);
  QLabel* endAHWidthLbl = new QLabel(i18n("W&idth:"), mainWidget);
  m_endAHWidthUSBox = new KoUnitDoubleSpinBox(mainWidget, 0.0, 1000.0, 0.1, 1.0, m_unit, 2);
  endAHWidthLbl->setBuddy(m_endAHWidthUSBox);
  QLabel* endAHHeightLbl = new QLabel(i18n("L&ength:"), mainWidget);
  m_endAHHeightUSBox = new KoUnitDoubleSpinBox(mainWidget, 0.0, 1000.0, 0.1, 1.0, m_unit, 2);
  endAHHeightLbl->setBuddy(m_endAHHeightUSBox);

  gl->addWidget(startAHHeaderIcon, 0, 0);
  gl->addMultiCellWidget(startAHHeaderLabel, 0, 0, 1, 2);
  gl->addMultiCellWidget(startAHHeaderLine, 1, 1, 0, 2);
  gl->addMultiCellWidget(startAHTypeLbl, 2, 2, 0, 1);
  gl->addWidget(m_startAHTypeCBox, 2, 2);
  gl->addMultiCellWidget(startAHWidthLbl, 3, 3, 0, 1);
  gl->addWidget(m_startAHWidthUSBox, 3, 2);
  gl->addMultiCellWidget(startAHHeightLbl, 4, 4, 0, 1);
  gl->addWidget(m_startAHHeightUSBox, 4, 2);

  gl->addMultiCell(new QSpacerItem(KDialog::marginHint(), KDialog::marginHint(),
                   QSizePolicy::Minimum, QSizePolicy::Fixed), 5, 5, 0, 2);

  gl->addWidget(endAHHeaderIcon, 6, 0);
  gl->addMultiCellWidget(endAHHeaderLabel, 6, 6, 1, 2);
  gl->addMultiCellWidget(endAHHeaderLine, 7, 7, 0, 2);
  gl->addMultiCellWidget(endAHTypeLbl, 8, 8, 0, 1);
  gl->addWidget(m_endAHTypeCBox, 8, 2);
  gl->addMultiCellWidget(endAHWidthLbl, 9, 9, 0, 1);
  gl->addWidget(m_endAHWidthUSBox, 9, 2);
  gl->addMultiCellWidget(endAHHeightLbl, 10, 10, 0, 1);
  gl->addWidget(m_endAHHeightUSBox, 10, 2);
  gl->setRowStretch(11, 10);
}

void KivioArrowHeadFormatDlg::loadArrowHeads(KComboBox* combo)
{
  QBitmap mask;
  QPixmap pixAll = Kivio::arrowHeadPixmap();
  QPixmap pix(pixAll.width(), 17);
  QPainter p(&pix, combo);
  int cindex = 0;

  // insert item "None"
  combo->insertItem(i18n("no line end", "None"),cindex++);

  for (int y = 0; y < pixAll.height(); y += 17 ) {
    p.drawPixmap(0, 0, pixAll, 0, y, pix.width(), pix.height());
    combo->insertItem(pix,cindex++);
  }

  p.end();
}

int KivioArrowHeadFormatDlg::startAHType()
{
  return m_startAHTypeCBox->currentItem();
}

double KivioArrowHeadFormatDlg::startAHWidth()
{
  return m_startAHWidthUSBox->value();
}

double KivioArrowHeadFormatDlg::startAHHeight()
{
  return m_startAHHeightUSBox->value();
}

int KivioArrowHeadFormatDlg::endAHType()
{
  return m_endAHTypeCBox->currentItem();
}

double KivioArrowHeadFormatDlg::endAHWidth()
{
  return m_endAHWidthUSBox->value();
}

double KivioArrowHeadFormatDlg::endAHHeight()
{
  return m_endAHHeightUSBox->value();
}

void KivioArrowHeadFormatDlg::setUnit(KoUnit::Unit u)
{
  m_unit = u;
  m_startAHWidthUSBox->setUnit(u);
  m_startAHHeightUSBox->setUnit(u);
  m_endAHWidthUSBox->setUnit(u);
  m_endAHHeightUSBox->setUnit(u);
}

void KivioArrowHeadFormatDlg::setStartAHType(int t)
{
  m_startAHTypeCBox->setCurrentItem(t);
}

void KivioArrowHeadFormatDlg::setStartAHWidth(double w)
{
  m_startAHWidthUSBox->setValue(KoUnit::toUserValue(w, m_unit));
}

void KivioArrowHeadFormatDlg::setStartAHHeight(double h)
{
  m_startAHHeightUSBox->setValue(KoUnit::toUserValue(h, m_unit));
}

void KivioArrowHeadFormatDlg::setEndAHType(int t)
{
  m_endAHTypeCBox->setCurrentItem(t);
}

void KivioArrowHeadFormatDlg::setEndAHWidth(double w)
{
  m_endAHWidthUSBox->setValue(KoUnit::toUserValue(w, m_unit));
}

void KivioArrowHeadFormatDlg::setEndAHHeight(double h)
{
  m_endAHHeightUSBox->setValue(KoUnit::toUserValue(h, m_unit));
}

void KivioArrowHeadFormatDlg::slotDefault()
{
  setStartAHType(0);
  setEndAHType(0);
  setStartAHWidth(10.0);
  setStartAHHeight(10.0);
  setEndAHWidth(10.0);
  setEndAHHeight(10.0);
}

#include "kivioarrowheadformatdlg.moc"
