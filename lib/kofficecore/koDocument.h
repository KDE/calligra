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

#include <qwmatrix.h>

#include <kparts/part.h>
#include <kurl.h>

#include <komlParser.h>

class QDomElement;
class QDomDocument;

class KoStore;
class KoMainWindow;

class KoDocumentChild;
class KoDocumentChildPicture;
class KoView;
class KoDocumentPrivate;
class KoDocumentInfo;

using std::istream;
using std::ostream;

/**
 *  The KOffice document class
 *
 *  This class provides some functionality each KOffice document should have.
 *
 *  @short The KOffice document class
 */
class KoDocument : public KParts::ReadWritePart
{
  friend class KoMainWindow;
  Q_OBJECT

public:

  // ####### Better make two constructors!
  /**
   *  Constructor.
   *
   * @param parent may be another KoDocument or a class derived from @ref QWidget.
   * @param name is used to identify this document via DCOP so you may want to
   *        pass a meaningful name here which matches the pattern [A-Za-z_][A-Za-z_0-9]*.
   * @param singleViewMode determines wether the document may only have one view. In this case
   *        the @param parent must be a QWidget derived class. KoDocument will then create a wrapper widget
   *        (@ref KoViewWrapperWidget) which is a child of @param parent. This widget can be retrieved
   *        by calling @ref #widget.
   */
  KoDocument( QObject* parent = 0, const char* name = 0, bool singleViewMode = false );

  /**
   *  Destructor.
   *
   * The destructor does not delete any attached @ref KoView objects and it does not
   * delete the attached widget as returned by @ref #widget.
   */
  virtual ~KoDocument();

  /**
   * Tells wether this document is in singleview mode. This mode can only be set
   * in the constructor.
   */
  bool singleViewMode() const;

  // ######## Where and why is this needed ?
  /**
   * Returns the action described action object. In fact only the "name" attribute
   * of @ref #element is of interest here. The method searches first in the
   * @ref KActionCollection of the first view and then in the KActionCollection of this
   * document.
   *
   * Please notice that KoDocument indirectly inherits KXMLGUIClient.
   *
   * @see KXMLGUIClient
   * @see KXMLGUIClient::actionCollection
   * @see KoView::action
   */
  virtual KAction *action( const QDomElement &element ) const;

    // ######## Where and why is this needed ?
  /**
   * Returns the DOM document which describes the GUI of the
   * first view.
   */
  virtual QDomDocument document() const;

  virtual void setManager( KParts::PartManager *manager );

  /**
   * Sets wether the document can be edited or is read only.
   * This recursively applied to all child documents and
   * @ref KoView::updateReadWrite is called for every attached
   * view.
   */
  virtual void setReadWrite( bool readwrite = true );

  /**
   * Used by KoApplication, when no document exists yet.
   *
   * With the help of @param instance or @ref KApplication::instance() this
   * method figures out which .desktop file matches this application. In this
   * file it searches for the "X-KDE-NativeMimeType" entry and returns it.
   *
   * @see KService
   * @see KDesktopFile
   */
  static QCString readNativeFormatMimeType( KInstance *instance = 0L );

  /**
   * To be preferred when a document exists. It is fast when calling
   * it multiple times since it caches the result that @ref #readNativeFormatMimeType
   * delivers.
   */
  QCString nativeFormatMimeType();

  /**
   *  Create a new view for the document.
   *
   *  You need to overload this method to create a view of your desired type.
   *
   *  After creating the new view your implementation should call @ref #addView.
   *
   *  @param parent Parent widget of the view.
   *  @param name   Name of the view. The name is used in DCOP, so the name should
   *                match the pattern [A-Za-z_][A-Za-z_0-9]*.
   *
   *  @see #createShell
   */
  virtual KoView *createView( QWidget *parent = 0, const char *name = 0 ) = 0;

  /**
   *  Create a new toplevel shell for the document. This in turn will create
   *  a new view.
   *
   *  You have to overload this method to return a shell of your desired type.
   *
   *  @see #createView
   */
  virtual KoMainWindow *createShell() = 0;

