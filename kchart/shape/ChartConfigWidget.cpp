
#include "ChartConfigWidget.h"

// Qt
#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>

// KDE
#include <KLocale>
#include <KIconLoader>
#include <KDebug>

// KDChart
#include <KDChartChart>
#include <KDChartPosition>

// KChart
#include "kchart_global.h"
#include "ChartShape.h"
#include "ui_ChartTableEditor.h"
#include "ui_ChartConfigWidget.h"
#include "ChartTableView.h"
#include "commands/ChartTypeCommand.h"

using namespace KChart;

class Button : public QToolButton
{
public:
    Button(QWidget* parent, const QString& text, const KIcon& icon)
        : QToolButton(parent)
    {
        setCheckable(true);
        setIcon(icon);
        //setIconSize(QSize(48, 48));
        setToolTip(text);
        setText(text);
        setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
};

// ================================================================

class ChartConfigWidget::Private
{
public:
    Private();
    ~Private();

    ChartShape* shape;
    OdfChartType type;
    OdfChartSubtype subtype;
    bool threeDMode;
    QVBoxLayout* leftLayout;
    QVBoxLayout* rightLayout;
    Ui::ChartConfigWidget ui;

    // Table Editor
    Ui::ChartTableEditor       tableEditor;
    QDialog                   *tableEditorDialog;
    ChartTableView            *tableView;

