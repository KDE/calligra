/****************************************************************************
 ** Copyright (C) 2006 Klar�vdalens Datakonsult AB.  All rights reserved.
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
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#include <QList>
#include <QtDebug>
#include <QGridLayout>
#include <QLabel>
#include <QHash>

#include <QPainter>
#include <QPaintEvent>
#include <QLayoutItem>
#include <QPushButton>

#include "KDChartChart.h"
#include "KDChartChart_p.h"
#include "KDChartCartesianCoordinatePlane.h"
#include "KDChartAbstractCartesianDiagram.h"
#include "KDChartHeaderFooter.h"
#include "KDChartEnums.h"
#include "KDChartLegend.h"
#include "KDChartLayoutItems.h"

#if defined KDAB_EVAL
#include "../evaldialog/evaldialog.h"
#endif

#include <KDABLibFakes>


using namespace KDChart;

void Chart::Private::slotUnregisterDestroyedLegend( Legend *l )
{
    legends.removeAll( l );
}

void Chart::Private::slotUnregisterDestroyedHeaderFooter( HeaderFooter* hf )
{
    headerFooters.removeAll( hf );
    hf->removeFromParentLayout();
    textLayoutItems.remove( textLayoutItems.indexOf( hf ) );
}

void Chart::Private::slotUnregisterDestroyedPlane( AbstractCoordinatePlane* plane )
{
    coordinatePlanes.removeAll( plane );
    Q_FOREACH ( AbstractCoordinatePlane* p, coordinatePlanes )
    {
        if ( p->referenceCoordinatePlane() == plane) {
            p->setReferenceCoordinatePlane(0);
        }
    }
    plane->layoutPlanes();
}

Chart::Private::Private( Chart* chart_ )
    : chart( chart_ )
    , layout( 0 )
    , vLayout( 0 )
    , planesLayout( 0 )
    , headerLayout( 0 )
    , footerLayout( 0 )
    , dataAndLegendLayout( 0 )
    , globalLeadingLeft( 0 )
    , globalLeadingRight( 0 )
    , globalLeadingTop( 0 )
    , globalLeadingBottom( 0 )
    , inPaint( false )
{
}

void Chart::Private::layoutHeadersAndFooters()
{
    Q_FOREACH( HeaderFooter *hf, headerFooters ) {
        // for now, there are only two types of Header/Footer,
        // we use a pointer to the right layout, depending on the type():
        QGridLayout * headerFooterLayout;
        switch( hf->type() ){
            case HeaderFooter::Header:
                headerFooterLayout = headerLayout;
                break;
            case HeaderFooter::Footer:
                headerFooterLayout = footerLayout;
                break;
            default:
                Q_ASSERT( false ); // all types need to be handled
                break;
        };

        if( hf->position() != Position::Unknown ) {
            int row, column;
            Qt::Alignment hAlign, vAlign;
            if( hf->position().isNorthSide() ){
                row = 0;
                vAlign = Qt::AlignTop;
            }
            else if( hf->position().isSouthSide() ){
                row = 2;
                vAlign = Qt::AlignBottom;
            }
            else{
                row = 1;
                vAlign = Qt::AlignVCenter;
            }
            if( hf->position().isWestSide() ){
                column = 0;
                hAlign = Qt::AlignLeft;
            }
            else if( hf->position().isEastSide() ){
                column = 2;
                hAlign = Qt::AlignRight;
            }
            else{
                column = 1;
                hAlign = Qt::AlignHCenter;
            }
            textLayoutItems << hf;
            hf->setParentLayout( headerFooterLayout );
            headerFooterLayout->addItem( hf, row, column, 1, 1, hAlign | vAlign );
        }
        else{
            qDebug( "Unknown header/footer position" );
        }
    }
}


void Chart::Private::layoutLegends()
{
    // To support more than one Legend, we first collect them all
    // in little lists: one list per grid position.
    // Since the dataAndLegendLayout is a 3x3 grid, we need 9 little lists.
    // (the middle field, will be used by floating legends, once they are impl'ed)
    QList<Legend*> infos[3][3];

    Q_FOREACH( Legend *legend, legends ) {

        bool bOK = true;
        int row, column;
        switch( legend->position().value() ) {
            case KDChartEnums::PositionNorthWest:  row = 0;  column = 0;
                break;
            case KDChartEnums::PositionNorth:      row = 0;  column = 1;
                break;
            case KDChartEnums::PositionNorthEast:  row = 0;  column = 2;
                break;
            case KDChartEnums::PositionEast:       row = 1;  column = 2;
                break;
            case KDChartEnums::PositionSouthEast:  row = 2;  column = 2;
                break;
            case KDChartEnums::PositionSouth:      row = 2;  column = 1;
                break;
            case KDChartEnums::PositionSouthWest:  row = 2;  column = 0;
                break;
            case KDChartEnums::PositionWest:       row = 1;  column = 0;
                break;
            case KDChartEnums::PositionCenter:
                qDebug( "Sorry: Legend not shown, because position Center is not supported." );
                bOK = false;
                break;
            default:
                qDebug( "Sorry: Legend not shown, because of unknown legend position." );
                bOK = false;
                break;
        }
        if( bOK )
            infos[row][column] << legend;
    }
    // We have collected all legend information,
    // so we can design their layout now.
    for (int iR = 0; iR < 3; ++iR) {
        for (int iC = 0; iC < 3; ++iC) {
            QList<Legend*>& list = infos[iR][iC];
            const int count = list.size();
            switch( count ){
            case 0:
                break;
            case 1: {
                    Legend* legend = list.first();
                    dataAndLegendLayout->addWidget(
                        legend, iR, iC, 1, 1, legend->alignment() );
                }
                break;
            default: {
                    // We have more than one legend in the same cell
                    // of the big dataAndLegendLayout grid.
                    //
                    // So we need to find out, if they are aligned the
                    // same way:
                    // Those legends, that are aligned the same way, will be drawn
                    // leftbound, on top of each other, in a little VBoxLayout.
                    //
                    // If not al of the legends are aligned the same way,
                    // there will be a grid with 3 cells: for left/mid/right side
                    // (or top/mid/bottom side, resp.) legends
                    Legend* legend = list.first();
                    Qt::Alignment alignment = legend->alignment();
                    bool haveSameAlign = true;
                    for (int i = 1; i < count; ++i) {
                        legend = list.at(i);
                        if( alignment != legend->alignment() ){
                            haveSameAlign = false;
                            break;
                        }
                    }
                    if( haveSameAlign ){
                        QVBoxLayout* vLayout = new QVBoxLayout();
                        for (int i = 0; i < count; ++i) {
                            vLayout->addWidget(
                                list.at(i), 0, Qt::AlignLeft );
                        }
                        dataAndLegendLayout->addLayout( vLayout, iR, iC, 1, 1, alignment );
                    }else{
                        QGridLayout* gridLayout = new QGridLayout();
#define ADD_VBOX_WITH_LEGENDS(row, column, align) \
{ \
    QVBoxLayout* innerLayout = new QVBoxLayout(); \
    for (int i = 0; i < count; ++i) { \
        legend = list.at(i); \
        if( legend->alignment() == ( align ) ) \
            innerLayout->addWidget( legend, 0, Qt::AlignLeft ); \
    } \
    gridLayout->addLayout( innerLayout, row, column, ( align  ) ); \
}
                        ADD_VBOX_WITH_LEGENDS( 0, 0, Qt::AlignTop     | Qt::AlignLeft )
                        ADD_VBOX_WITH_LEGENDS( 0, 1, Qt::AlignTop     | Qt::AlignHCenter )
                        ADD_VBOX_WITH_LEGENDS( 0, 2, Qt::AlignTop     | Qt::AlignRight )

                        ADD_VBOX_WITH_LEGENDS( 1, 0, Qt::AlignVCenter | Qt::AlignLeft )
                        ADD_VBOX_WITH_LEGENDS( 1, 2, Qt::AlignVCenter | Qt::AlignRight )

                        ADD_VBOX_WITH_LEGENDS( 2, 0, Qt::AlignBottom  | Qt::AlignLeft )
                        ADD_VBOX_WITH_LEGENDS( 2, 1, Qt::AlignBottom  | Qt::AlignHCenter )
                        ADD_VBOX_WITH_LEGENDS( 2, 2, Qt::AlignBottom  | Qt::AlignRight )

                        dataAndLegendLayout->addLayout( gridLayout, iR, iC, 1, 1 );
                    }
                }
            }
        }
    }
}



QHash<AbstractCoordinatePlane*, PlaneInfo> Chart::Private::buildPlaneLayoutInfos()
{
    /* There are two ways in which planes can be caused to interact in
     * where they are put layouting wise: The first is the reference plane. If
     * such a reference plane is set, on a plane, it will use the same cell in the
     * layout as that one. In addition to this, planes can share an axis. In that case
     * they will be layed out in relation to each other as suggested by the position
     * of the axis. If, for example Plane1 and Plane2 share an axis at position Left,
     * that will result in the layout: Axis Plane1 Plane 2, vertically. If Plane1
     * also happens to be Plane2's referece plane, both planes are drawn over each
     * other. The reference plane concept allows two planes to share the same space
     * even if neither has any axis, and in case there are shared axis, it is used
     * to decided, whether the planes should be painted on top of each other or
     * layed out vertically or horizontally next to each other. */
    QHash<CartesianAxis*, AxisInfo> axisInfos;
    QHash<AbstractCoordinatePlane*, PlaneInfo> planeInfos;
    Q_FOREACH(AbstractCoordinatePlane* plane, coordinatePlanes )
    {
        PlaneInfo p;
        // first check if we share space with another plane
        p.referencePlane = plane->referenceCoordinatePlane();
        planeInfos.insert( plane, p );

        Q_FOREACH( AbstractDiagram* abstractDiagram, plane->diagrams() ) {
            AbstractCartesianDiagram* diagram =
                    dynamic_cast<AbstractCartesianDiagram*> ( abstractDiagram );
            if( !diagram ) continue;

            Q_FOREACH( CartesianAxis* axis, diagram->axes() ) {
                if ( !axisInfos.contains( axis ) ) {
                    /* If this is the first time we see this axis, add it, with the
                     * current plane. The first plane added to the chart that has
                     * the axis associated with it thus "owns" it, and decides about
                     * layout. */
                    AxisInfo i;
                    i.plane = plane;
                    axisInfos.insert( axis, i );
                } else {
                    AxisInfo i = axisInfos[axis];
                    if ( i.plane == plane ) continue; // we don't want duplicates, only shared

                    /* The user expects diagrams to be added on top, and to the right
                     * so that horizontally we need to move the new diagram, vertically
                     * the reference one. */
                    PlaneInfo pi = planeInfos[plane];
                    // plane-to-plane linking overrides linking via axes
                    if ( !pi.referencePlane ) {
                        // we're not the first plane to see this axis, mark us as a slave
                        pi.referencePlane = i.plane;
                        if ( axis->position() == CartesianAxis::Left
                            ||  axis->position() == CartesianAxis::Right )
                            pi.horizontalOffset += 1;
                        planeInfos[plane] = pi;

                        pi = planeInfos[i.plane];
                        if ( axis->position() == CartesianAxis::Top
                            || axis->position() == CartesianAxis::Bottom  )
                            pi.verticalOffset += 1;

                        planeInfos[i.plane] = pi;
                    }
                }
            }
        }
        // Create a new grid layout for each plane that has no reference.
        p = planeInfos[plane];
        if ( p.referencePlane == 0 ) {
            p.gridLayout = new QGridLayout();
            planeInfos[plane] = p;
        }
    }
    return planeInfos;
}

