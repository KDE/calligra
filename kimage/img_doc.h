#ifndef __img_doc_h__
#define __img_doc_h__

class ImageDocument_impl;

#include <document_impl.h>
#include <view_impl.h>
// #include <opstore.h>

#include "img_view.h"

#include <qimage.h>
#include <qlist.h>
#include <qobject.h>
#include <qrect.h>

#define MIME_TYPE "application/x-kimage"
#define EDITOR "IDL:KImage/ImageDocument:1.0"

class ImageChild
{
public:
  ImageChild( ImageDocument_impl *_img, const QRect& _rect, OPParts::Document_ptr _doc );
  ~ImageChild();
  
  const QRect& geometry() { return m_geometry; }
  OPParts::Document_ptr document() { return OPParts::Document::_duplicate( m_rDoc ); }
  ImageDocument_impl* parent() { return m_pImageDoc; }

  void setGeometry( const QRect& _rect ) { m_geometry = _rect; }
  
protected:
  ImageDocument_impl *m_pImageDoc;
  Document_ref m_rDoc;
  QRect m_geometry;
};

class ImageDocument_impl : public QObject,
			   virtual public Document_impl,
			   virtual public KImage::ImageDocument_skel
{
  Q_OBJECT
public:
  // C++
  ImageDocument_impl();
  ~ImageDocument_impl();
  
  virtual void cleanUp();
  
  // IDL
  virtual CORBA::Boolean open( const char *_filename );
  virtual CORBA::Boolean saveAs( const char *_filename, const char *_format );

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
  
  virtual void insertObject( const QRect& _rect );
  virtual void changeChildGeometry( ImageChild *_child, const QRect& );
  
  virtual QListIterator<ImageChild> childIterator();
  
  // virtual OBJECT saveToStore( Store &_store );
  // virtual bool loadFromStore( Store &_store, OBJECT _obj );
  
  // virtual void sendWarning( const char * );

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
  void sig_updateChildGeometry( ImageChild *_child );
  void sig_removeObject( ImageChild *_child );
  void sig_fitToWindow( bool _fit );  

protected:
  QImage m_imgImage;

  QList<ImageView_impl> m_lstViews;

  QList<ImageChild> m_lstChildren;

  bool m_bFitToWindow;

  bool m_bModified;
};

#endif
