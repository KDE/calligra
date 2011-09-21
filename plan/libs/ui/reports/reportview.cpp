/*
 * KPlato Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2010, 2011 by Dag Andersen <danders@get2net.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "reportview.h"
#include "report.h"
#include "reportview_p.h"
#include "reportdata.h"
#include "reportsourceeditor.h"
#include "reportscripts.h"
#include "reportexportpanel.h"
#include "ReportODTRenderer.h"
#include "kptnodechartmodel.h"

#include <KoReportPage.h>
#include <KoReportPreRenderer.h>
#include <KoReportPrintRenderer.h>
#include <renderobjects.h>
#include <KoReportKSpreadRenderer.h>
#include <KoReportHTMLCSSRenderer.h>
#include <reportsection.h>
#include <koproperty/EditorView.h>

#include "kptglobal.h"
#include "kptaccountsmodel.h"
#include "kptflatproxymodel.h"
#include "kptnodeitemmodel.h"
#include "kpttaskstatusmodel.h"
#include "kptresourcemodel.h"
#include "kptresourceappointmentsmodel.h"
#include "kptschedule.h"

#include "KoDocument.h"

#include <KTabWidget>
#include <KAction>
#include <KActionCollection>
#include <KMessageBox>
#include <kfiledialog.h>
#include <kio/netaccess.h>
#include <kdebug.h>
#include <KToolBar>
#include <KFileDialog>
#include <KPushButton>

#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLayout>
#include <QDockWidget>
#include <QModelIndex>
#include <QStandardItem>
#include <QStandardItemModel>

namespace KPlato
{

//----------------
ReportPrintingDialog::ReportPrintingDialog( ViewBase *view, ORODocument *reportDocument )
    : KoPrintingDialog( view ),
    m_reportDocument( reportDocument )
{
    printer().setFromTo( documentFirstPage(), documentLastPage() );
    m_context.printer = &printer();
    m_context.painter = 0;
    KoReportRendererFactory factory;
    m_renderer = factory.createInstance( "print" );

    //FIXME: This should be done by KoReportPrintRender but setupPrinter() is private
    QPrinter *pPrinter = &printer();
    pPrinter->setCreator("KPlato");
    pPrinter->setDocName(reportDocument->title());
    pPrinter->setFullPage(true);
    pPrinter->setOrientation((reportDocument->pageOptions().isPortrait() ? QPrinter::Portrait : QPrinter::Landscape));
    pPrinter->setPageOrder(QPrinter::FirstPageFirst);

    if (reportDocument->pageOptions().getPageSize().isEmpty())
        pPrinter->setPageSize(QPrinter::Custom);
    else
        pPrinter->setPageSize(KoPageFormat::printerPageSize(KoPageFormat::formatFromString(reportDocument->pageOptions().getPageSize())));

}

ReportPrintingDialog::~ReportPrintingDialog()
{
    delete m_renderer;
}

void ReportPrintingDialog::startPrinting( RemovePolicy removePolicy )
{
     //HACK fix when KoRreportPrinter can print single pages
    setPageRange( QList<int>() << printer().fromPage() );

    KoPrintingDialog::startPrinting( removePolicy );
}

int ReportPrintingDialog::documentLastPage() const
{
    return m_reportDocument->pages();
}


void ReportPrintingDialog::printPage( int page, QPainter &painter )
{
    m_context.painter = &painter;
    m_renderer->render( m_context, m_reportDocument, page );
}

QAbstractPrintDialog::PrintDialogOptions ReportPrintingDialog::printDialogOptions() const
{
    return QAbstractPrintDialog::PrintToFile |
           QAbstractPrintDialog::PrintPageRange |
           QAbstractPrintDialog::PrintCollateCopies |
           QAbstractPrintDialog::DontUseSheet;
}

//---------------------
ReportView::ReportView( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
//    kDebug()<<"--------------- ReportView ------------------";

    m_preRenderer = 0;
    setObjectName("ReportView");
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setBackgroundRole(QPalette::Dark);
    m_scrollArea->viewport()->setAutoFillBackground(true);
    QVBoxLayout *l = new QVBoxLayout( this );
    l->addWidget( m_scrollArea );
    m_pageSelector = new ReportNavigator( this );
    l->addWidget( m_pageSelector );

    setupGui();

    connect(m_pageSelector->ui_next, SIGNAL(clicked()), this, SLOT(nextPage()));
    connect(m_pageSelector->ui_prev, SIGNAL(clicked()), this, SLOT(prevPage()));
    connect(m_pageSelector->ui_first, SIGNAL(clicked()), this, SLOT(firstPage()));
    connect(m_pageSelector->ui_last, SIGNAL(clicked()), this, SLOT(lastPage()));
    connect(m_pageSelector->ui_selector, SIGNAL(valueChanged(int)), SLOT(renderPage(int)));

    refresh();
}

QMap<QString, QAbstractItemModel*> ReportView::createReportModels( Project *project, ScheduleManager *manager, QObject *parent ) const
{
    QMap<QString, QAbstractItemModel*> map;
    QSortFilterProxyModel *sf = 0;
    ItemModelBase *m = 0;

    QRegExp rex( QString( "^(%1|%2)$" ).arg( (int)Node::Type_Task ).arg( (int)Node::Type_Milestone ) );
    sf = new QSortFilterProxyModel( parent );
    sf->setFilterKeyColumn( NodeModel::NodeType );
    sf->setFilterRole( Qt::EditRole );
    sf->setFilterRegExp( rex );
    sf->setDynamicSortFilter( true );
    FlatProxyModel *fm = new FlatProxyModel( sf );
    sf->setSourceModel( fm );
    m = new GeneralNodeItemModel( fm );
    fm->setSourceModel( m );
    m->setProject( project );
    m->setScheduleManager( manager );
//     connect( this, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), m, SLOT( setScheduleManager( ScheduleManager* ) ) );
    map.insert( "tasks", sf );

    //QRegExp rex( QString( "^(%1|%2)$" ).arg( (int)Node::Type_Task ).arg( (int)Node::Type_Milestone ) );
    sf = new QSortFilterProxyModel( parent );
    sf->setFilterKeyColumn( NodeModel::NodeType );
    sf->setFilterRole( Qt::EditRole );
    sf->setFilterRegExp( rex );
    sf->setDynamicSortFilter( true );
    fm = new FlatProxyModel( sf );
    sf->setSourceModel( fm );
    m = new TaskStatusItemModel( fm );
    fm->setSourceModel( m );
    m->setProject( project );
    m->setScheduleManager( manager );
//    connect( this, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), m, SLOT( setScheduleManager( ScheduleManager* ) ) );
    map.insert( "taskstatus", sf );

    QRegExp rrex( QString( "^(%1)$" ).arg( (int)OT_Appointment ) );
    sf = new QSortFilterProxyModel( parent );
    sf->setFilterKeyColumn( 0 );
    sf->setFilterRole( Role::ObjectType );
    sf->setFilterRegExp( rrex );
    sf->setDynamicSortFilter( true );
    fm = new FlatProxyModel( sf );
    sf->setSourceModel( fm );
    m = new ResourceAppointmentsRowModel( fm );
    fm->setSourceModel( m );
    m->setProject( project );
    m->setScheduleManager( manager );
//     connect( this, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), m, SLOT( setScheduleManager( ScheduleManager* ) ) );
    map.insert( "resourceassignments", sf );

    rex.setPattern( QString( "^(%1)$" ).arg( (int)OT_Resource ) );
    sf = new QSortFilterProxyModel( parent );
    sf->setFilterKeyColumn( 0 );
    sf->setFilterRole( Role::ObjectType );
    sf->setFilterRegExp( rex );
    sf->setDynamicSortFilter( true );
    fm = new FlatProxyModel( sf );
    sf->setSourceModel( fm );
    m = new ResourceItemModel( fm );
    fm->setSourceModel( m );
    m->setProject( project );
    m->setScheduleManager( manager );
//     connect( this, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), m, SLOT( setScheduleManager( ScheduleManager* ) ) );
    map.insert( "resources", sf );

    ChartProxyModel *cpm = new ChartProxyModel( parent );
    // hide effort
    cpm->setRejectColumns( QList<int>() << 3 << 4 << 5 );
    cpm->setZeroColumns( QList<int>() << 3 << 4 << 5 );
    m = new ChartItemModel( cpm );
    cpm->setSourceModel( m );
    m->setProject( project );
    m->setScheduleManager( manager );
    static_cast<ChartItemModel*>( m )->setNodes( QList<Node*>() << project );
//     connect( this, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), m, SLOT( setScheduleManager( ScheduleManager* ) ) );
    map.insert( "costperformance", cpm  );

    cpm = new ChartProxyModel( parent );
    // hide cost
    cpm->setRejectColumns( QList<int>() << 0 << 1 << 2 );
    cpm->setZeroColumns( QList<int>() << 0 << 1 << 2 );
    m = new ChartItemModel( cpm );
    cpm->setSourceModel( m );
    m->setProject( project );
    m->setScheduleManager( manager );
    static_cast<ChartItemModel*>( m )->setNodes( QList<Node*>() << project );
//     connect( this, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), m, SLOT( setScheduleManager( ScheduleManager* ) ) );
    map.insert( "effortperformance", cpm  );

    fm = new FlatProxyModel( parent );
    m = new CostBreakdownItemModel( fm );
    fm->setSourceModel( m );
    m->setProject( project );
    m->setScheduleManager( manager );
//     connect( this, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), m, SLOT( setScheduleManager( ScheduleManager* ) ) );
    map.insert( "costbreakdown", fm  );

    return map;
}

//-----------------

void ReportView::renderPage( int page )
{
    m_reportWidget->renderPage( page );
}

void ReportView::nextPage()
{
    m_pageSelector->ui_selector->setValue( m_pageSelector->ui_selector->value() + 1 );
}

void ReportView::prevPage()
{
    m_pageSelector->ui_selector->setValue( m_pageSelector->ui_selector->value() - 1 );
}

void ReportView::firstPage()
{
    m_pageSelector->ui_selector->setValue( 1 );
}

void ReportView::lastPage()
{
    m_pageSelector->ui_selector->setValue( m_pageSelector->ui_max->value() );
}

void ReportView::slotEditReport()
{
    emit editReportDesign( this );
}

KoPrintJob *ReportView::createPrintJob()
{
    return new ReportPrintingDialog( this, m_reportDocument );
}

void ReportView::slotExport()
{
    ReportExportPanel *p = new ReportExportPanel();
    p->setObjectName( "ReportExportPanel" );
    KFileDialog *dia = new KFileDialog( KUrl(), QString(), this, p );
    dia->setOperationMode( KFileDialog::Saving );
    dia->setMode( KFile::File );
    dia->setConfirmOverwrite( true );
    dia->setInlinePreviewShown( true );
    dia->setCaption( i18nc( "@title:window", "Export Report" ) );
//    dia->setFilter( QString( "*.ods|%1\n*|%2" ).arg( i18n( "Open document spreadsheet" ) ).arg( i18n( "All Files" ) ) );

    connect(dia, SIGNAL(finished(int)), SLOT(slotExportFinished(int)));
    dia->show();
    dia->raise();
    dia->activateWindow();
}

void ReportView::slotExportFinished( int result )
{
    //TODO confirm overwrite
    KFileDialog *dia = dynamic_cast<KFileDialog*>( sender() );
    if ( dia == 0 ) {
        return;
    }
    ReportExportPanel *p = dia->findChild<ReportExportPanel*>("ReportExportPanel");
    Q_ASSERT( p );
    if ( p && result == QDialog::Accepted ) {
        KoReportRendererContext context;
        context.destinationUrl = dia->selectedUrl();
        if (! context.destinationUrl.isValid() ) {
            KMessageBox::error(this, i18nc( "@info", "Cannot export report. Invalid url:<br>file:<br><filename>%1</filename>", context.destinationUrl.url() ), i18n( "Not Saved" ) );
        } else {
            switch ( p->selectedFormat() ) {
                case Reports::EF_Odt: exportToOdt( context ); break;
                case Reports::EF_Ods: exportToOds( context ); break;
                case Reports::EF_Html: exportToHtml( context ); break;
                case Reports::EF_XHtml: exportToXHtml( context ); break;
                default:
                    KMessageBox::error(this, i18n("Cannot export report. Unknown file format"), i18n( "Not Saved" ) );
                    break;
            }
        }
    }
    dia->deleteLater();
}

void ReportView::exportToOdt( KoReportRendererContext &context )
{
    kDebug()<<"Export to odt:"<<context.destinationUrl;
    KoReportRendererBase *renderer = new ReportODTRenderer();
//    renderer = m_factory.createInstance("odt");
    if ( renderer == 0 ) {
        kError()<<"Cannot create odt renderer";
        return;
    }
    if (!renderer->render(context, m_reportDocument)) {
        KMessageBox::error(this, i18nc( "@info", "Failed to export to <filename>%1</filename>", context.destinationUrl.prettyUrl()) , i18n("Export to text document failed"));
    }
}

void ReportView::exportToOds( KoReportRendererContext &context )
{
    kDebug()<<"Export to ods:"<<context.destinationUrl;
    KoReportRendererBase *renderer;
    renderer = m_factory.createInstance("ods");
    if ( renderer == 0 ) {
        kError()<<"Cannot create ods renderer";
        return;
    }
    if (!renderer->render(context, m_reportDocument)) {
        KMessageBox::error(this, i18nc( "@info", "Failed to export to <filename>%1</filename>", context.destinationUrl.prettyUrl()) , i18n("Export to spreadsheet failed"));
    }
}

void ReportView::exportToHtml( KoReportRendererContext &context )
{
    kDebug()<<"Export to html:"<<context.destinationUrl;
    KoReportRendererBase *renderer;
    renderer = m_factory.createInstance("htmltable");
    if ( renderer == 0 ) {
        kError()<<"Cannot create html renderer";
        return;
    }
    if (!renderer->render(context, m_reportDocument)) {
        KMessageBox::error(this, i18nc( "@info", "Failed to export to <filename>%1</filename>", context.destinationUrl.prettyUrl()) , i18n("Export to HTML failed"));
    }
}

void ReportView::exportToXHtml( KoReportRendererContext &context )
{
    kDebug()<<"Export to xhtml:"<<context.destinationUrl;
    KoReportRendererBase *renderer;
    renderer = m_factory.createInstance("htmlcss");
    if ( renderer == 0 ) {
        kError()<<"Cannot create xhtml css renderer";
        return;
    }
    if (!renderer->render(context, m_reportDocument)) {
        KMessageBox::error(this, i18nc( "@info", "Failed to export to <filename>%1</filename>", context.destinationUrl.prettyUrl()) , i18n("Export to XHTML failed"));
    }
}

void ReportView::setupGui()
{
    /*KActionCollection *coll = actionCollection();*/
    KAction *a = 0;
    QString name = "reportview_list";

    a = new KAction( KIcon( "document-edit" ), i18n( "Edit Report" ), this );
    a->setToolTip( i18nc( "@info:tooltip", "Edit the report definition" ) );
    a->setWhatsThis( i18nc( "@info:whatsthis", "Opens the report design in the report design dialog." ) );
    connect(a, SIGNAL(triggered()), this, SLOT(slotEditReport()));
    addAction( name, a );

    a = new KAction( KIcon( "document-export" ), i18n( "Export" ), this );
    a->setToolTip( i18nc( "@info:tooltip", "Export to file" ) );
    a->setWhatsThis( i18nc( "@info:whatsthis", "Exports the report to a supported file format." ) );
    connect(a, SIGNAL(triggered()), this, SLOT(slotExport()));
    addAction( name, a );
}

