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
#ifndef __koView_h__
#define __koView_h__

#include <qwidget.h>
#include <qguardedptr.h>

#include <kparts/part.h>
#include <kparts/partmanager.h>

#include <koDocumentChild.h>
#include <koChild.h>

#include <klocale.h>

class KAction;
class KActionCollection;
class QCustomEvent;
class KoDocument;
class KoMainWindow;
class KoViewPrivate;
class KoViewChild;
class KoFrame;
class KStatusBar;
class KInstance;
class DCOPObject;

namespace KParts
{
  class PartManager;
  class PartActivateEvent;
  class PartSelectEvent;
};

/**
 * This class is used to display a @ref KoDocument.
 *
 * Multiple views can be attached to one document at a time.
 */
class KoView : public QWidget, public KParts::PartBase
{
  friend class KoDocument;
  Q_OBJECT
public:
  /**
   * Creates a new view for the document. Usually you dont create views yourself
   * since the koffice components come with their own view classes which inherit
   * KoView.
   *
   * The standard way to retrieve a KoView is to call @ref KoDocument::createView.
   *
   * @param document is the document which should be displayed in this view. This pointer
   *                 must not be zero.
   *  @param name   Name of the view. The name is used in DCOP, so the name should
   *                match the pattern [A-Za-z_][A-Za-z_0-9]*.
   *
   */
  KoView( KoDocument *document, QWidget *parent = 0, const char *name = 0 );
  /**
   * Destroys the view and unregisters at the document.
   */
  virtual ~KoView();

  /**
   *  Retrieves the document object of this view.
   */
  KoDocument *koDocument() const;

  virtual void setPartManager( KParts::PartManager *manager );
  virtual KParts::PartManager *partManager() const;

  /**
   * Returns the action described action object. In fact only the "name" attribute
   * of @ref #element is of interest here. The method searches in the
   * @ref KActionCollection of this view.
   *
   * Please notice that KoView indirectly inherits KXMLGUIClient.
   *
   * @see KXMLGUIClient
   * @see KXMLGUIClient::actionCollection
   * @see KoDocument::action
   */
  virtual KAction *action( const QDomElement &element ) const;

  /**
   *  Retrieves the document that is hit. This can be an embedded document.
   *
   *  The default implementation asks @ref KoDocument::hitTest. This
   *  will iterate over all child documents to detect a hit.
   *
   *  If your koffice component has multiple pages, like for example KSpread, then the hittest
   *  may not succeed for a child that is not on the visible page. In those
   *  cases you need to reimplement this method.
   */
  virtual KoDocument *hitTest( const QPoint &pos );

  /**
   * Retrieves the left border width that is displayed around the content if
   * the view is active.
   *
   * In a spread sheet this border is for example used to display the
   * rows, while a top border is used to display the names of the cells
   * and a right and bottom border is used to display scrollbars. If the view
   * becomes inactive, then this stuff is not displayed anymore.
   *
   * @ref KoFrame uses this border information. If an embedded document becomes active
   * then it is resized so that it has enough space to display the borders and to
   * display the same content as before the activation.
   * So if for example all of your borders are 20 pixel, then activating the embedded
   * document causes the KoView to move 20 pixel up/left and the size and width increas
   * by 20+20 pixel each.
   *
   * The default border is 0.
   */
  virtual int leftBorder() const;
  /**
   * @see #leftBorder
   */
  virtual int rightBorder() const;
  /**
   * @see #leftBorder
   */
  virtual int topBorder() const;
  /**
   * @see #leftBorder
   */
  virtual int bottomBorder() const;

  /**
   * Scales the view on the content. This does not affect the contents
   * data structures. You can use this mechanism to implement a zoom
   * for example.
   *
   * The method calls @ref QWidget::update so that the scaled content
   * is automatically displayed.
   *
   * The default scaling is 1.0 in both orientations.
   */
  virtual void setZoom( double zoom );
  /**
   * @see #setZoom
   */
  virtual double zoom() const;

  /**
   * Overload this function if the content will be displayed
   * on some child widget instead of the view directly.
   *
   * By default this function returns a pointer to the view.
   */
  virtual QWidget *canvas();

  /**
   * Overload this function if the content will be displayed
   * with an offset relative to the upper left corner
   * of the canvas widget.
   *
   * By default this function returns 0.
   */
  virtual int canvasXOffset() const;

  /**
   * Overload this function if the content will be displayed
   * with an offset relative to the upper left corner
   * of the canvas widget.
   *
   * By default this function returns 0.
   */
  virtual int canvasYOffset() const;

