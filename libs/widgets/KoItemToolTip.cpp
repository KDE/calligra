/*
  SPDX-FileCopyrightText: 2006 Gábor Lehel <illissius@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "KoItemToolTip.h"

#include <QApplication>
#include <QModelIndex>
#include <QPaintEvent>
#include <QPainter>
#include <QPersistentModelIndex>
#include <QStyleOptionViewItem>
#include <QTextDocument>
#include <QToolTip>

class Q_DECL_HIDDEN KoItemToolTip::Private
{
public:
    QPersistentModelIndex index;
    QString html;

    Private() = default;
};

KoItemToolTip::KoItemToolTip()
    : d(new Private)
{
}

KoItemToolTip::~KoItemToolTip() = default;

void KoItemToolTip::showTip(QWidget *widget, const QPoint &pos, const QRect &rect, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    auto html = createDocument(index);

    if (index != d->index || d->html != html) {
        d->index = index;
        d->html = html;

        QToolTip::showText(pos, html, widget, rect, 10000);
        return;
    }
}

void KoItemToolTip::hide() const
{
    QToolTip::hideText();
}
