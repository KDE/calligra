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

#include <container.h>
#include <part.h>

#include <komlParser.h>
#include <koStore.h>

#include <kdebug.h>

#include <qrect.h>
#include <qpicture.h>
#include <qstring.h>

#include <iostream.h>

class KoDocument : public ContainerPart
{
    Q_OBJECT
public:
  KoDocument( QObject* parent = 0, const char* name = 0 );
  virtual ~KoDocument() { };

  virtual bool initDoc() = 0;

  virtual bool isModified() const;
  virtual void setModified( bool _mod = true );
  virtual bool isEmpty() const;

  virtual void setURL( const QString& url );
  virtual QString url() const;

  virtual bool loadFromURL( const QString& url );
  virtual bool loadFromStore( KoStore* store, const QString& url );

  virtual bool saveToURL( const QString& url, const QCString& format );
  virtual bool saveToStore( KoStore* store, const QCString& format, const QString& path );

  virtual QCString mimeType() const = 0;

  /**
   * Overloaded from ContainerPart to set the modified flag.
   */
  virtual void insertChild( PartChild* child );

protected:
  /**
   *  This function is called from @ref #loadFromURL and @ref #loadFromStore.
   *  It decides wether XML or binary data has to be read and calls
   *  @ref #loadBinary or @ref #loadXML depending on this result.
   *
   *  Usually you dont want to overload this function.
   *
   *  @param _store may be 0L.
   */
  virtual bool load( istream& in, KoStore* _store );
  /**
   *  @param _stream       The stream, from which the binary should be read.
   *  @param _randomaccess Tells whether input stream is a serial stream
   *                       or a random access stream, usually a @ref ifstream
   *                       or a @ref istringstream.
   *  @param _store        Pointer to a Store object. May be 0L.
   */
  virtual bool loadBinary( istream& , bool /*_randomaccess*/, KoStore* /*_store*/ );
  /**
   *  This function loads a XML document. It is called by @ref KoDocument#load.
   */
  virtual bool loadXML( KOMLParser&, KoStore* );
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
   *  url before the filename, so that everything is kept relative
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
   * wont embed other documents, otherwise you have to overload the function.
   */
  virtual bool hasToWriteMultipart();

  /**
   * Return true if url() is a real filename, false if url() is
   * an internal url in the store, like "tar:/..."
   */
  virtual bool isStoredExtern();

private:
    QString m_strURL;
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

  virtual KoDocument* document() { return (KoDocument*)part(); }

  virtual QString url();

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
   *  to actually load the embedded documents.
   */
  virtual bool load( KOMLParser& parser, vector<KOMLAttrib>& _attribs );
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
