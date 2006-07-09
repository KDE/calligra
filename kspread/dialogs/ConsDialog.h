/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000-2001 Werner Trobin <trobin@kde.org>
             (C) 2000-2001 Laurent Montel <montel@kde.org>
             (C) 1999-2002 David Faure <faure@kde.org>
             (C) 1999 Stephan Kulow <coolo@kde.org>
             (C) 1999 Reginald Stadlbauer <reggie@kde.org>
             (C) 1998-1999 Torben Weis <weis@kde.org>

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

#ifndef __kspread_dlg_cons__
#define __kspread_dlg_cons__

#include <QStringList>
//Added by qt3to4:
#include <QCloseEvent>
#include <kdialog.h>

class QLineEdit;
class QPushButton;
class QComboBox;
class QCheckBox;
class Q3ListBox;

namespace KSpread
{
class Sheet;
class View;

class ConsolidateDialog : public KDialog
{
  Q_OBJECT
public:
  ConsolidateDialog( View* parent, const char* name );
  ~ConsolidateDialog();
  QStringList refs();

public slots:
  virtual void slotOk();
  virtual void slotCancel();
  void slotAdd();
  void slotRemove();

  void slotSelectionChanged();
  void slotReturnPressed();

protected:
  virtual void closeEvent ( QCloseEvent * );

  View* m_pView;
  QLineEdit* m_pRef;
  Q3ListBox* m_pRefs;
  QComboBox* m_pFunction;
  QPushButton* m_pAdd;
  QPushButton* m_pRemove;
  QCheckBox* m_pRow;
  QCheckBox* m_pCol;
  QCheckBox* m_pCopy;

  QString evaluate( const QString& formula, Sheet* sheet );

  enum { Sum = 0, Average, Count, Max, Min, Product, StdDev, Var };
};

} // namespace KSpread

#endif
