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

#include <kpagedialog.h>

class QLabel;
class KLineEdit;
class KTextEdit;
class KComboBox;
class QCheckBox;

namespace Calligra
{
namespace Sheets
{
class Selection;

/**
 * \ingroup UI
 * Dialog for setting cell validations.
 */
class ValidityDialog : public KPageDialog
{
    Q_OBJECT

public:
    ValidityDialog(QWidget* parent, Selection* selection);
    void init();

public Q_SLOTS:
    void OkPressed();
    void clearAllPressed();
    void changeIndexCond(int);
    void changeIndexType(int);

protected:
    void displayOrNotListOfValidity(bool _displayList);

    Selection* m_selection;

    KLineEdit *val_max;
    KLineEdit *val_min;
    QLabel *edit1;
    QLabel *edit2;
    QLabel *chooseLabel;
    KComboBox *choose;
    KComboBox *chooseAction;
    KComboBox *chooseType;
    KLineEdit * title;
    KTextEdit *message;
    QCheckBox *displayMessage;
    QCheckBox *allowEmptyCell;
    QCheckBox *displayHelp;
    KTextEdit *messageHelp;
    KLineEdit *titleHelp;
    KTextEdit *validityList;
    QLabel *validityLabelList;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_VALIDITY_DIALOG
