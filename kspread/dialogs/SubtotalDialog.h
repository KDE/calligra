/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#ifndef __kspread_dlg_subtotal__
#define __kspread_dlg_subtotal__

#include <QPoint>
#include <QRect>

#include <kdialog.h>

#include "ui_SubtotalWidget.h"

class QLineEdit;

namespace KSpread
{
class Sheet;
class View;

class SubtotalDialog : public KDialog, public Ui::SubtotalWidget
{
  Q_OBJECT

 public:
  SubtotalDialog( View * parent, QRect const & selection );
  ~SubtotalDialog();

  QRect const & selection() const { return m_selection; }
  Sheet * sheet() const { return m_pSheet; }

 private slots:
  void slotOk();
  void slotCancel();
  void slotUser1();

 private:
  View  *          m_pView;
  Sheet *          m_pSheet;
  QRect            m_selection;

  void fillColumnBoxes();
  void fillFunctionBox();
  void removeSubtotalLines();
  bool addSubtotal( int mainCol, int column, int row, int topRow,
                    bool addRow, QString const & text );
};

} // namespace KSpread

#endif