template <typename T>
static T* findOrCreateLayoutByObjectName( QLayout * parentLayout, const char* name )
{
    T *box = qFindChild<T*>( parentLayout, QString::fromLatin1( name ) );
    if ( !box ) {
        box = new T();
        box->setObjectName( QString::fromLatin1( name ) );
    }
    return box;
}

static QVBoxLayout* findOrCreateVBoxLayoutByObjectName( QLayout* parentLayout, const char* name )
{
    return findOrCreateLayoutByObjectName<QVBoxLayout>( parentLayout, name );
}

static QHBoxLayout* findOrCreateHBoxLayoutByObjectName( QLayout* parentLayout, const char* name )
{
    return findOrCreateLayoutByObjectName<QHBoxLayout>( parentLayout, name );
}

void Chart::Private::slotLayoutPlanes()
{
    //qDebug() << "KDChart::Chart is layouting the planes";
    if ( planesLayout && dataAndLegendLayout )
        dataAndLegendLayout->removeItem( planesLayout );

    KDAB_FOREACH( KDChart::AbstractArea* plane, planeLayoutItems ) {
        plane->removeFromParentLayout();
    }
    planeLayoutItems.clear();
    delete planesLayout;
    planesLayout = new QVBoxLayout();

    /* First go through all planes and all axes and figure out whether the planes
     * need to coordinate. If they do, they share a grid layout, if not, each
     * get their own. See buildPlaneLayoutInfos() for more details. */
    QHash<AbstractCoordinatePlane*, PlaneInfo> planeInfos = buildPlaneLayoutInfos();
    QHash<AbstractAxis*, AxisInfo> axisInfos;
    KDAB_FOREACH( AbstractCoordinatePlane* plane, coordinatePlanes ) {
        Q_ASSERT( planeInfos.contains(plane) );
        const PlaneInfo pi = planeInfos[plane];
        int column = pi.horizontalOffset;
        int row = pi.verticalOffset;
        QGridLayout *planeLayout = pi.gridLayout;
        if ( !planeLayout ) {
            // this plane is sharing an axis with another one, so use
            // the grid of that one as well
            planeLayout = planeInfos[pi.referencePlane].gridLayout;
        } else {
            planesLayout->addLayout( planeLayout );
        }
        Q_ASSERT( planeLayout );
        /* Put the plane in the center of the layout. If this is our own, that's
         * the middle of the layout, if we are sharing, it's a cell in the center
         * column of the shared grid. */
        planeLayoutItems << plane;
        plane->setParentLayout( planeLayout );
        planeLayout->addItem( plane, row, column, 1, 1, 0 );
        //qDebug() << "Chart slotLayoutPlanes() calls planeLayout->addItem("<< row << column << ")";
        planeLayout->setRowStretch(    row,    2 );
        planeLayout->setColumnStretch( column, 2 );

        KDAB_FOREACH( AbstractDiagram* abstractDiagram, plane->diagrams() )
        {
            AbstractCartesianDiagram* diagram =
                    dynamic_cast<AbstractCartesianDiagram*> ( abstractDiagram );
            //qDebug() << "--------------- diagram ???????????????????? -----------------";
            if( !diagram ) continue;  // FIXME polar ?
            //qDebug() << "--------------- diagram ! ! ! ! ! ! ! ! ! !  -----------------";
            // collect all axes of a kind into sublayouts
            QVBoxLayout *topAxesLayout = findOrCreateVBoxLayoutByObjectName( planeLayout, "topAxesLayout" );
            topAxesLayout->setSizeConstraint( QLayout::SetFixedSize );
            QVBoxLayout *bottomAxesLayout = findOrCreateVBoxLayoutByObjectName( planeLayout, "bottomAxesLayout" );
            bottomAxesLayout->setSizeConstraint( QLayout::SetFixedSize );
            QHBoxLayout *leftAxesLayout = findOrCreateHBoxLayoutByObjectName( planeLayout, "leftAxesLayout" );
            leftAxesLayout->setSizeConstraint( QLayout::SetFixedSize );
            QHBoxLayout *rightAxesLayout = findOrCreateHBoxLayoutByObjectName( planeLayout, "rightAxesLayout" );
            rightAxesLayout->setSizeConstraint( QLayout::SetFixedSize );

            //leftAxesLayout->setSizeConstraint( QLayout::SetFixedSize );

            KDAB_FOREACH( CartesianAxis* axis, diagram->axes() ) {
                if ( axisInfos.contains( axis ) ) continue; // already layed this one out
                Q_ASSERT ( axis );
                //qDebug() << "--------------- axis added to planeLayoutItems  -----------------";
                planeLayoutItems << axis;
                switch ( axis->position() )
                {
                    case CartesianAxis::Top:
                        axis->setParentLayout( topAxesLayout );
                        topAxesLayout->addItem( axis );
                        break;
                    case CartesianAxis::Bottom:
                        axis->setParentLayout( bottomAxesLayout );
                        bottomAxesLayout->addItem( axis );
                        break;
                    case CartesianAxis::Left:
                        axis->setParentLayout( leftAxesLayout );
                        leftAxesLayout->addItem( axis );
                        break;
                    case CartesianAxis::Right:
                        axis->setParentLayout( rightAxesLayout );
                        rightAxesLayout->addItem( axis );
                        break;
                    default:
                        Q_ASSERT_X( false, "Chart::paintEvent",
                                    "unknown axis position" );
                        break;
                };
                axisInfos.insert( axis, AxisInfo() );
            }
            /* Put each stack of axes-layouts in the cells surrounding the
             * associated plane. We are laying out in the oder the planes
             * were added, and the first one gets to lay out shared axes.
             * Private axes go here as well, of course. */
            if ( !topAxesLayout->parent() )
                planeLayout->addLayout( topAxesLayout,    0,       1 );
            if ( !bottomAxesLayout->parent() )
                planeLayout->addLayout( bottomAxesLayout, row + 1, 1 );
            if ( !leftAxesLayout->parent() ){
                planeLayout->addLayout( leftAxesLayout,   row,     0 );
                //planeLayout->setRowStretch(    row, 0 );
                //planeLayout->setColumnStretch( 0,   0 );
            }
            if ( !rightAxesLayout->parent() )
                planeLayout->addLayout( rightAxesLayout,  row,     column + 1);
        }

    }
    // re-add our grid(s) to the chart's layout
    if ( dataAndLegendLayout ){
        dataAndLegendLayout->addLayout( planesLayout, 1, 1 );
        dataAndLegendLayout->setRowStretch(    1, 2 );
        dataAndLegendLayout->setColumnStretch( 1, 2 );
    }

    slotRelayout();
    //qDebug() << "KDChart::Chart finished layouting the planes.";
}

