/*
 * KPlato Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2010 by Dag Andersen <danders@get2net.dk>
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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "reportview.h"
#include "report.h"
#include "reportview_p.h"
#include "reportpage.h"
#include "reportdata.h"
#include "reportsourceeditor.h"
#include "reportscripts.h"

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
    m_cxt.printer = &printer();
    m_cxt.painter = 0;
    KoReportRendererFactory factory;
    m_renderer = factory.createInstance( "print" );
}

ReportPrintingDialog::~ReportPrintingDialog()
{
    delete m_renderer;
}

int ReportPrintingDialog::documentLastPage() const
{
    return m_reportDocument->pages();
}


void ReportPrintingDialog::printPage( int page, QPainter &painter )
{
    m_cxt.painter = &painter;
    m_renderer->render( m_cxt, m_reportDocument, page );
}

//---------------------
ReportView::ReportView( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
//    qDebug()<<"--------------- ReportView ------------------";

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

//    qDebug()<<"<-------------- ReportView -----------------<";
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
//     connect( this, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), m, SLOT( setScheduleManager( ScheduleManager* ) ) );
    map.insert( "taskstatus", sf );

    fm = new FlatProxyModel( parent );
    m = new ResourceAppointmentsRowModel( fm );
    fm->setSourceModel( m );
    m->setProject( project );
    m->setScheduleManager( manager );
//     connect( this, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), m, SLOT( setScheduleManager( ScheduleManager* ) ) );
    map.insert( "resourceassignments", fm );

    fm = new FlatProxyModel( parent );
    m = new ResourceItemModel( fm );
    fm->setSourceModel( m );
    m->setProject( project );
    m->setScheduleManager( manager );
//     connect( this, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), m, SLOT( setScheduleManager( ScheduleManager* ) ) );
    map.insert( "resourcesandgroups", fm );

    fm = new FlatProxyModel( parent );
    m = new CostBreakdownItemModel( fm );
    fm->setSourceModel( m );
    m->setProject( project );
    m->setScheduleManager( manager );
//     connect( this, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), m, SLOT( setScheduleManager( ScheduleManager* ) ) );
    map.insert( "costbreakdown", fm  );

//     QRegExp px( QString( "^(%1)$" ).arg( (int)Node::Type_Project ) );
//     sf = new QSortFilterProxyModel( parent );
//     sf->setFilterKeyColumn( NodeModel::NodeType );
//     sf->setFilterRole( Qt::EditRole );
//     sf->setFilterRegExp( px );
//     sf->setDynamicSortFilter( true );
//     NodeItemModel *n = new NodeItemModel( fm );
//     sf->setSourceModel( n );
//     n->setShowProject( true );
//     n->setProject( project );
//     n->setScheduleManager( manager );
////     connect( this, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), n, SLOT( setScheduleManager( ScheduleManager* ) ) );
//     map.insert( "project", sf );

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

void ReportView::slotExportOds()
{
    KFileDialog *dia = new KFileDialog( KUrl(), QString(), this );
    dia->setOperationMode( KFileDialog::Saving );
    dia->setMode( KFile::File );
    dia->setConfirmOverwrite( true );
    dia->setInlinePreviewShown( true );
    dia->setCaption( i18nc( "@title:window", "Export Report" ) );
    dia->setFilter( QString( "*.ods|%1\n*|%2" ).arg( i18n( "Open document spreadsheet" ) ).arg( i18n( "All Files" ) ) );

    connect(dia, SIGNAL(okClicked()), SLOT(slotExportOdsOkClicked()));
    dia->show();
    dia->raise();
    dia->activateWindow();
}

void ReportView::slotExportOdsOkClicked()
{
    KFileDialog *dia = dynamic_cast<KFileDialog*>( sender() );
    if ( dia == 0 ) {
        return;
    }
    KoReportRendererBase *renderer;
    KoReportRendererContext cxt;

    renderer = m_factory.createInstance("ods");
    if ( renderer == 0 ) {
        return;
    }
    cxt.destinationUrl = dia->selectedUrl();
    if (!cxt.destinationUrl.isValid()) {
        KMessageBox::error(this, i18n("Cannot export report. The URL was invalid"), i18n( "Not Saved" ) );
        return;
    }

    if (!renderer->render(cxt, m_reportDocument)) {
        KMessageBox::error(this, i18nc( "@info", "Failed to export to <filename>%1</filename>", cxt.destinationUrl.prettyUrl()) , i18n("Export to spreadsheet failed"));
    }
    dia->deleteLater();
}

void ReportView::slotExportHTML()
{
    KFileDialog *dia = new KFileDialog( KUrl(), QString(), this );
    dia->setOperationMode( KFileDialog::Saving );
    dia->setMode( KFile::File );
    dia->setConfirmOverwrite( true );
    dia->setInlinePreviewShown( true );
    dia->setCaption( i18nc( "@title:window", "Export Report" ) );
    dia->setFilter( QString( "*.html|%1\n*|%2" ).arg( i18n( "HTML files" ) ).arg( i18n( "All Files" ) ) );

    connect(dia, SIGNAL(okClicked()), SLOT(slotExportHTMLOkClicked()));
    dia->show();
    dia->raise();
    dia->activateWindow();

}

void ReportView::slotExportHTMLOkClicked()
{
    KFileDialog *dia = dynamic_cast<KFileDialog*>( sender() );
    if ( dia == 0 ) {
        return;
    }
    KoReportRendererBase *renderer;
    KoReportRendererContext cxt;

    renderer = m_factory.createInstance("htmlcss");
    if ( renderer == 0 ) {
        return;
    }
    cxt.destinationUrl = dia->selectedUrl();
    if (!cxt.destinationUrl.isValid()) {
        KMessageBox::error(this, i18n("Cannot export report. The URL was invalid"), i18n( "Not Saved" ) );
        return;
    }

    if (!renderer->render(cxt, m_reportDocument)) {
        KMessageBox::error(this, i18nc( "@info", "Failed to export to <filename>%1</filename>", cxt.destinationUrl.prettyUrl()) , i18n("Export to HTML failed"));
    }
    dia->deleteLater();
}

void ReportView::setupGui()
{
    KActionCollection *coll = actionCollection();
    KAction *a = 0;
    QString name = "reportview_list";

    a = new KAction( KIcon( "document-edit" ), i18n( "Edit Report" ), this );
    a->setToolTip( i18nc( "@info:tooltip", "Edit the report definition" ) );
    a->setWhatsThis( i18nc( "@info:whatsthis", "Opens the report design in the report design dialog." ) );
    connect(a, SIGNAL(triggered()), this, SLOT(slotEditReport()));
    addAction( name, a );

//#ifdef HAVE_KSPREAD
    a = new KAction( KIcon( "application-vnd.oasis.opendocument.spreadsheet" ), i18nc( "@label:button", "Spreadsheet" ), this );
    a->setToolTip( i18nc( "@info:tooltip", "Export to spreadsheet" ) );
    a->setWhatsThis( i18nc( "@info:whatsthis", "Exports report to an open document spreadsheet file." ) );
    connect(a, SIGNAL(triggered()), this, SLOT(slotExportOds()));
    addAction( name, a );
//#endif

    a = new KAction( KIcon( "application-xhtml+xml" ), i18n( "HTML" ), this );
    a->setToolTip( i18nc( "@info:tooltip", "Export to HTML" ) );
    a->setWhatsThis( i18nc( "@info:whatsthis", "Exports the report to a HTML file in css format." ) );
    connect(a, SIGNAL(triggered()), this, SLOT(slotExportHTML()));
    addAction( name, a );

/*    a = new KAction( KIcon( "kword" ), i18n( "Open in KWord" ), this );
    a->setToolTip(i18n("Open the report in KWord"));
    a->setWhatsThis(i18n("Opens the current report in KWord."));
    a->setEnabled(false);
//! @todo connect(a, SIGNAL(triggered()), this, SLOT(slotRenderKWord()));
    addAction( name, a );*/

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
//    qDebug()<<"ReportView::refresh:"<<sender();
    delete m_preRenderer;
    QDomElement e = m_design.documentElement();
    m_preRenderer = new KoReportPreRenderer( e.firstChildElement( "report:content" ) );
    if ( ! m_preRenderer->isValid()) {
        qDebug()<<"ReportView::refresh: Invalid design document";
        return;
    }
    ReportData *rd = createReportData( e );
    m_preRenderer->setSourceData( rd );
    m_preRenderer->registerScriptObject(new ProjectAccess( rd ), "project");
    
    m_reportDocument = m_preRenderer->generate();
    m_pageSelector->setMaximum( m_reportDocument ? m_reportDocument->pages() : 1 );
    m_pageSelector->setCurrentPage( 1 );

    m_reportWidget = new ReportPage(this, m_reportDocument);
    m_reportWidget->setObjectName("KPlatoReportPage");
    m_scrollArea->setWidget(m_reportWidget);
    return;
}

