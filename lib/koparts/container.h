#ifndef CONTAINER_H
#define CONTAINER_H

#include "part.h"
#include "view.h"

#include <qlist.h>
#include <qwmatrix.h>

class QPainter;
class QRect;
class KAction;
class QPointArray;

class ContainerHandler;
class ContainerView;

/**
 *  class ContainerPart
 *
 *  This class contains all data of a part and can paint it into a painter
 *  device. It also handles children.
 *
 *  @short Contains the data and can paint it.
 */
class ContainerPart : public Part
{
    Q_OBJECT

public:

    /**
     *  Konstructor.
     *
     *  Initializes the Part.
     */
    ContainerPart( QObject* parent = 0, const char* name = 0 );

    /**
     *  Destructor.
     */
    ~ContainerPart();

    /**
     * Inserts the new child in the list of children and emits the
     * @ref #childChanged signal.
     */
    virtual void insertChild( PartChild* child );
    
    /**
     * @return the list of all children. Do not modify the
     *         returned list.
     */
    QList<PartChild>& children();
    
    /**
     * @return the PartChild associated with the given Part, but only if
     *         "part" is a direct child of this part.
     *
     * This is a convenience function. You could get the same result
     * by traversing the list returned by @ref #children.
     */
    PartChild* child( Part* part );

    /**
     *  Paints the whole part and all its children into the given painter object.
     *
     *  @see #paintChild #paintChildren #paintContent
     */
    virtual void paintEverything( QPainter& painter, const QRect& rect,
				  bool transparent = FALSE, View* view = 0 );

    /**
     *  Paints all children. Normally called by @ref paintEverything.
     *
     *  @see #paintEverything #paintChild
     */
    virtual void paintChildren( QPainter& painter, const QRect& rect, View* view = 0 );

    /**
     *  Paint a special child. Normally called by @ref paintChildren.
     *
     *  @see #paintEverything #paintChildren
     */
    virtual void paintChild( PartChild* child, QPainter& painter, View* view = 0 );

    /**
     *  Paints the data itself. Normally called by @ref paintEverthing
     *
     *  @see #paintEverything
     */
    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE ) = 0;

    /**
     *  Retrieve a part that is hit.
     */
    virtual Part* hitTest( const QPoint& p, const QWMatrix& matrix );

signals:

    /**
     * This signal is emitted, if a direct or indirect child part changes
     * and needs to be updated in all views.
     *
     * If one of your child parts emits the childChanged signal, then you may
     * usually just want to redraw this child. In this case you can ignore the parameter
     * passes by the signal.
     */
    void childChanged( PartChild* );

private:

    QList<PartChild> m_children;
};

/**
 *  class ContainerView
 */
class ContainerView : public View
{
    friend ContainerHandler;

    Q_OBJECT

public:

    ContainerView( Part* part, QWidget* parent = 0, const char* name = 0 );

    ~ContainerView();

    virtual void paintEverything( QPainter& painter, const QRect& rect, bool transparent = FALSE );

    virtual void setScaling( double x, double y );

    virtual Part* hitTest( const QPoint& pos );

    virtual bool hasPartInWindow( Part* );

    ViewChild* child( Part* part );

    ViewChild* child( View* view );

signals:

    void childSelected( PartChild* );

    void childUnselected( PartChild* );

    void childActivated( PartChild* );

    void childDeactivated( PartChild* );

    void regionInvalidated( const QRegion&, bool erase );

    void invalidated();

protected:

    void viewSelectEvent( ViewSelectEvent* );

    void viewActivateEvent( ViewActivateEvent* );

    virtual QWMatrix matrix() const;

    /**
     * @return the selected child. The function returns 0 if
     *         no direct child is currently selected.
     */
    PartChild* selectedChild();

    /**
     * @return the active child. The function returns 0 if
     *         no direct child is currently active.
     */
    PartChild* activeChild();

protected slots:

    virtual void slotChildChanged( PartChild* );

    virtual void slotChildActivated( bool );

private:

    QList<ViewChild> m_children;
};

#endif
