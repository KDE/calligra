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

#ifndef STATUSBARLAYOUT_H
#define STATUSBARLAYOUT_H

// Qt
#include <QtGui/QLayout>
#include <QtCore/QList>

class QWidgetItem;


namespace Kasten2
{

class StatusBarLayout : public QLayout
{
  public:
    explicit StatusBarLayout( QWidget* parent = 0 );
    virtual ~StatusBarLayout();

  public:
    void addWidget( QWidget* widget );

  public: // QLayout API
    virtual int count() const;
    virtual QLayoutItem* itemAt( int index ) const;
    virtual int indexOf( QWidget* widget ) const;

    virtual void addItem( QLayoutItem* item );
    virtual QLayoutItem* takeAt( int index );

  public: // QLayoutItem API
    virtual Qt::Orientations expandingDirections() const;
    virtual bool isEmpty() const;
    virtual QSize sizeHint() const;
    virtual QSize minimumSize () const;

    virtual void invalidate();
    virtual void setGeometry( const QRect& rect );

  protected:
//     void updateMarginAndSpacing();
    void updateLayoutStructs() const;

  protected:
    QList<QWidgetItem*> mWidgetList;

    bool mIsDirty :1;
    bool mIsEmpty :1;
    QSize mSizeHint;
};

}

#endif
