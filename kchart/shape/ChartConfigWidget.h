
#ifndef KCHART_CHART_CONFIG_WIDGET
#define KCHART_CHART_CONFIG_WIDGET

#include "KoShapeConfigWidgetBase.h"

#include "kchart_export.h"
#include "kchart_global.h"

#include "ui_ChartConfigWidget.h"

class KoShape;

namespace KDChart
{
    class Position;
    class CartesianAxis;
}; 

namespace KChart
{
class ChartShape;

/**
 * Chart type configuration widget.
 */
class CHARTSHAPELIB_EXPORT ChartConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT

public:
    ChartConfigWidget();
    ~ChartConfigWidget();

    void open( KoShape* shape );
    void save();
    KAction* createAction();

    /// reimplemented 
    virtual bool showOnShapeCreate() { return true; }

public slots:
    void chartSubtypeSelected( int type );
    void chartTypeSelected( QAction *action );
    void setThreeDMode( bool threeD );
    void update();

    void setDataInRows( bool checked );
    void slotShowTableEditor( bool show );

    void datasetColorSelected( const QColor& color );
    void ui_datasetShowValuesChanged( bool b );

    void setLegendOrientationIsVertical( bool );
    void setLegendOrientation( int boxEntryIndex );
    void setLegendAlignment( int boxEntryIndex );
    void setLegendFixedPosition( int buttonGroupIndex );
    //void setLegendShowTitle( bool toggled );
    void updateFixedPosition( const KDChart::Position );
    void selectDataset( int dataset );
    
    void ui_axisSelectionChanged( int index );
    void ui_axisTitleChanged( const QString& title );
    void ui_axisShowTitleChanged( bool b );
    void ui_axisShowGridLinesChanged( bool b );
    void ui_axisUseLogarithmicScalingChanged( bool b );
    void ui_axisStepWidthChanged( double width );
    void ui_axisUseAutomaticStepWidthChanged( bool b );
    void ui_axisSubStepWidthChanged( double width );
    void ui_axisUseAutomaticSubStepWidthChanged( bool b );
    void ui_axisScalingButtonClicked();
    void ui_axisAdded();
    void ui_addAxisClicked();
    void ui_removeAxisClicked();

signals:
    void chartTypeChanged( OdfChartType type, OdfChartSubtype subType );
    void chartTypeChanged( OdfChartType );
    void chartSubtypeChanged( OdfChartSubtype );
    void threeDModeToggled( bool threeD );
    
    void showVerticalLinesChanged( bool b );
    void showHorizontalLinesChanged( bool b );
    
    void datasetColorChanged( int, const QColor& color );
    void datasetShowValuesChanged( int dataset, bool b );
    void gapBetweenBarsChanged( int percent );
    void gapBetweenSetsChanged( int percent );
    
    void showLegendChanged( bool b );

    void dataDirectionChanged( Qt::Orientation );
    void firstRowIsLabelChanged( bool b );
    void firstColumnIsLabelChanged( bool b );

    void axisAdded( AxisPosition, const QString& title );    
    void axisRemoved( KDChart::CartesianAxis *axis );
    void axisTitleChanged( KDChart::CartesianAxis* axis, const QString& title );
    void axisShowGridLinesChanged( KDChart::CartesianAxis* axis, bool b );
    void axisUseLogarithmicScalingChanged( KDChart::CartesianAxis *axis, bool b );
    void axisStepWidthChanged( KDChart::CartesianAxis *axis, double width );
    void axisSubStepWidthChanged( KDChart::CartesianAxis *axis, double width );

    void legendTitleChanged( const QString& );
    void legendFontChanged( const QFont& font );
    void legendTitleFontChanged( const QFont& font );
    void legendFontSizeChanged( int size );
    void legendSpacingChanged( int spacing );
    void legendShowLinesToggled( bool toggled );
    void legendOrientationChanged( Qt::Orientation orientation );
    void legendAlignmentChanged( Qt::Alignment alignment );
    void legendFixedPositionChanged( KDChart::Position position );
    void legendBackgroundColorChanged( const QColor& color );
    void legendFrameColorChanged( const QColor& color );
    void legendShowFrameChanged( bool show );

private:
    void setupDialogs();
    void createActions();

    class Private;
    Private * const d;
};

}  // namespace KChart

#endif // KCHART_CHART_TYPE_CONFIG_WIDGET