ReportData *ReportView::createReportData( const QDomElement &element )
{
    // get the data source 
    QDomElement e = element.firstChildElement( "data-source" );
    QString modelname = e.attribute( "select-from" );

    //FIXME a smarter report data creator
    ReportData *r = 0;
    if ( modelname == "costbreakdown" ) {
        r = new ChartReportData();
    } else {
        r = new ReportData();
    }
    r->setProject( project() );
    r->setScheduleManager( m_schedulemanager );
    r->setModel( m_modelmap.value( modelname ) );
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
    qDebug()<<"ReportView::loadContext:"<<context.tagName();
    
    QDomDocument doc( "context" );
    QDomElement e = KoXml::asQDomElement( doc, context ).firstChildElement( "kplatoreportdefinition" );
    if ( ! e.isNull() ) {
        m_design = QDomDocument( "context" );
        m_design.appendChild( e );
        qDebug()<<m_design.toString();
    } else qDebug()<<"Invalid context xml";
    refresh();
    return true;
}

void ReportView::saveContext( QDomElement &context ) const
{
    qDebug()<<"ReportView::saveContext:"<<m_design.documentElement().tagName();
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
    //qDebug()<<"ReportNavigator::setButtonsEnabled:"<<ui_selector->value()<<ui_selector->minimum()<<ui_selector->maximum();
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
    qDebug()<<"ReportDesignDialog::slotSaveToFile:";
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
    m_view->loadXML( document() );
    m_panel->m_modified = false;
}

