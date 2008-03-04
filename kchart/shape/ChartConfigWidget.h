
#ifndef KCHART_CHART_CONFIG_WIDGET
#define KCHART_CHART_CONFIG_WIDGET

// Local
#include "ChartShape.h"
#include "ui_ChartConfigWidget.h"

// KOffice
#include <KoShapeConfigWidgetBase.h>

class KoShape;

namespace KDChart
{
    class Position;
    class CartesianAxis;
}

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
    void chartSubTypeSelected( int type );
    void chartTypeSelected( QAction *action );
    void setThreeDMode( bool threeD );
    void update();

    void setDataInRows( bool checked );
    void slotShowTableEditor( bool show );

    void dataSetChartTypeSelected( QAction *action );
    void datasetColorSelected( const QColor& color );
    void ui_datasetShowValuesChanged( bool b );
    void ui_datasetShowLabelsChanged( bool b );
    void ui_dataSetSelectionChanged( int index );
    void ui_dataSetAxisSelectionChanged( int index );

    void setLegendOrientationIsVertical( bool );
    void setLegendOrientation( int boxEntryIndex );
    void setLegendAlignment( int boxEntryIndex );
    void setLegendFixedPosition( int buttonGroupIndex );
    //void setLegendShowTitle( bool toggled );
    void updateFixedPosition( LegendPosition position );
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
    void chartTypeChanged( ChartType type );
    void chartSubTypeChanged( ChartSubtype subType );
    void dataSetChartTypeChanged( DataSet *dataSet, ChartType type );
    void dataSetChartSubTypeChanged( DataSet *dataSet, ChartSubtype subType );
    void threeDModeToggled( bool threeD );
    void showTitleChanged( bool );
    void showSubTitleChanged( bool );
    void showFooterChanged( bool );
    
    void showVerticalLinesChanged( bool b );
    void showHorizontalLinesChanged( bool b );
    
    void datasetColorChanged( int, const QColor& color );
    void datasetShowValuesChanged( DataSet *dataSet, bool b );
    void datasetShowLabelsChanged( DataSet *dataSet, bool b );
    void gapBetweenBarsChanged( int percent );
    void gapBetweenSetsChanged( int percent );
    
    void showLegendChanged( bool b );

    void dataDirectionChanged( Qt::Orientation );
    void firstRowIsLabelChanged( bool b );
    void firstColumnIsLabelChanged( bool b );

    void axisAdded( AxisPosition, const QString& title );    
    void axisRemoved( Axis *axis );
    void axisShowTitleChanged( Axis *axis, bool b );
    void axisTitleChanged( Axis *axis, const QString& title );
    void axisShowGridLinesChanged( Axis *axis, bool b );
    void axisUseLogarithmicScalingChanged( Axis *axis, bool b );
    void axisStepWidthChanged( Axis *axis, double width );
    void axisSubStepWidthChanged( Axis *axis, double width );

    void legendTitleChanged( const QString& );
    void legendFontChanged( const QFont& font );
    void legendTitleFontChanged( const QFont& font );
    void legendFontSizeChanged( int size );
    void legendSpacingChanged( int spacing );
    void legendShowLinesToggled( bool toggled );
    void legendOrientationChanged( Qt::Orientation orientation );
    void legendAlignmentChanged( Qt::Alignment alignment );
    void legendFixedPositionChanged( LegendPosition position );
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
