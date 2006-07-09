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

#ifndef KIVIOSTENCILFORMATDLG_H
#define KIVIOSTENCILFORMATDLG_H

#include <kdialogbase.h>

#include <KoUnit.h>

class KoUnitDoubleSpinBox;
class KColorButton;
class KComboBox;
class KivioView;
class QColor;

class KivioStencilFormatDlg : public KDialogBase
{
  Q_OBJECT
  public:
    KivioStencilFormatDlg(KivioView* parent, const char* name = 0);

    double lineWidth();
    QColor lineColor();
    int linePattern();
    QColor fillColor();
    int fillPattern();
    int lineEndStyle();

  public slots:
    void setLineWidth(double w, KoUnit::Unit u);
    void setLineColor(QColor c);
    void setLinePattern(int p);
    void setFillColor(QColor c);
    void setFillPattern(int p);
    void setLineEndStyle(int s);

  protected slots:
    void slotDefault();

  protected:
    void init();
    void initLinePatterns();
    void initFillPatterns();
    void initLineEndStyles();

  protected:
    KoUnitDoubleSpinBox* m_lineWidthUSBox;
    KColorButton* m_lineCBtn;
    KColorButton* m_fillCBtn;
    KComboBox* m_patternCBox;
    KComboBox* m_fillPatternCBox;
    KComboBox* m_lineEndStyleCBox;
    KoUnit::Unit m_unit;
};

#endif
