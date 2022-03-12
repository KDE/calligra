/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1999-2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CONDITIONAL_DIALOG
#define CALLIGRA_SHEETS_CONDITIONAL_DIALOG

#include <KoDialog.h>

#include "core/Condition.h"

class KComboBox;
class KLineEdit;

namespace Calligra
{
namespace Sheets
{
class Selection;

/**
 * \ingroup UI
 * Widget representing a conditional cell style.
 */
class ConditionalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConditionalWidget(QWidget * parent = 0, const char * name = 0, Qt::WindowFlags fl = Qt::WindowFlags());
    ~ConditionalWidget() override;

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

public Q_SLOTS:
    void slotTextChanged1(const QString &);
    void slotTextChanged2(const QString &);
    void slotTextChanged3(const QString &);
};


/**
 * \ingroup UI
 * Dialog to set conditional cell styles.
 */
class ConditionalDialog : public KoDialog
{
    Q_OBJECT
public:
    ConditionalDialog(QWidget* parent, Selection* selection);

    void init();

public Q_SLOTS:
    void slotOk();

protected:
    Selection*          m_selection;
    ConditionalWidget * m_dlg;
    Validity::Type   m_result;

private:
    void init(Conditional const & tmp, int numCondition);
    Validity::Type typeOfCondition(KComboBox const * const cb) const;

    bool checkInputData(KLineEdit const * const edit1,
                        KLineEdit const * const edit2);
    bool checkInputData();
    bool getCondition(Conditional & newCondition, const KComboBox * cb,
                      const KLineEdit * edit1, const KLineEdit * edit2,
                      const KComboBox * sb);

};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CONDITIONAL_DIALOG
