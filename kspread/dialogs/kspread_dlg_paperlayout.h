/* This file is part of the KDE project
   Copyright (C) 2002-2003 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 Laurent Montel <montel@kde.org>
   
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

#ifndef __kspread_dlg_paperlayout__
#define __kspread_dlg_paperlayout__

#include <kdialogbase.h>
#include <koPageLayoutDia.h>

class KSpreadSheet;
class QCheckBox;
class QComboBox;
class QRadioButton;
class QLineEdit;
class KSpreadView;

class KSpreadPaperLayout: public KoPageLayoutDia
{
  Q_OBJECT
public:
  KSpreadPaperLayout( QWidget* parent, const char* name,
                      const KoPageLayout& layout,
                      const KoHeadFoot& headfoot,
                      int tabs, KoUnit::Unit unit,
                      KSpreadSheet * sheet, KSpreadView *view);

  bool eventFilter( QObject* obj, QEvent* ev );

protected slots:
  virtual void slotOk();
  virtual void slotCancel();
  void slotSelectionChanged( KSpreadSheet* _sheet, const QRect& _selection );

protected:
  void initTab();

protected:
  virtual void closeEvent ( QCloseEvent * );

private slots:
    void slotChooseZoom( int index );
    void slotChoosePageLimit( int index );

private:
  void initGeneralOptions( QWidget * tab, QVBoxLayout * vbox );
  void initRanges( QWidget * tab, QVBoxLayout * vbox );
  void initScaleOptions( QWidget * tab, QVBoxLayout * vbox );

  KSpreadSheet * m_pSheet;
  KSpreadView  * m_pView;
  QCheckBox * pApplyToAll;
  QCheckBox * pPrintGrid;
  QCheckBox * pPrintCommentIndicator;
  QCheckBox * pPrintFormulaIndicator;
  QLineEdit * ePrintRange;
  QLineEdit * eRepeatCols;
  QLineEdit * eRepeatRows;
  QLineEdit * m_focus;
  QComboBox * m_cZoom;
  QComboBox * m_cLimitPagesX;
  QComboBox * m_cLimitPagesY;
  QRadioButton * m_rScalingZoom;
  QRadioButton * m_rScalingLimitPages;
};

#endif