void ReportView::setGuiActive( bool active ) // virtual slot
{
    if ( active ) {
        refresh();
    }
    ViewBase::setGuiActive( active );
}

QMap<QString, QAbstractItemModel*> ReportView::reportModels() const
{
    return m_modelmap;
}

void ReportView::setReportModels( const QMap<QString, QAbstractItemModel*> &map )
{
    qDeleteAll( m_modelmap );
    m_modelmap = map;
}

void ReportView::refresh()
{
    delete m_preRenderer;
    QDomElement e = m_design.documentElement();
    m_preRenderer = new KoReportPreRenderer( e.firstChildElement( "report:content" ) );
    if ( ! m_preRenderer->isValid()) {
        kDebug()<<"Invalid design document";
        return;
    }
    ReportData *rd = createReportData( e );
    m_preRenderer->setSourceData( rd );
    m_preRenderer->registerScriptObject(new ProjectAccess( rd ), "project");

    m_reportDocument = m_preRenderer->generate();
    m_pageSelector->setMaximum( m_reportDocument ? m_reportDocument->pages() : 1 );
    m_pageSelector->setCurrentPage( 1 );

    m_reportWidget = new KoReportPage(this, m_reportDocument);
    m_reportWidget->setObjectName("ReportPage");
    m_scrollArea->setWidget(m_reportWidget);
    return;
}

