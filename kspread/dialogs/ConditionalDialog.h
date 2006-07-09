/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 1999-2004 Laurent Montel <montel@kde.org>

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

#ifndef __kspread_dlg_conditional__
#define __kspread_dlg_conditional__

#include <kdialog.h>
#include <QWidget>

#include "Condition.h"

class QComboBox;
class KLineEdit;

namespace KSpread
{
class Style;
class View;

class ConditionalWidget : public QWidget
{
  Q_OBJECT

 public:
  ConditionalWidget( QWidget * parent = 0, const char * name = 0, Qt::WFlags fl = 0 );
  ~ConditionalWidget();

  QComboBox * m_condition_1;
  QComboBox * m_style_1;
  KLineEdit * m_firstValue_1;
  KLineEdit * m_secondValue_1;

  QComboBox * m_condition_2;
  QComboBox * m_style_2;
  KLineEdit * m_firstValue_2;
  KLineEdit * m_secondValue_2;

  QComboBox * m_condition_3;
  QComboBox * m_style_3;
  KLineEdit * m_firstValue_3;
  KLineEdit * m_secondValue_3;

 public slots:
  void slotTextChanged1( const QString & );
  void slotTextChanged2( const QString & );
  void slotTextChanged3( const QString & );
};


class ConditionalDialog : public KDialog
{
  Q_OBJECT
 public:
  ConditionalDialog( View * parent, const char * name,
                         const QRect & marker );

  void init();

 public slots:
  void slotOk();

 protected:
  View *              m_view;
  ConditionalWidget * m_dlg;
  QRect               m_marker;
  Conditional::Type   m_result;

 private:
   void init( Conditional const & tmp, int numCondition );
   Conditional::Type typeOfCondition( QComboBox const * const cb ) const;

  bool checkInputData( KLineEdit const * const edit1,
                       KLineEdit const * const edit2 );
  bool checkInputData();
  bool getCondition( Conditional & newCondition, const QComboBox * cb,
                     const KLineEdit * edit1, const KLineEdit * edit2,
                     const QComboBox * sb, Style * style );

};

} // namespace KSpread

#endif

