/*
 * SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOTABLEVIEW_H
#define KOTABLEVIEW_H

#include <QTableView>

#include "kowidgets_export.h"

class QEvent;
class QModelIndex;

/**
 * @brief The KoTableView class provides a QTableView with fixed columns or rows
 */
class KOWIDGETS_EXPORT KoTableView : public QTableView
{
    Q_OBJECT

public:
    enum ViewMode {
        FIXED_COLUMNS, /// The number of columns is fixed
        FIXED_ROWS /// The number of rows is fixed
    };

    explicit KoTableView(QWidget *parent = nullptr);
    ~KoTableView() override = default;

    /** reimplemented
     * This will draw a number of rows based on the number of columns if m_viewMode is FIXED_COLUMS
     * And it will draw a number of columns based on the number of rows if m_viewMode is FIXED_ROWS
     */
    void resizeEvent(QResizeEvent *event) override;

    void setViewMode(ViewMode mode);

    void updateView();

Q_SIGNALS:

    void sigSizeChanged();

private:
    ViewMode m_viewMode;
};

#endif // KOTABLEVIEW_H