ReportData *ReportView::createReportData( const QDomElement &element )
{
    // get the data source
    QDomElement e = element.firstChildElement( "data-source" );
    QString modelname = e.attribute( "select-from" );

    return createReportData( modelname );
}

void ReportView::createReportData( const QString &type, ReportData *rd )
{
    Q_ASSERT( rd );
    rd->setDataSource( createReportData( type ) );
}

ReportData *ReportView::createReportData( const QString &type )
{
    kDebug()<<type;
    //FIXME a smarter report data creator
    ReportData *r = 0;
    if ( type == "costbreakdown" || type =="costperformance" || type =="effortperformance" ) {
        r = new ChartReportData();
        static_cast<ChartReportData*>( r )->cbs = ( type == "costbreakdown" ? true : false );
    } else {
        r = new ReportData();
    }
    r->setModel( m_modelmap.value( type ) );
    r->setProject( project() );
    r->setScheduleManager( m_schedulemanager );
    r->setObjectName( type );
    connect( r, SIGNAL(createReportData(const QString&, ReportData*)), SLOT(createReportData(const QString&, ReportData*)));

    return r;
}

bool ReportView::loadXML( const QDomDocument &doc )
{
    m_design = doc;
    refresh();
    return true;
}

bool ReportView::loadContext( const KoXmlElement &context )
{
    QDomDocument doc( "context" );
    QDomElement e = KoXml::asQDomElement( doc, context ).firstChildElement( "planreportdefinition" );
    if ( e.isNull() ) {
        // try to handle old definition
        e = KoXml::asQDomElement( doc, context ).firstChildElement( "kplatoreportdefinition" );
    }
    if ( ! e.isNull() ) {
        m_design = QDomDocument( "context" );
        m_design.appendChild( e );
    } else kDebug()<<"Invalid context xml";
    refresh();
    return true;
}

