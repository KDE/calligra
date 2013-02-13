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

#ifndef ABSTRACTVIEWAREA_H
#define ABSTRACTVIEWAREA_H

// lib
#include "kastengui_export.h"
// Qt
#include <QtCore/QObject>

class QWidget;


namespace Kasten2
{

class AbstractViewAreaPrivate;


// TODO: rename abstractdocumentviewarea?
class KASTENGUI_EXPORT AbstractViewArea : public QObject
{
    Q_OBJECT

  protected:
    AbstractViewArea();
    explicit AbstractViewArea( AbstractViewAreaPrivate* d );

  public:
    virtual ~AbstractViewArea();

  public: // API to be implemented
    virtual void setFocus() = 0;

    virtual QWidget* widget() const = 0;
//     virtual QString title() const = 0;
//     virtual AbstractTool* tool() const = 0;
    virtual bool hasFocus() const = 0;

  Q_SIGNALS:
    // area has focus in the window
    void focusChanged( bool hasFocus );

  protected:
    AbstractViewAreaPrivate* const d_ptr;
    Q_DECLARE_PRIVATE( AbstractViewArea )
};

}

#endif
