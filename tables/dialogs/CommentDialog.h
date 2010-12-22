/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
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
   Boston, MA 02110-1301, USA.
*/

#ifndef CALLIGRA_TABLES_COMMENT_DIALOG
#define CALLIGRA_TABLES_COMMENT_DIALOG

#include <kdialog.h>

class KTextEdit;

namespace Calligra
{
namespace Tables
{
class Selection;

/**
 * \ingroup UI
 * Dialog to add a comment.
 */
class CommentDialog : public KDialog
{
    Q_OBJECT
public:
    CommentDialog(QWidget* parent, Selection* selection);

public slots:
    void slotOk();
    void slotTextChanged();
protected:

    Selection* m_selection;
    KTextEdit *multiLine;
};

} // namespace Tables
} // namespace Calligra

#endif // CALLIGRA_TABLES_COMMENT_DIALOG
