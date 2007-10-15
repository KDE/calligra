
#ifndef KCHART_CHART_TYPE_CONFIG_WIDGET
#define KCHART_CHART_TYPE_CONFIG_WIDGET

#include "KoShapeConfigWidgetBase.h"

#include "kchart_global.h"

#include "ui_ChartTypeConfigWidget.h"

class KoShape;

namespace KChart
{
class ChartShape;

/**
 * Chart type configuration widget.
 */
class ChartTypeConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT

public:
    ChartTypeConfigWidget();
    ~ChartTypeConfigWidget();

    void open( KoShape* shape );
    void save();
    KAction* createAction();

    /// reimplemented 
    virtual bool showOnShapeCreate() { return true; }

private Q_SLOTS:
    void chartSubtypeSelected( int type );
    void chartTypeSelected( int type );
    void setThreeDMode( bool threeD );
    void updateChartTypeOptions( ChartTypeOptions options );

Q_SIGNALS:
    void chartTypeChange( KChart::OdfChartType );
    void chartSubtypeChange( KChart::OdfChartSubtype );
    void threeDModeToggled( bool threeD );

private:
    class Private;
    Private * const d;
};

}  // namespace KChart

#endif // KCHART_CHART_TYPE_CONFIG_WIDGET
