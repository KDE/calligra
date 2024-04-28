/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 1999-2005 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_VALIDITY_DIALOG
#define CALLIGRA_SHEETS_VALIDITY_DIALOG

#include "ActionDialog.h"
#include "engine/Validity.h"

class QLabel;
class KLineEdit;
class KTextEdit;
class KComboBox;
class QCheckBox;

namespace Calligra
{
namespace Sheets
{
class CalculationSettings;

/**
 * \ingroup UI
 * Dialog for setting cell validations.
 */
class ValidityDialog : public ActionDialog
{
    Q_OBJECT

public:
    ValidityDialog(QWidget *parent, CalculationSettings *settings, ValueParser *parser);
    Validity getValidity();
    void setValidity(Validity validity);

Q_SIGNALS:
    void applyValidity(const Validity &validity);

public Q_SLOTS:
    void clearAllPressed();
    void changeIndexCond(int);
    void changeIndexType(int);

protected:
    virtual void onApply() override;

    void displayOrNotListOfValidity(bool _displayList);

    CalculationSettings *m_settings;
    ValueParser *m_parser;

    KLineEdit *val_max;
    KLineEdit *val_min;
    QLabel *edit1;
    QLabel *edit2;
    QLabel *chooseLabel;
    KComboBox *choose;
    KComboBox *chooseAction;
    KComboBox *chooseType;
    KLineEdit *title;
    KTextEdit *message;
    QCheckBox *displayMessage;
    QCheckBox *allowEmptyCell;
    QCheckBox *displayHelp;
    KTextEdit *messageHelp;
    KLineEdit *titleHelp;
    KTextEdit *validityList;
    QLabel *validityLabelList;

    double minval, maxval;
    QTime mintime, maxtime;
    QDate mindate, maxdate;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_VALIDITY_DIALOG
