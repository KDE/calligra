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

/**
 * This class holds the views of embedded documents.
 * It derived from QWidget, this means you can move
 * and resize it as usual.
 * Destroying this widget means that the hosted view
 * becomes destroyed, too.
 */
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
  virtual ~ImageView_impl();

  // IDL  
  virtual void toggleFitToWindow();
  virtual void newView();
  /**
   * Called if the corresponding menu entry has been selected.
   * The user is queried for the kind of object he wants to
   * insert ( using @ref KoPartSelectDia ) and afterwards
   * the user must draw a rectangle to define size and position
   * of the new object.
   *
   * @see startRectSelection
   */
  virtual void insertObject();
  virtual void importImage();
  virtual void exportImage();

  // C++
  virtual void setFitToWindow( bool _fit );
    
  virtual void setDocument( ImageDocument_impl *_doc );

  virtual void createGUI();
  /**
   * Called on the first paint event. This function
   * brings the view and the document in sync.
   */
  virtual void construct();

public slots:
  // Document signals
  void slotImageModified();
  /**
   * Called if there is a new embedded document. In this case we have
   * to create a view for it.
   */
  void slotInsertObject( ImageChild *_child );
  /**
   * Called if the size/position of an embedded document changed.
   * In this case we have to adapt position and size of the
   * corresponding view.
   */
  void slotUpdateChildGeometry( ImageChild *_child );
  void slotFitToWindow( bool _fit );
  
  // ImageFrame signals
  /**
   * Called if the user changed the size of the view.
   * In this case we must tell the document that its
   * embedded document has to change its size. This will
   * in turn cause all the other views to adapt their embedded
   * view, too.
   */
  void slotGeometryEnd( PartFrame_impl* );
  /**
   * Called if the user changed the position of the view.
   * In this case we must tell the document that its
   * embedded document has to change its position. This will
   * in turn cause all the other views to adapt their
   * embedded views, too.
   */
  void slotMoveEnd( PartFrame_impl* );
  
signals:  
  void rectangleSelected( const QRect& );

protected:
  virtual void cleanUp();
  
  void updatePixmap();
  void scale();

  /**
   * This function is used to initiate inserting
   * of a new object. The user has now to draw
   * a rectangle to define the objects size and
   * position.
   *
   * @see #paintRectSelection
   * @see #mousePressEvent
   * @see #mouseMoveEvent
   * @see #mouseReleaseEvent
   */
  void startRectSelection( const char *_part_name );
  /**
   * Called if the user presses ESC to abort the inserting
   * process.
   *
   * @param _part_name is stored in @ref #m_strNewPart for
   *                   later usage.
   *
   * @see #keyPressEvent
   */
  void cancelRectSelection();
  /**
   * Draws the rectangle.
   *
   * @see #paintEvent
   */
  void paintRectSelection();
  
  void paintEvent( QPaintEvent *_ev );
  void resizeEvent( QResizeEvent *_ev );
  void keyPressEvent( QKeyEvent *_ev );
  void mousePressEvent( QMouseEvent *_ev );
  void mouseReleaseEvent( QMouseEvent *_ev );
  void mouseMoveEvent( QMouseEvent *_ev );

  bool m_bRectSelection;
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

  QString m_strNewPart;
};

#endif
