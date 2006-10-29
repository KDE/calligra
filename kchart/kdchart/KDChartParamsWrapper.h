#ifndef KDCHARTPARAMSWRAPPER_H
#define KDCHARTPARAMSWRAPPER_H
#include <qobject.h>
#include <KDFrame.h>
#include <KDChartParams.h>
#include <qvariant.h>
class KDChartAxisParams;

class KDChartParamsWrapper :public QObject
{
    Q_OBJECT

public:
    KDChartParamsWrapper( KDChartParams* );
public slots:
    const KDChartAxisParams* axisParams( uint n ) const;
    void setExplodeValues( QVariant explodeList );
    void setExplodeFactors( QVariant factors );
    void setAxisLabelStringParams( uint n,
                                   QVariant axisLabelStringList,
                                   QVariant axisShortLabelStringList,
                                   const QString& valueStart = QString::null,
                                   const QString& valueEnd   = QString::null );

    // These methods need to be here, for the enums to work.
    void setSimpleFrame( uint        area,
                         int         outerGapX = 0,
                         int         outerGapY = 0,
                         int         innerGapX = 0,
                         int         innerGapY = 0,
                         bool        addFrameWidthToLayout      = true,
                         bool        addFrameHeightToLayout     = true,
                         int         simpleFrame    = KDFrame::FrameFlat,
                         int                     lineWidth      = 1,
                         int                     midLineWidth   = 0,
                         QPen                    pen            = QPen(),
                         QBrush                  background     = QBrush( Qt::NoBrush ),
                         const QPixmap*          backPixmap     = 0, // no pixmap
                         int backPixmapMode = KDFrame::PixStretched,
                         int                     shadowWidth    = 0,
                         int                     sunPos         = KDFrame::CornerTopLeft )
        {
            _data->setSimpleFrame(area, outerGapX, outerGapY, innerGapX, innerGapY, addFrameWidthToLayout, addFrameHeightToLayout,
                                  (KDFrame::SimpleFrame) simpleFrame, lineWidth, midLineWidth, pen, background, backPixmap,
                                  (KDFrame::BackPixmapMode) backPixmapMode, shadowWidth, (KDFrame::CornerName) sunPos );
        }

    void setDataRegionFrame( uint dataRow,
                             uint dataCol,
                             uint data3rd, // important: we ignore the data3rd parameter for now!
                             int  innerGapX = 0,
                             int  innerGapY = 0,
                             bool addFrameWidthToLayout      = true,
                             bool addFrameHeightToLayout     = true,
                             int  simpleFrame    = KDFrame::FrameFlat,
                             int  lineWidth      = 1,
                             int  midLineWidth   = 0,
                             QPen pen            = QPen(),
                             int  shadowWidth    = 0,
                             int  sunPos         = KDFrame::CornerTopLeft )
        {
            _data->setDataRegionFrame( dataRow, dataCol,
                                       data3rd, innerGapX, innerGapY, addFrameWidthToLayout, addFrameHeightToLayout,
                                       (KDFrame::SimpleFrame) simpleFrame, lineWidth, midLineWidth, pen, shadowWidth, (KDFrame::CornerName) sunPos );
        }

    bool moveDataRegionFrame( uint oldDataRow,
                             uint oldDataCol,
                             uint oldData3rd, // important: we ignore the data3rd parameter for now!
                             uint newDataRow,
                             uint newDataCol,
                             uint newData3rd// important: we ignore the data3rd parameter for now!
                             )
    {
        return _data->moveDataRegionFrame(
            oldDataRow, oldDataCol, oldData3rd, newDataRow, newDataCol, newData3rd );
    }