void ReportView::saveContext( QDomElement &context ) const
{
    context.appendChild( m_design.documentElement().cloneNode() );
}

QDomDocument ReportView::document() const
{
    return m_design.cloneNode().toDocument();
}

//------------------
ReportNavigator::ReportNavigator( QWidget *parent )
    : QWidget( parent )
{
    setupUi( this );
    ui_first->setIcon( KIcon( "go-first-view-page" ) );
    ui_last->setIcon( KIcon( "go-last-view-page" ) );
    ui_prev->setIcon( KIcon( "go-previous-view-page" ) );
    ui_next->setIcon( KIcon( "go-next-view-page" ) );

    connect( ui_max, SIGNAL(valueChanged(int)), SLOT(slotMaxChanged(int)));

    connect( ui_selector, SIGNAL(valueChanged(int)), SLOT(setButtonsEnabled()) );

    ui_max->setValue( 1 );
}

void ReportNavigator::setMaximum( int value )
{
    ui_max->setMaximum( value );
    ui_max->setValue( value );
}

void ReportNavigator::setCurrentPage( int page )
{
    ui_selector->setValue( page );
}


void ReportNavigator::slotMaxChanged( int value )
{
    ui_selector->setMaximum( value );
    setButtonsEnabled();
}

void ReportNavigator::setButtonsEnabled()
{
    bool backw = ui_selector->value() > ui_selector->minimum();
    ui_first->setEnabled( backw );
    ui_prev->setEnabled( backw );

    bool forw = ui_selector->value() < ui_selector->maximum();
    ui_last->setEnabled( forw );
    ui_next->setEnabled( forw );
}

