/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/     

#ifndef __ko_document_h__
#define __ko_document_h__

class KoDocumentChild;
class KoDocumentChildPicture;

#include "koffice.h"

#include <opDocument.h>

#include <qrect.h>
#include <qstring.h>
#include <qcstring.h>
#include <qvaluelist.h>
#include <qdom.h>

class QIODevice;
class QPicture;

namespace KOStore
{
  class Store;
  typedef Store *Store_ptr;
};

class KoDocument : virtual public OPDocumentIf,
		           virtual public KOffice::Document_skel
{
public:
  // C++
  KoDocument();
  virtual ~KoDocument() { };

  virtual void cleanUp();

  // IDL
  /**
   *  Called from your parent if the root object ( your direct or indirect
   *  parent ) wants to know about all of its direct and indirect children.
   *  Usually you wont overload this one. Overload @ref #makeChildListIntern
   *  if your document supports embedding.
   *  This function modifies the @ref #m_strURL of the document. It is set to
   *  '_url' if @ref #url is empty or has the "store" protocol, otherwise
   *  it is unchanged. That means that externally stored docunents wont
   *  get another URL.
   */
  virtual void makeChildList( KOffice::Document_ptr _root, const char *_url );
  /**
   *  Children use this function to register themselves. You must call
   *  the @ref #makeChildList method of the child to force its registration.
   */
  virtual void addToChildList( KOffice::Document_ptr _child, const char *_url );

  // IDL
  virtual CORBA::Boolean isModified()
  { return m_bModified; }
  
  // IDL
  virtual void setURL( const char *_url );
  virtual char* url();
  
  // IDL
  virtual CORBA::Boolean loadFromURL( const char *_url, const char *_format );
  virtual CORBA::Boolean loadFromStore( KOStore::Store_ptr _store, const char *_url );

  // IDL
  virtual CORBA::Boolean saveToURL( const char *_url, const char *_format );
  virtual CORBA::Boolean saveToStore( KOStore::Store_ptr _store, const char *_format );

  // C++
  virtual void setModified( bool _mod = true ) { m_bModified = _mod; }
  
protected:
  // C++
  /**
   *  This function is called from @ref #loadFromURL and @ref #loadFromStore.
   *  It decides wether XML or binary data has to be read and calls
   *  @ref #loadBinary or @ref #loadXML depending on this result.
   *
   *  Usually you dont want to overload this function.
   *
   *  @param _store may be 0L.
   */
  virtual bool load( QByteArray& buffer, KOStore::Store_ptr _store );
  /**
   *  @param _stream       The stream, from which the binary should be read.
   *  @param _store        Pointer to a Store object. May be 0L.
   */
  virtual bool loadBinary( QByteArray&, KOStore::Store_ptr /*_store*/ )
  { kdebug( KDEBUG_ERROR, 30003, "KoDocument::loadBinary not implemented" ); return false; };
  /**
   *  This function loads a XML document. It is called by @ref KoDocument#load.
   */
  virtual bool loadXML( const QDOM::Document&, KOStore::Store_ptr  )
  { kdebug( KDEBUG_ERROR, 30003, "KoDocument::loadXML not implemented" ); return false; };
  /**
   *  You need to overload this function if your document may contain
   *  embedded documents. This function is called to load embedded documents.
   *
   *  An example implementation may look like this:
   *  <PRE>
   *  QListIterator<KSpreadChild> it( m_lstChildren );
   *  for( ; it.current(); ++it )
   *  {
   *    if ( !it.current()->loadDocument( _store ) )
   *    {
   *      return false;
   *    }
   *  }
   *  return true;
   *  </PRE>
   */
  virtual bool loadChildren( KOStore::Store_ptr ) { return true; }
  /**
   *  Overload this function if you have to load additional files
   *  from a store. This function is called after @ref #loadXML or
   *  @ref #loadBinary and after @ref #loadChildren have been called.
   */
  virtual bool completeLoading( KOStore::Store_ptr /* _store */ )
  { return true; }
  /**
   *  If you want to write additional files to a store,
   *  the you must do it here.
   */
  virtual bool completeSaving( KOStore::Store_ptr /* _store */ )
  { return true; }
  
  // C++
  /**
   *  Save the entire document with all of its children. You have to
   *  overload this function to implement saving.
   */
  virtual bool save( QIODevice*, KOStore::Store_ptr, const char* /* format */ )
  { kdebug( KDEBUG_ERROR, 30003, "KoDocument::save not implemented" ); return false; };
  /**
   *  Usually you dont want to overload this function. It saves all
   *  children which have been registered due to @ref #makeChildListIntern.
   */
  virtual bool saveChildren( KOStore::Store_ptr _store );
  /**
   *  Called from @ref #makeChildList. This function should call all
   *  children to register as child by '_root'. If your document
   *  supports embedding, then you have to overload this function.
   *  An implementation may look like this:
   *  <PRE>
   *  int i = 0;
   *
   *  QListIterator<ImageChild> it( m_lstChildren );
   *  for( ; it.current(); ++it )
   *  {
   *    QString tmp;
   *    tmp.sprintf( "/%i", i++ );
   *    QString path( _path );
   *    path += tmp.data();
   *
   *    KOffice::Document_var doc = it.current()->document();    
   *    doc->makeChildList( _doc, path );
   *  }
   *  </PRE>
   */
  virtual void makeChildListIntern( KOffice::Document_ptr _root, const char *_path );
  /**
   *  Fills @ref #m_lstAllChildren. Usually you wont overload this one.
   *  If the document is the root document, you must call this function
   *  before you can save your document.
   */
  virtual void makeChildListIntern();

  /**
   *  Overload this function with your personal text.
   */
  virtual const char* copyright() { return ""; }
  virtual const char* comment() { return ""; }
  /**
   *  An example implementation may look like this one:
   *  <PRE>
   *  QListIterator<KSpreadChild> it( m_lstChildren );
   *  for( ; it.current(); ++it )
   *  {
   *    if ( !it.current()->isStoredExtern() )
   *    {
   *      return true;    
   *    }
   *  }
   *  return false;
   *  </PRE>
   */
  virtual bool hasToWriteMultipart() = 0;

  /**
   *  Internal class. Dont use.
   */
  class SimpleDocumentChild
  {
  public:
    SimpleDocumentChild()
    { }
    SimpleDocumentChild( const SimpleDocumentChild& d )
    {
      m_vDoc = d.m_vDoc;
      m_strURL = d.m_strURL;
    }
    SimpleDocumentChild( KOffice::Document_ptr _doc, const char *_url )
    {
      m_vDoc = KOffice::Document::_duplicate( _doc );
      m_strURL = _url;
    }
    QString url()
    { return m_strURL; }
    KOffice::Document_ptr document()
    { return KOffice::Document::_duplicate( m_vDoc ); }
    
  protected:
    KOffice::Document_var m_vDoc;
    QString m_strURL;
  };
  
  /**
   *  Holds a list of all direct and indirect children. Call @ref #makeChildList
   *  to fill this list. By default this list is empty and it is not
   *  automatically filled if new children are inserted.
   */
  QValueList<SimpleDocumentChild> m_lstAllChildren;

  QString m_strURL;
  bool m_bModified;
};

