/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef searchdia_h
#define searchdia_h

#include <koFind.h>
#include <koReplace.h>
#include <qcolor.h>
#include <qstring.h>
#include <qstringlist.h>

class KWSearchContextUI;

//
// This class represents the KWord-specific search extension items, and also the
// corresponding replace items.
//
class KWSearchContext
{
public:

    // Options.

    typedef enum
    {
        Family = 1 * KoFindDialog::MinimumUserOption,
        Color = 2 * KoFindDialog::MinimumUserOption,
        Size = 4 * KoFindDialog::MinimumUserOption,
        Bold = 8 * KoFindDialog::MinimumUserOption,
        Italic = 16 * KoFindDialog::MinimumUserOption,
        Underline = 32 * KoFindDialog::MinimumUserOption,
        VertAlign = 64 * KoFindDialog::MinimumUserOption
    } Options;

    KWSearchContext();

    QString m_family;
    QColor m_color;
    int m_size;
    //KWFormat::VertAlign vertAlign;
    QStringList m_strings;
    long m_optionsMask;
    long m_options;
};

//
// This class is the KWord search dialog.
//
class KWSearchDia:
    public KoFindDialog
{
    Q_OBJECT

public:

    KWSearchDia( QWidget *parent, const char *name, KWSearchContext *find );

protected slots:

    void slotOk();

private:

    KWSearchContextUI *m_find;
};

//
// This class is the KWord replace dialog.
//
class KWReplaceDia:
    public KoReplaceDialog
{
    Q_OBJECT

public:

    KWReplaceDia( QWidget *parent, const char *name, KWSearchContext *find, KWSearchContext *replace );

protected slots:

    void slotOk();

private:

    KWSearchContextUI *m_find;
    KWSearchContextUI *m_replace;
};

#endif
