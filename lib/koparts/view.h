#ifndef OFFICE_VIEW_H
#define OFFICE_VIEW_H

#include <qwidget.h>
#include <qvaluelist.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include <qevent.h>

class Part;
class PartChild;
class Shell;
class View;
class Frame;
class ViewSelectEvent;
class ViewActivateEvent;
class Plugin;

class ViewChild : public QObject
{
    Q_OBJECT
public:
    ViewChild( PartChild* child, Frame* frame );
    ~ViewChild();

    PartChild* partChild();
    Frame* frame();

private slots:
    void slotDestroyed();

private:
    Frame* m_frame;
    PartChild* m_child;
};

/**
 *  class View
 *
 *  A view paints the data of a part object.
 */
class View : public QWidget
{
    Q_OBJECT

public:

    /**
     *  Initializes a view with a part.
     */
    View( Part* part, QWidget* parent = 0, const char* name = 0 );

    ~View();

    /**
     *  Retrieves the part object of this view.
     */
    Part* part();

    /**
     *  Retrieves the shell object of this view.
     */
    Shell* shell();

    /**
     *  Retrieves an action with a given name.
     *
     *  @return The retrieved action, may be NULL.
     */
    QAction* action( const char* name );
    
    /**
     *  Retrieves all possible actions of the view.
     */
    QActionCollection* actionCollection();

    virtual int leftBorder() const;
    virtual int rightBorder() const;
    virtual int topBorder() const;
    virtual int bottomBorder() const;

    virtual void setScaling( double x, double y );
    double xScaling() const;
    double yScaling() const;

    /**
     *  Retrieves the part that is hit. This can be an embedded part.
     */
    virtual Part* hitTest( const QPoint& pos );

    /**
     * Reimplemented to catch some events about activation.
     */
    virtual void customEvent( QCustomEvent* );

    /**
     * Overload this function if the content will be displayed
     * on some child widget instead of the view directly.
     *
     * By default this function returns a pointer to the view.
     */
    virtual QWidget* canvas();
    
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
     * Loads a plugin for this view.
     *
     * @param libname is the library in which the plugin is
     *        implemented. This is usually something like
     *        "libkspreadcalc".
     */
    virtual Plugin* plugin( const char* libname );

    virtual bool doubleClickActivation() const;

signals:

    void selected( bool );
    void activated( bool );

protected:

    virtual void viewSelectEvent( ViewSelectEvent* );
    virtual void viewActivateEvent( ViewActivateEvent* );

private:

    Part* m_part;
    QActionCollection m_collection;
    double m_scaleX;
    double m_scaleY;
};

/**
 *  @internal
 */
class ViewActivateEvent : public QEvent
{
public:
    ViewActivateEvent( Part* part, bool activated );

    bool activated() const;
    bool deactivated() const;

    Part* part();

private:
    bool m_active;
    Part* m_part;
};

/**
 *  @internal
 */
class ViewSelectEvent : public QEvent
{
public:
    ViewSelectEvent( Part* part, bool selected );

    bool selected() const;
    Part* part();

private:
    bool m_selected;
    Part* m_part;
};

#endif
