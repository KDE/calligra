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
#ifndef __kivio_map_h__
#define __kivio_map_h__

class KivioPage;
class KivioMap;
class KivioDoc;

class KoStore;
class KoXmlWriter;
class KoGenStyles;

class QDomElement;
class QDomDocument;
class DCOPObject;

#include <qintdict.h>
#include <qobject.h>
#include <qptrlist.h>
#include <qstringlist.h>

/*
 * A map is a simple container for all pages. Usually a complete map is saved in one file.
 */

class KivioMap : public QObject
{
  public:
    /**
    * Created an empty map.
    */
    KivioMap( KivioDoc* doc, const char* name = 0 );
    /**
    * This deletes all pages contained in this map.
    */
    virtual ~KivioMap();
  
    QDomElement save( QDomDocument& doc );
    void saveOasis(KoStore* store, KoXmlWriter* docWriter, KoGenStyles* styles);
    bool loadXML( const QDomElement& mymap );
  
    /**
    * @param _page becomes added to the map.
    */
    void addPage( KivioPage* );
  
    void takePage( KivioPage* page );
    void insertPage( KivioPage* page );
  
    /**
    * The page named @param _from is being moved to the page @param _to.
    * If @param _before is true @param _from is inserted before (after otherwise)
    * @param _to.
    */
    void movePage( const QString & _from, const QString & _to, bool _before = true );
  
    KivioPage* findPage( const QString& name );
  
    /**
    * Use the @ref #nextPage function to get all the other pages.
    * Attention: Function is not reentrant.
    *
    * @return a pointer to the first page in this map.
    */
    KivioPage* firstPage();

    /**
     * @return a pointer to the last page in this map.
     */
    KivioPage* lastPage();

    /**
    * Call @ref #firstPage first. This will set the list pointer to
    * the first page. Attention: Function is not reentrant.
    *
    * @return a pointer to the next page in this map.
    */
    KivioPage* nextPage();
  
    QPtrList<KivioPage>& pageList() { return m_lstPages; }
    
    /**
    * Returns list of visible pages as stringlist.
    */
    QStringList visiblePages() const;
    
    /**
    * Returns list of hidden pages as stringlist.
    */
    QStringList hiddenPages() const;
  
    /**
    * @return amount of pages in this map.
    */
    int count()const;
  
    void update();
  
    KivioDoc* doc()const;
  
    virtual DCOPObject* dcopObject();

    void clear();

  private:
    QPtrList<KivioPage> m_lstPages;
    QPtrList<KivioPage> m_lstDeletedPages;
  
    KivioDoc* m_pDoc;
    DCOPObject* m_dcop;
};

#endif
