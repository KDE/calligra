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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kiviostencilformatdlg.h"

#include <qlabel.h>
#include <qlayout.h>

#include <kcolorbutton.h>
#include <klocale.h>

#include <koUnitWidgets.h>

#include "kivio_view.h"

KivioStencilFormatDlg::KivioStencilFormatDlg(KivioView* parent, const char* name)
  : KDialogBase(parent, name, true, i18n("Stencil & Connector Format"), Ok|Cancel|Default, Ok)
{
  m_unit = KoUnit::U_PT;
  init();
}

void KivioStencilFormatDlg::init()
{
  QWidget* mainWidget = new QWidget(this);
  setMainWidget(mainWidget);
  QGridLayout* gl = new QGridLayout(mainWidget, 3, 2, KDialog::marginHint(), KDialog::spacingHint());

  QLabel* lineWidthLbl = new QLabel(i18n("Line &width:"), mainWidget);
  m_lineWidthUSBox = new KoUnitDoubleSpinBox(mainWidget, 0.0, 1000.0, 0.1, 1.0, m_unit, 2);
  lineWidthLbl->setBuddy(m_lineWidthUSBox);
  QLabel* lineColorLbl = new QLabel(i18n("Line &color:"), mainWidget);
  m_lineCBtn = new KColorButton(mainWidget);
  lineColorLbl->setBuddy(m_lineCBtn);
  QLabel* fillColorLbl = new QLabel(i18n("&Fill color:"), mainWidget);
  m_fillCBtn = new KColorButton(mainWidget);
  fillColorLbl->setBuddy(m_fillCBtn);

  gl->addWidget(lineWidthLbl, 0, 0);
  gl->addWidget(m_lineWidthUSBox, 0, 1);
  gl->addWidget(lineColorLbl, 1, 0);
  gl->addWidget(m_lineCBtn, 1, 1);
  gl->addWidget(fillColorLbl, 2, 0);
  gl->addWidget(m_fillCBtn, 2, 1);
}

double KivioStencilFormatDlg::lineWidth()
{
  return KoUnit::ptFromUnit(m_lineWidthUSBox->value(), m_unit);
}

QColor KivioStencilFormatDlg::lineColor()
{
  return m_lineCBtn->color();
}

QColor KivioStencilFormatDlg::fillColor()
{
  return m_fillCBtn->color();
}

void KivioStencilFormatDlg::setLineWidth(double w, KoUnit::Unit u)
{
  m_unit = u;
  m_lineWidthUSBox->setUnit(u);
  m_lineWidthUSBox->setValue(KoUnit::ptToUnit(w, u));
}

void KivioStencilFormatDlg::setLineColor(QColor c)
{
  m_lineCBtn->setColor(c);
}

void KivioStencilFormatDlg::setFillColor(QColor c)
{
  m_fillCBtn->setColor(c);
}

void KivioStencilFormatDlg::slotDefault()
{
  setLineWidth(1.0, m_unit);
  setLineColor(QColor(0, 0, 0));
  setFillColor(QColor(255, 255, 255));
}

#include "kiviostencilformatdlg.moc"
