#ifndef KDCHARTAXISPARAMSWRAPPER_H
#define KDCHARTAXISPARAMSWRAPPER_H
#include <qobject.h>
#include <KDChartEnums.h>
#include <KDChartAxisParams.h>
#include <qstringlist.h>
#include <qvariant.h>
class KDChartAxisParams;

class KDChartAxisParamsWrapper :public QObject
{
    Q_OBJECT

public:
    KDChartAxisParamsWrapper( KDChartAxisParams* );

public slots:
    // These overload are only for convenience.
    void setAxisValueStart( double axisValueStart );
    void setAxisValueEnd( double axisValueEnd );
    void setAxisValueStart( const QVariant axisValueStart );
    void setAxisValueEnd( const QVariant axisValueEnd );

public slots:
    void setAxisValues( bool axisSteadyValueCalc = true,
                        QVariant axisValueStart = KDCHART_AXIS_LABELS_AUTO_LIMIT,
                        QVariant axisValueEnd = KDCHART_AXIS_LABELS_AUTO_LIMIT,
                        double axisValueDelta = KDCHART_AXIS_LABELS_AUTO_DELTA,
                        int axisDigitsBehindComma = KDCHART_AXIS_LABELS_AUTO_DIGITS,
                        int axisMaxEmptyInnerSpan = KDCHART_DONT_CHANGE_EMPTY_INNER_SPAN_NOW,
                        int takeLabelsFromDataRow = KDChartAxisParams::LabelsFromDataRowNo,
                        int labelTextsDataRow = 0,
                        const QVariant& axisLabelStringList = QVariant( QStringList() ),
                        const QVariant& axisShortLabelsStringList = QVariant( QStringList() ),
                        int axisValueLeaveOut = KDCHART_AXIS_LABELS_AUTO_LEAVEOUT,
                        int axisValueDeltaScale = KDChartAxisParams::ValueScaleNumerical );

    void setAxisLabelStringLists( const QVariant& axisLabelStringList,
                                  const QVariant& axisShortLabelStringList,
                                  const QString& valueStart = QString::null,
                                  const QString& valueEnd   = QString::null );

    void setAxisLabelTexts( const QVariant& axisLabelTexts );
private:
    KDChartAxisParams* _data;
};


#endif /* KDCHARTAXISPARAMSWRAPPER_H */

