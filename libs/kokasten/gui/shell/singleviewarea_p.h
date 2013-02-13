/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef SINGLEVIEWAREA_P_H
#define SINGLEVIEWAREA_P_H

// lib
#include "abstractviewarea_p.h"
#include "singleviewarea.h"
#include "viewareabox.h"
#include "abstractview.h"

class QDragMoveEvent;
class QDropEvent;


namespace Kasten2
{

class SingleViewAreaPrivate : public AbstractViewAreaPrivate
{
  public:
    explicit SingleViewAreaPrivate( SingleViewArea* parent );

    virtual ~SingleViewAreaPrivate();

  public:
    void init();

  public: // AbstractViewArea API
    void setFocus();
    QWidget* widget() const;
    bool hasFocus() const;

  public: // If::ToolInlineViewable API
    void setCurrentToolInlineView( AbstractToolInlineView* view );

  public:
    void setView( AbstractView* view );

  protected:
    Q_DECLARE_PUBLIC( SingleViewArea )

  protected:
    ViewAreaBox* mViewAreaBox;
    AbstractView* mCurrentView;
};


inline QWidget* SingleViewAreaPrivate::widget()  const { return mViewAreaBox; }
inline bool SingleViewAreaPrivate::hasFocus()    const
{
    return mCurrentView ? mCurrentView->hasFocus() : false;
}

inline void SingleViewAreaPrivate::setFocus()    { mCurrentView->setFocus(); }

}

#endif
