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

#include <container.h>
#include <part.h>
#include <kurl.h>

#include <komlParser.h>

class QDomElement;
class QDomDocument;

class KoStore;

class KoDocumentChild;
class KoDocumentChildPicture;

/**
 *  The KOffice document class
 *
 *  This class provides some functionality each KOffice document should have.
 *
 *  @short The KOffice document class
 */
class KoDocument : public ContainerPart
{
  Q_OBJECT
    
public:

  /**
   *  Constructor.
   */
  KoDocument( QObject* parent = 0, const char* name = 0 );

  /**
   *  Destructor.
   */
  virtual ~KoDocument() { };

  /**
   *  Initializes an empty document. You have to overlaod this method
   *  to initalize all your document variables.
   */
  virtual bool initDoc() = 0;

  /**
   *  Retrieves, if the document is modified or not. 
   */
  virtual bool isModified() const;

  /**
   *  Sets the modified flag on the document. This means that it has
   *  to be saved or not before deleting it.
   */
  virtual void setModified( bool _mod = true );

  /**
   *  Retrieves, if the document is empty or not.
   */
  virtual bool isEmpty() const;

  /**
   *  Sets the URL where the document is located and should be saved.
   */
  virtual void setURL( const KURL& url );

  /**
   *  Retrieves the URL of the document where the file is located.
   */
  virtual const KURL & url() const;

  /**
   *  Loads a document from a given URL.
   */
  virtual bool loadFromURL( const KURL& url );

  /**
   *  Loads a document from a store.
   * @param url An internal url, like tar:/1/2
   */
  virtual bool loadFromStore( KoStore* store, const KURL& url );

  /**
   *  Saves the document to a given URL.
   */
  virtual bool saveToURL( const KURL& url, const QCString& format );

  /**
   *  Saves a document to a store.
   */
  virtual bool saveToStore( KoStore* store, const QCString& format, const QString& path );

  /**
   *  Retrieves the mimetype of the document.
   */
  virtual QCString mimeType() const = 0;

  /**
   * Overloaded from ContainerPart to set the modified flag.
   */
  virtual void insertChild( PartChild* child );

protected:

  /**
   *  This function is called from @ref #loadFromURL and @ref #loadFromStore.
   *  It decides whether XML or binary data has to be read and calls
   *  @ref #loadBinary or @ref #loadXML depending on this result.
   *
   *  Usually you dont want to overload this function.
   *
   *  @param _store may be 0L.
   */
  virtual bool load( istream& in, KoStore* _store );

  /**
   *  This method loads a binary document. It is called by @ref #load.
   *
   *  You have to overload it, if you want to load a binary file.
   *
   *  @param _stream       The stream, from which the binary should be read.
   *  @param _randomaccess Tells whether input stream is a serial stream
   *                       or a random access stream, usually a @ref ifstream
   *                       or a @ref istringstream.
   *  @param _store        Pointer to a Store object. May be 0L.
   *  @return              Loading was successful or not.
   */
  virtual bool loadBinary( istream& , bool /*_randomaccess*/, KoStore* /*_store*/ );

  /**
   *  This method loads a XML document. It is called by @ref #load.
   *
   *  You have to overload it, if you want to load a XML file.
   *
   *  @param _parser Parser from which to load the document.
   *  @param _store  Pointer to a Store object. May be 0L.
   *  @return              Loading was successful or not.
   */
  virtual bool loadXML( KOMLParser& _parser, KoStore* _store );

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
  virtual bool loadChildren( KoStore* );

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
  virtual bool saveChildren( KoStore* store, const char *path );

  /**
   *  Overload this function if you have to load additional files
   *  from a store. This function is called after @ref #loadXML or
   *  @ref #loadBinary and after @ref #loadChildren have been called.
   */
  virtual bool completeLoading( KoStore* store );

