// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KOGUIDES_H
#define KOGUIDES_H

#include <qevent.h>
#include <qobject.h>

#include <koffice_export.h>

class QPaintDevice;
class KoPoint;
class KoRect;
class KoView;
class KoZoomHandler;

class KOFFICEUI_EXPORT KoGuides : public QObject
{
    Q_OBJECT
public:        
    /**
     * @brief Constructor
     *
     * @param view The view in which the guides will be shown
     * @param zoomHandler The zoom handler of the view
     */
    KoGuides( KoView *view, KoZoomHandler *zoomHandler );

    /**
     * @brief Destructor
     */
    ~KoGuides();
        
    /**
     * @brief Paint the guides
     *
     * @param painter with which the guides are painted
     */
    void paintGuides( QPainter &painter );

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
    void getGuideLines( QValueList<double> &horizontalPos, QValueList<double> &verticalPos ) const;

    /**
     * @brief Snap rect to guidelines
     *
     * This looks fo a guide which is in reach for the guide as defined in snap.
     *
     * @param rect the rect which should be snapped
     * @param snap the distance the guide should snap
     *
     * @return the distance to the guide or ( 0, 0 ) if there is no guide to snap to.
     */
    KoPoint snapToGuideLines( KoRect &rect, int snap );

    /**
     * @brief Find the next guide distance to snap to
     *
     * @param rect the rect which should be snapped
     * @param diffx The range in x distance in which the guide has to be.
     * @param diffy The range in y distance in which the guide has to be.
     *
     * @return the distance to the guide or ( 0, 0 ) if there is no guide to snap to.
     */
    KoPoint diffGuide( KoRect &rect, double diffx, double diffy );

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
     *
     * @param view The view in which the guide lines are changed.
     */
    void guideLinesChanged( KoView * view );

    /**
     * @brief This signal is emitted when guides start/stop moving.
     *
     * @param state true when starting moving guides, false when stopping.
     */
    void moveGuides( bool state );

private slots:
    /**
     * @brief Execute a dialog to set the position of the guide
     */
    void slotChangePosition();

    /**
     * @brief remove all selected guides
     */
    void slotRemove();

private:
    /// Strukt holding the data of a guide line
    struct KoGuideLine 
    {
        KoGuideLine( Qt::Orientation o, double pos )
        : orientation( o )
        , position( pos )
        , selected( false )
        {}
        Qt::Orientation orientation;
        double position;
        bool selected;
    };

    /**
     * @brief Paint the canvas
     */ 
    void paint();

    /**
     * @brief Add a guide line with the orientation o at the position pos
     *
     * @param pos where to insert the guide
     * @param o orientation of the guide line
     */
    void add( Qt::Orientation o, QPoint &pos );
    
    /**
     * @brief Select a guide
     *
     * @param gd guide to select
     */
    void select( KoGuideLine *guideLine );

    /**
     * @brief Unselect a guide
     *
     * @param gd guide to unselect
     */
    void unselect( KoGuideLine *guideLine );
        
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
    KoGuideLine * find( KoPoint &p, double diff );

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
    /// zoom handler of the view
    KoZoomHandler * m_zoomHandler;
    /// list of the position of unselected guide lines
    QValueList<KoGuideLine *> m_guideLines;
    /// list of the position of selected guide lines
    QValueList<KoGuideLine *> m_selectedGuideLines;
    /// used to save the last mouse position
    QPoint m_lastPoint;
    /// true if a guide is selected at the moment
    bool m_mouseSelected;
    /// true if a guide is inserted at the moment
    bool m_insertGuide;
    /// popup menu
    class Popup;
    Popup * m_popup;
};

#endif /* KOGUIDES_H */
