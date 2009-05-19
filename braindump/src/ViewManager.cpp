/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ViewManager.h"

#include <KoShapeManager.h>

#include "Canvas.h"
#include "Section.h"
#include "View.h"

ViewManager::ViewManager() : m_lastViewInFocus(0)
{
}

void ViewManager::addShape(KoShape* shape)
{
  if(!shape)
    return;
  Section * page( sectionByShape( shape ) );

  foreach( View *view, m_views )
  {
    
    if ( page == view->activeSection() ) {
        view->canvas()->shapeManager()->add( shape );
    }
  }
}

void ViewManager::removeShape(KoShape* shape)
{
  Q_UNUSED(shape);
  qFatal("Unimplemented");
#if 0
    if(!shape)
        return;

    KoPAPageBase * page( pageByShape( shape ) );

    foreach( KoView *view, views() )
    {
        View * kopaView = static_cast<View*>( view );
        kopaView->viewMode()->removeShape( shape );
    }

    emit shapeRemoved( shape );

    page->shapeRemoved( shape );
    postRemoveShape( page, shape );
#endif
}

QMap<QString, KoDataCenter *> ViewManager::dataCenterMap() const
{
  Q_ASSERT(m_lastViewInFocus);
  if(m_lastViewInFocus->activeSection())
  {
    return m_lastViewInFocus->activeSection()->dataCenterMap();
  }
  return QMap<QString, KoDataCenter *>();
}

Section* ViewManager::sectionByShape( KoShape * shape ) const
{
    KoShape * parent = shape;
    Section* page = 0;
    while ( !page && ( parent = parent->parent() ) )
    {
        page = dynamic_cast<Section*>( parent );
    }
    return page;
}

void ViewManager::addView(View* view)
{
  Q_ASSERT(not m_views.contains(view));
  m_views.append(view);
}
void ViewManager::removeView(View* view)
{
  Q_ASSERT(m_views.contains(view));
  m_views.removeAll(view);
}

void ViewManager::viewHasFocus(View* view)
{
  m_lastViewInFocus = view;
}