typedef KOMVar<KOffice::Document> KoDocument_ref;

/**
 *  Holds an embedded object. If you want to handle embedded documents
 *  you should derived from this class. This class helps you to load/save
 *  the embedded document and to display it.
 */
class KoDocumentChild
{
public:
  KoDocumentChild( const QRect& _rect, KOffice::Document_ptr _doc );
  KoDocumentChild();
  virtual ~KoDocumentChild();
  
  virtual KOffice::Document_ptr document()
  { return KOffice::Document::_duplicate( m_rDoc ); }
  virtual const char* url()
  { return m_strURL; }
  virtual const char* mimeType()
  { return m_strMimeType; }
  virtual const QRect& geometry()
  { return m_geometry; }
  virtual void setGeometry( const QRect& _rect );

  /**
   *  Writes the <object> tag, but does NOT write the content of the
   *  embedded documents. Saving the embedded documents themselves
   *  is done in @ref Document_impl. This function just stores information
   *  about the position and id of the embedded document.
   */
  virtual QDOM::Element save( QDOM::Document& );
  /**
   *  Parses the <object> tag. This does NOT mean creating the child documents.
   *  AFTER the 'parser' finished parsing, you must use @ref #loadDocument
   *  or @ref #loadDocumentMimePart to actually load the embedded documents.
   */
  virtual bool load( const QDOM::Element& );
  /**
   *  Actually loads the embedded document from the disk/net or from the store,
   *  depending in @ref #m_strURL. This function is called for all children
   *  once @ref KoDocument finished loading its own data.
   */
  virtual bool loadDocument( KOStore::Store_ptr, const char *_format );

  /**
   *  @return TRUE if the child is stored in some external file.
   *          That means it needs no @ref #KoStore to be saved together
   *          with its parent document.
   */
  virtual bool isStoredExtern();
  
  /**
   *  @param _force_update may be set to true. In this case this function
   *  always requests a new picture from the child
   *  instead of returning the cached one.
   */
  virtual QPicture* draw( float _scale = 1.0, bool _force_update = false );

  /**
   * Creates a new view of the child document. One can create multiple
   * views for one child document.
   */
  virtual KOffice::View_ptr createView( KOffice::MainWindow_ptr _main );
  
protected:
  /**
   * CORBA reference to the embedded document.
   */
  KoDocument_ref m_rDoc;
  /**
   *  The geometry is assumed to be always unzoomed. Its coordinates
   *  are absolute with regard to the upper left corner of the parent document.
   */
  QRect m_geometry;
  /**
   *  Holds the source of this object, for example "file:/home/weis/image.gif"
   *  or "store:/table1/2" if it is stored in a koffice store. If this string
   *  is empty then the document was created from scratch and not saved yet.
   *  Those documents are usually stored in a compound document later.
   */
  QString m_strURL;
  /**
   *  Mimetype of the embedded document.
   */
  QString m_strMimeType;

  /**
   * A cache for the picture.
   */
  QPicture* m_pPicture;
  /**
   * Scaling of the cached QPicture.
   */
  float m_pictureScale;
  
  /**
   * Tells wether the child is able to print itself.
   */
  bool m_bHasPrintingExtension;
};

/**
 * @deprecated.
 */
class KoDocumentChildPicture
{
public:
  KoDocumentChildPicture( KoDocumentChild *_child );
  virtual ~KoDocumentChildPicture();
  virtual KoDocumentChild* child()
  { return m_pChild; }
  virtual QPicture* picture()
  { return m_pChild->draw(); }
  virtual const QRect& geometry()
  { return m_pChild->geometry(); }
  
protected:
  KoDocumentChild* m_pChild;
};

#endif
