#include "KDChartAxisParamsWrapper.h"
#include <KDChartAxisParams.h>

KDChartAxisParamsWrapper::KDChartAxisParamsWrapper( KDChartAxisParams* data ) :QObject(0), _data(data)
{
}

void KDChartAxisParamsWrapper::setAxisValueStart( double axisValueStart )
{
    _data->setAxisValueStart( axisValueStart );
}

void KDChartAxisParamsWrapper::setAxisValueEnd( double axisValueEnd )
{
    _data->setAxisValueEnd( axisValueEnd);
}

void KDChartAxisParamsWrapper::setAxisValueStart( const QVariant axisValueStart )
{
    _data->setAxisValueStart( axisValueStart );
}

void KDChartAxisParamsWrapper::setAxisValueEnd( const QVariant axisValueEnd )
{
    _data->setAxisValueEnd( axisValueEnd );
}

void KDChartAxisParamsWrapper::setAxisValues( bool axisSteadyValueCalc,
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
                                              int axisValueDeltaScale )
{
    QStringList l1 = axisLabelStringList.toStringList();
    QStringList l2 = axisShortLabelsStringList.toStringList();
    _data->setAxisValues( axisSteadyValueCalc,
                          axisValueStart,
                          axisValueEnd,
                          axisValueDelta,
                          axisDigitsBehindComma,
                          axisMaxEmptyInnerSpan,
                          (KDChartAxisParams::LabelsFromDataRow) takeLabelsFromDataRow,
                          labelTextsDataRow,
                          &l1,
                          &l2,
                          axisValueLeaveOut,
                          (KDChartAxisParams::ValueScale) axisValueDeltaScale );
}

void KDChartAxisParamsWrapper::setAxisLabelStringLists( const QVariant& axisLabelStringList,
                                                        const QVariant& axisShortLabelStringList,
                                                        const QString& valueStart,
                                                        const QString& valueEnd )
{
    QStringList l1 = axisLabelStringList.toStringList();
    QStringList l2 = axisShortLabelStringList.toStringList();
    _data->setAxisLabelStringLists( &l1, &l2, valueStart, valueEnd );
}

void KDChartAxisParamsWrapper::setAxisLabelTexts( const QVariant& axisLabelTexts )
{
    QStringList l = axisLabelTexts.toStringList();
    _data->setAxisLabelTexts( &l );
}

#include "KDChartAxisParamsWrapper.moc"
