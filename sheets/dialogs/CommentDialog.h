/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_COMMENT_DIALOG
#define CALLIGRA_SHEETS_COMMENT_DIALOG

#include <KoDialog.h>

class KTextEdit;

namespace Calligra
{
namespace Sheets
{
class Selection;

/**
 * \ingroup UI
 * Dialog to add a comment.
 */
class CommentDialog : public KoDialog
{
    Q_OBJECT
public:
    CommentDialog(QWidget* parent, Selection* selection);

public Q_SLOTS:
    void slotOk();
    void slotTextChanged();
protected:

    Selection* m_selection;
    KTextEdit *multiLine;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_COMMENT_DIALOG
