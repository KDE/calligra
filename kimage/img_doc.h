#ifndef __img_doc_h__
#define __img_doc_h__

class ImageDocument_impl;

#include <koDocument.h>
#include <koPrintExt.h>
#include <document_impl.h>
#include <view_impl.h>

#include <iostream>

#include "img_view.h"

#include <qimage.h>
#include <qlist.h>
#include <qobject.h>
#include <qrect.h>

#define MIME_TYPE "application/x-kimage"
#define KImageRepoID "IDL:KImage/ImageDocument:1.0"

/**
 * This class is used to hold informations about embedded
 * documents.
 */
class ImageChild : public KoDocumentChild
{
public:
  ImageChild( ImageDocument_impl *_img, const QRect& _rect, OPParts::Document_ptr _doc );
  ImageChild( ImageDocument_impl *_img );
  ~ImageChild();
  
  ImageDocument_impl* parent() { return m_pImageDoc; }
  
protected:
  ImageDocument_impl *m_pImageDoc;
};

class ImageDocument_impl : public QObject,
			   virtual public KoDocument,
			   virtual public KoPrintExt,
			   virtual public KImage::ImageDocument_skel
{
  Q_OBJECT
public:
  // C++
  ImageDocument_impl();
  ~ImageDocument_impl();
  
protected:
  virtual void cleanUp();

  virtual bool hasToWriteMultipart();
  
public:
  // IDL
  virtual CORBA::Boolean init();

  // C++
  virtual bool load( istream& in, bool _randomaccess );
  virtual bool load( KOMLParser& parser );
  virtual bool loadChildren( OPParts::MimeMultipartDict_ptr _dict );
  virtual bool save( ostream& out );
  
  // IDL
  virtual CORBA::Boolean import( const char *_filename );
  virtual CORBA::Boolean export( const char *_filename, const char *_format );
  
  virtual OPParts::View_ptr createView();

  virtual void viewList( OPParts::Document::ViewList*& _list );

  virtual char* mimeType() { return CORBA::string_dup( MIME_TYPE ); }
  
  virtual CORBA::Boolean isModified() { return m_bModified; }
  
  // C++
  virtual const QImage& image();
  virtual bool isFitToWindow() { return m_bFitToWindow; }
  virtual void setFitToWindow( bool _fit );
  
  virtual QStrList outputFormats();
  virtual QStrList inputFormats();

  virtual void addView( ImageView_impl *_view );
  virtual void removeView( ImageView_impl *_view );

  /**
   * This function is called to insert a new child. It emits @ref #sig_insertObject
   * on demand. Every connected view must then create a view of the inserted
   * document and display it.
   *
   * @param _part_name is the name of the server as registered in the CORBE
   *                   implementation repository. It is the same name mentioned in
   *                   the *.kdelnk files in $(KDEDIR)/share/apps/koffice/partlnk.
   *                   Example:<pre>
   *                     # KDE Config File
   *                     [KDE Desktop Entry]
   *                     Name=KSpread
   *                     Exec=/home/weis/kde/koffice2/kspread/kspread --server
   *                     Comment=Torben's Spread Sheet
   *                     MimeTypes=application/x-kspread,
   *                     RepoID=IDL:KSpread/Factory:1.0,
   *                     ActivationMode=shared
   *                     Type=OpenPart
   *                     Icon=unknown.xpm
   *                     MiniIcon=unknown.xpm</pre>
   *                   Usually you will use the @ref KoPartSelectDia to query the
   *                   user for this name.
   * @param _rect is the rectangualr area where the new child should appear.
   *
   * @see ImageView_impl::slotInsertObject
   */
  virtual void insertObject( const QRect& _rect, const char *_part_name );
  /**
   * This function is called to change a childs position and size.
   * It emits @ref #sig_updateChildGeometry in turn so that all
   * views become updated.
   */
  virtual void changeChildGeometry( ImageChild *_child, const QRect& );
  
  virtual QListIterator<ImageChild> childIterator();
  
signals:
  /**
   * Emitted if the image itself changed, due to the use of filters
   * or just because another image has been imported.
   */
  void sig_imageModified();
  /**
   * Emitted if a new object has been inserted in the document.
   */
  void sig_insertObject( ImageChild *_child );
  /**
   * Emitted if a childs geometry or size has been modified.
   */
  void sig_updateChildGeometry( ImageChild *_child );
  /**
   * Emitted if a child is in the process of being removed.
   */
  void sig_removeObject( ImageChild *_child );
  void sig_fitToWindow( bool _fit );  

protected:
  virtual void insertChild( ImageChild* );
  virtual void makeChildListIntern( OPParts::Document_ptr _doc, const char *_path );
  
  virtual void draw( QPaintDevice*, CORBA::Long _width, CORBA::Long _height );

  QImage m_imgImage;
  string m_strExternFile;
  
  QList<ImageView_impl> m_lstViews;

  QList<ImageChild> m_lstChildren;

  bool m_bFitToWindow;

  bool m_bModified;
};

#endif