void ReportDesignDialog::slotViewCreated( ViewBase *view )
{
    m_view = dynamic_cast<ReportView*>( view );
    //qDebug()<<"ReportDesignDialog::slotViewCreated:"<<view;
    if ( m_view == 0 ) {
        return;
    }
    m_view->loadXML( document() );
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

    QSplitter *sp2 = new QSplitter( sp1 );
    sp2->setOrientation( Qt::Vertical );

    m_sourceeditor = new ReportSourceEditor( sp2 );
    ReportSourceModel *model = createSourceModel( m_sourceeditor );
    m_sourceeditor->setModel( model );
    
    m_propertyeditor = new KoProperty::EditorView( sp2 );
    
    QScrollArea *sa = new QScrollArea( sp1 );
    m_designer = new KoReportDesigner( sa );
    sa->setWidget( m_designer );

    connect( model, SIGNAL( selectFromChanged( const QString& ) ), SLOT( setReportData( const QString& ) ) );

    connect( this, SIGNAL( insertItem( const QString& ) ), m_designer, SLOT( slotItem( const QString& ) ) );

    connect( m_designer, SIGNAL( propertySetChanged() ), SLOT( slotPropertySetChanged() ) );
    connect( m_designer, SIGNAL( dirty() ), SLOT( setModified() ) );

    // Populate toolbar
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

    foreach( QAction *a, m_designer->actions() ) {
        a->setParent( this );
        tb->addAction( a );
        connect( a, SIGNAL( triggered( bool ) ), SLOT( slotInsertAction() ) );
    }
}

