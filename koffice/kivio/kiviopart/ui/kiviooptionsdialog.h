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

#ifndef KIVIOOPTIONSDIALOG_H
#define KIVIOOPTIONSDIALOG_H

#include <kdialogbase.h>
#include <KoPageLayout.h>
#include <KoUnit.h>
#include <klistview.h>

class KivioView;
class QComboBox;
class QCheckBox;
class QLabel;
class QRadioButton;
class KColorButton;
class KURLRequester;
class KoUnitDoubleSpinBox;
class KivioGuideLineData;
class QGroupBox;
class QFont;

// class GuidesListViewItem : public KListViewItem
// {
//   public:
//     GuidesListViewItem(QListView* parent, KivioGuideLineData *gd, KoUnit::Unit u);
//     ~GuidesListViewItem();
// 
//     void setUnit(KoUnit::Unit u);
//     void setPosition(double p, KoUnit::Unit u);
//     void setOrientation(Qt::Orientation o);
//     KivioGuideLineData* guideData() { return m_data; }
// 
//   private:
//     KivioGuideLineData* m_data;
// };

/***********************************************************************/

class KivioOptionsDialog : public KDialogBase
{
  Q_OBJECT
  public:
    KivioOptionsDialog(KivioView* parent, const char* name = 0);

  protected:
    void initPage();
    void initGrid();

    void applyPage();
    void applyGrid();

    void defaultPage();
    void defaultGrid();

    void setLayoutText(const KoPageLayout& l);
    void setFontText(const QFont& f);

  protected slots:
    void pageLayoutDlg();
    void fontDlg();
    void unitChanged(int u);
    void slotOk();
    void slotApply();
    void slotDefault();
    void setMaxHorizSnap(double v);
    void setMaxVertSnap(double v);

  private:
    // Page indecies
    int m_pageIndex;
    int m_gridIndex;

    QComboBox* m_unitCombo;
    QCheckBox* m_marginsChBox;
    QCheckBox* m_rulersChBox;
    QLabel* m_layoutTxtLbl;
    KoPageLayout m_layout;
    QLabel* m_fontTxtLbl;
    QFont m_font;
    KoUnitDoubleSpinBox* m_spaceHorizUSpin;
    KoUnitDoubleSpinBox* m_spaceVertUSpin;
    KoUnitDoubleSpinBox* m_snapHorizUSpin;
    KoUnitDoubleSpinBox* m_snapVertUSpin;
    QCheckBox* m_gridChBox;
    QCheckBox* m_snapChBox;
    KColorButton* m_gridColorBtn;
};

#endif
