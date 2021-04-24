/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2000-2001 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SERIES_DIALOG
#define CALLIGRA_SHEETS_SERIES_DIALOG

#include <KoDialog.h>

class QRadioButton;
class QDoubleSpinBox;

namespace Calligra
{
namespace Sheets
{
class Selection;
class Sheet;

/**
 * \ingroup UI
 * Dialog to insert a value series.
 */
class SeriesDialog : public KoDialog
{
    Q_OBJECT
public:

    SeriesDialog(QWidget* parent, Selection* selection);

    Sheet* sheet;

public Q_SLOTS:
    void slotButtonClicked(int button) override;

protected:
    Selection* m_selection;
    QDoubleSpinBox* start;
    QDoubleSpinBox* end;
    QDoubleSpinBox* step;

    QRadioButton* column;
    QRadioButton* row;
    QRadioButton* linear;
    QRadioButton* geometric;
    QPoint  marker;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SERIES_DIALOG