void Chart::Private::createLayouts( QWidget* w )
{
    KDAB_FOREACH( KDChart::TextArea* textLayoutItem, textLayoutItems ) {
        textLayoutItem->removeFromParentLayout();
    }
    textLayoutItems.clear();

    KDAB_FOREACH( KDChart::AbstractArea* layoutItem, layoutItems ) {
        layoutItem->removeFromParentLayout();
    }
    layoutItems.clear();

    // layout for the planes is handled separately, so we don't want to delete it here
    if ( dataAndLegendLayout) {
        dataAndLegendLayout->removeItem( planesLayout );
        planesLayout->setParent( 0 );
    }
    // nuke the old bunch
    delete layout;

    // The HBox d->layout provides the left and right global leadings
    layout = new QHBoxLayout( w );
    layout->addSpacing( globalLeadingLeft );

    // The vLayout provides top and bottom global leadings and lays
    // out headers/footers and the data area.
    vLayout = new QVBoxLayout();
    layout->addLayout( vLayout, 2 );
    layout->addSpacing( globalLeadingRight );

    // 1. the gap above the top edge of the headers area
    vLayout->addSpacing( globalLeadingTop );
    // 2. the header(s) area
    headerLayout = new QGridLayout();
    vLayout->addLayout( headerLayout );
    // 3. the area containing coordinate plane(s), axes, legend(s)
    dataAndLegendLayout = new QGridLayout();
    vLayout->addLayout( dataAndLegendLayout, 2 );
    // 4. the footer(s) area
    footerLayout = new QGridLayout();
    vLayout->addLayout( footerLayout );
    // 5. the gap below the bottom edge of the headers area
    vLayout->addSpacing( globalLeadingBottom );

    // the data+axes area
    dataAndLegendLayout->addLayout( planesLayout, 1, 1 );
    dataAndLegendLayout->setRowStretch(    1, 1 );
    dataAndLegendLayout->setColumnStretch( 1, 1 );
}

