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

#ifndef KIVIOARROWHEADFORMATDLG_H
#define KIVIOARROWHEADFORMATDLG_H

#include <kdialogbase.h>

#include <KoUnit.h>

class KoUnitDoubleSpinBox;
class KComboBox;
class KivioView;

class KivioArrowHeadFormatDlg : public KDialogBase
{
  Q_OBJECT
  public:
    KivioArrowHeadFormatDlg(KivioView* parent, const char* name = 0);

    int startAHType();
    double startAHWidth();
    double startAHHeight();
    int endAHType();
    double endAHWidth();
    double endAHHeight();

  public slots:
    void setUnit(KoUnit::Unit u);
    void setStartAHType(int t);
    void setStartAHWidth(double w);
    void setStartAHHeight(double h);
    void setEndAHType(int t);
    void setEndAHWidth(double w);
    void setEndAHHeight(double h);

  protected slots:
    void slotDefault();

  protected:
    void init();
    void loadArrowHeads(KComboBox* combo);

  protected:
    KoUnitDoubleSpinBox* m_startAHWidthUSBox;
    KoUnitDoubleSpinBox* m_startAHHeightUSBox;
    KoUnitDoubleSpinBox* m_endAHWidthUSBox;
    KoUnitDoubleSpinBox* m_endAHHeightUSBox;
    KComboBox* m_startAHTypeCBox;
    KComboBox* m_endAHTypeCBox;
    KoUnit::Unit m_unit;
};

#endif
