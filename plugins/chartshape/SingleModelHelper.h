/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_SINGLE_MODEL_HELPER_H
#define KCHART_SINGLE_MODEL_HELPER_H

// Qt
#include <QObject>

namespace KoChart
{

class ChartProxyModel;
class Table;

class SingleModelHelper : public QObject
{
    Q_OBJECT

public:
    SingleModelHelper(Table *table, ChartProxyModel *proxyModel);

private Q_SLOTS:
    void slotModelStructureChanged();

private:
    Table *const m_table;
    ChartProxyModel *const m_proxyModel;
};

} // namespace KoChart

#endif // KCHART_SINGLE_MODEL_HELPER_H