  /**
   * Overload this function if You need to perform some actions
   * after KoView (the part widget) is inserted into canvas.
   * You should call for example addChild(QWidget*) method
   * of QScrollView here, if canvas is a viewport of QScrollView.
   *
   * By default this function does nothing.
   */
  virtual void canvasAddChild( KoViewChild *child );

  /**
   * @return the selected child. The function returns 0 if
   *         no direct child is currently selected.
   */
   virtual KoDocumentChild *selectedChild();

  /**
   * @return the active child. The function returns 0 if
   *         no direct child is currently active.
   */
  virtual KoDocumentChild *activeChild();

    // #########
  virtual void paintEverything( QPainter &painter, const QRect &rect, bool transparent = false );

  /**
   * @return TRUE if the document @p doc is represented in this view by
   *         some KoViewChild.
   *
   * This is just a convenience function for @ref #child.
   */
  bool hasDocumentInWindow( KoDocument *doc );

  /**
   * Returns the matrix which is used by the view to transform the content.
   * Currently only sclaing is supported.
   *
   * The matrix changes when calling @ref #setZoom.
   */
  QWMatrix matrix() const;

  /**
   * @return the KoViewChild which is responsible for the @p view or 0.
   *
   * This method does no recursion.
   */
  KoViewChild *child( KoView *view );
  /**
   * A convenience function which returns the KoViewChild which in turn holds the
   * @ref KoView that in turn holds the @p document.
   */
  KoViewChild *child( KoDocument *document );

  /**
   * Return a DCOP interface for this view
   * KOffice Application are strongly recommended to reimplement this method,
   * so that their dcop interface provides more functionality than the basic KoViewIface
   */
  virtual DCOPObject * dcopObject();

  virtual void setupPrinter( QPrinter &printer );
  virtual void print( QPrinter &printer );

  /**
   * @return the KoMainWindow in which this view is currently.
   * NOTE: this could be 0L, if the main window isn't a koffice main window.
   * (e.g. it can be any KParts application).
   */
  KoMainWindow * shell() const;

  /**
   * @return the statusbar of the KoMainWindow in which this view is currently.
   * NOTE: this could be 0L, if the main window isn't a koffice main window.
   * (e.g. it can be any KParts application).
   */
  KStatusBar * statusBar();

public slots:

    virtual void newView();

protected:
  /**
   * This method handles two events: @ref KParts::PartActivateEvent and @ref KParts::PartSelectEvent.
   * The handlers @ref #partActivateEvent or @ref #partSelectEvent are called if such an event is found.
   */
  virtual void customEvent( QCustomEvent *ev );

  /**
   * Handles the event KParts::PartActivateEvent.
   */
  virtual void partActivateEvent( KParts::PartActivateEvent *event );
  virtual void partSelectEvent( KParts::PartSelectEvent *event );
  virtual void guiActivateEvent( KParts::GUIActivateEvent * );


  /**
   * You have to implement this method and disable/enable certain functionality (actions for example) in
   * your view to allow/disallow editing of the document.
   */
  virtual void updateReadWrite( bool readwrite ) = 0;

  virtual void setupGlobalActions( void );

  KAction *actionNewView;

signals:
  void activated( bool active );
  void selected( bool select );

  void childSelected( KoDocumentChild *child );
  void childUnselected( KoDocumentChild *child );

  void childActivated( KoDocumentChild *child );
  void childDeactivated( KoDocumentChild *child );

  void regionInvalidated( const QRegion &region, bool erase );

  void invalidated();

// KDE invents public signals :)
#undef signals
#define signals public
signals:

  /**
    * Make it possible for ie. plugins to request
    * the embedding of an image into the current
    * document. Used ie. by the scan-plugin
  */
  void embeddImage(const QString &filename);

#undef signals
#define signals protected

protected slots:
  virtual void slotChildActivated( bool a );
  virtual void slotChildChanged( KoDocumentChild *child );

private:
  KoViewPrivate *d;
};

class KoViewChild : public KoChild
{
  Q_OBJECT
public:
  KoViewChild( KoDocumentChild *child, KoView *_parentView );
  virtual ~KoViewChild();

  KoDocumentChild *documentChild() const { return m_child; }
  KoView *parentView() const { return m_parentView; }
  KoFrame *frame() const { return m_frame; }

private slots:
  void slotFrameGeometryChanged();
  void slotDocGeometryChanged();
private:
  QGuardedPtr<KoDocumentChild> m_child;
  QGuardedPtr<KoFrame> m_frame;
  QGuardedPtr<KoView> m_parentView;
  class KoViewChildPrivate;
  KoViewChildPrivate *d;
};

#endif