    void setPrintDataValues( bool active,
                             uint chart = KDCHART_ALL_CHARTS,
                             int divPow10 = 0,
                             int digitsBehindComma = KDCHART_DATA_VALUE_AUTO_DIGITS,
                             QFont* font   = 0,
                             uint size     = UINT_MAX, //  <-- makes us use the *default* font params
                             //                                by IGNORING settings of
                             //                                the following parameters!
                             const QColor* color = KDCHART_DATA_VALUE_AUTO_COLOR,
                             int negativePosition = KDChartEnums::PosCenter,
                             uint negativeAlign    = Qt::AlignCenter,
                             int  negativeDeltaX   =    0,
                             int  negativeDeltaY   =    0,
                             int  negativeRotation =    0,
                             int positivePosition = KDChartEnums::PosCenter,
                             uint positiveAlign    = Qt::AlignCenter,
                             int  positiveDeltaX   =    0,
                             int  positiveDeltaY   =    0,
                             int  positiveRotation =    0,
                             int policy = KDChartEnums::LayoutPolicyRotate )
        {
            _data->setPrintDataValues( active, chart, divPow10, digitsBehindComma,font, size, color, (KDChartEnums::PositionFlag) negativePosition,
                                       negativeAlign, negativeDeltaX, negativeDeltaY, negativeRotation, (KDChartEnums::PositionFlag) positivePosition,
                                       positiveAlign, positiveDeltaX, positiveDeltaY, positiveRotation, (KDChartEnums::TextLayoutPolicy) policy );
        }


    void setDataValuesPlacing( int position,
                               uint align,
                               int  deltaX,
                               int  deltaY,
                               int  rotation,
                               bool specifyingPositiveValues = true,
                               uint chart = KDCHART_ALL_CHARTS )
        {
            _data->setDataValuesPlacing( (KDChartEnums::PositionFlag) position, align, deltaX, deltaY, rotation, specifyingPositiveValues, chart );
        }


    void setDataValuesPolicy( int policy = KDChartEnums::LayoutPolicyRotate,
                              uint chart = KDCHART_ALL_CHARTS )
        {
            _data->setDataValuesPolicy( (KDChartEnums::TextLayoutPolicy) policy, chart );
        }


    void setPolarDelimsAndLabelsAtPos( int pos,
                                       bool showDelimiters,
                                       bool showLabels )
        {
            _data->setPolarDelimsAndLabelsAtPos( (KDChartEnums::PositionFlag) pos, showDelimiters, showLabels );
        }


    bool polarDelimAtPos( int pos ) const
        {
            return _data->polarDelimAtPos( (KDChartEnums::PositionFlag) pos );
        }



    bool polarLabelsAtPos( int pos ) const
        {
            return _data->polarLabelsAtPos( (KDChartEnums::PositionFlag) pos );
        }


    void setAxisType( uint n, int axisType )
        {
            _data->setAxisType( n, (KDChartAxisParams::AxisType) axisType );
        }


    void setAxisLabelTextParams( uint n,
                                 bool axisSteadyValueCalc   = true,
                                 QVariant axisValueStart = KDCHART_AXIS_LABELS_AUTO_LIMIT,
                                 QVariant axisValueEnd   = KDCHART_AXIS_LABELS_AUTO_LIMIT,
                                 double axisValueDelta      = KDCHART_AXIS_LABELS_AUTO_DELTA,
                                 int axisDigitsBehindComma  = KDCHART_AXIS_LABELS_AUTO_DIGITS,
                                 int axisMaxEmptyInnerSpan  = 67,
                                 int takeLabelsFromDataRow  = KDChartAxisParams::LabelsFromDataRowNo,
                                 int labelTextsDataRow      = 0,
                                 const QVariant& axisLabelStringList = QStringList(),
                                 const QVariant& axisShortLabelsStringList = QStringList(),
                                 int axisValueLeaveOut      = KDCHART_AXIS_LABELS_AUTO_LEAVEOUT,
                                 int axisValueDeltaScale = KDChartAxisParams::ValueScaleNumerical );



private:
    KDChartParams* _data;
};



#endif /* KDCHARTPARAMSWRAPPER_H */

