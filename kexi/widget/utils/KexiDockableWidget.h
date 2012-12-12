/* This file is part of the KDE project
   Copyright (C) 2009 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIDOCKABLEWIDGET_H
#define KEXIDOCKABLEWIDGET_H

#include <QWidget>
#include <QPointer>
#include <kexi_export.h>

//! @short Ordinary QWidget that supports forcing size hint
/*! The setSizeHint() method is needed be the main window to properly 
    restore size on application startup.

    This widgets acts as a parent for a single child that can be
    set by setWidget(), so it is not necessary use inheritance.
*/
class KEXIGUIUTILS_EXPORT KexiDockableWidget : public QWidget
{
public:
    KexiDockableWidget(QWidget* parent = 0);
    virtual ~KexiDockableWidget();

    //! Sets child widget @a widget. 
    //! The child is automatically added to internal layout (without margins)
    //! This method works only once.
    void setWidget(QWidget* widget);

    //! @return widget that has been set as a child.
    QWidget *widget() const;

    virtual QSize sizeHint() const;

    void setSizeHint(const QSize& size);


private:
    class Private;
    Private * const d;
};

#endif