//----------------------------
ReportDesignDialog::ReportDesignDialog( QWidget *parent )
    : KDialog( parent ),
    m_view( 0 )
{
    setCaption( i18nc( "@title:window", "Report Designer" ) );
    m_panel = new ReportDesignPanel( this );

    setMainWidget( m_panel );
}

ReportDesignDialog::ReportDesignDialog( Project *project, ScheduleManager *manager, const QDomElement &element, const QMap<QString, QAbstractItemModel*> &models, QWidget *parent )
    : KDialog( parent ),
    m_view( 0 )
{
    setCaption( i18nc( "@title:window", "Report Designer" ) );
    setButtons( KDialog::Close | KDialog::User1 | KDialog::User2 );
    setButtonText( KDialog::User1, i18n( "Save To View" ) );
    setButtonIcon( KDialog::User1, KIcon( "window-new" ) );
    setButtonText( KDialog::User2, i18n( "Save To File" ) );
    setButtonIcon( KDialog::User2, KIcon( "document-save-as" ) );

    m_panel = new ReportDesignPanel( project, manager, element, models, this );

    setMainWidget( m_panel );

    connect( this, SIGNAL( user1Clicked() ), SLOT( slotSaveToView() ) );
    connect( this, SIGNAL( user2Clicked() ), SLOT( slotSaveToFile() ) );
}

