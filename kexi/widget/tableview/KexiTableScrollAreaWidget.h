/* This file is part of the KDE project
   Copyright (C) 2014 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXITABLESCROLLAREAWIDGET_H
#define KEXITABLESCROLLAREAWIDGET_H

#include <QWidget>

class QMouseEvent;
class KexiTableScrollArea;

//! A widget displaying contents of KexiTableScrollArea
class KexiTableScrollAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KexiTableScrollAreaWidget(KexiTableScrollArea *area);
    virtual ~KexiTableScrollAreaWidget();

    KexiTableScrollArea *scrollArea;

protected:
    // Events that are passed to KexiTableScrollArea
    virtual void paintEvent(QPaintEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual void leaveEvent(QEvent *e);

    friend class KexiTableScrollArea;
};

#endif // KEXITABLESCROLLAREAWIDGET_H
