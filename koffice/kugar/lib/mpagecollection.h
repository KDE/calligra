/***************************************************************************
             mpagecollection.h  -  Kugar report page collection
             -------------------
   begin     : Fri Aug 20 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#ifndef MPAGECOLLECTION_H
#define MPAGECOLLECTION_H

#include <qobject.h>
#include <qptrlist.h>
#include <qpicture.h>
#include <qsize.h>

/**Kugar report page collection
  *@author Mutiny Bay Software
  */

namespace Kugar
{

class MPageCollection : public QObject
{

public:
    /** Constructor */
    MPageCollection( QObject *parent );
    /** Copy constructor */
    MPageCollection( const MPageCollection& mPageCollection );
    /** Assignment operator */
    MPageCollection operator=( const MPageCollection& mPageCollection );
    /** Destructor */
    virtual ~MPageCollection();

private:
    /** The report page list */
    QPtrList<QPicture> pages;
    /** Page dimensions */
    QSize dimensions;
    /** Page size */
    int size;
    /** Page orientation */
    int orientation;

public:
    /** Clears the page collection */
    void clear();

    /** Appends a new page to the page collection */
    void appendPage();

    /** Gets the current page in the page collection,
      * the current page may be null
      */
    QPicture* getCurrentPage();
    /** Gets the first page in the page collection,
      * returns NULL if the list is empty
      */
    QPicture* getFirstPage();
    /** Get the previous page in the page collection,
      * returns NULL if the beginning of the list has been reached
      */
    QPicture* getPreviousPage();
    /** Gets the next page in the page collection,
      * returns NULL if the end of the list has been reached
      */
    QPicture* getNextPage();
    /** Gets the last page in the page collection,
      * returns NULL if the list empty
      */
    QPicture* getLastPage();

    /** Get the index of the current page */
    int getCurrentIndex();
    /** Set the current page to page at idx */
    void setCurrentPage( int idx );

    /** Sets the page size */
    void setPageSize( int s );
    /** Sets the page orientation */
    void setPageOrientation( int o );
    /** Sets the page dimensions  */
    void setPageDimensions( QSize dim );
    /** Returns the page size */
    int pageSize();
    /** Returns the page orientation */
    int pageOrientation();
    /** Returns the page dimensions  */
    QSize pageDimensions();
    /** Returns the number of pages in the page collection */
    int pageCount();

    void addRef();
    void removeRef();

private:
    /** Copies member data from one object to another.
      * Used by the copy constructor and assignment operator
      */
    void copy( const MPageCollection* mPageCollection );
    int m_ref;
};

}

#endif
