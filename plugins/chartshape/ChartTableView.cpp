/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2007 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "ChartTableView.h"

using namespace KoChart;

ChartTableView::ChartTableView(QWidget *parent /* = 0 */)
    : QTableView(parent)
{
}

ChartTableView::~ChartTableView() = default;

/**
 * \reimpl
 */
void ChartTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    Q_EMIT currentIndexChanged(current);
}
