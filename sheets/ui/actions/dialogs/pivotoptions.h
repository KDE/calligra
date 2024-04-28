/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012-2013 Jigar Raisinghani <jigarraisinghani@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#ifndef PIVOTOPTIONS_H
#define PIVOTOPTIONS_H

#include <KoDialog.h>

namespace Calligra
{
namespace Sheets
{
/**
 * \ingroup UI
 * Dialog to create PivotOptions.
 */
class Selection;
class PivotOptions : public KoDialog
{
    Q_OBJECT

public:
    explicit PivotOptions(QWidget *parent, Selection *selection);
    ~PivotOptions() override;
    QString returnFunction();
public Q_SLOTS:
    void on_Ok_clicked();

private:
    //     void selectBase();
    class Private;
    Private *const d;
};
} // Sheets
} // Calligra
#endif // PIVOTOPTIONS_H
