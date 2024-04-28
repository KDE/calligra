/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012-2013 Jigar Raisinghani <jigarraisinghani@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local

#ifndef PIVOTMAIN_H
#define PIVOTMAIN_H

#include <KoDialog.h>

class QListWidgetItem;

namespace Calligra
{
namespace Sheets
{

class Sheet;
class Selection;

class PivotMain : public KoDialog
{
    Q_OBJECT

public:
    explicit PivotMain(QWidget *parent, Selection *selection);
    QVector<QString> ValueData(const QString &str);
    bool checkCondition(const QString &field, const QString &condition, const QString &value, int line);
    Sheet *filter();
    ~PivotMain() override;

private Q_SLOTS:

    // void on_Options_clicked();
    void styling(Sheet *mySheet);
    void labels_itemChanged(QListWidgetItem *item);
    void values_itemChanged(QListWidgetItem *item);
    void columns_itemChanged(QListWidgetItem *item);
    void rows_itemChanged(QListWidgetItem *item);

    void on_AddFilter_clicked();
    void on_Ok_clicked();
    void Summarize();
    void Reset();
    void clean(Sheet *sheet);

private:
    void extractColumnNames();
    PivotMain *ui;
    class Private;
    Private *const d;
};
} // Sheets
} // Calligra
#endif // PIVOTMAIN_H