  /**
   * Adds a view to the document.
   *
   * This calls @ref KoView::updateReadWrite to tell the new view
   * wether the document is readonly or not.
   *
   * You may want to call this method after you created a new view.
   * Usually this is done by @ref #createView for you, so you dont need to
   * call this method anyway.
   *
   * The document detects automatically when a view is destroyed since this
   * triggers the @ref #slotViewDestroyed slot.
   */
  virtual void addView( KoView *view );

  /**
   *  Retrieves the first view of the document.
   *
   *  @see nextView
   */
  virtual KoView *firstView();

  /**
   *  Retrieves the next view of the document after you called @ref firstView
   *  or nextView itself.
   *
   *  @see firstView
   */
  virtual KoView *nextView();

  /**
   * Reimplemented from @ref KParts::Part
   */
  virtual KParts::Part *hitTest( QWidget *widget, const QPoint &globalPos );


  /**
   *  Find the most nested child document which contains the
   *  questionable point. The point is in the coordinate system
   *  of this part. If no child document contains this point, then
   *  a pointer to this document is returned.
   *
   *  This function has to be overloaded if the document features child documents.
   *
   *  @param matrix transforms points from the documents coordinate system
   *         to the coordinate system of the questionable point.
   *  @param p is in some unknown coordinate system, but the matrix can
   *         be used to transform a point of this parts coordinate system
   *         to the coordinate system of p.
   *
   *  @return Pointer to the document, that was hit.
   */
  virtual KoDocument *hitTest( const QPoint &pos, const QWMatrix &matrix = QWMatrix() );

  /**
   *  Paints the whole document into the given painter object.
   *
   *  @param painter     The painter object into that should be drawn.
   *  @param rect        The rect that should be used in the painter object.
   *  @param transparent If TRUE then the entire rectangle is erased before painting.
   *  @param view        The KoView is needed to fid about about the active widget.
   */
  virtual void paintEverything( QPainter &painter, const QRect &rect, bool transparent = false, KoView *view = 0L );

  /**
   *  Paints all of the documents children into the given painter object.
   *
   *  @see #paintChild #paintEverything #paintContent
   */
  virtual void paintChildren( QPainter &painter, const QRect &rect, KoView *view );

  /**
   *  Paint a special child. Normally called by @ref paintChildren.
   *
   *  @see #paintEverything #paintChildren #paintContent
   */
  virtual void paintChild( KoDocumentChild *child, QPainter &painter, KoView *view );

  /**
   *  Paints the data itself. Normally called by @ref paintEverthing. It does not
   *  paint the children.
   *
   *  @see #paintEverything
   */
  virtual void paintContent( QPainter &painter, const QRect &rect, bool transparent = false ) = 0;

  /**
   *  Initializes an empty document (or displays the template dialog).
   *  You have to overload this method to initalize all your document variables.
   */
  virtual bool initDoc() = 0;

  /**
   *  Sets the modified flag on the document. This means that it has
   *  to be saved or not before deleting it.
   */
  virtual void setModified( bool _mod );

  /**
   *  Sets wether a filter change this document.
   */
  virtual void changedByFilter( bool changed=true ) const;

  /**
   *  @return true if the document is empty.
   */
  virtual bool isEmpty() const { return m_bEmpty; }

  /**
   *  Sets the document to empty. Used after loading a template
   *  (which is not empty, but not the user's input).
   *
   * @ref #isEmpty
   */
  virtual void setEmpty() { m_bEmpty = true; }

    // ############# Can be protected, or ?
  /**
   *  Loads a document from @ref KReadOnlyPart::m_file (KParts takes care of downloading
   *  remote documents).
   *  Applies a filter if necessary, and calls loadNativeFormat in any case
   *  You should not have to reimplement, except for very special cases.
   *
   * This method is called from the @ref KReadOnlyPart::openURL method.
   */
  virtual bool openFile();

    // ############# Can be protected, or ?
  /**
   *  Loads a document in the native format from a given URL.
   *  Reimplement if your native format isn't XML.
   *
   *  @param file the file to load - usually @ref KReadOnlyPart::m_file or the result of a filter
   */
  virtual bool loadNativeFormat( const QString & file );

  /**
   *  Loads a document from a store.
   *  You should never have to reimplement.

   *  @param url An internal url, like tar:/1/2
   */
  virtual bool loadFromStore( KoStore* store, const KURL& url );

