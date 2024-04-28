/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KORESOURCEITEMVIEW_H
#define KORESOURCEITEMVIEW_H

#include <KoIconToolTip.h>
#include <KoTableView.h>

class QEvent;
class QModelIndex;

/// The resource view
class KoResourceItemView : public KoTableView
{
    Q_OBJECT

public:
    explicit KoResourceItemView(QWidget *parent = nullptr);
    ~KoResourceItemView() override
    {
        disconnect();
    }

    /// reimplemented
    bool viewportEvent(QEvent *event) override;

Q_SIGNALS:

    void currentResourceChanged(const QModelIndex &);
    void contextMenuRequested(const QPoint &);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

private:
    KoIconToolTip m_tip;
};

#endif // KORESOURCEITEMVIEW_H
