/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <qprinter.h>
#include <qdom.h>
#include <qmessagebox.h>

#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>

#include "kivio_map.h"
#include "kivio_doc.h"
#include "kivio_view.h"
#include "kivio_canvas.h"
#include "kivio_page.h"
#include "KIvioMapIface.h"

#include <time.h>
#include <stdlib.h>

KivioMap::KivioMap( KivioDoc* doc, const char* name )
: QObject(doc,name)
{
  m_pDoc = doc;
  m_lstPages.setAutoDelete(true);
  m_dcop = 0;

}

KivioMap::~KivioMap()
{
    delete m_dcop;

}

void KivioMap::takePage( KivioPage* page )
{
    int pos=m_lstPages.findRef(page);
    m_lstPages.take( pos );
    m_lstDeletedPages.append( page );
}

void KivioMap::insertPage( KivioPage* page )
{
    int pos=m_lstDeletedPages.findRef(page);
    if ( pos != -1 )
        m_lstDeletedPages.take( pos);
    m_lstPages.append(page);
}

void KivioMap::addPage( KivioPage* page )
{
  m_lstPages.append(page);
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

  QDomElement mymap = doc.createElement("KivioMap");

  // Before we save, tell all the pages/layers/stencil/targets/connectors to generate
  // their ids so we can restore connections when reloaded.
  QPtrListIterator<KivioPage> it2(m_lstPages);
  for( ; it2.current(); ++it2 )
  {
    next = it2.current()->generateStencilIds( next );
  }

  // Now save the pages
  QPtrListIterator<KivioPage> it(m_lstPages);
  for( ; it.current(); ++it )
  {
    QDomElement e = it.current()->save(doc);
    if (e.isNull())
      return e;
    mymap.appendChild(e);
  }

  return mymap;
}

void KivioMap::saveOasis(KoStore* store, KoXmlWriter* docWriter, KoGenStyles* styles)
{
  QPtrListIterator<KivioPage> it(m_lstPages);
  
  for( ; it.current(); ++it )
  {
    it.current()->saveOasis(store, docWriter, styles);
  }
}

bool KivioMap::loadXML( const QDomElement& mymap )
{
  m_lstPages.clear();
  m_lstDeletedPages.clear();

  // FIXME: make this load the real page units and whatever
  // else
  QDomNode n = mymap.firstChild();
  while( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() && e.tagName() == "KivioPage" ) {
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
  QPtrListIterator<KivioPage> it( m_lstPages );
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

KivioDoc* KivioMap::doc()const
{
  return m_pDoc;
}

KivioPage* KivioMap::firstPage()
{
  return m_lstPages.first();
}

KivioPage* KivioMap::lastPage()
{
  return m_lstPages.last();
}

KivioPage* KivioMap::nextPage()
{
  return m_lstPages.next();
}

int KivioMap::count() const
{
  return m_lstPages.count();
}

DCOPObject* KivioMap::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KIvioMapIface( this );

    return m_dcop;
}

QStringList KivioMap::visiblePages() const
{
  QStringList pages;
  
  QPtrListIterator<KivioPage> it( m_lstPages );
  for( ; it.current(); ++it )
  {
    KivioPage* page = it.current();
    if( !page->isHidden() )
      pages.append( page->pageName() );
  }
  
  return pages;
}

QStringList KivioMap::hiddenPages() const
{
  QStringList pages;
  
  QPtrListIterator<KivioPage> it( m_lstPages );
  for( ; it.current(); ++it )
  {
    KivioPage* page = it.current();
    if( page->isHidden() )
      pages.append( page->pageName() );
  }
  
  return pages;
}

void KivioMap::clear()
{
  m_lstPages.clear();
  m_lstDeletedPages.clear();
}