  /**
   *  If you want to write additional files to a store,
   *  the you must do it here.
   *  In the implementation, you should prepend the document
   *  url (using url().url()) before the filename, so that everything is kept relative
   *  to this document. For instance it will produce urls such as
   *  tar:/1/pictures/picture0.png, if the doc url is tar:/1
   *  But do this ONLY if the document is stored extern (see @ref #isStoredExtern)
   */
  virtual bool completeSaving( KoStore* );

  /**
   *  Saves only an OBJECT tag for this document.
   */
  virtual bool save( ostream&, const char* format );

  /**
   *  Overload this function with your personal text.
   */
  virtual QString copyright() const;

  /**
   *  Retrieves a comment of the document.
   */
  virtual QString comment() const;
  
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
   *
   * By default the function returns FALSE. That is ok if your document
   * won't embed other documents, otherwise you have to overload the function.
   */
  virtual bool hasToWriteMultipart();

  /**
   * Return true if url() is a real filename, false if url() is
   * an internal url in the store, like "tar:/..."
   */
  virtual bool isStoredExtern();

private:

    KURL m_strURL;
    bool m_bModified;
    bool m_bEmpty;
};

/**
 *  Holds an embedded object.
 */
class KoDocumentChild : public PartChild
{

public:

  KoDocumentChild( KoDocument* parent, KoDocument* doc, const QRect& geometry );
  KoDocumentChild( KoDocument* parent );
  virtual ~KoDocumentChild();

  virtual KoDocument* document() { return (KoDocument*) part(); }
  /**
   * Can be empty (which is why it doesn't return a const KURL &)
   */
  virtual KURL url();

  /**
   *  Writes the OBJECT tag, but does NOT write the content of the
   *  embedded documents. Saving the embedded documents themselves
   *  is done in @ref Document_impl. This function just stores information
   *  about the position and id of the embedded document.
   */
  virtual bool save( ostream& out );
  
  /**
   *  Writes the OBJECT tag, but does NOT write the content of the
   *  embedded documents. Saving the embedded documents themselves
   *  is done in @ref Document_impl. This function just stores information
   *  about the position and id of the embedded document.
   *
   *  Use this function if your application uses the DOM.
   */
  virtual QDomElement save( QDomDocument& doc );
  
  /**
   *  Parses the OBJECT tag. This does NOT mean creating the child documents.
   *  AFTER the 'parser' finished parsing, you must use @ref #loadDocument
   *  to actually load the embedded documents.
   */
  virtual bool load( KOMLParser& parser, vector<KOMLAttrib>& _attribs );
  
  /**
   *  Parses the OBJECT tag. This does NOT mean creating the child documents.
   *  AFTER the 'parser' finished parsing, you must use @ref #loadDocument
   *  to actually load the embedded documents.
   *
   *  Use this function if your application uses the DOM.
   */
  virtual bool load( const QDomElement& element );
  
  /**
   *  Actually loads the document from the disk/net or from the store,
   *  depending on @ref #url
   */
  virtual bool loadDocument( KoStore* );

  virtual bool isStoredExtern();

protected:

  /**
   * Called if @ref #load finds a tag that it does not understand.
   *
   * @return TRUE if the tag could be handled. The default implementation
   *         returns FALSE.
   */
  virtual bool loadTag( KOMLParser& parser, const string& tag, vector<KOMLAttrib>& lst2 );

private:

  /**
   *  Holds the source of this object, for example "file:/home/weis/image.gif"
   *  or "tar:/table1/2" if it is stored in a koffice store. This variable is
   *  set after parsing the OBJECT tag in @ref #load and is reset after
   *  calling @ref #loadDocument.
   */
  QString m_tmpURL;
  
  /**
   * This variable is
   *  set after parsing the OBJECT tag in @ref #load and is reset after
   *  calling @ref #loadDocument.
   */
  QRect m_tmpGeometry;
  
  /**
   * This variable is
   *  set after parsing the OBJECT tag in @ref #load and is reset after
   *  calling @ref #loadDocument.
   */
  QString m_tmpMimeType;
};

#endif
