/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2005-2006 Klar�vdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KD Chart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#ifndef KDCHARTLEGEND_H
#define KDCHARTLEGEND_H

#include "KDChartAbstractAreaWidget.h"
#include "KDChartPosition.h"
#include "KDChartMarkerAttributes.h"

class QTextTable;

namespace KDChart {

    class AbstractDiagram;
    typedef QList<AbstractDiagram*> DiagramList;

/**
  * @brief Legend defines the interface for the legend drawing class.
  *
  * Legend is the class for drawing legends for all kinds of diagrams ("chart types").
  *
  * Legend is drawn on chart level, not per diagram, but you can have more than one
  * legend per chart, using KDChart::Chart::addLegend().
  *
  * \note Legend is different from all other classes ofd KD Chart, since it can be
  * displayed outside of the Chart's area.  If you want to, you can embedd the legend
  * into your own widget, or into another part of a bigger grid, into which you might
  * have inserted the Chart.
  *
  * On the other hand, please note that you MUST call Chart::addLegend to get your
  * legend positioned into the correct place of your chart - if you want to have
  * the legend shown inside of the chart (that's probably true for most cases).
  */
class KDCHART_EXPORT Legend : public AbstractAreaWidget
{
    Q_OBJECT

    Q_DISABLE_COPY( Legend )
    KDCHART_DECLARE_PRIVATE_DERIVED_QWIDGET( Legend )

public:
    explicit Legend( QWidget* parent = 0 );
    explicit Legend( KDChart::AbstractDiagram* diagram, QWidget* parent );
    virtual ~Legend();

    virtual Legend * clone() const;

    //QSize calcSizeHint() const;
    virtual void resizeEvent( QResizeEvent * event );

    virtual void paint( QPainter* painter );
    virtual void setVisible( bool visible );

    /**
        Specifies the reference area for font size of title text,
        and for font size of the item texts, IF automatic area
        detection is set.

        \note This parameter is ignored, if the Measure given for
        setTitleTextAttributes (or setTextAttributes, resp.) is
        not specifying automatic area detection.

        If no reference area is specified, but automatic area
        detection is set, then the size of the legend's parent
        widget will be used.

        \sa KDChart::Measure, KDChartEnums::MeasureCalculationMode
    */
    void setReferenceArea( const QWidget* area );
    /**
        Returns the reference area, that is used for font size of title text,
        and for font size of the item texts, IF automatic area
        detection is set.

        \sa setReferenceArea
    */
    const QWidget* referenceArea() const;

    /**
      * The first diagram of the legend or 0 if there was none added to the legend.
      * @return The first diagram of the legend or 0.
      *
      * \sa diagrams, addDiagram, removeDiagram, removeDiagrams, replaceDiagram, setDiagram
      */
    KDChart::AbstractDiagram* diagram() const;

    /**
      * The list of all diagrams associated with the legend.
      * @return The list of all diagrams associated with the legend.
      *
      * \sa diagram, addDiagram, removeDiagram, removeDiagrams, replaceDiagram, setDiagram
      */
    DiagramList diagrams() const;

    /**
      * Add the given diagram to the legend.
      * @param newDiagram The diagram to add.
      *
      * \sa diagram, diagrams, removeDiagram, removeDiagrams, replaceDiagram, setDiagram
      */
    void addDiagram( KDChart::AbstractDiagram* newDiagram );

    /**
      * Removes the diagram from the legend's list of diagrams.
      *
      * \sa diagram, diagrams, addDiagram, removeDiagrams, replaceDiagram, setDiagram
      */
    void removeDiagram( KDChart::AbstractDiagram* oldDiagram );

    /**
      * Removes all of the diagram from the legend's list of diagrams.
      *
      * \sa diagram, diagrams, addDiagram, removeDiagram, replaceDiagram, setDiagram
      */
    void removeDiagrams();

    /**
      * Replaces the old diagram, or appends the
      * new diagram, it there is none yet.
      *
      * @param newDiagram The diagram to be used instead of the old one.
      * If this parameter is zero, the first diagram will just be removed.
      *
      * @param oldDiagram The diagram to be removed by the new one. This
      * diagram will be deleted automatically. If the parameter is omitted,
      * the very first diagram will be replaced. In case, there was no
      * diagram yet, the new diagram will just be added.
      *
      * \sa diagram, diagrams, addDiagram, removeDiagram, removeDiagrams, setDiagram
      */
    void replaceDiagram( KDChart::AbstractDiagram* newDiagram,
                         KDChart::AbstractDiagram* oldDiagram = 0 );

    /**
      * @brief A convenience method doing the same as replaceDiagram( newDiagram, 0 );
      *
      * Replaces the first diagram by the given diagram.
      * If the legend's list of diagram is empty the given diagram is added to the list.
      *
      * \sa diagram, diagrams, addDiagram, removeDiagram, removeDiagrams, replaceDiagram
      */
    void setDiagram( KDChart::AbstractDiagram* newDiagram );

    void setPosition( Position position );
    Position position() const;

    void setAlignment( Qt::Alignment );
    Qt::Alignment alignment() const;

    void setOrientation( Qt::Orientation orientation );
    Qt::Orientation orientation() const;

    void setShowLines( bool legendShowLines );
    bool showLines() const;

    void resetTexts();
    void setText( uint dataset, const QString& text );
    QString text( uint dataset ) const;

    uint datasetCount() const;

    void setDefaultColors();
    void setRainbowColors();
    void setSubduedColors( bool ordered = false );

    void setBrushesFromDiagram( KDChart::AbstractDiagram* diagram );

    void setColor( uint dataset, const QColor& color );
    void setBrush( uint dataset, const QBrush& brush );
    QBrush brush( uint dataset ) const;

    void setPen( uint dataset, const QPen& pen );
    QPen pen( uint dataset ) const;

    void setMarkerAttributes( uint dataset, const MarkerAttributes& );
    MarkerAttributes markerAttributes( uint dataset ) const;

    void setTextAttributes( const TextAttributes &a );
    TextAttributes textAttributes() const;

    void setTitleText( const QString& text );
    QString titleText() const;

    void setTitleTextAttributes( const TextAttributes &a );
    TextAttributes titleTextAttributes() const;

    // FIXME same as frameSettings()->padding()?
    void setSpacing( uint space );
    uint spacing() const;

    // called internally by KDChart::Chart, when painting into a custom QPainter
    virtual void forceRebuild();

/*public static*/
//    static LegendPosition stringToPosition( QString name, bool* ok=0 );

Q_SIGNALS:
    void destroyedLegend( Legend* );
    /** Emitted upon change of a property of the Legend or any of its components. */
    void propertiesChanged();

private Q_SLOTS:
    void emitPositionChanged();
    void resetDiagram( AbstractDiagram* );
    void setNeedRebuild();
    void buildLegend();
}; // End of class Legend

}


#endif // KDCHARTLEGEND_H
