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

namespace std { };
using namespace std;
#include <qwmatrix.h>

#include <kparts/part.h>
#include <kurl.h>
#include <kservice.h>

#include <komlParser.h>

class QDomElement;
class QDomDocument;

class KoStore;
class KoMainWindow;

class KoChild;
class KoDocumentChild;
class KoDocumentChildPicture;
class KoView;
class KoDocumentPrivate;
class KoDocumentInfo;
class DCOPObject;


/**
 *  The KOffice document class
 *
 *  This class provides some functionality each KOffice document should have.
 *
 *  @short The KOffice document class
 */
class KoDocument : public KParts::ReadWritePart
{
  Q_OBJECT

public:

  /**
   *  Constructor.
   * The first 4  arguments are the same as the ones passed to KParts::Factory::createPart.
   *
   * @param parentWidget the parent widget, in case we create a wrapper widget
   *        (in single view mode).
   *        Usually the first argument passed by KParts::Factory::createPart.
   * @param parent may be another KoDocument, or anything else.
   *        Usually the third argument of KParts::Factory::createPart.
   * @param name is used to identify this document via DCOP so you may want to
   *        pass a meaningful name here which matches the pattern [A-Za-z_][A-Za-z_0-9]*.
   * @param singleViewMode determines whether the document may only have one view. In this case
   *        the @param parent must be a QWidget derived class. KoDocument will then create a wrapper widget
   *        (@ref KoViewWrapperWidget) which is a child of @param parentWidget.
   *        This widget can be retrieved by calling @ref #widget.
   */
  KoDocument( QWidget* parentWidget,
              const char* widgetName,
              QObject* parent,
              const char* name,
              bool singleViewMode = false );

  /**
   *  Destructor.
   *
   * The destructor does not delete any attached @ref KoView objects and it does not
   * delete the attached widget as returned by @ref #widget.
   */
  virtual ~KoDocument();

  virtual void delayedDestruction();

  /**
   * Tells whether this document is in singleview mode. This mode can only be set
   * in the constructor.
   */
  bool isSingleViewMode() const;

  /**
   * Is the document embedded?
   */
  bool isEmbedded() const;

  // ######## Where and why is this needed ?
  // Well, normally all the KOffice apps define the actions in the
  // view (therefore there is an actionCollection in the KoView class).
  // As Simon pointed out this is not correct. They should only define
  // view-actions (like zooming and stuff) in the view. Every action
  // which changes the document should be defined here. That's at least
  // what I understood :) (Werner)
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
  virtual QDomDocument domDocument() const;

  virtual void setManager( KParts::PartManager *manager );

  /**
   * Sets whether the document can be edited or is read only.
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

    static KService::Ptr readNativeService( KInstance *instance = 0L );

  /**
   * To be preferred when a document exists. It is fast when calling
   * it multiple times since it caches the result that @ref #readNativeFormatMimeType
   * delivers.
   */
  QCString nativeFormatMimeType();

    KService::Ptr nativeService();

  /**
   *  Create a new view for the document.
   *  @see #createShell
   */
  KoView *createView( QWidget *parent = 0, const char *name = 0 );

  /**
   * Adds a view to the document.
   *
   * This calls @ref KoView::updateReadWrite to tell the new view
   * whether the document is readonly or not.
   */
  virtual void addView( KoView *view );

  virtual void removeView( KoView *view );

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
   *  Retrieves the number of views for this document
   */
  virtual unsigned int viewCount();

  virtual KoView *view( int idx );

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
   *  @param transparent If true then the entire rectangle is erased before painting.
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
   *  Initializes an empty document (display the template dialog!).
   *  You have to overload this method to initalize all your document variables.
   */
  virtual bool initDoc() = 0;

  /**
   *  Sets the modified flag on the document. This means that it has
   *  to be saved or not before deleting it.
   */
  virtual void setModified( bool _mod );

  /**
   *  Tells the document that its title has been modified, either because
   *  the modified status changes (this is done by @ref setModified) or
   *  because the URL or the document-info's title changed.
   */
  virtual void setTitleModified();

  /**
   *  Sets whether a filter change this document.
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

  /**
   *  Loads a document from a store.
   *  You should never have to reimplement.
   *  @param url An internal url, like tar:/1/2
   */
  virtual bool loadFromStore( KoStore* store, const KURL& url );

  /**
   *  Saves a document to a store.
   *  You should not have to reimplement this - but call it in @ref saveChildren.
   */
  virtual bool saveToStore( KoStore* store, const QString& path );

