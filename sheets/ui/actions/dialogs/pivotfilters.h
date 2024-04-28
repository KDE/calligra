/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012-2013 Jigar Raisinghani <jigarraisinghani@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local

#ifndef PIVOTFILTERS_H
#define PIVOTFILTERS_H

#include <KoDialog.h>

class QComboBox;

namespace Calligra
{
namespace Sheets
{

class Selection;
class PivotFilters : public KoDialog

{
    Q_OBJECT

public:
    explicit PivotFilters(QWidget *parent, Selection *selection);
    QVector<QString> filterData();
    ~PivotFilters() override;
private Q_SLOTS:
    void activateBoxes();
    void activateBoxes2();
    void fillValue();
    void fillValue2();
    void fillValue3();

private:
    void selectFields(QComboBox *box);
    class Private;
    Private *const d;
};
} // Sheets
} // Calligra
#endif // PIVOTFILTERS_H
