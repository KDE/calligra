/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CHARACTER_SELECT_DIALOG
#define CALLIGRA_SHEETS_CHARACTER_SELECT_DIALOG

#include "ActionDialog.h"

class QWidget;

namespace Calligra
{
namespace Sheets
{

/**
 * A dialog for selecting a character.
 */
class CharacterSelectDialog : public ActionDialog
{
    Q_OBJECT

public:
    // constructor
    CharacterSelectDialog(QWidget *parent, const QString &_font);
    ~CharacterSelectDialog() override;

Q_SIGNALS:
    /**
     * Emitted when the user presses the 'insert' button.
     * @param character the character that the user selected
     * @param font the font name that was selected when the user inserted the character.
     */
    void insertChar(QChar character, const QString &font);

protected:
    virtual QWidget *defaultWidget() override;
    virtual void onApply() override;

private:
    QChar chr() const;
    QString font() const;

private Q_SLOTS:
    void slotDoubleClicked();

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CHARACTER_SELECT_DIALOG
