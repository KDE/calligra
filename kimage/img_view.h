#ifndef __img_view_h__
#define __img_view_h__

class ImageView_impl;
class ImageDocument_impl;
class ImageChild;

#include <view_impl.h>
#include <document_impl.h>
#include <part_frame_impl.h>
#include <menu_impl.h>
#include <toolbar_impl.h>

#include "img.h"

#include <qpixmap.h>
#include <qwidget.h>
#include <qrect.h>
#include <qlist.h>

class ImageFrame : public PartFrame_impl
{
  Q_OBJECT
public:
  ImageFrame( ImageView_impl*, ImageChild* );
  
  ImageChild* child() { return m_pImageChild; }
  /**
   * @return the view owning this frame.
   */
  ImageView_impl* view() { return m_pImageView; }
  
protected:
  ImageChild *m_pImageChild;
  ImageView_impl *m_pImageView;
};

class ImageView_impl : public QWidget,
		       virtual public View_impl,
		       virtual public KImage::ImageView_skel
{
  Q_OBJECT
public:
  // C++
  ImageView_impl( QWidget *_parent = 0L, const char *_name = 0L );
  ~ImageView_impl();

  virtual void cleanUp();
  
  // IDL  
  virtual void toggleFitToWindow();
  virtual void newView();
  virtual void insertImage();
  virtual void importImage();
  virtual void exportImage();

  // C++
  virtual void setFitToWindow( bool _fit );
    
  virtual void setDocument( ImageDocument_impl *_doc );

  virtual void createGUI();
  /**
   * Called on the first paint event. This function
   * bringd the view and the document in sync.
   */
  virtual void construct();

public slots:
  // Document signals
  void slotImageModified();
  void slotInsertObject( ImageChild *_child );
  void slotUpdateChildGeometry( ImageChild *_child );
  void slotFitToWindow( bool _fit );
  
  // ImageFrame signals
  void slotGeometryEnd( PartFrame_impl* );
  void slotMoveEnd( PartFrame_impl* );
  
signals:  
  void rectangleSelected( const QRect& );

protected:
  void updatePixmap();
  void scale();

  void setRectSelection( QObject *_handler );
  void paintRectSelection();
  
  void paintEvent( QPaintEvent *_ev );
  void resizeEvent( QResizeEvent *_ev );
  void keyPressEvent( QKeyEvent *_ev );
  void mousePressEvent( QMouseEvent *_ev );
  void mouseReleaseEvent( QMouseEvent *_ev );
  void mouseMoveEvent( QMouseEvent *_ev );

  bool m_bRectSelection;
  QObject* m_pRectSelectionHandler;
  QRect m_rctRectSelection;
  
  ImageDocument_impl *m_pImageDoc;

  QPixmap m_pixImage;
  bool m_bImageModified;
  /**
   * When an instance is created b_mUnderConstruction is true.
   * On the first paint event @ref #update is
   * called. @ref #update sets this flag to false.
   */
  bool m_bUnderConstruction;
  
  OPParts::MenuBarFactory_var m_vMenuBarFactory;
  MenuBar_ref m_rMenuBar;
  CORBA::Long m_idMenuView;
  CORBA::Long m_idMenuView_FitToWindow;
  CORBA::Long m_idMenuView_NewView;
  CORBA::Long m_idMenuEdit;
  CORBA::Long m_idMenuEdit_NewImage;
  CORBA::Long m_idMenuEdit_ImportImage;
  CORBA::Long m_idMenuEdit_ExportImage;
  
  OPParts::ToolBarFactory_var m_vToolBarFactory;
  ToolBar_ref m_rToolBarFile;
  CORBA::Long m_idButtonFile_Open;
  ToolBar_ref m_rToolBarEdit;
  CORBA::Long m_idButtonEdit_Darker;

  QList<ImageFrame> m_lstFrames;
};

class InsertObjectHandler : public QObject
{
  Q_OBJECT
public:
  InsertObjectHandler( ImageDocument_impl* );
  
public slots:
  void slotInsertObject( const QRect& );

protected:
  ImageDocument_impl *m_pDoc;
};

#endif