ReportDesignDialog::ReportDesignDialog( Project *project, ScheduleManager *manager, ReportView *view, QWidget *parent )
    : KDialog( parent ),
    m_view( view )
{
    setCaption( i18nc( "@title:window", "Edit Report" ) );
    setButtons( KDialog::Close | KDialog::User1 | KDialog::User2 );
    setButtonText( KDialog::User1, i18n( "Save To View" ) );
    setButtonIcon( KDialog::User1, KIcon( "window" ) );
    setButtonText( KDialog::User2, i18n( "Save To File" ) );
    setButtonIcon( KDialog::User2, KIcon( "document-save-as" ) );

    QMap<QString, QAbstractItemModel*> models;
    QDomElement e;
    if ( view ) {
        e = view->document().documentElement();
        models = view->reportModels();
    }
    m_panel = new ReportDesignPanel( project, manager, e, models, this );

    setMainWidget( m_panel );

    connect( this, SIGNAL( user1Clicked() ), SLOT( slotSaveToView() ) );
    connect( this, SIGNAL( user2Clicked() ), SLOT( slotSaveToFile() ) );
}

void ReportDesignDialog::closeEvent ( QCloseEvent * e )
{
    if ( m_panel->m_modified ) {
        //NOTE: When the close (x) button in the window frame is clicked, QWidget automatically hides us if we don't handle it
        KPushButton *b = button( KDialog::Close );
        if ( b ) {
            b->animateClick();
            e->ignore();
            return;
        }
    }
    KDialog::closeEvent ( e );
}

void ReportDesignDialog::slotButtonClicked( int button )
{
    if ( button == KDialog::Close ) {
        if ( m_panel->m_modified ) {
            int res = KMessageBox::warningContinueCancel( this,
                    i18nc( "@info", "The report definition has been modified.<br/>"
                    "<emphasis>If you continue, the modifications will be lost.</emphasis>" ) );

            if ( res == KMessageBox::Cancel ) {
                return;
            }
        }
        hide();
        reject();
        return;
    }
    KDialog::slotButtonClicked( button );
}

void ReportDesignDialog::slotSaveToFile()
{
    KFileDialog *dialog = new KFileDialog(KUrl(), QString(), this);
    dialog->exec();
    KUrl url(dialog->selectedUrl());
    delete dialog;

    if (url.isEmpty()) {
        return;
    }
    if ( ! url.isLocalFile() ) {
        KMessageBox::sorry( this, i18n( "Can only save to a local file." ) );
        return;
    }
    QFile file( url.toLocalFile() );
    if ( ! file.open( QIODevice::WriteOnly ) ) {
        KMessageBox::sorry( this, i18nc( "@info", "Cannot open file:<br/><filename>%1</filename>", file.fileName() ) );
        return;
    }
    QTextStream out( &file );
    out << document().toString();
    m_panel->m_modified = false;
    file.close();
}

void ReportDesignDialog::slotSaveToView()
{
    if ( m_view == 0 ) {
        emit createReportView( this );
        return;
    }
    if ( m_panel->m_modified ) {
        saveToView();
    }
}

void ReportDesignDialog::slotViewCreated( ViewBase *view )
{
    m_view = dynamic_cast<ReportView*>( view );
    saveToView(); // allways save
}

void ReportDesignDialog::saveToView()
{
    if ( m_view == 0 ) {
        return;
    }
    KUndo2Command *cmd = new ModifyReportDefinitionCmd( m_view, document(), i18nc( "(qtundo-format)", "Modify report definition" ) );
    emit modifyReportDefinition( cmd );
    m_panel->m_modified = false;
}

