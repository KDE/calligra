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
#include <komlParser.h>
#include <koStoreIf.h>

#include <vector>

#include <qrect.h>
#include <qpicture.h>
#include <qstring.h>


class KoDocument : virtual public OPDocumentIf,
		           virtual public KOffice::Document_skel
{
public:
  // C++
  KoDocument();
  virtual ~KoDocument() { };

  virtual void cleanUp();

  // IDL
  virtual CORBA::Boolean isModified() { return m_bModified; }
  
  // IDL
  virtual void setURL( const char *_url );
  virtual char* url();
  
  // IDL
  virtual CORBA::Boolean loadFromURL( const char *_url );
  virtual CORBA::Boolean loadFromStore( KOStore::Store_ptr _store, const char *_url );

  // IDL
  virtual CORBA::Boolean saveToURL( const char *_url, const char *_format );
  virtual CORBA::Boolean saveToStore( KOStore::Store_ptr _store, const char *_format, const char *_path );

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
  virtual bool load( istream& in, KOStore::Store_ptr _store );
  /**
   *  @param _stream       The stream, from which the binary should be read.
   *  @param _randomaccess Tells whether input stream is a serial stream
   *                       or a random access stream, usually a @ref ifstream
   *                       or a @ref istringstream.
   *  @param _store        Pointer to a Store object. May be 0L.
   */
  virtual bool loadBinary( istream& , bool /*_randomaccess*/, KOStore::Store_ptr /*_store*/ )
  { kdebug( KDEBUG_ERROR, 30003, "KoDocument::loadBinary not implemented" ); return false; };
  /**
   *  This function loads a XML document. It is called by @ref KoDocument#load.
   */
  virtual bool loadXML( KOMLParser&, KOStore::Store_ptr  )
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
   *  Saves all children. If your document supports embedding, then you have
   *  to overload this function. An implementation may look like this:
   *  <PRE>
   *  int i = 0;
   *  QListIterator<KWordChild> it( m_lstChildren );
   *  for( ; it.current(); ++it )
   *  {
   *      // set the child document's url to an internal url (ex: "tar:/0/1")
   *      QString internURL = QString( "%1/%2" ).arg( _path ).arg( i++ );
   *      KOffice::Document_var doc = it.current()->document();
   *      if ( !doc->saveToStore( _store, 0L, internURL ) )
   *         return false;
   *  } 
   *  return true;
   *  </PRE>
   */
  virtual bool saveChildren( KOStore::Store_ptr _store, const char *_path );
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
   *  In the implementation, you should prepend the document
   *  url before the filename, so that everything is kept relative
   *  to this document. For instance it will produce urls such as
   *  tar:/1/pictures/picture0.png, if the doc url is tar:/1
   *  But do this ONLY if the document is stored extern (see @ref #isStoredExtern)
   */
  virtual bool completeSaving( KOStore::Store_ptr /* _store */ )
  { return true; }
  
  // C++
  /**
   *  Saves only an OBJECT tag for this document.
   */
  virtual bool save( ostream&, const char* /* _format */ )
  { kdebug( KDEBUG_ERROR, 30003, "KoDocument::save not implemented" ); return false; };

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
   * Return true if url() is a real filename, false if url() is
   * an internal url in the store, like "tar:/..."
   */
  virtual bool isStoredExtern();

  QString m_strURL;
  bool m_bModified;
};

typedef KOMVar<KOffice::Document> KoDocument_ref;

/**
 *  Holds an embedded object.
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
   *  Writes the OBJECT tag, but does NOT write the content of the
   *  embedded documents. Saving the embedded documents themselves
   *  is done in @ref Document_impl. This function just stores information
   *  about the position and id of the embedded document.
   */
  virtual bool save( ostream& out );
  /**
   *  Parses the OBJECT tag. This does NOT mean creating the child documents.
   *  AFTER the 'parser' finished parsing, you must use @ref #loadDocument
   *  or @ref #loadDocumentMimePart to actually load the embedded documents.
   */
  virtual bool load( KOMLParser& parser, vector<KOMLAttrib>& _attribs );
  /**
   *  Actually loads the document from the disk/net or from the store,
   *  depending on @ref #m_strURL
   */
  virtual bool loadDocument( KOStore::Store_ptr );

  virtual bool isStoredExtern();
  
  /**
   *  @param _force_update may be set to true. In this case this function
   *  always requests a new picture from the child
   *  instead of returning the cached one.
   */
  virtual QPicture* draw( float _scale = 1.0, bool _force_update = false );

  virtual KOffice::View_ptr createView( KOffice::MainWindow_ptr _main );
  
protected:
  KoDocument_ref m_rDoc;
  /**
   *  The geometry is assumed to be always unzoomed.
   */
  QRect m_geometry;
  /**
   *  Holds the source of this object, for example "file:/home/weis/image.gif"
   *  or "tar:/table1/2" if it is stored in a koffice store. If this string
   *  is empty then the document was created from scratch and not saved yet.
   *  Those documents are usually stored in a compound document later.
   */
  QString m_strURL;
  QString m_strMimeType;

  QPicture* m_pPicture;
  float m_pictureScale;
  
  bool m_bHasPrintingExtension;
};

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
