/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <qprinter.h>
#include <qdom.h>
#include <qmessagebox.h>

#include "kivio_map.h"
#include "kivio_doc.h"
#include "kivio_view.h"
#include "kivio_canvas.h"
#include "kivio_page.h"

#include <time.h>
#include <stdlib.h>

KivioMap::KivioMap( KivioDoc* doc, const char* name )
: QObject(doc,name)
{
  m_pDoc = doc;
  m_lstPages.setAutoDelete(true);
}

KivioMap::~KivioMap()
{
}

void KivioMap::addPage( KivioPage* page )
{
  m_lstPages.append(page);
}

void KivioMap::removePage( KivioPage* page )
{
  m_lstPages.setAutoDelete(false);
  m_lstPages.removeRef(page);
  m_lstPages.setAutoDelete(true);
}

void KivioMap::movePage( const QString& fromPageName, const QString& toPageName, bool before )
{
  KivioPage* pagefrom = findPage(fromPageName);
  KivioPage* pageto = findPage(toPageName);

  int from = m_lstPages.find(pagefrom);
  int to = m_lstPages.find(pageto);
  if (!before)
    ++to;

  if ( to > (int)m_lstPages.count() ) {
    m_lstPages.append(pagefrom);
    m_lstPages.take(from);
  } else
    if ( from < to ) {
      m_lstPages.insert(to,pagefrom);
      m_lstPages.take(from);
    } else {
      m_lstPages.take(from);
      m_lstPages.insert(to,pagefrom);
    }
}

QDomElement KivioMap::save( QDomDocument& doc )
{
    int next = 1;
    
  QDomElement mymap = doc.createElement("map");

  // Before we save, tell all the pages/layers/stencil/targets/connectors to generate
  // their ids so we can restore connections when reloaded.
  QListIterator<KivioPage> it2(m_lstPages);
  for( ; it2.current(); ++it2 )
  {
    next = it2.current()->generateStencilIds( next );
  }
  
  // Now save the pages
  QListIterator<KivioPage> it(m_lstPages);
  for( ; it.current(); ++it )
  {
    QDomElement e = it.current()->save(doc);
    if (e.isNull())
      return e;
    mymap.appendChild(e);
  }

  return mymap;
}

bool KivioMap::loadXML( const QDomElement& mymap )
{
  // FIXME: make this load the real page units and whatever
  // else
  QDomNode n = mymap.firstChild();
  while( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() && e.tagName() == "page" ) {
      KivioPage *t = m_pDoc->createPage();
      m_pDoc->addPage( t );
      if ( !t->loadXML( e ) )
        return false;
    }
    n = n.nextSibling();
  }
  return true;
}

void KivioMap::update()
{
  QListIterator<KivioPage> it( m_lstPages );
  for( ; it.current(); ++it )
    it.current()->update();
}

KivioPage* KivioMap::findPage( const QString& name )
{
  KivioPage *t;

  for ( t = m_lstPages.first(); t; t = m_lstPages.next() ) {
    if ( name == t->pageName() )
      return t;
    }

  return 0L;
}

KivioDoc* KivioMap::doc()
{
  return m_pDoc;
}

KivioPage* KivioMap::firstPage()
{
  return m_lstPages.first();
}

KivioPage* KivioMap::nextPage()
{
  return m_lstPages.next();
}

int KivioMap::count()
{
  return m_lstPages.count();
}
