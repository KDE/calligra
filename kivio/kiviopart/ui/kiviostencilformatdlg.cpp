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

#include "kiviostencilformatdlg.h"

#include <QLabel>
#include <QLayout>
#include <qbitmap.h>
#include <qpixmap.h>
#include <qpainter.h>

#include <kcolorbutton.h>
#include <klocale.h>

#include <KoUnitWidgets.h>

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
  QLabel* patternLbl = new QLabel(i18n("Line &style:"), mainWidget);
  m_patternCBox = new KComboBox(mainWidget);
  patternLbl->setBuddy(m_patternCBox);
  initLinePatterns();
  QLabel* lineEndStyleLbl = new QLabel(i18n("Line &end style:"), mainWidget);
  m_lineEndStyleCBox = new KComboBox(mainWidget);
  lineEndStyleLbl->setBuddy(m_lineEndStyleCBox);
  initLineEndStyles();
  QLabel* fillColorLbl = new QLabel(i18n("&Fill color:"), mainWidget);
  m_fillCBtn = new KColorButton(mainWidget);
  fillColorLbl->setBuddy(m_fillCBtn);
  QLabel* fillPatternLbl = new QLabel(i18n("F&ill style:"), mainWidget);
  m_fillPatternCBox = new KComboBox(mainWidget);
  fillPatternLbl->setBuddy(m_fillPatternCBox);
  initFillPatterns();

  gl->addWidget(lineWidthLbl, 0, 0);
  gl->addWidget(m_lineWidthUSBox, 0, 1);
  gl->addWidget(lineColorLbl, 1, 0);
  gl->addWidget(m_lineCBtn, 1, 1);
  gl->addWidget(patternLbl, 2, 0);
  gl->addWidget(m_patternCBox, 2, 1);
  gl->addWidget(lineEndStyleLbl, 3, 0);
  gl->addWidget(m_lineEndStyleCBox, 3, 1);
  gl->addWidget(fillColorLbl, 4, 0);
  gl->addWidget(m_fillCBtn, 4, 1);
  gl->addWidget(fillPatternLbl, 5, 0);
  gl->addWidget(m_fillPatternCBox, 5, 1);
}

void KivioStencilFormatDlg::initLinePatterns()
{
  QBitmap mask;
  QPixmap pix(m_patternCBox->width(), 17);
  QPainter p(&pix, m_patternCBox);
  QPen pen;
  pen.setColor(QColor(0, 0, 0));
  pen.setWidth(3);

  for (int i = NoPen; i <= DashDotDotLine ; i++ ) {
    pix.fill(white);
    pen.setStyle(static_cast<PenStyle>(i));
    p.setPen(pen);
    p.drawLine(0, 8, pix.width(), 8);

    mask = pix;
    pix.setMask(mask);
    m_patternCBox->insertItem(pix, i);
  }

  p.end();
}

void KivioStencilFormatDlg::initFillPatterns()
{
  QBitmap mask;
  QPixmap pix(m_fillPatternCBox->width(), m_fillPatternCBox->fontMetrics().height() + 4);
  QPainter p(&pix, m_fillPatternCBox);
  QBrush brush;
  brush.setColor(QColor(0, 0, 0));

  for (int i = NoBrush; i <= DiagCrossPattern; i++ ) {
    pix.fill(white);
    brush.setStyle(static_cast<BrushStyle>(i));
    p.setBrush(brush);
    p.fillRect(0, 0, pix.width(), pix.height(), brush);

    m_fillPatternCBox->insertItem(pix, i);
  }

  p.end();
}

void KivioStencilFormatDlg::initLineEndStyles()
{
  QBitmap mask;
  QPixmap pix(m_lineEndStyleCBox->width(), 17);
  QPainter p(&pix, m_lineEndStyleCBox);
  QPen pen;
  pen.setColor(QColor(0, 0, 0));
  pen.setWidth(4);
  pen.setStyle(SolidLine);
  pen.setJoinStyle(RoundJoin);
  p.setBrush(white);

  for (int i = 0; i < 3; i++) {
    pix.fill(white);
    pen.setCapStyle(static_cast<PenCapStyle>(i * 0x10));
    p.setPen(pen);
    p.drawLine(6, 8, pix.width() - 12, 8);

    mask = pix;
    pix.setMask(mask);
    m_lineEndStyleCBox->insertItem(pix, i);
  }

  p.end();
}

double KivioStencilFormatDlg::lineWidth()
{
  return m_lineWidthUSBox->value();
}

QColor KivioStencilFormatDlg::lineColor()
{
  return m_lineCBtn->color();
}

int KivioStencilFormatDlg::linePattern()
{
  return m_patternCBox->currentItem();
}

QColor KivioStencilFormatDlg::fillColor()
{
  return m_fillCBtn->color();
}

int KivioStencilFormatDlg::fillPattern()
{
  return m_fillPatternCBox->currentItem();
}

int KivioStencilFormatDlg::lineEndStyle()
{
  return m_lineEndStyleCBox->currentItem() * 0x10;
}

void KivioStencilFormatDlg::setLineWidth(double w, KoUnit::Unit u)
{
  m_unit = u;
  m_lineWidthUSBox->setUnit(u);
  m_lineWidthUSBox->setValue(KoUnit::toUserValue(w, u));
}

void KivioStencilFormatDlg::setLineColor(QColor c)
{
  m_lineCBtn->setColor(c);
}

void KivioStencilFormatDlg::setLinePattern(int p)
{
  m_patternCBox->setCurrentItem(p);
}

void KivioStencilFormatDlg::setFillColor(QColor c)
{
  m_fillCBtn->setColor(c);
}

void KivioStencilFormatDlg::setFillPattern(int p)
{
  m_fillPatternCBox->setCurrentItem(p);
}

void KivioStencilFormatDlg::setLineEndStyle(int s)
{
  m_lineEndStyleCBox->setCurrentItem(s / 0x10);
}

void KivioStencilFormatDlg::slotDefault()
{
  setLineWidth(1.0, m_unit);
  setLineColor(QColor(0, 0, 0));
  setLinePattern(1);
  setFillColor(QColor(255, 255, 255));
  setFillPattern(1);
}

#include "kiviostencilformatdlg.moc"
