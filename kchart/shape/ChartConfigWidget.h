
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
    void updateChartTypeOptions( ChartTypeOptions options );

    void setDataInRows( bool checked );
    void slotShowTableEditor( bool show );

    void setLegendOrientationIsVertical( bool );
    void setLegendOrientation( int boxEntryIndex );
    void setLegendAlignment( int boxEntryIndex );
    void setLegendFixedPosition( int buttonGroupIndex );
    //void setLegendShowTitle( bool toggled );
    void updateFixedPosition( const KDChart::Position );
    void selectDataset( int dataset );

signals:
    void chartTypeChanged( OdfChartType type, OdfChartSubtype subType );
    void chartTypeChanged( OdfChartType );
    void chartSubtypeChanged( OdfChartSubtype );
    void threeDModeToggled( bool threeD );
    
    void showVerticalLinesChanged( bool b );
    void showHorizontalLinesChanged( bool b );
    
    void gapBetweenBarsChanged( int percent );
    void gapBetweenSetsChanged( int percent );
    
    void xAxisTitleChanged( const QString& title );
    void yAxisTitleChanged( const QString& title );
    
    void showLegendChanged( bool b );

    void dataDirectionChanged( Qt::Orientation );
    void firstRowIsLabelChanged( bool b );
    void firstColumnIsLabelChanged( bool b );

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
    void createActions();

    class Private;
    Private * const d;
};

}  // namespace KChart

#endif // KCHART_CHART_TYPE_CONFIG_WIDGET
