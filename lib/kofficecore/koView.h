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

class QAction;
class QActionCollection;
class QCustomEvent;
class KoDocument;
class KoDocumentChild;
class KoViewPrivate;
class KoViewChild;
class KoFrame;
class KInstance;

namespace KParts
{
  class PartManager;
  class PartActivateEvent;
  class PartSelectEvent;
};

class KoView : public QWidget, public KParts::PartBase
{
  friend class KoDocument;
  Q_OBJECT
public:
  KoView( KoDocument *document, QWidget *parent = 0, const char *name = 0 );
  virtual ~KoView();

  /**
   *  Retrieves the document object of this view.
   */
  KoDocument *koDocument() const;

  virtual void setPartManager( KParts::PartManager *manager );
  virtual KParts::PartManager *partManager() const;

  /**
   *  Retrieves the document that is hit. This can be an embedded document.
   */
  virtual KoDocument *hitTest( const QPoint &pos );

  virtual int leftBorder() const;
  virtual int rightBorder() const;
  virtual int topBorder() const;
  virtual int bottomBorder() const;

  virtual void setScaling( double x, double y );
  virtual double xScaling() const;
  virtual double yScaling() const;

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
   * @return the selected child. The function returns 0 if
   *         no direct child is currently selected.
   */
  virtual KoDocumentChild *selectedChild();

  /**
   * @return the active child. The function returns 0 if
   *         no direct child is currently active.
   */
  virtual KoDocumentChild *activeChild();

  virtual void paintEverything( QPainter &painter, const QRect &rect, bool transparent = false );

  bool hasDocumentInWindow( KoDocument *doc );

  QWMatrix matrix() const;

  KoViewChild *child( KoView *view );
  KoViewChild *child( KoDocument *document );

protected:
  virtual void customEvent( QCustomEvent *ev );

  virtual void partActivateEvent( KParts::PartActivateEvent *event );
  virtual void partSelectEvent( KParts::PartSelectEvent *event );

  /**
   * You have to implement this method and disable/enable certain functionality (actions for example) in
   * your view to allow/disallow editing of the document.
   */
  virtual void updateReadWrite( bool readwrite ) = 0;

signals:
  void activated( bool active );
  void selected( bool select );

  void childSelected( KoDocumentChild *child );
  void childUnselected( KoDocumentChild *child );

  void childActivated( KoDocumentChild *child );
  void childDeactivated( KoDocumentChild *child );

  void regionInvalidated( const QRegion &region, bool erase );

  void invalidated();

protected slots:
  virtual void slotChildActivated( bool a );
  virtual void slotChildChanged( KoDocumentChild *child );

private:
  KoViewPrivate *d;
};

class KoViewChild
{
public:
  KoViewChild( KoDocumentChild *child, KoFrame *frame );
  virtual ~KoViewChild();

  QGuardedPtr<KoDocumentChild> documentChild() const { return m_child; }
  QGuardedPtr<KoFrame> frame() const { return m_frame; }

private:
  QGuardedPtr<KoDocumentChild> m_child;
  QGuardedPtr<KoFrame> m_frame;
};

#endif
