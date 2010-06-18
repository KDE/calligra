/* This file is part of the KDE project
   Copyright (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000 David Faure <faure@kde.org>
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

#ifndef KSPREAD_RESIZE2_DIALOG
#define KSPREAD_RESIZE2_DIALOG

#include <kdialog.h>

class KoUnitDoubleSpinBox;

namespace KSpread
{
class Selection;

/**
 * \ingroup UI
 * Dialog to resize rows.
 */
class ResizeRow: public KDialog
{
    Q_OBJECT

public:
    explicit ResizeRow(QWidget* parent, Selection* selection);
    double rowHeight;

protected slots:
    virtual void slotOk();
    virtual void slotDefault();

protected:
    Selection* m_selection;
    KoUnitDoubleSpinBox *m_pHeight;
};

/**
 * \ingroup UI
 * Dialog to resize columns.
 */
class ResizeColumn: public KDialog
{
    Q_OBJECT

public:
    explicit ResizeColumn(QWidget* parent, Selection* selection);
    double columnWidth;

protected slots:
    virtual void slotOk();
    virtual void slotDefault();

protected:
    Selection* m_selection;
    KoUnitDoubleSpinBox *m_pWidth;
};

} // namespace KSpread

#endif // KSPREAD_RESIZE2_DIALOG
