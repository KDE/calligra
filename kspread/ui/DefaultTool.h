/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_DEFAULT_TOOL
#define KSPREAD_DEFAULT_TOOL

#include "CellToolBase.h"

namespace KSpread
{

class DefaultTool : public CellToolBase
{
    Q_OBJECT

public:
    explicit DefaultTool( KoCanvasBase* canvas );
    ~DefaultTool();

#if 0 // KSPREAD_MOUSE_STRATEGIES
    virtual void mousePressEvent( KoPointerEvent* event ) ;
    virtual void mouseReleaseEvent( KoPointerEvent* event );
    virtual void mouseMoveEvent( KoPointerEvent* event );
    virtual void mouseDoubleClickEvent( KoPointerEvent* event );
#endif

public Q_SLOTS:
    virtual void activate(bool temporary = false);

protected:
#if 0 // KSPREAD_MOUSE_STRATEGIES
    virtual KoInteractionStrategy* createStrategy(KoPointerEvent* event);
#endif
    virtual Selection* selection();
    virtual QPointF offset() const;
    virtual QSizeF size() const;
    virtual int maxCol() const;
    virtual int maxRow() const;

protected Q_SLOTS:
    // -- misc actions --
    void definePrintRange();

private:
    Q_DISABLE_COPY( DefaultTool )

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_DEFAULT_TOOL
