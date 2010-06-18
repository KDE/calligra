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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_CONDITIONAL_DIALOG
#define KSPREAD_CONDITIONAL_DIALOG

#include <kdialog.h>
#include <QWidget>

#include "Condition.h"

class KComboBox;
class KLineEdit;

namespace KSpread
{
class Selection;
class Style;

/**
 * \ingroup UI
 * Widget representing a conditional cell style.
 */
class ConditionalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConditionalWidget(QWidget * parent = 0, const char * name = 0, Qt::WFlags fl = 0);
    ~ConditionalWidget();

    KComboBox * m_condition_1;
    KComboBox * m_style_1;
    KLineEdit * m_firstValue_1;
    KLineEdit * m_secondValue_1;

    KComboBox * m_condition_2;
    KComboBox * m_style_2;
    KLineEdit * m_firstValue_2;
    KLineEdit * m_secondValue_2;

    KComboBox * m_condition_3;
    KComboBox * m_style_3;
    KLineEdit * m_firstValue_3;
    KLineEdit * m_secondValue_3;

public slots:
    void slotTextChanged1(const QString &);
    void slotTextChanged2(const QString &);
    void slotTextChanged3(const QString &);
};


/**
 * \ingroup UI
 * Dialog to set conditional cell styles.
 */
class ConditionalDialog : public KDialog
{
    Q_OBJECT
public:
    ConditionalDialog(QWidget* parent, Selection* selection);

    void init();

public slots:
    void slotOk();

protected:
    Selection*          m_selection;
    ConditionalWidget * m_dlg;
    Conditional::Type   m_result;

private:
    void init(Conditional const & tmp, int numCondition);
    Conditional::Type typeOfCondition(KComboBox const * const cb) const;

    bool checkInputData(KLineEdit const * const edit1,
                        KLineEdit const * const edit2);
    bool checkInputData();
    bool getCondition(Conditional & newCondition, const KComboBox * cb,
                      const KLineEdit * edit1, const KLineEdit * edit2,
                      const KComboBox * sb);

};

} // namespace KSpread

#endif // KSPREAD_CONDITIONAL_DIALOG