void Chart::Private::slotRelayout()
{
    //qDebug() << "Chart relayouting started.";
    createLayouts( chart );

    layoutHeadersAndFooters();
    layoutLegends();
    layout->activate();
    //dataAndLegendLayout->activate();
    KDAB_FOREACH (AbstractCoordinatePlane* plane, coordinatePlanes )
        plane->layoutDiagrams();
   //qDebug() << "Chart relayouting done.";
}

// ******** Chart interface implementation ***********

Chart::Chart ( QWidget* parent )
    : QWidget ( parent )
    , _d( new Private( this ) )
{
#if defined KDAB_EVAL
    EvalDialog::checkEvalLicense( "KD Chart" );
#endif

    addCoordinatePlane( new CartesianCoordinatePlane ( this ) );
}

Chart::~Chart()
{
    delete _d;
}

#define d d_func()

AbstractCoordinatePlane* Chart::coordinatePlane()
{
    if ( d->coordinatePlanes.isEmpty() )
    {
        qWarning() << "Chart::coordinatePlane: warning: no coordinate plane defined.";
        return 0;
    } else {
        return d->coordinatePlanes.first();
    }
}

CoordinatePlaneList Chart::coordinatePlanes()
{
    return d->coordinatePlanes;
}

void Chart::addCoordinatePlane( AbstractCoordinatePlane* plane )
{
    connect( plane, SIGNAL( destroyedCoordinatePlane( AbstractCoordinatePlane* ) ),
             d,   SLOT( slotUnregisterDestroyedPlane( AbstractCoordinatePlane* ) ) );
    connect( plane, SIGNAL( needUpdate() ),       this,   SLOT( update() ) );
    connect( plane, SIGNAL( needRelayout() ),     d,      SLOT( slotRelayout() ) ) ;
    connect( plane, SIGNAL( needLayoutPlanes() ), d,      SLOT( slotLayoutPlanes() ) ) ;
    connect( plane, SIGNAL( propertiesChanged() ),this, SIGNAL( propertiesChanged() ) );
    d->coordinatePlanes.append( plane );
    plane->setParent( this );
    d->slotLayoutPlanes();
}

