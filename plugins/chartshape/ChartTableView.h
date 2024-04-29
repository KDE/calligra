/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2007 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CHART_TABLE_VIEW
#define CHART_TABLE_VIEW

#include <QTableView>

namespace KoChart
{

class ChartTableView : public QTableView
{
    Q_OBJECT

public:
    explicit ChartTableView(QWidget *parent = nullptr);
    ~ChartTableView();

Q_SIGNALS:
    void currentIndexChanged(const QModelIndex &index);

protected Q_SLOTS:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
};

}

#endif