    // Legend
    QButtonGroup                *positionButtonGroup;
    int                          lastHorizontalAlignment;
    int                          lastVerticalAlignment;
    KDChart::Position            fixedPosition;
    KDChart::Position            lastFixedPosition;
};

ChartConfigWidget::Private::Private()
{
    lastHorizontalAlignment = 1; // Qt::AlignCenter
    lastVerticalAlignment   = 1; // Qt::AlignCenter
    fixedPosition           = KDChart::Position::East;
    lastFixedPosition       = KDChart::Position::East;
}

ChartConfigWidget::Private::~Private()
{
}

ChartConfigWidget::ChartConfigWidget()
    : d(new Private)
{
    d->shape = 0;
    d->type = KChart::BarChartType;
    d->subtype = KChart::NormalChartSubtype;
    setObjectName("Chart Type");
    d->ui.setupUi( this );
    
    // Quick-access settings
    connect( d->ui.threeDLook, SIGNAL( toggled( bool ) ),
             this, SLOT( setThreeDMode( bool ) ) );
    connect( d->ui.showLegend, SIGNAL( toggled( bool ) ),
             this, SIGNAL( showLegendChanged( bool ) ) );
    connect( d->ui.editData, SIGNAL( clicked( bool ) ),
             this, SLOT( slotShowTableEditor( bool ) ) );
    
    // "Axis Tab"
    connect( d->ui.xAxisTitle, SIGNAL( textChanged( const QString& ) ),
             this, SIGNAL( xAxisTitleChanged( const QString& ) ) );
    connect( d->ui.yAxisTitle, SIGNAL( textChanged( const QString& ) ),
             this, SIGNAL( yAxisTitleChanged( const QString& ) ) );
    connect( d->ui.showVerticalLines, SIGNAL( toggled( bool ) ),
             this, SIGNAL( showVerticalLinesChanged( bool ) ) );
    connect( d->ui.showHorizontalLines, SIGNAL( toggled( bool ) ),
             this, SIGNAL( showHorizontalLinesChanged( bool ) ) );
    
    // "Datasets" Tab
    connect( d->ui.gapBetweenBars, SIGNAL( valueChanged( int ) ),
             this, SIGNAL( gapBetweenBarsChanged( int ) ) );
    connect( d->ui.gapBetweenSets, SIGNAL( valueChanged( int ) ),
             this, SIGNAL( gapBetweenSetsChanged( int ) ) );
    
    // "Legend" Tab
    connect( d->ui.legendTitle, SIGNAL( textChanged( const QString& ) ),
             this, SIGNAL( legendTitleChanged( const QString& ) ) );
    connect( d->ui.legendShowFrame, SIGNAL( toggled( bool ) ),
             this, SIGNAL( legendShowFrameChanged( bool ) ) );
    
    /*
    d->ui.typeCombobox->addItem( KIcon("chart_bar"), i18n("Bar Chart"), BarChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_line"), i18n("Line Chart"), LineChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_area"), i18n("Area Chart"), AreaChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_pie"), i18n("Pie Chart"), CircleChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_ring"), i18n("Ring Chart"), RingChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_point"), i18n("Scatter Chart"), ScatterChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_pie"), i18n("Radar Chart"), RadarChartType );
    */
    // TODO (Johannes): stock and bubble charts are not yet implemented in KDChart.
    //d->ui.typeCombobox->addItem( KIcon("chart_stock"), i18n("Stock Chart"), StockChartType );
    //d->ui.typeCombobox->addItem( KIcon("chart_bubble"), i18n("Bubble Chart"), BubbleChartType );

    //connect( d->ui.typeCombobox, SIGNAL( currentIndexChanged( int ) ),
    //         this, SLOT( chartTypeSelected( int ) ) );

    /*
    QButtonGroup *subtypeButtonGroup = new QButtonGroup( this );
    subtypeButtonGroup->setExclusive( true );
    d->ui.subtypeNormal->setChecked( true );
    d->ui.subtypeNormal->setIcon(  KIcon("chart_bar_beside" ) );
    d->ui.subtypeStacked->setIcon( KIcon("chart_bar_layer" ) );
    d->ui.subtypePercent->setIcon( KIcon("chart_bar_percent" ) );

    subtypeButtonGroup->addButton( d->ui.subtypeNormal, NormalChartSubtype );
    subtypeButtonGroup->addButton( d->ui.subtypeStacked, StackedChartSubtype );
    subtypeButtonGroup->addButton( d->ui.subtypePercent, PercentChartSubtype );
    */

    // The default chart is a BarChartType, so let's prepare for that
    //d->ui.linesInBarChart->hide();
    //d->ui.linesInBarChartArea->hide();

    //connect( subtypeButtonGroup, SIGNAL( buttonClicked( int ) ),
    //         this, SLOT( chartSubtypeSelected( int ) ) );

    d->tableEditorDialog = new QDialog( this );
    d->tableEditor.setupUi( d->tableEditorDialog );
    d->tableView = new ChartTableView;
    d->tableEditor.gridLayout->addWidget( d->tableView );
    d->tableEditorDialog->hide();

    //d->ui.m_firstRowAsLabel->setChecked( true );
    //d->ui.m_firstColumnAsLabel->setChecked( true );
    //d->ui.areaLabel->hide();
    //d->ui.area->hide();

    createActions();

    // We need only connect one of the data direction buttons, since
    // they are mutually exclusive.
    //connect( d->ui.m_dataInRows, SIGNAL( toggled( bool ) ),
    //         this,               SLOT( setDataInRows( bool ) ) );
    //connect( d->ui.m_firstRowAsLabel, SIGNAL( toggled( bool ) ),
    //         this,                    SIGNAL( firstRowIsLabelChanged( bool ) ) );
    //connect( d->ui.m_firstColumnAsLabel, SIGNAL( toggled( bool ) ),
    //         this,                       SIGNAL( firstColumnIsLabelChanged( bool ) ) );
}

ChartConfigWidget::~ChartConfigWidget()
{
    delete d;
}

void ChartConfigWidget::open( KoShape* shape )
{
    d->shape = dynamic_cast<ChartShape*>( shape );
}

void ChartConfigWidget::save()
{
    ChartTypeCommand command( d->shape );
    command.setChartType( d->type, d->subtype );
    command.redo();
}

KAction* ChartConfigWidget::createAction()
{
    return 0;
}

void ChartConfigWidget::chartTypeSelected( int type )
{
    d->type = ( OdfChartType )type;
    emit chartTypeChange( d->type );

    switch ( type ) {
        case BarChartType:
            // Hide them for now, as the functionality for it is not yet implemented
            /*
            d->ui.linesInBarChart->show();
            d->ui.linesInBarChartArea->show();
            */
            //d->ui.linesInBarChart->hide();
            //d->ui.linesInBarChartArea->hide();
            d->ui.threeDLook->setEnabled( true );
            break;
        case LineChartType:
            //d->ui.linesInBarChart->hide();
            //d->ui.linesInBarChartArea->hide();
            d->ui.threeDLook->setEnabled( true );
            break;
        case ScatterChartType:
            d->ui.threeDLook->setEnabled( false );
            break;
        case AreaChartType:
            //d->ui.linesInBarChart->hide();
            //d->ui.linesInBarChartArea->hide();
            d->ui.threeDLook->setEnabled( false );
            break;
        case CircleChartType:
        case BubbleChartType:
            d->ui.threeDLook->setEnabled( true );
            break;
        default:
            d->ui.threeDLook->setEnabled( false );
            break;
    }
    updateChartTypeOptions( d->shape->chartTypeOptions( d->type ) );
}

void ChartConfigWidget::chartSubtypeSelected( int type )
{
    d->subtype = (OdfChartSubtype) type;
    emit chartSubtypeChange( d->subtype );
}

void ChartConfigWidget::setThreeDMode( bool threeD )
{
    d->threeDMode = threeD;
    emit threeDModeToggled( threeD );
}

void ChartConfigWidget::updateChartTypeOptions( ChartTypeOptions options )
{
    switch ( options.subtype ) {
        case NormalChartSubtype:
            //d->ui.subtypeNormal->blockSignals( true );
            //d->ui.subtypeNormal->setChecked( true );
            //d->ui.subtypeNormal->blockSignals( false );
            break;
        case StackedChartSubtype:
            //d->ui.subtypeStacked->blockSignals( true );
            //d->ui.subtypeStacked->setChecked( true );
            //d->ui.subtypeStacked->blockSignals( false );
            break;
        case PercentChartSubtype:
            //d->ui.subtypePercent->blockSignals( true );
            //d->ui.subtypePercent->setChecked( true );
            //d->ui.subtypePercent->blockSignals( false );
            break;
        case NoChartSubtype:
        default:
            break;
    }
    d->ui.threeDLook->setChecked( d->shape->threeDMode() );
}


void ChartConfigWidget::slotShowTableEditor( bool show )
{
    if ( show )
        d->tableEditorDialog->hide();
    else
        d->tableEditorDialog->show();
}

void ChartConfigWidget::setDataInRows( bool checked )
{
    if ( checked )
        emit dataDirectionChanged( Qt::Horizontal );
    else
        emit dataDirectionChanged( Qt::Vertical );
}


void ChartConfigWidget::setLegendOrientation( int boxEntryIndex )
{
    //emit legendOrientationChanged( ( Qt::Orientation ) ( d->ui.orientation->itemData( boxEntryIndex ).toInt() ) );
}
/*
void ChartConfigWidget::setLegendShowTitle( bool show )
{
    if ( show ) {
        d->ui.legendTitle->setEnabled( true );
        emit legendTitleChanged( d->ui.legendTitle->text() );
    } else {
        d->ui.legendTitle->setEnabled( false );
        emit legendTitleChanged( "" );
    }
}
*/
void ChartConfigWidget::setLegendAlignment( int boxEntryIndex )
{
    if (    d->fixedPosition == KDChart::Position::North
         || d->fixedPosition == KDChart::Position::South ) {
        //d->lastHorizontalAlignment = d->ui.alignment->currentIndex();
     } else if (   d->fixedPosition == KDChart::Position::East
                || d->fixedPosition == KDChart::Position::West ) {
        //d->lastVerticalAlignment = d->ui.alignment->currentIndex();
    }
    //emit legendAlignmentChanged( ( Qt::Alignment ) ( d->ui.alignment->itemData( boxEntryIndex ).toInt() ) );
}

void ChartConfigWidget::setLegendFixedPosition( int buttonGroupIndex )
{
    d->lastFixedPosition = d->fixedPosition;
    //d->fixedPosition = buttonIndexToFixedPosition[ buttonGroupIndex ];
    //emit legendFixedPositionChanged( buttonIndexToFixedPosition[ buttonGroupIndex ] );
}

void ChartConfigWidget::updateFixedPosition( const KDChart::Position position )
{
/*
    if (    position == KDChart::Position::North
         || position == KDChart::Position::South ) {
        d->ui.alignment->setEnabled( true );
        d->ui.alignment->setItemText( 0, i18n( "Left" ) );
        d->ui.alignment->setItemData( 0, Qt::AlignLeft );
        d->ui.alignment->setItemData( 1, Qt::AlignCenter );
        d->ui.alignment->setItemText( 2, i18n( "Right" ) );
        d->ui.alignment->setItemData( 2, Qt::AlignRight );
        // Set the alignment to the one last used for horizontal legend alignment
        if (    d->lastFixedPosition != KDChart::Position::North
             && d->lastFixedPosition != KDChart::Position::South ) {
            // Make sure that the combobox gets updated. Since we changed the values of the entries,
            // same index doesn't mean same value, though it will think so. Solution: Select no entry first
            d->ui.alignment->blockSignals( true );
            d->ui.alignment->setCurrentIndex( -1 );
            d->ui.alignment->blockSignals( false );

            d->ui.alignment->setCurrentIndex( d->lastHorizontalAlignment );
        }
    } else if (    position == KDChart::Position::East
                || position == KDChart::Position::West ) {
        d->ui.alignment->setEnabled( true );
        d->ui.alignment->setItemText( 0, i18n( "Top" ) );
        d->ui.alignment->setItemData( 0, Qt::AlignTop );
        d->ui.alignment->setItemData( 1, Qt::AlignCenter );
        d->ui.alignment->setItemText( 2, i18n( "Bottom" ) );
        d->ui.alignment->setItemData( 2, Qt::AlignBottom );
        // Set the alignment to the one last used for vertical legend alignment
        if (    d->lastFixedPosition != KDChart::Position::East
             && d->lastFixedPosition != KDChart::Position::West ) {
            // Make sure that the combobox gets updated. Since we changed the values of the entries,
            // same index doesn't mean same value, though it will think so. Solution: Select no entry first
            d->ui.alignment->blockSignals( true );
            d->ui.alignment->setCurrentIndex( -1 );
            d->ui.alignment->blockSignals( false );

            d->ui.alignment->setCurrentIndex( d->lastVerticalAlignment );
        }
    } else {
        d->ui.alignment->setEnabled( false );
    }

    for( int i = 0; i < NUM_FIXED_POSITIONS; i++ ) {
        if( position == buttonIndexToFixedPosition[i] ) {
            if ( d->positionButtonGroup->checkedId() != i )
                d->positionButtonGroup->button( i )->setChecked( true );
            break;
        }
    }
*/
}


void ChartConfigWidget::createActions()
{
    QAction *cutRowsAction       = new QAction( KIcon( "edit-cut" ), i18n( "Cut Rows" ),    d->tableView );
    QAction *cutColumnsAction    = new QAction( KIcon( "edit-cut" ), i18n( "Cut Columns" ), d->tableView );
    QAction *cutCellsAction    = new QAction( KIcon( "edit-cut" ), i18n( "Cut Cells" ), d->tableView );
    QAction *copyRowsAction      = new QAction( KIcon( "edit-copy" ), i18n( "Copy Rows" ), d->tableView );
    QAction *copyColumnsAction   = new QAction( KIcon( "edit-copy" ), i18n( "Copy Columns" ), d->tableView );
    QAction *copyCellsAction   = new QAction( KIcon( "edit-copy" ), i18n( "Copy Cells" ), d->tableView );
    QAction *deleteRowsAction    = new QAction( KIcon( "edit-delete" ), i18n( "Delete Rows" ), d->tableView );
    QAction *deleteColumnsAction = new QAction( KIcon( "edit-delete" ), i18n( "Delete Columns" ), d->tableView );
    QAction *deleteCellsAction = new QAction( KIcon( "edit-delete" ), i18n( "Delete Cells" ), d->tableView );
    QAction *insertRowsAction    = new QAction( KIcon( "edit-paste" ), i18n( "Insert Rows" ), d->tableView );
    QAction *insertColumnsAction = new QAction( KIcon( "edit-paste" ), i18n( "Insert Columns" ), d->tableView );
    QAction *insertCellsAction = new QAction( KIcon( "edit-paste" ), i18n( "Insert Cells" ), d->tableView );

    QAction *separator1 = new QAction( d->tableView );
    QAction *separator2 = new QAction( d->tableView );
    separator1->setSeparator( true );
    separator2->setSeparator( true );

    d->tableView->addAction( copyRowsAction );
    d->tableView->addAction( cutRowsAction );
    d->tableView->addAction( deleteRowsAction );
    d->tableView->addAction( insertRowsAction );
    d->tableView->addAction( separator1 );
    d->tableView->addAction( copyColumnsAction );
    d->tableView->addAction( cutColumnsAction );
    d->tableView->addAction( deleteColumnsAction );
    d->tableView->addAction( insertColumnsAction );
    d->tableView->addAction( separator2 );
    d->tableView->addAction( copyCellsAction );
    d->tableView->addAction( cutCellsAction );
    d->tableView->addAction( deleteCellsAction );
    d->tableView->addAction( insertCellsAction );

    d->tableView->setContextMenuPolicy( Qt::ActionsContextMenu );
}

#include "ChartConfigWidget.moc"