ReportDesignPanel::ReportDesignPanel( Project *project, ScheduleManager *manager, const QDomElement &element, const QMap<QString, QAbstractItemModel*> &models, QWidget *parent )
    : QWidget( parent ),
    m_modified( false )
{
    QVBoxLayout *l = new QVBoxLayout( this );

    KToolBar *tb = new KToolBar( this );
    l->addWidget( tb );

    QSplitter *sp1 = new QSplitter( this );
    l->addWidget( sp1 );

    QSplitter *sp2 = new QSplitter( sp1 );
    sp2->setOrientation( Qt::Vertical );

    m_sourceeditor = new ReportSourceEditor( sp2 );
    ReportSourceModel *model = createSourceModel( m_sourceeditor );
    m_sourceeditor->setModel( model );
    m_sourceeditor->setSourceData( element.firstChildElement( "data-source" ) );
    
    m_propertyeditor = new KoProperty::EditorView( sp2 );
    
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
    
    connect( model, SIGNAL( selectFromChanged( const QString& ) ), SLOT( setReportData( const QString& ) ) );

    connect( this, SIGNAL( insertItem( const QString& ) ), m_designer, SLOT( slotItem( const QString& ) ) );

    connect( m_designer, SIGNAL( propertySetChanged() ), SLOT( slotPropertySetChanged() ) );
    connect( m_designer, SIGNAL( dirty() ), SLOT( setModified() ) );

    // Populate toolbar
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

    foreach( QAction *a, m_designer->actions() ) {
        a->setParent( this );
        tb->addAction( a );
        connect( a, SIGNAL( triggered( bool ) ), SLOT( slotInsertAction() ) );
    }
}

void ReportDesignPanel::slotPropertySetChanged()
{
    //qDebug()<<"ReportDesignPanel::slotPropertySetChanged:"<<m_designer->itemPropertySet();
    if ( m_propertyeditor ) {
        m_propertyeditor->changeSet( m_designer->itemPropertySet() );
    }
}

void ReportDesignPanel::slotInsertAction()
{
    //qDebug()<<"ReportDesignerView::slotInsertAction:"<<sender();
    emit insertItem( sender()->objectName() );
}

void ReportDesignPanel::setReportData( const QString &tag )
{
    m_designer->setReportData( createReportData( tag ) );
}

QDomDocument ReportDesignPanel::document() const
{
    QDomDocument document( "kplatoreportdefinition" );
    document.appendChild( document.createProcessingInstruction(
                              "xml",
                              "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement e = document.createElement( "kplatoreportdefinition" );
    e.setAttribute( "editor", "KPlato" );
    e.setAttribute( "mime", "application/x-vnd.kde.kplato.report.definition" );
    e.setAttribute( "version", "1.0" );
    document.appendChild( e );

    if ( m_sourceeditor ) {
        m_sourceeditor->sourceData( e );
    } else qDebug()<<"ReportDesignerView::document: source data not available";
    e.appendChild( m_designer->document() );
/*    qDebug()<<"ReportDesignerView::document:";
    qDebug()<<document.toString();*/
    return document;
}

void ReportDesignPanel::createReportData( const QString &type, KoReportData *rd )
{
    rd = createReportData( type );
}

ReportData *ReportDesignPanel::createReportData( const QString &type )
{
    //FIXME a smarter report data creator
    ReportData *r = 0;
    if ( type == "costbreakdown" ) {
        r = new ChartReportData();
    } else {
        r = new ReportData();
    }
    connect( r, SIGNAL( createReportData( const QString&, KoReportData* ) ), SLOT( createReportData( const QString&, KoReportData* ) ) );
    r->setModel( m_modelmap.value( type ) );
    //qDebug()<<"ReportDesignerView::createReportData:"<<type<<r<<r->itemModel();
    if ( r->itemModel() ) {
        //qDebug()<<"ReportDesignerView::createReportData:"<<r->itemModel()->project()<<r->itemModel()->scheduleManager();
    }
    Q_ASSERT( r );
    return r;
}

ReportSourceModel *ReportDesignPanel::createSourceModel( QObject *parent ) const
{
    ReportSourceModel *m = new ReportSourceModel( 3, 2, parent );
    m->setHorizontalHeaderLabels( QStringList() << "Name" << "Value" );
    QList<QStandardItem*> lst;
    
    lst << new QStandardItem( "Source type" );
    lst.last()->setData( "source-type" );
    lst << new QStandardItem( "Source" );
    lst.last()->setData( "source" );

    QStandardItem *item = new QStandardItem( "Select from" );
    item->setData( "select-from" );
    item->setEditable( false );
    lst << item;

    foreach ( QStandardItem *item, lst ) {
        m->setItem( lst.indexOf( item ), item );
        item->setEditable( false );
    }
    int row = 0;
    item = new QStandardItem( "KPlato Project" );
    item->setData( "KPlato Project" );
    m->setItem( row++, 1, item );
    item = new QStandardItem( "Internal" );
    item->setData( "Internal" );
    m->setItem( row++, 1, item );
    
    // Child items
    QList<QStandardItem*> children;
    children << new QStandardItem( "Tasks" ) << new QStandardItem();
    children[ 0 ]->setData( "tasks", Reports::TagRole );
    children[ 1 ]->setCheckable( true );
    lst[ 2 ]->appendRow( children );
    children.clear();

    children << new QStandardItem( "Task status" ) << new QStandardItem();
    children[ 0 ]->setData( "taskstatus", Reports::TagRole );
    children[ 1 ]->setCheckable( true );
    lst[ 2 ]->appendRow( children );
    children.clear();

    children << new QStandardItem( "Resourceassignements" ) << new QStandardItem();
    children[ 0 ]->setData( "resourceassignments", Reports::TagRole );
    children[ 1 ]->setCheckable( true );
    lst[ 2 ]->appendRow( children );
    children.clear();

//     children << new QStandardItem( "Resources" ) << new QStandardItem();
//     children[ 0 ]->setData( "resources", Reports::TagRole );
//     children[ 1 ]->setCheckable( true );
//     lst[ 2 ]->appendRow( children );
//     children.clear();
// 
//     children << new QStandardItem( "Resourcegroups" ) << new QStandardItem();
//     children[ 0 ]->setData( "resourcegroups", Reports::TagRole );
//     children[ 1 ]->setCheckable( true );
//     lst[ 2 ]->appendRow( children );
//     children.clear();

    children << new QStandardItem( "Resources and Groups" ) << new QStandardItem();
    children[ 0 ]->setData( "resourcesandgroups", Reports::TagRole );
    children[ 1 ]->setCheckable( true );
    lst[ 2 ]->appendRow( children );
    children.clear();

    return m;
}


//-----------
ReportSourceModel::ReportSourceModel( QObject *parent )
    : QStandardItemModel( parent )
{
    connect( this, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), SLOT( slotSourceChanged( const QModelIndex&, const QModelIndex& ) ) );
}

ReportSourceModel::ReportSourceModel( int rows, int columns, QObject *parent )
    : QStandardItemModel( rows, columns, parent )
{
    connect( this, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), SLOT( slotSourceChanged( const QModelIndex&, const QModelIndex& ) ) );
}

