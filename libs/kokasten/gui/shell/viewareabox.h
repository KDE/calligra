/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VIEWAREABOX_H
#define VIEWAREABOX_H

// Qt
#include <QtGui/QWidget>

class QShortcut;


namespace Kasten2
{

class ViewAreaBox : public QWidget
{
    Q_OBJECT

  public:
    enum Area { TopArea, BottomArea };

  public:
    /// does not take ownership of @p centralWidget, can be 0.
    explicit ViewAreaBox( QWidget* centralWidget, QWidget* parent = 0 );
    virtual ~ViewAreaBox();

  public:
    QWidget* centralWidget() const;
    QWidget* bottomWidget() const;

  public:
    /// does not take ownership of @p centralWidget, can be 0.
    void setCentralWidget( QWidget* centralWidget );
    /// takes ownership of @p bottomWidget, deletes current bottom widget. Can be 0.
    void setBottomWidget( QWidget* bottomWidget );

  protected Q_SLOTS:
    void onDone();

  protected:
    QWidget* mCentralWidget;

    QWidget* mBottomWidget;
    QShortcut* mEscapeShortcut;
};

}

#endif
