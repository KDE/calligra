/*
 * KPlato Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2009 by Dag Andersen <danders@get2net.dk>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "reportview.h"
#include "report.h"
#include "reportview_p.h"
#include "reportpage.h"
#include "reportdata.h"
#include "reportsourceeditor.h"

#include <orprerender.h>
#include <orprintrender.h>
#include <renderobjects.h>
#include <krhtmlrender.h>
#include <reportsection.h>
#include <EditorView.h>

#include "kptnodeitemmodel.h"
#include "kptitemmodelbase.h"

#include "KoDocument.h"

#include <KTabWidget>
#include <KAction>
#include <KActionCollection>
#include <KMessageBox>
#include <kfiledialog.h>
#include <kio/netaccess.h>
#include <kdebug.h>

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

Report::Report(KoDocument *part, QWidget *parent)
    : SplitterView(part, parent)
{
    m_tab = addTabWidget();
    m_designer = new ReportDesignerView( part, m_tab );
    ReportView *v = new ReportView( part, m_tab, m_designer );
    addView( v, m_tab, "Report" );
    addView( m_designer, m_tab, "Design" );
    
    connect( m_designer, SIGNAL( dataChanged() ), v, SLOT( refresh() ) );
}

void Report::setGuiActive( bool active ) // virtual slot
{
    ViewBase *v = qobject_cast<ViewBase*>( m_tab->currentWidget() );
    qDebug()<<"Report::setGuiActive:"<<active<<m_activeview<<"->"<<v;
    if ( m_activeview && m_activeview != v ) {
        m_activeview->setGuiActive( false );
    }
    if ( active ) {
        m_activeview = v;
    }
    if ( v ) {
        v->setGuiActive( active );
    }
    SplitterView::setGuiActive( active );
}

QDockWidget *Report::createPropertyDocker()
{
    KTabWidget *tab = new KTabWidget();
    ReportSourceEditor *se = new ReportSourceEditor( tab );
    tab->addTab( se, "Source" );
    KoProperty::EditorView *e = new KoProperty::EditorView( tab );
    tab->addTab( e, "Properties" );
    QDockWidget *dock = new QDockWidget( this );
    dock->setWindowTitle( "Properties editor" );
    dock->setWidget( tab );
    m_designer->setPropertyEditor( dock, e, se );
    return dock;
}

void Report::insertDataModel( const QString &tag, QAbstractItemModel *model )
{
    QList<ReportDesignerView*> lst = findChildren<ReportDesignerView*>();
    foreach ( ReportDesignerView *v, lst ) {
        v->insertDataModel( tag, model );
    }
}

bool Report::loadContext( const KoXmlElement &context )
{
    qDebug()<<"Report::loadContext:";
    return SplitterView::loadContext( context );

}

void Report::saveContext( QDomElement &context ) const
{
    qDebug()<<"Report::saveContext:";
    SplitterView::saveContext( context );
}


//-----------------
ReportView::ReportView( KoDocument *part, QWidget *parent, ReportDesignerView *designer )
    : ViewBase( part, parent ),
    m_designer( designer )
{
    m_preRenderer = 0;
    setObjectName("KPlatoReportView");
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

void ReportView::slotPrintReport()
{
    QPrinter printer;
    ORPrintRender pr;

    // do some printer initialization
    pr.setPrinter(&printer);
    pr.setupPrinter(m_reportDocument, &printer);

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    if (dialog->exec() != QDialog::Accepted)
        return;

    pr.render(m_reportDocument);
}

#if 0
void ReportView::slotRenderKSpread()
{
#ifdef HAVE_KSPREAD
    KRKSpreadRender ks;
    KUrl saveUrl = KFileDialog::getSaveUrl(KUrl(), QString(), this, i18n("Save Report to.."));
    if (!saveUrl.isValid()) {
        KMessageBox::error(this, i18n("Report not exported.The URL was invalid"), i18n("Not Saved"));
        return;
    }

    if (KIO::NetAccess::exists(saveUrl, KIO::NetAccess::DestinationSide, this)) {
        int wantSave = KMessageBox::warningContinueCancel(this, i18n("The file %1 exists.\nDo you wish to overwrite it?", saveUrl.path()), i18n("Warning"), KGuiItem(i18n("Overwrite")));
        if (wantSave != KMessageBox::Continue) {
            return;
        }
    }
    if (!ks.render(m_reportDocument, saveUrl)) {
        KMessageBox::error(this, i18n("Failed to open %1 in KSpread", saveUrl.prettyUrl()) , i18n("Opening in KSpread failed"));
    }
#endif
}
#endif

void ReportView::slotExportHTML()
{
    KUrl saveUrl = KFileDialog::getSaveUrl(KUrl(), QString(), this, i18n("Save Report to.."));
    if (!saveUrl.isValid()) {
        KMessageBox::error(this, i18n("Report not exported, no file selected for writing to"), i18n("Not Saved"));
        return;
    }
    if (KIO::NetAccess::exists(saveUrl, KIO::NetAccess::DestinationSide, this)) {
        int wantSave = KMessageBox::warningContinueCancel(this, i18n("The file %1 exists.\nDo you wish to overwrite it?", saveUrl.path()), i18n("Warning"), KGuiItem(i18n("Overwrite")));
        if (wantSave != KMessageBox::Continue) {
            return;
        }
    }

    bool css = (KMessageBox::questionYesNo(this, i18n("Would you like to export using a Cascading Style Sheet which will give output closer to the original, or export using a Table which outputs a much simpler format."), i18n("Export Style"), KGuiItem("CSS"), KGuiItem("Table")) == KMessageBox::Yes);

    KRHtmlRender hr;
    if (!hr.render(m_reportDocument, saveUrl, css)) {
        KMessageBox::error(this, i18n("Exporting report to %1 failed", saveUrl.prettyUrl()), i18n("Saving failed"));
    } else {
        KMessageBox::information(this, i18n("Report saved to %1", saveUrl.prettyUrl()) , i18n("Saved OK"));
    }
}

void ReportView::setupGui()
{
    KActionCollection *coll = actionCollection();
    KAction *a = 0;
    QString name = "reportview_list";

#if 0
#ifdef HAVE_KSPREAD
    a = new KAction(KIcon("kspread"), i18n("Open in KSpread"), this));
    a->setToolTip(i18n("Open the report in KSpread"));
    a->setWhatsThis(i18n("Opens the current report in KSpread."));
    connect(a, SIGNAL(triggered()), this, SLOT(slotRenderKSpread()));
    addAction( name, a );
#endif
#endif

    a = new KAction( KIcon( "text-html" ), i18n( "Export to HTML" ), this );
    a->setToolTip( i18n( "Export the report to HTML" ) );
    a->setWhatsThis( i18n( "Exports the report to a HTML file." ) );
    connect(a, SIGNAL(triggered()), this, SLOT(slotExportHTML()));
    addAction( name, a );

    a = new KAction( KIcon( "printer" ), i18n( "Print" ), this );
    a->setToolTip(i18n("Print Report"));
    a->setWhatsThis(i18n("Prints the current report."));
    connect(a, SIGNAL(triggered()), this, SLOT(slotPrintReport()));
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

void ReportView::refresh()
{
    qDebug()<<"ReportView::refresh:";
    delete m_preRenderer;
    QDomElement e = tempData().documentElement();
    m_preRenderer = new ORPreRender( e.firstChildElement( "report:content" ) );
    if ( ! m_preRenderer->isValid()) {
        qDebug()<<"ReportView::refresh: Invalid tempData";
        return;
    }
    m_preRenderer->setSourceData( createReportData( e ) );
    m_reportDocument = m_preRenderer->generate();
    m_pageSelector->setMaximum( m_reportDocument ? m_reportDocument->pages() : 1 );
    m_pageSelector->setCurrentPage( 1 );

    m_reportWidget = new ReportPage(this, m_reportDocument);
    m_reportWidget->setObjectName("KPlatoReportPage");
    m_scrollArea->setWidget(m_reportWidget);
    return;
}

ReportData *ReportView::createReportData( const QDomElement &element ) const
{
    ReportData *rd = new ReportData();

    // get the source connection data
    QDomElement e = element.firstChildElement( "connection" );
    QString from;
    QDomNamedNodeMap map = e.firstChildElement( "select-from" ).attributes();
    for ( int i = 0; i < map.count(); ++i ) {
        QDomNode n = map.item( i );
        if ( n.nodeValue() == "checked" ) {
            from = n.nodeName();
            break;
        }
    }
    
    QAbstractItemModel *m = m_designer->dataModel( from );
    if ( m ) {
        ItemModelBase *im = qobject_cast<ItemModelBase*>( m );
        if ( im ) {
            im->refresh();
        } else {
            QAbstractProxyModel *pm = qobject_cast<QAbstractProxyModel*>( m );
            while ( pm ) {
                im = qobject_cast<ItemModelBase*>( pm->sourceModel() );
                if ( im ) {
                    im->refresh();
                    break;
                }
                pm = qobject_cast<QAbstractProxyModel*>( pm->sourceModel() );
            }
        }
        m = setGroupByModels( element, m );
    } else {
        qDebug()<<"ReportView::createReportData: No model selected";
    }
    qDebug()<<"ReportView::createReportData:"<<project();
    rd->setModel( m );
    return rd;
}

QAbstractItemModel *ReportView::setGroupByModels( const QDomElement &element, QAbstractItemModel *model ) const
{
    QSortFilterProxyModel *sf = 0;
    QAbstractItemModel *source_model = model;
    QDomElement e = element.firstChildElement( "report:content" );
    e = e.firstChildElement( "report:body" );
    e = e.firstChildElement( "report:detail" );
    e = e.firstChildElement( "report:group" );
    ReportData rd;
    rd.setModel( model );
    for ( ; ! e.isNull(); e = e.nextSiblingElement( "report:group" ) ) {
        int column = rd.fieldNumber( e.attribute( "report:group-column" ) );
        if ( column < 0 ) {
            continue;
        }
        sf = new QSortFilterProxyModel( const_cast<ReportView*>( this ) );
        sf->setSourceModel( source_model );
        sf->sort( column );
        source_model = sf;
    }
    return sf ? sf : model;
}

QDomDocument ReportView::tempData() const
{
    if ( m_designer == 0 ) {
        return QDomDocument();
    }
    QDomDocument doc = m_designer->document();
    qDebug()<<"ReportView::tempData:";
//    qDebug()<<doc.toString();
    return doc;
}

KoReportData* ReportView::sourceData(QDomElement e)
{
    KoReportData *kodata;
    kodata = 0;
    return kodata;
}

bool ReportView::loadContext( const KoXmlElement &context )
{
    qDebug()<<"ReportView::loadContext:";
    return true;
}

void ReportView::saveContext( QDomElement &context ) const
{
    qDebug()<<"ReportView::saveContext:";
}

//---------------------------

KPlato_ReportDesigner::KPlato_ReportDesigner( QWidget *parent )
    : ReportDesigner( parent )
{
}

//--
ReportDesignerView::ReportDesignerView(KoDocument *part, QWidget *parent)
    : ViewBase(part, parent),
    m_designer( 0 ),
    m_propertydocker( 0 ),
    m_propertyeditor( 0 ),
    m_sourceeditor( 0 )
{
    qDebug()<<"ReportDesignerView::ReportDesignerView -----------";
    setObjectName( "KPlatoReportDesigner" );

    m_scrollarea = new QScrollArea( this );
    QVBoxLayout * l = new QVBoxLayout( this );
    l->addWidget( m_scrollarea );
    
    setReportDesigner( new ReportDesigner( this ) );
    
    setupGui();
    
    
//     actionCollection()->addAction(KStandardAction::Cut,  "edit-cut", m_designer, SLOT( slotEditCut() ));
//     actionCollection()->addAction(KStandardAction::Copy,  "edit-copy", m_designer, SLOT( slotEditCopy() ));
//     actionCollection()->addAction(KStandardAction::Paste,  "edit-paste", m_designer, SLOT( slotEditPaste() ));

//     connect(m_reportDesigner, SIGNAL(propertySetChanged()), this, SLOT(slotDesignerPropertySetChanged()));
//     connect(m_reportDesigner, SIGNAL(dirty()), this, SLOT(setDirty()));

}

void ReportDesignerView::setReportDesigner( ReportDesigner *designer )
{
    if ( m_designer ) {
        m_scrollarea->takeWidget();
        delete m_designer;
    }

    m_designer = designer;
    m_scrollarea->setWidget( m_designer );

    connect( this, SIGNAL( insertItem( const QString& ) ), m_designer, SLOT( slotItem( const QString& ) ) );
    
    connect( m_designer, SIGNAL( propertySetChanged() ), SLOT( slotPropertySetChanged() ) );
}

void ReportDesignerView::setupGui()
{
    KActionCollection *coll = actionCollection();

    QString name = "reportdesigner_add_list";

    QList<QAction*> lst = ReportDesigner::actions();
    foreach ( QAction *a, lst ) {
        a->setParent( this );
        coll->addAction( a->objectName(), a );
        connect( a, SIGNAL( triggered( bool ) ), SLOT( slotInsertAction() ) );
        addAction( name, a );
    }

    name = "reportdesigner_edit_list";

    KAction *a = new KAction( i18n( "Section Editor" ), this );
    a->setObjectName("sectionedit");
    connect(a, SIGNAL(triggered(bool)), this, SLOT(slotSectionEditor()));
    addAction( name, a );
    
    a = new KAction( KIcon( "arrow-up" ), i18n( "Raise" ), this );
    connect(a, SIGNAL(activated()), m_designer, SLOT(slotRaiseSelected()));
    addAction( name, a );
    a = new KAction( KIcon( "arrow-down" ), i18n( "Lower" ), this );
    connect(a, SIGNAL(activated()), m_designer, SLOT(slotLowerSelected()));
    addAction( name, a );

    a = coll->addAction( KStandardAction::Clear, "edit-delete", m_designer,  SLOT( slotEditDelete() ) );
    addAction( name, a );

    qDebug()<<"ReportDesignerView::setupGui:"<<actionList( name );
}

void ReportDesignerView::slotSectionEditor()
{
    qDebug()<<"ReportDesignerView::slotSectionEditor:"<<sender();
    m_designer->slotSectionEditor();
}

void ReportDesignerView::slotInsertAction()
{
    qDebug()<<"ReportDesignerView::slotInsertAction:"<<sender();
    emit insertItem( sender()->objectName() );
}

void ReportDesignerView::setGuiActive( bool active ) // virtual slot
{
    qDebug()<<"ReportDesignerView::setGuiActive:"<<active<<m_propertydocker;
    if ( m_propertydocker ) {
        m_propertydocker->setVisible( active );
    }
    ViewBase::setGuiActive( active );
}

void ReportDesignerView::setPropertyEditor( QDockWidget *docker, KoProperty::EditorView *editor, ReportSourceEditor *se )
{
    m_propertydocker = docker;
    m_propertyeditor = editor;
    m_sourceeditor = se;
    setSourceModel( sourceModel() );
}

QAbstractItemModel *ReportDesignerView::sourceModel()
{
    QStandardItemModel *m = new QStandardItemModel( 3, 2, this );
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

    children << new QStandardItem( "Resources" ) << new QStandardItem();
    children[ 0 ]->setData( "resources", Reports::TagRole );
    children[ 1 ]->setCheckable( true );
    lst[ 2 ]->appendRow( children );
    children.clear();

    children << new QStandardItem( "Resourcegroups" ) << new QStandardItem();
    children[ 0 ]->setData( "resourcegroups", Reports::TagRole );
    children[ 1 ]->setCheckable( true );
    lst[ 2 ]->appendRow( children );
    children.clear();

    children << new QStandardItem( "Resources and Groups" ) << new QStandardItem();
    children[ 0 ]->setData( "resourcesandgroups", Reports::TagRole );
    children[ 1 ]->setCheckable( true );
    lst[ 2 ]->appendRow( children );
    children.clear();

    return m;
}

void ReportDesignerView::setSourceModel( QAbstractItemModel *model )
{
    m_sourceeditor->setModel( model );
    connect( model, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), SLOT( slotSourceChanged( const QModelIndex&, const QModelIndex& ) ) );
}

void ReportDesignerView::insertDataModel( const QString &tag, QAbstractItemModel *model )
{
    if ( ! m_modelmap.contains( tag ) ) {
        m_modelmap[ tag ] = model;
    } else {
        qDebug()<<"ReportView::insertDataModel: Model tag already exists:"<<tag;
    }
    qDebug()<<"ReportView::insertDataModel:"<<m_modelmap;
}

QAbstractItemModel *ReportDesignerView::dataModel( const QString &tag ) const
{
    return m_modelmap.value( tag );
}

void ReportDesignerView::slotPropertySetChanged()
{
    qDebug()<<"ReportDesignerView::slotPropertySetChanged:"<<m_designer->itemPropertySet();
    if ( m_propertyeditor ) {
        m_propertyeditor->changeSet( m_designer->itemPropertySet() );
    }
}

void ReportDesignerView::slotSourceChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight )
{
    qDebug()<<"ReportDesignerView::slotSourceChanged:"<<topLeft<<bottomRight;
    QAbstractItemModel *m = const_cast<QAbstractItemModel*>( topLeft.model() );
    QModelIndex parent = topLeft.parent();
    if ( ! parent.isValid() ) {
        return;
    }
    if ( parent.data( Reports::TagRole ).toString() == "select-from" )
    {
        if ( topLeft.data( Qt::CheckStateRole ).toInt() == Qt::Checked ) {
            ReportData *rd = new ReportData();
            // set select-from value and tag
            QModelIndex idx = m->index( parent.row(), 1, parent.parent() );
            QString tag = m->index( topLeft.row(), 0, parent ).data( Reports::TagRole ).toString();
            QString value = m->index( topLeft.row(), 0, parent ).data().toString();
            m->setData( idx, value, Qt::EditRole );
            m->setData( idx, tag, Reports::TagRole );
            // update checked mark
            setChecked( topLeft );
            qDebug()<<"ReportDesignerView::slotSourceChanged:"<<tag<<m_modelmap[ tag ];
            rd->setModel( m_modelmap[ tag ] );
            m_designer->setReportData( rd );
        } else {
            // set checked flag; it's not allowed to be reset by user
            disconnect( m, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotSourceChanged( const QModelIndex&, const QModelIndex& ) ) );
            m->setData( topLeft, Qt::Checked, Qt::CheckStateRole );
            connect( m, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotSourceChanged( const QModelIndex&, const QModelIndex& ) ) );
        }
    }
}

void ReportDesignerView::setChecked( const QModelIndex &idx )
{
    QAbstractItemModel *m = const_cast<QAbstractItemModel*>( idx.model() );

    disconnect( m, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotSourceChanged( const QModelIndex&, const QModelIndex& ) ) );
    m->setData( idx, "checked", Reports::TagRole );
    for ( int row = 0; row < idx.model()->rowCount( idx.parent() ); ++row ) {
        QModelIndex i = m->index( row, 1, idx.parent() );
        if ( i != idx ) {
            m->setData( i, QVariant( (int)Qt::Unchecked ), Qt::CheckStateRole );
            m->setData( i, "unchecked", Reports::TagRole );
        }
    }
    connect( m, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotSourceChanged( const QModelIndex&, const QModelIndex& ) ) );
}

QDomDocument ReportDesignerView::document() const
{
    QDomDocument doc( "kplatoreportdesigner" );
    QDomElement e = doc.createElement( "kplatoreportdesign" );
    doc.appendChild( e );
    if ( m_sourceeditor ) {
        m_sourceeditor->sourceData( e );
    } else qDebug()<<"ReportDesignerView::document: source data not available";
    e.appendChild( m_designer->document() );
    qDebug()<<"ReportDesignerView::document:";
//    qDebug()<<doc.toString();
    return doc;
}

bool ReportDesignerView::loadContext( const KoXmlElement &context )
{
    qDebug()<<"ReportDesignerView::loadContext:";
    QDomDocument doc( "Context" );
    QDomElement e = KoXml::asQDomElement( doc, context ).firstChildElement( "kplatoreportdesign" );
    qDebug()<<doc.toString();

    if ( e.tagName() == "kplatoreportdesign" ) {
        setReportDesigner( new ReportDesigner( this, e.firstChildElement( "report:content" ) ) );
        qDebug()<<"ReportDesignerView::loadContext: load connection";
        m_sourceeditor->setSourceData( e.firstChildElement( "connection" ) );
        emit dataChanged();
        return true;
    }
    return false;
}

void ReportDesignerView::saveContext( QDomElement &context ) const
{
    qDebug()<<"ReportDesignerView::saveContext:";
    QDomDocument doc = document();
    qDebug()<<doc.toString();
    context.appendChild( doc.documentElement() );
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
    qDebug()<<"ReportNavigator::setButtonsEnabled:"<<ui_selector->value()<<ui_selector->minimum()<<ui_selector->maximum();
    bool backw = ui_selector->value() > ui_selector->minimum();
    ui_first->setEnabled( backw );
    ui_prev->setEnabled( backw );

    bool forw = ui_selector->value() < ui_selector->maximum();
    ui_last->setEnabled( forw );
    ui_next->setEnabled( forw );
}

} // namespace KPlato

#include "reportview.moc"
#include "reportview_p.moc"
