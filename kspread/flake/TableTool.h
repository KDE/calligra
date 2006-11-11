/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_TABLE_TOOL
#define KSPREAD_TABLE_TOOL

#include <KoTool.h>

namespace KSpread
{

class TableTool : public KoTool
{
public:
  explicit TableTool( KoCanvasBase* canvas );
  ~TableTool();

  virtual void paint( QPainter& painter, KoViewConverter& converter );

  virtual void mousePressEvent( KoPointerEvent* event ) ;
  virtual void mouseMoveEvent( KoPointerEvent* event );
  virtual void mouseReleaseEvent( KoPointerEvent* event );

  virtual void activate( bool temporary = false );
  virtual void deactivate();

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_TABLE_TOOL
