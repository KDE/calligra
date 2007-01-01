/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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

#ifndef KDCHARTCHART_P_H
#define KDCHARTCHART_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the KD Chart API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QObject>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "KDChartChart.h"
#include "KDChartAbstractArea.h"
#include "KDChartTextArea.h"
#include "KDChartLayoutItems.h"

#include <KDABLibFakes>


namespace KDChart {

/*
  struct PlaneInfo can't be declared inside Chart::Private, otherwise MSVC.net says:
  qhash.h(195) : error C2248: 'KDChart::Chart::Private' : cannot access protected class declared in class 'KDChart::Chart'
  KDChartChart_p.h(58) : see declaration of 'KDChart::Chart::Private'
  KDChartChart.h(61) : see declaration of 'KDChart::Chart'
  KDChartChart.cpp(262) : see reference to class template instantiation 'QHash<Key,T>' being compiled, with
            Key=KDChart::AbstractCoordinatePlane *,
            T=KDChart::Chart::Private::PlaneInfo
*/
/**
 * \internal
 */
struct PlaneInfo {
    PlaneInfo()
        : referencePlane(0)
            , horizontalOffset(1)
            , verticalOffset(1)
            , gridLayout( 0 )
            {}
            AbstractCoordinatePlane *referencePlane;
            int horizontalOffset;
            int verticalOffset;
            QGridLayout *gridLayout;
};


/**
 * \internal
 */
class Chart::Private : public QObject
{
    Q_OBJECT
    public:
        CoordinatePlaneList coordinatePlanes;
        HeaderFooterList headerFooters;
        LegendList legends;

        Chart* chart;
        QHBoxLayout* layout;
        QVBoxLayout* vLayout;
        QVBoxLayout* planesLayout;
        QGridLayout* headerLayout;
        QGridLayout* footerLayout;
        QGridLayout* dataAndLegendLayout;

        QVector<KDChart::TextArea*> textLayoutItems;
        QVector<KDChart::AbstractArea*> layoutItems;
        QVector<KDChart::AbstractArea*> planeLayoutItems;
        QVector<KDChart::Legend*> legendLayoutItems;

        int globalLeadingLeft, globalLeadingRight, globalLeadingTop, globalLeadingBottom;

        bool inPaint;

        Private ( Chart* );

        virtual ~Private() {}

        void createLayouts( QWidget * parent );
        void layoutLegends();
        void layoutHeadersAndFooters();

        struct AxisInfo {
            AxisInfo()
                :plane(0)
            {}
            AbstractCoordinatePlane *plane;
        };

        QHash<AbstractCoordinatePlane*, PlaneInfo> buildPlaneLayoutInfos();

    public Q_SLOTS:
        void slotLayoutPlanes();
        void slotRelayout();
        void slotUnregisterDestroyedLegend( Legend * legend );
        void slotUnregisterDestroyedHeaderFooter( HeaderFooter* headerFooter );
        void slotUnregisterDestroyedPlane( AbstractCoordinatePlane* plane );
};

}

#endif
