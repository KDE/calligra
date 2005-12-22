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

#ifndef KIVIOTEXTFORMATDLG_H
#define KIVIOTEXTFORMATDLG_H

#include <kdialogbase.h>

class KivioView;
class KFontChooser;
class KColorButton;
class QButtonGroup;
class QLabel;
class QFont;
class QColor;

class KivioTextFormatDlg : public KDialogBase
{
  Q_OBJECT
  public:
    KivioTextFormatDlg(KivioView* parent, const char* name = 0);

    int valign();
    int halign();
    QFont font();
    QColor textColor();

  public slots:
    void setVAlign(int i);
    void setHAlign(int i);
    void setFont(QFont f);
    void setTextColor(QColor c);

  protected:
    void initFontTab();
    void initPositionTab();

  protected slots:
    void updateVAlign(int i);
    void updateHAlign(int i);
    void slotDefault();

  protected:
    KFontChooser* m_fontChooser;
    KColorButton* m_textCBtn;
    QButtonGroup* m_halignBGrp;
    QButtonGroup* m_valignBGrp;
    QLabel* m_preview;
    int m_valign;
    int m_halign;
};

#endif
