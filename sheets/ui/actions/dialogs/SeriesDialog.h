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

#include "ActionDialog.h"
#include <QRadioButton>

class QDoubleSpinBox;

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup UI
 * Dialog to insert a value series.
 */
class SeriesDialog : public ActionDialog
{
    Q_OBJECT
public:
    SeriesDialog(QWidget *parent);

    double dstep() const
    {
        return m_dstep;
    }
    double dstart() const
    {
        return m_dstart;
    }
    double dend() const
    {
        return m_dend;
    }

    bool isColumn() const
    {
        return column->isChecked();
    }
    bool isLinear() const
    {
        return linear->isChecked();
    }

Q_SIGNALS:
    void insertSeries(double start, double end, double step, bool isColumn, bool isLinear);

protected:
    virtual void onApply() override;

    QDoubleSpinBox *start;
    QDoubleSpinBox *end;
    QDoubleSpinBox *step;

    QRadioButton *column;
    QRadioButton *row;
    QRadioButton *linear;
    QRadioButton *geometric;
    QPoint marker;

    double m_dstep, m_dend, m_dstart;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SERIES_DIALOG