  /**
   *  Reimplement this method to load the contents of your KOffice document,
   *  from the XML document.
   *
   *  You are supposed to use the QDomDocument. The QIODevice is provided only
   *  for the cases where some pre-processing is needed, like kpresenter's kprconverter.
   *  Note that the QIODevice could be 0L, when called from an import filter.
   */
  virtual bool loadXML( QIODevice *, const QDomDocument & doc ) = 0;

  /**
   *  Reimplement this to save the contents of the KOffice document into
   *  a QDomDocument. The framework takes care of saving it to the store.
   */
  virtual QDomDocument saveXML();

  /**
   *  Save the document. The default implementation is to call
   *  @ref saveXML. This method exists only for applications that
   *  don't use QDomDocument for saving, i.e. kword and kpresenter.
   */
  virtual bool saveToStream( QIODevice * dev );

  /**
   *  Loads a document in the native format from a given URL.
   *  Reimplement if your native format isn't XML.
   *
   *  @param file the file to load - usually @ref KReadOnlyPart::m_file or the result of a filter
   */
  virtual bool loadNativeFormat( const QString & file );

  /**
   *  Saves the document in native format, to a given file
   *  You should never have to reimplement.
   *  Made public for writing templates.
   */
  virtual bool saveNativeFormat( const QString & file );

  /**
   * Inserts the new child in the list of children and emits the
   * @ref #childChanged signal.
   *
   * At the same time this method marks this document as modified.
   *
   * To remove a child, just delete it. KoDocument will detect this
   * and remove the child from its lists.
   *
   * @see #isModified
   */
  virtual void insertChild( const KoDocumentChild *child );
    
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

  void setViewBuildDocument( KoView *view, const QDomDocument &doc );
  QDomDocument viewBuildDocument( KoView *view );

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

  virtual const KoMainWindow *firstShell();
  virtual const KoMainWindow *nextShell();
  virtual unsigned int shellCount() const;

  /**
   * Returns the list of all the currently opened documents
   */
  static QList<KoDocument> *documentList() { return s_documentList; }

  /**
   * Return a DCOP interface for this document
   * KOffice Application are strongly recommended to reimplement this method,
   * so that their dcop interface provides more functionality than the basic KoDocumentIface
   */
  virtual DCOPObject * dcopObject();

public slots:
  void slotProgress(int value) { emit sigProgress(value); }

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
  void sigProgress(int value);

protected:

  virtual KoView *createViewInstance( QWidget *parent, const char *name ) = 0;

  /**
   *  Loads a document from @ref KReadOnlyPart::m_file (KParts takes care of downloading
   *  remote documents).
   *  Applies a filter if necessary, and calls loadNativeFormat in any case
   *  You should not have to reimplement, except for very special cases.
   *
   * This method is called from the @ref KReadOnlyPart::openURL method.
   */
  virtual bool openFile();

  /**
   *  Saves a document to @ref KReadOnlyPart::m_file (KParts takes care of uploading
   *  remote documents)
   *  Applies a filter if necessary, and calls saveNativeFormat in any case
   *  You should not have to reimplement, except for very special cases.
   */
  virtual bool saveFile();

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
   *    if ( !((KoDocumentChild*)(it.current()))->document()->saveToStore( _store, internURL ) )
   *      return false;
   *  }
   *  return true;
   *  </PRE>
   */
  virtual bool saveChildren( KoStore* store, const QString &path );

  /**
   *  Overload this function if you have to load additional files
   *  from a store. This function is called after @ref #loadXML
   *  and after @ref #loadChildren have been called.
   */
  virtual bool completeLoading( KoStore* store );

  /**
   *  If you want to write additional files to a store,
   *  then you must do it here.
   *  In the implementation, you should prepend the document
   *  url (using url().url()) before the filename, so that everything is kept relative
   *  to this document. For instance it will produce urls such as
   *  tar:/1/pictures/picture0.png, if the doc url is tar:/1
   *  But do this ONLY if the document is not stored extern (see @ref #isStoredExtern).
   *  If it is, then the pictures should be saved to tar:/pictures.
   */
  virtual bool completeSaving( KoStore* store );

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

  /// @internal
  virtual void setModified() { KParts::ReadWritePart::setModified(); }

  /// @internal
  virtual void insertChild(QObject *o) { QObject::insertChild(o); }

private slots:
  void slotChildChanged( KoChild *c );
  void slotChildDestroyed();
  void slotDestruct();

private:

    KoDocumentPrivate *d;
    KService::Ptr m_nativeService;
    KURL m_strURL;
    bool m_bEmpty;
    static QList<KoDocument> *s_documentList;
};

#endif