QDomDocument ReportDesignDialog::document() const
{
    return m_panel->document();
}

//----
ReportDesignPanel::ReportDesignPanel( QWidget *parent )
    : QWidget( parent ),
    m_modified( false )
{
    QVBoxLayout *l = new QVBoxLayout( this );

    KToolBar *tb = new KToolBar( this );
    l->addWidget( tb );


    QSplitter *sp1 = new QSplitter( this );
    l->addWidget( sp1 );

    QFrame *frame = new QFrame( sp1 );
    frame->setFrameShadow( QFrame::Sunken );
    frame->setFrameShape( QFrame::StyledPanel );

    l = new QVBoxLayout( frame );

    m_sourceeditor = new ReportSourceEditor( frame );
    l->addWidget( m_sourceeditor );
    QStandardItemModel *model = createSourceModel( m_sourceeditor );
    m_sourceeditor->setModel( model );

    m_propertyeditor = new KoProperty::EditorView( frame );
    l->addWidget( m_propertyeditor );

    QScrollArea *sa = new QScrollArea( sp1 );
    m_designer = new KoReportDesigner( sa );
    sa->setWidget( m_designer );

    connect( model, SIGNAL( selectFromChanged( const QString& ) ), SLOT( setReportData( const QString& ) ) );

    connect( this, SIGNAL( insertItem( const QString& ) ), m_designer, SLOT( slotItem( const QString& ) ) );

    connect( m_designer, SIGNAL( propertySetChanged() ), SLOT( slotPropertySetChanged() ) );
    connect( m_designer, SIGNAL( dirty() ), SLOT( setModified() ) );

    populateToolbar( tb );
}

ReportDesignPanel::ReportDesignPanel( Project */*project*/, ScheduleManager */*manager*/, const QDomElement &element, const QMap<QString, QAbstractItemModel*> &models, QWidget *parent )
    : QWidget( parent ),
    m_modified( false )
{
    QVBoxLayout *l = new QVBoxLayout( this );

    KToolBar *tb = new KToolBar( this );
    l->addWidget( tb );


    QSplitter *sp1 = new QSplitter( this );
    l->addWidget( sp1 );

    QFrame *frame = new QFrame( sp1 );
    frame->setFrameShadow( QFrame::Sunken );
    frame->setFrameShape( QFrame::StyledPanel );

    l = new QVBoxLayout( frame );

    m_sourceeditor = new ReportSourceEditor( frame );
    l->addWidget( m_sourceeditor );
    QStandardItemModel *model = createSourceModel( m_sourceeditor );
    m_sourceeditor->setModel( model );
    m_sourceeditor->setSourceData( element.firstChildElement( "data-source" ) );

    m_propertyeditor = new KoProperty::EditorView( frame );
    l->addWidget( m_propertyeditor );

    QScrollArea *sa = new QScrollArea( sp1 );
    QDomElement e = element.firstChildElement( "report:content" );
    if ( e.isNull() ) {
        m_designer = new KoReportDesigner( sa );
    } else {
        m_designer = new KoReportDesigner( sa, e );
    }
    sa->setWidget( m_designer );

    m_modelmap = models;
    m_designer->setReportData( createReportData( m_sourceeditor->selectFromTag() ) );
    slotPropertySetChanged();

    connect( m_sourceeditor, SIGNAL( selectFromChanged( const QString& ) ), SLOT( setReportData( const QString& ) ) );

    connect( this, SIGNAL( insertItem( const QString& ) ), m_designer, SLOT( slotItem( const QString& ) ) );

    connect( m_designer, SIGNAL( propertySetChanged() ), SLOT( slotPropertySetChanged() ) );
    connect( m_designer, SIGNAL( dirty() ), SLOT( setModified() ) );

    populateToolbar( tb );
}

