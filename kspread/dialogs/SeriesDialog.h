/* This file is part of the KDE project
   Copyright (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2000-2001 Werner Trobin <trobin@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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

#ifndef KSPREAD_SERIES_DIALOG
#define KSPREAD_SERIES_DIALOG

#include <kdialog.h>

class QRadioButton;
class KDoubleNumInput;

namespace KSpread
{
class Selection;
class Sheet;

/**
 * \ingroup UI
 * Dialog to insert a value series.
 */
class SeriesDialog : public KDialog
{
    Q_OBJECT
public:

    SeriesDialog(QWidget* parent, Selection* selection);

    Sheet* sheet;

public slots:
    virtual void slotButtonClicked(int button);

protected:
    Selection* m_selection;
    KDoubleNumInput *start;
    KDoubleNumInput *end;
    KDoubleNumInput *step;

    QRadioButton* column;
    QRadioButton* row;
    QRadioButton* linear;
    QRadioButton* geometric;
    QPoint  marker;
};

} // namespace KSpread

#endif // KSPREAD_SERIES_DIALOG
