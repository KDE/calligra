/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_RESIZE2_DIALOG
#define CALLIGRA_SHEETS_RESIZE2_DIALOG

#include <KoDialog.h>
#include <KoUnit.h>

class KoUnitDoubleSpinBox;

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup UI
 * Dialog to resize rows.
 */
class ResizeRowDialog : public KoDialog
{
    Q_OBJECT

public:
    explicit ResizeRowDialog(QWidget *parent, KoUnit unit);
    void setRowHeight(double height);
    double rowHeight();
    void setDefault(double d)
    {
        defaultHeight = d;
    }

protected Q_SLOTS:
    virtual void slotOk();
    virtual void slotDefault();

protected:
    KoUnitDoubleSpinBox *m_pHeight;
    double defaultHeight;
};

/**
 * \ingroup UI
 * Dialog to resize columns.
 */
class ResizeColumnDialog : public KoDialog
{
    Q_OBJECT

public:
    explicit ResizeColumnDialog(QWidget *parent, KoUnit unit);
    void setColWidth(double width);
    double colWidth();
    void setDefault(double d)
    {
        defaultWidth = d;
    }

protected Q_SLOTS:
    virtual void slotOk();
    virtual void slotDefault();

protected:
    KoUnitDoubleSpinBox *m_pWidth;
    double defaultWidth;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_RESIZE2_DIALOG
