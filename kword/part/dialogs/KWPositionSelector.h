/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#ifndef KWPOSITIONSELECTOR_H
#define KWPOSITIONSELECTOR_H

#include <QWidget>
#include <KoFlake.h>

/**
 * Widget to show a set of radio buttons so the user can select a position.
 */
class KWPositionSelector : public QWidget {
    Q_OBJECT
public:
    KWPositionSelector(QWidget *parent);
    ~KWPositionSelector();

    KoFlake::Position position() const;
    void setPosition(KoFlake::Position position);

signals:
    void positionSelected(KoFlake::Position position);

protected:
    /// reimplemented
    virtual void paintEvent (QPaintEvent *event);

private slots:
    void positionChanged(int position);

private:
    class Private;
    Private * const d;
};

#endif