void Chart::replaceCoordinatePlane( AbstractCoordinatePlane* plane,
                                    AbstractCoordinatePlane* oldPlane_ )
{
    if( plane && oldPlane_ != plane ){
        AbstractCoordinatePlane* oldPlane = oldPlane_;
        if( d->coordinatePlanes.count() ){
            if( ! oldPlane )
                oldPlane = d->coordinatePlanes.first();
            takeCoordinatePlane( oldPlane );
        }
        delete oldPlane;
        addCoordinatePlane( plane );
    }
}

void Chart::takeCoordinatePlane( AbstractCoordinatePlane* plane )
{
    const int idx = d->coordinatePlanes.indexOf( plane );
    if( idx != -1 ){
        d->coordinatePlanes.takeAt( idx );
        disconnect( plane, SIGNAL( destroyedCoordinatePlane( AbstractCoordinatePlane* ) ),
                    d, SLOT( slotUnregisterDestroyedPlane( AbstractCoordinatePlane* ) ) );
        plane->removeFromParentLayout();
        plane->setParent( 0 );
    }
    d->slotLayoutPlanes();
}

void Chart::setGlobalLeading( int left, int top, int right, int bottom )
{
    setGlobalLeadingLeft( left );
    setGlobalLeadingTop( top );
    setGlobalLeadingRight( right );
    setGlobalLeadingBottom( bottom );
    d->slotRelayout();
}

