/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2010-2014 Yue Liu <yue.liu@mail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STENCILLISTVIEW_H
#define STENCILLISTVIEW_H

#include <QListView>

class StencilListView : public QListView
{
    Q_OBJECT

public:
    using QListView::contentsSize;

    explicit StencilListView(QWidget *parent = nullptr);

Q_SIGNALS:
    void pressed(const QString &name, const QString &xml, const QPoint &globalPos);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // STENCILLISTVIEW_H