        // ############# Can be protected or friend, or ?
  /**
   *  This method is needed for the new filter API. You have to
   *  reimplement it to load XML if you want to support filters
   *  which handle QDomDocuments. The default implementation returns
   *  false - i.e. not successfully loaded
   */
  virtual bool loadXML( const QDomDocument&, KoStore* = 0L ) { return false; }

    // ############# Can be protected, or ?
  /**
   *  Saves a document to @ref KReadOnlyPart::m_file (KParts takes care of uploading
   *  remote documents)
   *  Applies a filter if necessary, and calls saveNativeFormat in any case
   *  You should not have to reimplement, except for very special cases.
   */
  virtual bool saveFile();

    // ############# Can be protected, or ?
  /**
   *  Saves the document in native format, to a given file
   *  You should never have to reimplement.
   */
  virtual bool saveNativeFormat( const QString & file );

  /**
   *  Saves a document to a store.
   */
  virtual bool saveToStore( KoStore* store, const QCString& format, const QString& path );

  /**
   *  Retrieves the mimetype of the document.
   *
   *  You need to reimplement this method.
   */
  virtual QCString mimeType() const = 0;

  /**
   * Inserts the new child in the list of children and emits the
   * @ref #childChanged signal.
   *
   * At the same time this method marks this document as modified.
   *
   * @see #isModified
   */
  virtual void insertChild( KoDocumentChild *child );

  /**
   * @return the list of all children. Do not modify the
   *         returned list.
   */
  QList<KoDocumentChild> &children() const;

  /**
   * @return the KoDocumentChild associated with the given Document, but only if
   *         "doc" is a direct child of this document.
   *
   * This is a convenience function. You could get the same result
   * by traversing the list returned by @ref #children.
   */
  KoDocumentChild *child( KoDocument *doc );

  KoDocumentInfo *documentInfo() const;

  void setViewContainerStates( KoView *view, const QMap<QString,QByteArray> &states );
  QMap<QString,QByteArray> viewContainerStates( KoView *view );

signals:
  /**
   * This signal is emitted, if a direct or indirect child document changes
   * and needs to be updated in all views.
   *
   * If one of your child documents emits the childChanged signal, then you may
   * usually just want to redraw this child. In this case you can ignore the parameter
   * passed by the signal.
   */
  void childChanged( KoDocumentChild *child );

protected slots:
  virtual void slotViewDestroyed();

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
  virtual bool loadBinary( std::istream& , bool /*_randomaccess*/, KoStore* /*_store*/ );

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
   *  QListIterator<KoDocumentChild> it( children() );
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
   *
   *  QListIterator<KoDocumentChild> it( children() );
   *  for( ; it.current(); ++it ) {
   *    QString internURL = QString( "%1/%2" ).arg( _path ).arg( i++ );
   *    if ( !((KoDocumentChild*)(it.current()))->document()->saveToStore( _store, "", internURL ) )
   *      return false;
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
   *
   *  This method is obsolete, dont use.
   */
  virtual bool save( ostream&, const char* format );

  // Hmm, name clash. We want to give access to parent's save()
  virtual bool save() { return KParts::ReadWritePart::save(); }

  /**
   *  Overload this function with your personal text.
   *  By default an empty string is returned.
   */
  virtual QString copyright() const;

  /**
   *  Retrieves a comment of the document.
   *  By default an empty string is returned.
   */
  virtual QString comment() const;

  /**
   *  An example implementation may look like this one:
   *  <PRE>
   *  QListIterator<KoDocumentChild> it( children() );
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

  /**
   * Sets the document URL to empty URL
   * KParts doesn't allow this, but KOffice apps have e.g. templates
   * After using loadNativeFormat on a template, one wants
   * to set the url to KURL()
   */
  void resetURL() { m_url = KURL(); }

  /**
   * Appends the shell to the list of shells which show this
   * document as their root document.
   *
   * This method is automatically called from @ref KoMainWindow::setRootDocument,
   * so you dont need to call it.
   */
  void addShell( KoMainWindow *shell );
  /**
   * Removes the shell from the list. That happens automatically if the shell changes its
   * root document. Usually you dont need to call this method.
   */
  void removeShell( KoMainWindow *shell );

private:

    KoDocumentPrivate *d;
    QCString m_nativeFormatMimeType;
    KURL m_strURL;
    bool m_bEmpty;
};

#endif
