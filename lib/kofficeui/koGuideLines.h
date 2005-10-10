// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright ( C ) 2005 Thorsten Zachmann <zachmann@kde.org>
   based on work done by Dave Marotti <landshark_666@hotmail.com>
*/
#ifndef KOGUIDELINES_H
#define KOGUIDELINES_H

#include <qevent.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qptrlist.h>

#include <koPoint.h>
#include <kozoomhandler.h>

class KoView;
class KPopupMenu;

class KoGuideLineData
{
public:
    KoGuideLineData( Qt::Orientation o, double pos, const QSize &size );
    ~KoGuideLineData();
        
    void resize( const QSize &size );
    
    Qt::Orientation orientation() { return m_orient; }
    void setOrientation( Qt::Orientation o ) { m_orient = o; }

    double position() { return m_pos; }
    void setPosition( double p ) { m_pos = p; };

    QPixmap * buffer() { return &m_buffer; } 

    bool isSelected() { return m_selected; }
    void setSelected( bool s ) { m_selected = s; }

    bool hasBuffer() { return m_hasBuffer; }
    void setHasBuffer( bool b ) { m_hasBuffer = b; }

private:
    Qt::Orientation m_orient;
    double m_pos;
    QPixmap m_buffer;
    bool m_hasBuffer;
    bool m_selected;
};

class KoGuidesList : public QPtrList<KoGuideLineData>
{
public:
    KoGuidesList() {}

protected:
    int compareItems( QPtrCollection::Item i1, QPtrCollection::Item i2 )
    {
        KoGuideLineData* s1 = (KoGuideLineData*)i1;
        KoGuideLineData* s2 = (KoGuideLineData*)i2;
        if ( ( s1->orientation() == Qt::Vertical ) && ( s2->orientation() == Qt::Horizontal ) )
            return -1;
        if ( ( s1->orientation() == Qt::Horizontal ) && ( s2->orientation() == Qt::Vertical ) )
            return 1;

        if ( s1->position() > s2->position() )
            return -1;

        if ( s1->position() < s2->position() )
            return 1;

        return 0;
    }
};


/**
 * @brief Class for handling of guide lines
 */
class KoGuideLines : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructor
     *
     * @param view The view in which the guides will be shown
     * @param zoomHandler The zoom handler of the view
     * @param buffer Buffer used for double buffering of the widget.
     *        The buffer has to have the same size as the widget. If buffer is
     *        0 no buffer will be used.
     */
    KoGuideLines( KoView *view, KoZoomHandler *zoomHandler, QPixmap *buffer );

    /**
     * @brief Destructor
     */
    ~KoGuideLines();

    /**
     * @brief Handle mousePressEvent
     *
     * This checks if a mousePressEvent would affect a guide line.
     * If the mouse is pressed over a guide line it gets selected.
     * Guide lines which were select get unselect.
     * If also the Ctrl Key is pressed the selection of the guide 
     * gets toggled.
     * If no guide is under the position all guides get deselected.
     *
     * @param e QMouseEvent
     *
     * @return true if the event was handled
     * @return false otherwise
     *         The event counts a not handled when only guides where
     *         deselected.
     */
    bool mousePressEvent( QMouseEvent *e );

    /**
     * @brief Handle mouseMoveEvent
     *
     * The mouse button is pressed and a guide was selected it moves the 
     * selected guides.
     * If the mouse is moved over a guide line the cursor gets updated.
     *
     * @param e QMouseEvent
     *
     * @return true if the event was handled (guide moved, cursor changed as
     *         guide lies below)    
     * @return false otherwise
     */
    bool mouseMoveEvent( QMouseEvent *e );
    
    /**
     *
     * @param e QMouseEvent
     *
     * @return true if the event was handled
     * @return false otherwise
     */
    bool mouseReleaseEvent( QMouseEvent *e );
    
    /**
     *
     * @param e QKeyEvent
     *
     * @return true if the event was handled
     * @return false otherwise
     */
    bool keyPressEvent( QKeyEvent *e );

    /**
     * @brief resize
     *
     * This have to be called when the view is resized
     */
    void resize();

    /**
     * @brief Erase the guide lines
     *
     * Erases the guide lines from the buffer and if set also from the canvas. 
     *
     * @param updateCanvas If true it also updates the canvas of the view.
     */
    void erase( bool updateCanvas = true );

    /**
     * @brief Paint the guide lines
     *
     * Paint the guide lines to the buffer and if set also to the canvas. 
     *
     * @param updateView If ture it also updates the canvas of the view.
     */
    void paint( bool updateCanvas = true );

    /**
     * @brief Set the guide lines.
     *
     * This removes all existing guides and set up ne ones at the positions given.
     *
     * @param horizontalPos A list of the position of the horizontal guide lines.
     * @param verticalPos A list of the position of the vertical guide lines.
     */
    void setGuideLines( const QValueList<double> &horizontalPos, const QValueList<double> &verticalPos );

    /**
     * @brief Get the position of the guide lines
     *
     * This filles the passed lists with the positions of the guide lines. 
     * The lists will be emptied before any positions are added.
     *
     * @param horizontalPos A list of the position of the horizontal guide lines.
     * @param verticalPos A list of the position of the vertical guide lines.
     */
    void getGuideLines( QValueList<double> &horizontalPos, QValueList<double> &verticalPos );

