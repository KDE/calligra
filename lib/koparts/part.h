#ifndef OFFICE_PART_H
#define OFFICE_PART_H

#include <qaction.h>
#include <qobject.h>
#include <qlist.h>
#include <qstring.h>
#include <qrect.h>
#include <qwmatrix.h>
#include <qregion.h>
#include <qpoint.h>

#include "view.h"

class QWidget;
class QPainter;

class Shell;

/**
 *  Contains the data and can paint it.
 */
class Part : public QObject
{
    Q_OBJECT

public:

    /**
     *  Constructor.
     *
     *  @param parent Pointer to parent object, may be NULL.
     *  @param name   Name of the part, may be NULL.
     */
    Part( QObject* parent = 0, const char* name = 0 );
    
    /**
     *  Destructor.
     */
    ~Part();

    /**
     *  @return a pointer to the parent part. If there is no parent
     *          or if the parent is not a Part, then 0 is returned.
     */
    Part* parentPart();

    /**
     *  Create a new view for the part.
     *
     *  You need to overload this method to create a view of your desired type.
     *
     *  @param parent Parent widget of the view.
     *  @param name   Name of the view.
     *
     *  @see createShell
     */
    virtual View* createView( QWidget* parent = 0, const char* name = 0 ) = 0;
    
    /**
     *  Create a new toplevel shell for the part. This in turn will create
     *  a new view.
     *
     *  You have to overload this method to return a shell of your desired type.
     *
     *  @see createView
     */
    virtual Shell* createShell() = 0;

    /**
     *  @return the XML GUI configuration of the part.
     *          This is used by the @ref Shell to construct menus and
     *          toolbars if the Part is activated.
     *
     *  This functionality is in the Part and not in the View since
     *  a part may be activated and have menus etc. without having a view.
     *  That is the case if the part is directly drawn on some widget.
     */
    QString config();

    /**
     *  A convenience function
     *
     *  @param p is in the coordinate system of this part.
     */
    Part* hitTest( const QPoint& p );
    
    /**
     *  Find the most nested child part which contains the
     *  questionable point. The point is in the coordinate system
     *  of this part. If no child part contains this point, then
     *  a pointer to this part is returned.
     *
     *  This function has to be overloaded if the part features child parts.
     *
     *  @param matrix transforms points from the parts coordinate system
     *         to the coordinate system of the questionable point.
     *  @param p is in some unknown coordinate system, but the matrix can
     *         be used to transform a point of this parts coordinate system
     *         to the coordinate system of p.
     *
     *  @return Pointer to the part, that was hit.
     */
    virtual Part* hitTest( const QPoint& p, const QWMatrix& matrix );

    /**
     *  Retrieves an action with a given name
     *
     *  @param name Name of the desired action.
     *
     *  @return The action, may be NULL if no action with the desired name found.
     */
    QAction* action( const char* name );

    /**
     *  Retrives all actions of this part.
     *
     *  @return An action collection with all actions.
     */
    QActionCollection* actionCollection();

    /**
     *  Paints the whole part into the given painter object.
     *
     *  @param painter     The painter object into that should be drawn.
     *  @param rect        The rect that should be used in the painter object.
     *  @param transparent .
     *  @param view        .
     */
    virtual void paintEverything( QPainter& painter, const QRect& rect,
				  bool transparent = FALSE, View* view = 0 ) = 0;

protected:

    /**
     *  Retrieves the resource filename of the part.
     *
     *  You have tp overload it to return the right filename.
     */
    virtual QString configFile() const = 0;

    virtual QString readConfigFile( const QString& filename ) const;

    /**
     *  Adds a view  to the part.
     */
    virtual void addView( View* );

    /**
     *  Retrieves the first view of the part.
     *
     *  @see nextView
     */
    virtual View* firstView();
    
    /**
     *  Retrieves the next view of the part after you called @ref firstView
     *  or nestView itself.
     *
     *  @see firstView
     */
    virtual View* nextView();

protected slots:

    void slotViewDestroyed();

private:

    QList<View> m_views;
    QString m_config;
    QActionCollection m_collection;
};

class PartChild : public QObject
{
    Q_OBJECT

public:

    enum Gadget { NoGadget, TopLeft, TopMid, TopRight, MidLeft, MidRight,
		  BottomLeft, BottomMid, BottomRight, Move };