void Chart::setGlobalLeadingLeft( int leading )
{
    d->globalLeadingLeft = leading;
    d->slotRelayout();
}

int Chart::globalLeadingLeft() const
{
    return d->globalLeadingLeft;
}

void Chart::setGlobalLeadingTop( int leading )
{
    d->globalLeadingTop = leading;
    d->slotRelayout();
}

int Chart::globalLeadingTop() const
{
    return d->globalLeadingTop;
}

void Chart::setGlobalLeadingRight( int leading )
{
    d->globalLeadingRight = leading;
    d->slotRelayout();
}

int Chart::globalLeadingRight() const
{
    return d->globalLeadingRight;
}

void Chart::setGlobalLeadingBottom( int leading )
{
    d->globalLeadingBottom = leading;
    d->slotRelayout();
}

int Chart::globalLeadingBottom() const
{
    return d->globalLeadingBottom;
}

//#define debug_recursive_paint
#ifdef debug_recursive_paint
static int nPaint=0;
#endif

void Chart::paint( QPainter* painter, const QRect& target_ )
{
    //qDebug() << "KDChart::Chart::paint() called, inPaint: " << d->inPaint;
#ifdef debug_recursive_paint
++nPaint;
if( 100<nPaint)
qFatal("nPaint > 100");
#endif
    if( d->inPaint || target_.isEmpty() || !painter ) return;

    d->inPaint = true;

    const QRect oldGeometry( geometry() );
    //painter->drawRect( oldGeometry );
    QRect target( target_ );
    QPoint translation(0,0);

    if( target != oldGeometry ){
        //qDebug() << "KDChart::Chart::paint() calling new setGeometry(" << target << ")";
        setGeometry( target );
        painter->drawRect( target );
        d->slotLayoutPlanes();
        translation.setX( target_.left() );
        translation.setY( target_.left() );
        painter->translate( translation );
    }

    KDAB_FOREACH( KDChart::AbstractArea* layoutItem, d->layoutItems ) {
        layoutItem->paintAll( *painter );
    }
    KDAB_FOREACH( KDChart::AbstractArea* planeLayoutItem, d->planeLayoutItems ) {
        planeLayoutItem->paintAll( *painter );
    }
    KDAB_FOREACH( KDChart::TextArea* textLayoutItem, d->textLayoutItems ) {
        textLayoutItem->paintAll( *painter );
    }
//int i=0;
    KDAB_FOREACH( Legend *legend, d->legends ) {
//qDebug("legend # %i",++i);
        if( ! legend->isHidden() ){
            //legend->forceRebuild();
/*
            const QPoint translation( legend->geometry().topLeft() );
            painter->translate( translation );
            legend->paintAll( *painter );
            if( ! translation.isNull() )
                painter->translate( -translation.x(), -translation.y() );
*/
            legend->paintIntoRect( *painter, legend->geometry() );
        }
    }

    if( target_ != oldGeometry ){
        //qDebug() << "KDChart::Chart::paint() calling new setGeometry(" << oldGeometry << ")";
        setGeometry( oldGeometry );
        if( ! translation.isNull() )
            painter->translate( -translation.x(), -translation.y() );
    }

    d->inPaint = false;
    //qDebug() << "KDChart::Chart::paint() done.\n";
}