void ReportSourceModel::slotSourceChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight )
{
    //qDebug()<<"ReportDesignerView::slotSourceChanged:"<<topLeft<<bottomRight;
    QModelIndex parent = topLeft.parent();
    if ( ! parent.isValid() ) {
        return;
    }
    if ( parent.data( Reports::TagRole ).toString() == "select-from" ) {
        if ( topLeft.data( Qt::CheckStateRole ).toInt() == Qt::Checked ) {
            // set select-from value and tag
            QModelIndex idx = index( parent.row(), 1, parent.parent() );
            QString tag = index( topLeft.row(), 0, parent ).data( Reports::TagRole ).toString();
            QString value = index( topLeft.row(), 0, parent ).data().toString();
            setData( idx, value, Qt::EditRole );
            setData( idx, tag, Reports::TagRole );
            // update checked mark
            setChecked( topLeft );
            //qDebug()<<"ReportSourceModel::slotSourceChanged:"<<tag<<m_modelmap[ tag ];
            emit selectFromChanged( tag );
        } else {
            // set checked flag; it's not allowed to be reset by user
            disconnect( this, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotSourceChanged( const QModelIndex&, const QModelIndex& ) ) );
            setData( topLeft, Qt::Checked, Qt::CheckStateRole );
            connect( this, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotSourceChanged( const QModelIndex&, const QModelIndex& ) ) );
        }
    }
}

void ReportSourceModel::setChecked( const QModelIndex &idx )
{
    disconnect( this, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotSourceChanged( const QModelIndex&, const QModelIndex& ) ) );
    setData( idx, "checked", Reports::TagRole );
    for ( int row = 0; row < rowCount( idx.parent() ); ++row ) {
        QModelIndex i = index( row, 1, idx.parent() );
        if ( i != idx ) {
            setData( i, QVariant( (int)Qt::Unchecked ), Qt::CheckStateRole );
            setData( i, "unchecked", Reports::TagRole );
        }
    }
    connect( this, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotSourceChanged( const QModelIndex&, const QModelIndex& ) ) );
}


} // namespace KPlato

#include "reportview.moc"
#include "reportview_p.moc"
