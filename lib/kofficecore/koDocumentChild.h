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
#ifndef __koDocumentChild_h__
#define __koDocumentChild_h__

#include <qobject.h>
#include <qwmatrix.h>
#include <qdom.h>

#include <kurl.h>

#include <komlParser.h>

class KoStore;
class KoDocument;
class KoDocumentChildPrivate;

/**
 *  Holds an embedded object.
 */
class KoDocumentChild : public QObject
{
  Q_OBJECT
public:
  enum Gadget { NoGadget, TopLeft, TopMid, TopRight, MidLeft, MidRight,
		BottomLeft, BottomMid, BottomRight, Move };

  KoDocumentChild( KoDocument* parent, KoDocument* doc, const QRect& geometry );

  /**
   * When using this constructor you must call @ref setDocument before
   * you can call any other function of this class.
   */
  KoDocumentChild( KoDocument* parent );

  virtual ~KoDocumentChild();

  /**
   * Call this function only directly after calling the constructor
   * that takes only a parent as argument.
   */
  virtual void setDocument( KoDocument *doc, const QRect &geometry );

  /**
   *  Sets a new geometry for this child document.
   */
  virtual void setGeometry( const QRect &rect );

  /**
   * @return the rectangle that would be used to display this
   *         child document if the child is not rotated or
   *         subject to some other geometric transformation.
   *         The rectangle is in the coordinate system of the parent.
   *
   * @see #setGeometry
   */
  virtual QRect geometry() const;

  virtual KoDocument *document() const;

  virtual KoDocument *parentDocument() const;

  /**
   * @return the region of this child part relative to the
   *         coordinate system of the parent.
   *         The region is transformed with the passed
   *         matrix.
   */
  virtual QRegion region( const QWMatrix& = QWMatrix() ) const;

  /**
   * @return the polygon which surrounds the child part. The points
   *         are in coordinates of the parent.
   *         The points are transformed with the
   *         passed matrix.
   */
  virtual QPointArray pointArray( const QWMatrix &matrix = QWMatrix() ) const;

  /**
   * Tests wether the part contains a certain point. The point is
   * in tghe corrdinate system of the parent.
   */
  virtual bool contains( const QPoint& ) const;

  /**
   * @return the effective bounding rect after all transformations.
   *         The coordinates of the rectangle are in the coordinate system
   *         of the parent.
   */
  virtual QRect boundingRect( const QWMatrix &matrix = QWMatrix() ) const;

  /**
   * Scales the content of the child part. However, that does not
   * affect the size of the child part.
   */
  virtual void setScaling( double x, double y );

  virtual double xScaling() const;

  virtual double yScaling() const;

  virtual void setShearing( double, double );

  virtual double xShearing() const;

  virtual double yShearing() const;

  virtual void setRotation( double );

  virtual double rotation() const;

  virtual void setRotationPoint( const QPoint& pos );

  virtual QPoint rotationPoint() const;

  /**
   * @return TRUE if the child part is an orthogonal rectangle relative
   *         to its parents corrdinate system.
   */
  virtual bool isRectangle() const;
	
  /**
   * Sets the clip region of the painter, so that only pixels of the
   * child part can be drawn.
   *
   * @param combine tells wether the new clip region is an intersection
   *        of the current region with the childs region or wether only
   *        the childs region is set.
   */
  virtual void setClipRegion( QPainter& painter, bool combine = TRUE );

  /**
   * Transforms the painter (its worldmatrix and the clipping)
   * in such a way, that the painter can be passed to the child part
   * for drawing.
   */
  virtual void transform( QPainter& painter );

  /**
   * @return the contents rectangle that is visible.
   *         This value depends on the scaling and the
   *         geometry.
   *
   * @see scaling geomtry
   */
  virtual QRect contentRect() const;

  virtual QRegion frameRegion( const QWMatrix& matrix, bool solid = FALSE ) const;

  virtual QPointArray framePointArray( const QWMatrix &matrix = QWMatrix() ) const;

  virtual KoDocument* hitTest( const QPoint& p, const QWMatrix& matrix = QWMatrix() );

  virtual Gadget gadgetHitTest( const QPoint& p, const QWMatrix& matrix );

  virtual QWMatrix matrix() const;

  virtual void lock();

  virtual void unlock();

  virtual QPointArray oldPointArray( const QWMatrix &matrix );

  virtual void setTransparent( bool transparent );

  virtual bool isTransparent() const;

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

signals:
  void changed( KoDocumentChild *thisChild );

protected:

    virtual QPointArray pointArray( const QRect& r, const QWMatrix& matrix ) const;

    virtual void updateMatrix();

  /**
   * Called if @ref #load finds a tag that it does not understand.
   *
   * @return TRUE if the tag could be handled. The default implementation
   *         returns FALSE.
   */
  virtual bool loadTag( KOMLParser& parser, const string& tag, vector<KOMLAttrib>& lst2 );

private:

  virtual void init();

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

  KoDocumentChildPrivate *d;
};


#endif