public slots:
    /**
     * @brief Move Guide
     *
     * This slot can be connected to void KoRuler::moveGuide( const QPoint &, bool, int );
     * It will add a new guide when you move from the ruler to the canvas. After that it
     * moves the guide.
     *
     * @param pos The pos of the mouse 
     * @param horizontal true if the guide is horizontal, false if vertical
     * @param rulerWidth The witdth of the ruler as the pos is seen from the ruler widget.
     */
    void moveGuide( const QPoint &pos, bool horizontal, int rulerWidth );

    /**
     * @brief Add Guide
     *
     * This slot can be connected to void KoRuler::addGuide( const QPoint &, bool, int );
     * It will finish the inserting of a guide from moveGuide().
     *
     * @param pos The pos of the mouse 
     * @param horizontal true if the guide is horizontal, false if vertical
     * @param rulerWidth The witdth of the ruler as the pos is seen from the ruler widget.
     */
    void addGuide( const QPoint &pos, bool horizontal, int rulerWidth );

signals:
    /**
     * @brief Signal that shows that the guide lines are changed
     *
     * This signal is emmited when the guide lines are changed ( moved / deleted )
     */
    void guideLinesChanged( KoView * view );
    

private:
    /// the pattern used for painting a guide line
    static QPixmap * m_pattern;
    /// the pattern used for painting a selected guide line
    static QPixmap * m_selectedPattern;

    /**
     * @brief Add a guide line with the orientation o at the position pos
     *
     * @param pos where to insert the guide
     * @param o orientation of the guide line
     */
    void add( QPoint &pos, Qt::Orientation o );
    
    /**
     * @brief Select a guide
     *
     * @param gd guide to select
     */
    void select( KoGuideLineData *gd );

    /**
     * @brief Unselect a guide
     *
     * @param gd guide to unselect
     */
    void unselect( KoGuideLineData *gd );
        
    /**
     * @brief Unselect all selected KoGuideLineData
     *
     * @return true, when selection was changed
     * @return false otherwise
     */
    bool unselectAll();

    /**
     * @brief remove all selected guides
     */
    void removeSelected();

    /**
     * @brief Check if at least one guide is selected
     *
     * @return true if at least on guide is seleted
     * @return false otherwise
     */
    bool hasSelected();

    /**
     * @brief Find a guide
     *
     * This function looks for a guide at x or y pos. The position can differ by
     * diff.
     *
     * @param x x position to look for a guide
     * @param y y position to look for a guide
     * @param diff how far next to a guide sould it also be found
     *
     * @return the fould guide
     * @return 0 if none is found
     */
    KoGuideLineData * find( double x, double y, double diff );

    /**
     * @brief Move selected guides.
     *
     * This moves all selected guides around. If more than one guide is selected it makes
     * sure the guides are not moved of the canvas.
     *
     * @param pos position of the mouse
     */
    void moveSelectedBy( QPoint &p );
    
    /**
     * @brief Resize the guide line masters
     *
     * @param size the size of the canvas
     * @param vLine the vertical line master
     * @param hLine the horizontal line master
     * @param linePattern the pattern to use
     */
    void resizeLinesPixmap( const QSize &s, QPixmap &vLine, QPixmap &hLine, const QPixmap &linePattern );

    /**
     * @brief Erase a guide 
     *
     * Erases the guide line from the buffer and if set also from the canvas. 
     *
     * @param gd guide
     * @param size of the canvas
     * @param updateView If ture it also updates the canvas of the view.
     */
    void eraseGuide( KoGuideLineData *gd, const QSize &size, bool updateCanvas );
    
    /**
     * @brief Paint a guide 
     *
     * Paint the guide line to the buffer and if set also from the canvas. 
     *
     * @param gd guide
     * @param size of the canvas
     * @param updateView If ture it also updates the canvas of the view.
     */
    void paintGuide( KoGuideLineData *gd, const QSize &size, bool updateCanvas, int dx, int dy );

    /**
     * @brief Map pos from screen
     * 
     * @param pos on screen
     *
     * @return pos in document
     */
    KoPoint mapFromScreen( const QPoint & pos );

    /**
     * @brief Map pos to screen
     * 
     * @param pos in document
     *
     * @return pos on screen
     */
    QPoint mapToScreen( const KoPoint & pos );

    /// view
    KoView * m_view;
    /// buffer for double buffering
    QPixmap * m_buffer;
    /// zoom handler of the view
    KoZoomHandler * m_zoomHandler;
    /// a list of all guides
    KoGuidesList m_guides;
    /// a list of all selected guides
    KoGuidesList m_selectedGuides;
    /// used to save the last mouse position
    QPoint m_lastPoint;
    /// true if a guide is selected at the moment
    bool m_mouseSelected;
    /// true if a guide is inserted at the moment
    bool m_insertGuide;
    /// pixmap master of a vertical guide
    QPixmap m_vGuideLine;
    /// pixmap master of a horizontal guide
    QPixmap m_hGuideLine;
    /// pixmap master of a selected vertical guide
    QPixmap m_selectedVGuideLine;
    /// pixmap master of a selected horizontal guide
    QPixmap m_selectedHGuideLine;
    /// TODO
    KPopupMenu *m_glMenu;
};

#endif /* KOGUIDELINES_H */