void Chart::resizeEvent ( QResizeEvent * event )
{
    d->slotLayoutPlanes();
}

void Chart::paintEvent( QPaintEvent* event )
{
    event->accept();

    QPainter painter( this );
    //FIXME(khz): Paint the background/frame too!
    //            (can we derive Chart from AreaWidget ??)
    paint( &painter, geometry() );
}

void Chart::addHeaderFooter( HeaderFooter* headerFooter )
{
    d->headerFooters.append( headerFooter );
    headerFooter->setParent( this );
    connect( headerFooter, SIGNAL( destroyedHeaderFooter( HeaderFooter* ) ),
             d, SLOT( slotUnregisterDestroyedHeaderFooter( HeaderFooter* ) ) );
    connect( headerFooter, SIGNAL( positionChanged( HeaderFooter* ) ),
             d, SLOT( slotRelayout() ) );
    d->slotRelayout();
}

void Chart::replaceHeaderFooter( HeaderFooter* headerFooter,
                                 HeaderFooter* oldHeaderFooter_ )
{
    if( headerFooter && oldHeaderFooter_ != headerFooter ){
        HeaderFooter* oldHeaderFooter = oldHeaderFooter_;
        if( d->headerFooters.count() ){
            if( ! oldHeaderFooter )
                oldHeaderFooter =  d->headerFooters.first();
            takeHeaderFooter( oldHeaderFooter );
        }
        delete oldHeaderFooter;
        addHeaderFooter( headerFooter );
    }
}

