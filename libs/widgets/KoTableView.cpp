/*
 * SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoTableView.h"

#include <QEvent>
#include <QHeaderView>

KoTableView::KoTableView(QWidget *parent)
    : QTableView(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    verticalHeader()->hide();
    horizontalHeader()->hide();
    verticalHeader()->setDefaultSectionSize(20);
    setContextMenuPolicy(Qt::DefaultContextMenu);
    setViewMode(FIXED_COLUMNS);
}

void KoTableView::resizeEvent(QResizeEvent *event)
{
    QTableView::resizeEvent(event);
    updateView();

    Q_EMIT sigSizeChanged();
}

void KoTableView::setViewMode(KoTableView::ViewMode mode)
{
    m_viewMode = mode;

    switch (m_viewMode) {
    case FIXED_COLUMNS:
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Horizontal scrollbar is never needed
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        break;
    case FIXED_ROWS:
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Vertical scrollbar is never needed
        break;
    default:
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        break;
    }
}

void KoTableView::updateView()
{
    int columnCount = model()->columnCount(QModelIndex());
    int rowCount = model()->rowCount(QModelIndex());
    int rowHeight, columnWidth;

    if (m_viewMode == FIXED_COLUMNS) {
        columnWidth = viewport()->size().width() / columnCount;

        for (int i = 0; i < columnCount; ++i) {
            setColumnWidth(i, columnWidth);
        }
        if (columnCount > 1) {
            for (int i = 0; i < rowCount; ++i) {
                setRowHeight(i, columnWidth);
            }
        }
    } else if (m_viewMode == FIXED_ROWS) {
        if (rowCount == 0)
            return; // Don't divide by zero
        rowHeight = viewport()->size().height() / rowCount;

        for (int i = 0; i < rowCount; ++i) {
            setRowHeight(i, rowHeight);
        }
    }
}
