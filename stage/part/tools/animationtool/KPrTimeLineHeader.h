/* This file is part of the KDE project
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KPRTIMELINEHEADER_H
#define KPRTIMELINEHEADER_H

#include <QWidget>

/**
  This widget is part of Animations Time Line Widget
  */
class KPrAnimationsTimeLineView;

class KPrTimeLineHeader : public QWidget
{
    Q_OBJECT
public:
    explicit KPrTimeLineHeader(QWidget *parent);
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override {return minimumSizeHint();}
protected:
    void paintEvent(QPaintEvent *event) override;
    /// Helper methods used to paint the header
    /// main paint method
    void paintHeader(QPainter *painter, const int RowHeight);

    /// Paint each column header (except for the scale)
    void paintHeaderItem(QPainter *painter, const QRect &rect, const QString &text);

    /// Paint the scale for the bar column
    void paintTimeScale(QPainter *painter, const QRect &rect);
    bool eventFilter(QObject *target, QEvent *event) override;
    KPrAnimationsTimeLineView *m_mainView;
};

#endif // KPRTIMELINEHEADER_H