void Chart::takeHeaderFooter( HeaderFooter* headerFooter )
{
    const int idx = d->headerFooters.indexOf( headerFooter );
    if( idx != -1 ){
        d->headerFooters.takeAt( idx );
        disconnect( headerFooter, SIGNAL( destroyedHeaderFooter( HeaderFooter* ) ),
                    d, SLOT( slotUnregisterDestroyedHeaderFooter( HeaderFooter* ) ) );
        headerFooter->setParent( 0 );
    }
    d->slotRelayout();
}

HeaderFooter* Chart::headerFooter()
{
    if( d->headerFooters.isEmpty() ) {
        return 0;
    } else {
        return d->headerFooters.first();
    }
}

HeaderFooterList Chart::headerFooters()
{
    return d->headerFooters;
}

void Chart::addLegend( Legend* legend )
{
    d->legends.append( legend );
    legend->setParent( this );
    connect( legend, SIGNAL( destroyedLegend( Legend* ) ),
             d, SLOT( slotUnregisterDestroyedLegend( Legend* ) ) );
    connect( legend, SIGNAL( positionChanged( AbstractAreaWidget* ) ),
             d, SLOT( slotLayoutPlanes() ) ); //slotRelayout() ) );
    connect( legend, SIGNAL( propertiesChanged() ),this, SIGNAL( propertiesChanged() ) );
    d->slotRelayout();
}

void Chart::replaceLegend( Legend* legend, Legend* oldLegend_ )
{
    if( legend && oldLegend_ != legend ){
        Legend* oldLegend = oldLegend_;
        if( d->legends.count() ){
            if( ! oldLegend )
                oldLegend = d->legends.first();
            takeLegend( oldLegend );
        }
        delete oldLegend;
        addLegend( legend );
    }
}

void Chart::takeLegend( Legend* legend )
{
    const int idx = d->legends.indexOf( legend );
    if( idx != -1 ){
        d->legends.takeAt( idx );
        disconnect( legend, SIGNAL( destroyedLegend( Legend* ) ),
                    d, SLOT( slotUnregisterDestroyedLegend( Legend* ) ) );
        legend->setParent( 0 );
    }
    d->slotRelayout();
}

Legend* Chart::legend()
{
    if ( d->legends.isEmpty() )
    {
        return 0;
    } else {
        return d->legends.first();
    }
}

LegendList Chart::legends()
{
    return d->legends;
}
