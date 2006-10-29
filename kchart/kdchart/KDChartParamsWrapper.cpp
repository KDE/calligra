#include "KDChartParamsWrapper.h"
#include <KDChartParams.h>

KDChartParamsWrapper::KDChartParamsWrapper( KDChartParams* data ) :QObject(0), _data(data)
{
}

const KDChartAxisParams* KDChartParamsWrapper::axisParams( uint n ) const
{
    const KDChartAxisParams& a = _data->axisParams( n );
    return &a;
}

void KDChartParamsWrapper::setExplodeValues( QVariant explodeList )
{
    if ( !explodeList.type() == QVariant::List )
        return;
    QValueList<QVariant> list = explodeList.toList();

    QValueList<int> res;
    for( QValueList<QVariant>::Iterator it = list.begin(); it != list.end(); ++it ) {
        int x = (*it).toInt();
        res.append(x);
    }
    _data->setExplodeValues( res );
}

void KDChartParamsWrapper::setExplodeFactors( QVariant factors )
{
    if ( !factors.type() == QVariant::List )
        return;

    QValueList<QVariant> list = factors.toList();

    KDChartParams::ExplodeFactorsMap res;
    int i = 0;
    for( QValueList<QVariant>::Iterator it = list.begin(); it != list.end(); ++it, ++i ) {
        res[i] = (*it).toDouble();
    }
    _data->setExplodeFactors( res );
}

void KDChartParamsWrapper::setAxisLabelStringParams( uint n, QVariant axisLabelStringList, QVariant axisShortLabelStringList,
                                                     const QString& valueStart, const QString& valueEnd )
{
    QStringList l1 = axisLabelStringList.toStringList();
    QStringList l2 = axisShortLabelStringList.toStringList();
    _data->setAxisLabelStringParams( n, &l1, &l2, valueStart, valueEnd );
}

void KDChartParamsWrapper::setAxisLabelTextParams( uint n,
                                                   bool axisSteadyValueCalc,
                                                   QVariant axisValueStart,
                                                   QVariant axisValueEnd,
                                                   double axisValueDelta,
                                                   int axisDigitsBehindComma,
                                                   int axisMaxEmptyInnerSpan,
                                                   int takeLabelsFromDataRow,
                                                   int labelTextsDataRow,
                                                   const QVariant& axisLabelStringList,
                                                   const QVariant& axisShortLabelsStringList,
                                                   int axisValueLeaveOut,
                                                   int axisValueDeltaScale ) {

    QStringList l1 = axisLabelStringList.toStringList();
    QStringList l2 = axisShortLabelsStringList.toStringList();

    _data->setAxisLabelTextParams( n, axisSteadyValueCalc, axisValueStart, axisValueEnd, axisValueDelta, axisDigitsBehindComma, axisMaxEmptyInnerSpan,
                                   (KDChartAxisParams::LabelsFromDataRow) takeLabelsFromDataRow, labelTextsDataRow, &l1, &l2,
                                   axisValueLeaveOut, (KDChartAxisParams::ValueScale) axisValueDeltaScale );
}
