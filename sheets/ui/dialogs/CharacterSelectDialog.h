/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CHARACTER_SELECT_DIALOG
#define CALLIGRA_SHEETS_CHARACTER_SELECT_DIALOG

#include <KoDialog.h>

class QWidget;

namespace Calligra
{
namespace Sheets
{

/**
 * A dialog for selecting a character.
 */
class CharacterSelectDialog : public KoDialog
{
    Q_OBJECT

public:

    // constructor
    CharacterSelectDialog(QWidget *parent, const QString &name, const QChar &_chr,
                  const QString &_font, bool _modal = true);

    //constructor when you want to insert multi char
    CharacterSelectDialog(QWidget *parent, const QString &name, const QString &_font,
                  const QChar &_chr, bool _modal = true);
    ~CharacterSelectDialog() override;
    /**
     * Shows the selection dialog and returns true if user pressed ok, after filling the font and character parameters.
     * @param font will be filled when the user pressed Ok with the selected font.
     * @param character will be filled when the user pressed Ok with the selected character.
     * @param parent the parent widget this dialog will be associated with.
     * @param name the char name.
     */
    static bool selectChar(QString &font, QChar &character, QWidget* parent = 0, const char* name = 0);

Q_SIGNALS:
    /**
     * Emitted when the user presses the 'insert' button.
     * @param character the character that the user selected
     * @param font the font name that was selected when the user inserted the character.
     */
    void insertChar(QChar character , const QString &font);

private:
    void initDialog(const QChar &_chr, const QString &_font);
    QChar chr() const;
    QString font() const;
    void closeDialog();

private Q_SLOTS:
    void slotUser1();
    void slotDoubleClicked();

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CHARACTER_SELECT_DIALOG