    PartChild( Part* parent, Part* part, const QRect& geometry );

    /**
     * When using this function you must call @ref setPart before
     * you can call any other function of this class.
     */
    PartChild( Part* parent );
    
    ~PartChild();

    /**
     * Call this function only directly after calling the constructor
     * that takes only a parent as argument.
     */
    virtual void setPart( Part* part, const QRect& geometry );

    /**
     *  Sets a new geometry for this child part.
     */
    void setGeometry( const QRect& rect );

    /**
     * @return the rectangle that would be used to display this
     *         child part if the child is not rotated or
     *         subject to some other geometric transformation.
     *         The rectangle is in the coordinate system of the parent.
     *
     * @see #setGeometry
     */
    QRect geometry() const;

    /**
     *  Retrieves the part object.
     */
    Part* part();

    /*
     * Set a different matrix. This is useful for shearing, scaling
     * and rotating the child part.
     *
     * @see #setScaling
     */
    // void setMatrix( const QWMatrix& );
    // QWMatrix matrix() const;
    
    /**
     * @return the region of this child part relative to the
     *         coordinate system of the parent.
     */
    QRegion region() const;
    
    /**
     * A convenience function. The region is transformed with the passed
     * matrix.
     */
    QRegion region( const QWMatrix& ) const;
    
    /**
     * @return the polygon which surrounds the child part. The points
     *         are in coordinates of the parent.
     */
    QPointArray pointArray() const;
    
    /**
     * A convenience function. The points are transformed with the
     * passed matrix.
     */
    QPointArray pointArray( const QWMatrix& ) const;
    
    /**
     * Tests wether the part contains a certain point. The point is
     * in tghe corrdinate system of the parent.
     */
    bool contains( const QPoint& ) const;
    
    /**
     * @return the effective bounding rect after all transformations.
     *         The coordinates of the rectangle are in the coordinate system
     *         of the parent.
     */
    QRect boundingRect() const;
    
    QRect boundingRect( const QWMatrix& ) const;

    /**
     * Scales the content of the child part. However, that does not
     * affect the size of the child part.
     */
    void setScaling( double, double );
    
    double xScaling() const;
    
    double yScaling() const;

    void setShearing( double, double );
    
    double xShearing() const;
    
    double yShearing() const;

    void setRotation( double );
    
    double rotation() const;

    void setRotationPoint( const QPoint& pos );
    
    QPoint rotationPoint() const;

    /**
     * @return TRUE if the child part is an orthogonal rectangle relative
     *         to its parents corrdinate system.
     */
    bool isRectangle() const;
	
    /**
     * Sets the clip region of the painter, so that only pixels of the
     * child part can be drawn.
     *
     * @param combine tells wether the new clip region is an intersection
     *        of the current region with the childs region or wether only
     *        the childs region is set.
     */
    void setClipRegion( QPainter& painter, bool combine = TRUE );
    
    /**
     * Transforms the painter (its worldmatrix and the clipping)
     * in such a way, that the painter can be passed to the child part
     * for drawing.
     */
    void transform( QPainter& painter );
    
    /**
     * @return the contents rectangle that is visible.
     *         This value depends on the scaling and the
     *         geometry.
     *
     * @see scaling geomtry
     */
    QRect contentRect() const;

    QRegion frameRegion( const QWMatrix& matrix, bool solid = FALSE ) const;
    
    QPointArray framePointArray() const;
    
    QPointArray framePointArray( const QWMatrix& ) const;

    Part* hitTest( const QPoint& p );
     
    Part* hitTest( const QPoint& p, const QWMatrix& matrix );

    Gadget gadgetHitTest( const QPoint& p, const QWMatrix& matrix );

    QWMatrix matrix() const;

    void lock();
     
    void unlock();

    QPointArray oldPointArray( const QWMatrix& );

    void setTransparent( bool );
    
    bool isTransparent();

signals:
     
    void changed( PartChild* child );

protected:
     
    QPointArray pointArray( const QRect& r, const QWMatrix& matrix ) const;
     
    void updateMatrix();

private:
     
    Part* m_part;
    QRect m_geometry;
    double m_rotation;
    double m_shearX;
    double m_shearY;
    QPoint m_rotationPoint;
    double m_scaleX;
    double m_scaleY;
    QWMatrix m_matrix;
    bool m_lock;
    QPointArray m_old;
    bool m_transparent;
};

#endif