void ReportDesignPanel::populateToolbar( KToolBar *tb )
{
    tb->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    KAction *a = new KAction( i18n( "Section Editor" ), this );
    a->setObjectName("sectionedit");
    connect(a, SIGNAL(triggered(bool)), m_designer, SLOT(slotSectionEditor()));
    tb->addAction( a );
    tb->addSeparator();
    a = new KAction( KIcon( "arrow-up" ), i18n( "Raise" ), this );
    connect(a, SIGNAL(activated()), m_designer, SLOT(slotRaiseSelected()));
    tb->addAction( a );
    a = new KAction( KIcon( "arrow-down" ), i18n( "Lower" ), this );
    connect(a, SIGNAL(activated()), m_designer, SLOT(slotLowerSelected()));
    tb->addAction( a );

    a = new KAction( KIcon( "edit-delete" ), i18n( "Remove" ), this );
    connect(a, SIGNAL(activated()), m_designer, SLOT(slotEditDelete()));
    tb->addAction( a );

    tb->addSeparator();

    foreach( QAction *a, m_designer->actions(this) ) {
        if ( a->objectName() == "report:image" || a->objectName() == "report:shape" ) {
            continue;
        }
        tb->addAction( a );
        connect( a, SIGNAL( triggered( bool ) ), SLOT( slotInsertAction() ) );
    }
}

void ReportDesignPanel::slotPropertySetChanged()
{
    if ( m_propertyeditor ) {
        m_propertyeditor->changeSet( m_designer->itemPropertySet() );
    }
}

void ReportDesignPanel::slotInsertAction()
{
    emit insertItem( sender()->objectName() );
}

void ReportDesignPanel::setReportData( const QString &tag )
{
    m_designer->setReportData( createReportData( tag ) );
}

QDomDocument ReportDesignPanel::document() const
{
    QDomDocument document( "planreportdefinition" );
    document.appendChild( document.createProcessingInstruction(
                              "xml",
                              "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement e = document.createElement( "planreportdefinition" );
    e.setAttribute( "editor", "Plan" );
    e.setAttribute( "mime", "application/x-vnd.kde.plan.report.definition" );
    e.setAttribute( "version", "1.0" );
    document.appendChild( e );

    if ( m_sourceeditor ) {
        m_sourceeditor->sourceData( e );
    }
    e.appendChild( m_designer->document() );
/*    qDebug()<<"ReportDesignerView::document:";
    qDebug()<<document.toString();*/
    return document;
}

void ReportDesignPanel::createReportData( const QString &type, ReportData *rd )
{
    Q_ASSERT( rd );
    rd->setDataSource( createReportData( type ) );
}

ReportData *ReportDesignPanel::createReportData( const QString &type )
{
    //FIXME a smarter report data creator
    ReportData *r = 0;
    if ( type == "costbreakdown" || type == "earnedvalue" ) {
        r = new ChartReportData();
    } else {
        r = new ReportData();
    }
    connect( r, SIGNAL(createReportData(const QString&, ReportData*)), SLOT(createReportData(const QString&, ReportData*)));
    r->setModel( m_modelmap.value( type ) );
    Q_ASSERT( r );
    return r;
}

QStandardItemModel *ReportDesignPanel::createSourceModel( QObject *parent ) const
{
    QStandardItemModel *m = new QStandardItemModel( parent );

    QStandardItem *item = new QStandardItem( i18n( "Tasks" ) );
    item->setData( "tasks", Reports::TagRole );
    item->setEditable( false );
    m->appendRow( item );

    item = new QStandardItem( i18n( "Task status" ) );
    item->setData( "taskstatus", Reports::TagRole );
    item->setEditable( false );
    m->appendRow( item );

    item = new QStandardItem( i18n( "Resource assignments" ) );
    item->setData( "resourceassignments", Reports::TagRole );
    item->setEditable( false );
    m->appendRow( item );

    item = new QStandardItem( i18n( "Resources" ) );
    item->setData( "resources", Reports::TagRole );
    item->setEditable( false );
    m->appendRow( item );

    return m;
}


//-------------------
ModifyReportDefinitionCmd ::ModifyReportDefinitionCmd( ReportView *view, const QDomDocument &value, const QString& name )
    : NamedCommand( name ),
    m_view( view ),
    m_newvalue( value.cloneNode().toDocument() ),
    m_oldvalue( m_view->document().cloneNode().toDocument() )
{
}
void ModifyReportDefinitionCmd ::execute()
{
    m_view->loadXML( m_newvalue );
}
void ModifyReportDefinitionCmd ::unexecute()
{
    m_view->loadXML( m_oldvalue );
}

} // namespace KPlato

#include "reportview.moc"
#include "reportview_p.moc"
