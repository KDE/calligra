/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1999-2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CONDITIONAL_DIALOG
#define CALLIGRA_SHEETS_CONDITIONAL_DIALOG

#include "ActionDialog.h"

#include "core/Condition.h"

class KComboBox;
class KLineEdit;

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup UI
 * Widget representing a conditional cell style.
 */
class ConditionalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConditionalWidget(QWidget *parent = nullptr, const char *name = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~ConditionalWidget() override;

    KComboBox *m_condition_1;
    KComboBox *m_style_1;
    KLineEdit *m_firstValue_1;
    KLineEdit *m_secondValue_1;

    KComboBox *m_condition_2;
    KComboBox *m_style_2;
    KLineEdit *m_firstValue_2;
    KLineEdit *m_secondValue_2;

    KComboBox *m_condition_3;
    KComboBox *m_style_3;
    KLineEdit *m_firstValue_3;
    KLineEdit *m_secondValue_3;

public Q_SLOTS:
    void slotTextChanged1(const QString &);
    void slotTextChanged2(const QString &);
    void slotTextChanged3(const QString &);
};

/**
 * \ingroup UI
 * Dialog to set conditional cell styles.
 */
class ConditionalDialog : public ActionDialog
{
    Q_OBJECT
public:
    ConditionalDialog(QWidget *parent);

    void init();

    void clear();
    void setStyleNames(const QStringList &list);
    void setValueRow(int id, Validity::Type type, const QString &val1, const QString &val2, const QString &style);

    Validity::Type getType(int id);
    QString getValue1(int id);
    QString getValue2(int id);
    QString getStyleName(int id);

Q_SIGNALS:
    void applyCondition();

protected:
    virtual void onApply() override;

    ConditionalWidget *m_dlg;
    Validity::Type m_result;

private:
    Validity::Type typeOfCondition(KComboBox const *const cb) const;

    bool checkInputData(KLineEdit const *const edit1, KLineEdit const *const edit2);
    bool checkInputData();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CONDITIONAL_DIALOG
