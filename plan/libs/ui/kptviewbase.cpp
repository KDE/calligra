/* This file is part of the KDE project
  Copyright (C) 2006 -2010, 2012 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "kptviewbase.h"
#include "kptitemmodelbase.h"
#include "kptproject.h"
#include "kptdebug.h"

#include <kaction.h>
#include <kparts/event.h>
#include <kxmlguifactory.h>
#include <kmessagebox.h>
#include <knotification.h>

#include <KoIcon.h>
#include "calligraversion.h"
#include <KoDocument.h>
#include <KoPart.h>
#include <KoShape.h>
#include <KoPageLayoutWidget.h>
#include <KoPagePreviewWidget.h>
#include "KoUnit.h"

#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QPoint>
#include <QScrollBar>
#include <QAbstractScrollArea>
#include <QMetaEnum>
#include <QStyleOption>
#include <QPainter>
#include <QMenu>
#include <QPainter>
#include <QMainWindow>

namespace KPlato
{


DockWidget::DockWidget( ViewBase *v, const QString &identity,  const QString &title )
    : QDockWidget( v ),
    view( v ),
    id( identity ),
    location( Qt::RightDockWidgetArea ),
    editor( false ),
    m_shown( true )
{
    setWindowTitle( title );
    setObjectName( v->objectName() + '-' + identity );
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), SLOT(setLocation(Qt::DockWidgetArea)));
}

void DockWidget::activate( QMainWindow *shell )
{
    connect(this, SIGNAL(visibilityChanged(bool)), this, SLOT(setShown(bool)));
    setVisible( m_shown );
    shell->addDockWidget( location, this );
}

void DockWidget::deactivate( QMainWindow *shell )
{
    disconnect(this, SIGNAL(visibilityChanged(bool)), this, SLOT(setShown(bool)));
    shell->removeDockWidget( this );
}

void DockWidget::setShown( bool show )
{
    m_shown = show;
    setVisible( show );
}

void DockWidget::setLocation( Qt::DockWidgetArea area )
{
    location = area;
}

bool DockWidget::saveXml( QDomElement &context ) const
{
    QDomElement e = context.ownerDocument().createElement( "docker" );
    context.appendChild( e );
    e.setAttribute( "id", id );
    e.setAttribute( "location", location );
    e.setAttribute( "floating", isFloating() );
    e.setAttribute( "visible", m_shown );
    return true;
}

void DockWidget::loadXml(const KoXmlElement& context)
{
    location = static_cast<Qt::DockWidgetArea>( context.attribute( "location", "0" ).toInt() );
    setFloating( (bool) context.attribute( "floating", "0" ).toInt() );
    m_shown = context.attribute( "visible", "1" ).toInt();
}

//------------------------
bool PrintingOptions::loadXml( KoXmlElement &element )
{
    KoXmlElement e;
    forEachElement( e, element ) {
        if ( e.tagName() == "header" ) {
            headerOptions.group = e.attribute( "group", "0" ).toInt();
            headerOptions.project = static_cast<Qt::CheckState>( e.attribute( "project", "0" ).toInt() );
            headerOptions.date = static_cast<Qt::CheckState>( e.attribute( "date", "0" ).toInt() );
            headerOptions.manager = static_cast<Qt::CheckState>( e.attribute( "manager", "0" ).toInt() );
            headerOptions.page = static_cast<Qt::CheckState>( e.attribute( "page", "0" ).toInt() );
        } else if ( e.tagName() == "footer" ) {
            footerOptions.group = e.attribute( "group", "0" ).toInt();
            footerOptions.project = static_cast<Qt::CheckState>( e.attribute( "project", "0" ).toInt() );
            footerOptions.date = static_cast<Qt::CheckState>( e.attribute( "date", "0" ).toInt() );
            footerOptions.manager = static_cast<Qt::CheckState>( e.attribute( "manager", "0" ).toInt() );
            footerOptions.page = static_cast<Qt::CheckState>( e.attribute( "page", "0" ).toInt() );
        }
    }
    return true;
}

void PrintingOptions::saveXml( QDomElement &element ) const
{
    QDomElement me = element.ownerDocument().createElement( "printing-options" );
    element.appendChild( me );

    QDomElement h = me.ownerDocument().createElement( "header" );
    me.appendChild( h );
    h.setAttribute( "group", headerOptions.group );
    h.setAttribute( "project", headerOptions.project );
    h.setAttribute( "date", headerOptions.date );
    h.setAttribute( "manager", headerOptions.manager );
    h.setAttribute( "page", headerOptions.page );

    QDomElement f = me.ownerDocument().createElement( "footer" );
    me.appendChild( f );
    f.setAttribute( "group", footerOptions.group );
    f.setAttribute( "project", footerOptions.project );
    f.setAttribute( "date", footerOptions.date );
    f.setAttribute( "manager", footerOptions.manager );
    f.setAttribute( "page", footerOptions.page );
}

//----------------------
PrintingHeaderFooter::PrintingHeaderFooter( const PrintingOptions &opt, QWidget *parent )
    : QWidget( parent )
{
    setupUi( this );
    setWindowTitle( i18n("Header and Footer" ));
    setOptions( opt );

    connect(ui_header, SIGNAL(toggled(bool)), SLOT(slotChanged()));
    connect(ui_headerProject, SIGNAL(stateChanged(int)), SLOT(slotChanged()));
    connect(ui_headerPage, SIGNAL(stateChanged(int)), SLOT(slotChanged()));
    connect(ui_headerManager, SIGNAL(stateChanged(int)), SLOT(slotChanged()));
    connect(ui_headerDate, SIGNAL(stateChanged(int)), SLOT(slotChanged()));

    connect(ui_footer, SIGNAL(toggled(bool)), SLOT(slotChanged()));
    connect(ui_footerProject, SIGNAL(stateChanged(int)), SLOT(slotChanged()));
    connect(ui_footerPage, SIGNAL(stateChanged(int)), SLOT(slotChanged()));
    connect(ui_footerManager, SIGNAL(stateChanged(int)), SLOT(slotChanged()));
    connect(ui_footerDate, SIGNAL(stateChanged(int)), SLOT(slotChanged()));
}

PrintingHeaderFooter::~PrintingHeaderFooter()
{
    //kDebug(planDbg());
}

void PrintingHeaderFooter::slotChanged()
{
    kDebug(planDbg());
    emit changed( options() );
}

void PrintingHeaderFooter::setOptions( const PrintingOptions &options )
{
    m_options = options;
    ui_header->setChecked( m_options.headerOptions.group );
    ui_headerProject->setCheckState( m_options.headerOptions.project );
    ui_headerDate->setCheckState( m_options.headerOptions.date );
    ui_headerManager->setCheckState( m_options.headerOptions.manager );
    ui_headerPage->setCheckState( m_options.headerOptions.page );

    ui_footer->setChecked( m_options.footerOptions.group );
    ui_footerProject->setCheckState( m_options.footerOptions.project );
    ui_footerDate->setCheckState( m_options.footerOptions.date );
    ui_footerManager->setCheckState( m_options.footerOptions.manager );
    ui_footerPage->setCheckState( m_options.footerOptions.page );

}

PrintingOptions PrintingHeaderFooter::options() const
{
    //kDebug(planDbg());
    PrintingOptions opt;
    opt.headerOptions.group = ui_header->isChecked();
    opt.headerOptions.project = ui_headerProject->checkState();
    opt.headerOptions.date = ui_headerDate->checkState();
    opt.headerOptions.manager = ui_headerManager->checkState();
    opt.headerOptions.page = ui_headerPage->checkState();

    opt.footerOptions.group = ui_footer->isChecked();
    opt.footerOptions.project = ui_footerProject->checkState();
    opt.footerOptions.date = ui_footerDate->checkState( );
    opt.footerOptions.manager = ui_footerManager->checkState();
    opt.footerOptions.page = ui_footerPage->checkState();
    return opt;
}

PrintingDialog::PrintingDialog( ViewBase *view )
    : KoPrintingDialog( view ),
    m_view( view ),
    m_widget( 0 )
{
    setPrinterPageLayout( view->pageLayout() );
    QImage px( 100, 600, QImage::Format_Mono );
    int dpm = printer().resolution() * 40;
    px.setDotsPerMeterX( dpm );
    px.setDotsPerMeterY( dpm );
    QPainter p( &px );
    m_textheight = p.boundingRect( QRectF(), Qt::AlignTop, "Aj" ).height();
    kDebug(planDbg())<<"textheight:"<<m_textheight;
}

PrintingDialog::~PrintingDialog()
{
}

QAbstractPrintDialog::PrintDialogOptions PrintingDialog::printDialogOptions() const
{
    return QAbstractPrintDialog::PrintToFile |
           QAbstractPrintDialog::PrintPageRange |
           QAbstractPrintDialog::PrintCollateCopies |
           QAbstractPrintDialog::DontUseSheet;
}

PrintingOptions PrintingDialog::printingOptions() const
{
    return m_view->printingOptions();
}

void PrintingDialog::setPrintingOptions( const PrintingOptions &opt )
{
    kDebug(planDbg());
    m_view->setPrintingOptions( opt );
    emit changed( opt );
    emit changed();
}

void PrintingDialog::setPrinterPageLayout( const KoPageLayout &pagelayout )
{
    QPrinter &p = printer();
    QPrinter::Orientation o;
    switch ( pagelayout.orientation ) {
        case KoPageFormat::Portrait: o = QPrinter::Portrait; break;
        case KoPageFormat::Landscape: o = QPrinter::Landscape; break;
        default: o = QPrinter::Portrait; break;
    }
    p.setOrientation( o );
    p.setPaperSize( KoPageFormat::printerPageSize( pagelayout.format ) );
    p.setPageMargins( pagelayout.leftMargin, pagelayout.topMargin, pagelayout.rightMargin, pagelayout.bottomMargin, QPrinter::Point );
}

void PrintingDialog::startPrinting(RemovePolicy removePolicy )
{
    setPrinterPageLayout( m_view->pageLayout() ); // FIXME: Something resets printer().paperSize() to A4 !
    KoPrintingDialog::startPrinting( removePolicy );
}

QWidget *PrintingDialog::createPageLayoutWidget() const
{
    QWidget *w = ViewBase::createPageLayoutWidget( m_view );
    KoPageLayoutWidget *pw = w->findChild<KoPageLayoutWidget*>();
    connect(pw, SIGNAL(layoutChanged(const KoPageLayout&)), m_view, SLOT(setPageLayout(const KoPageLayout&)));
    connect(pw, SIGNAL(layoutChanged(const KoPageLayout&)), this, SLOT(setPrinterPageLayout(const KoPageLayout&)));
    connect(pw, SIGNAL(layoutChanged(const KoPageLayout&)), this, SIGNAL(changed()));
    return w;
}

QList<QWidget*> PrintingDialog::createOptionWidgets() const
{
    //kDebug(planDbg());
    PrintingHeaderFooter *w = new PrintingHeaderFooter( printingOptions() );
    connect(w, SIGNAL(changed(PrintingOptions)), this, SLOT(setPrintingOptions(const PrintingOptions&)));
    const_cast<PrintingDialog*>( this )->m_widget = w;

    return QList<QWidget*>() << w;
}

QList<KoShape*> PrintingDialog::shapesOnPage(int)
{
    return QList<KoShape*>();
}

void PrintingDialog::drawBottomRect( QPainter &p, const QRect &r )
{
    p.drawLine( r.topLeft(), r.bottomLeft() );
    p.drawLine( r.bottomLeft(), r.bottomRight() );
    p.drawLine( r.topRight(), r.bottomRight() );
}

QRect PrintingDialog::headerRect() const
{
    PrintingOptions options =  m_view->printingOptions();
    if ( options.headerOptions.group == false ) {
        return QRect();
    }
    int height = headerFooterHeight( options.headerOptions );
    return QRect( 0, 0, const_cast<PrintingDialog*>( this )->printer().pageRect().width(), height );
}

QRect PrintingDialog::footerRect() const
{
    PrintingOptions options =  m_view->printingOptions();
    if ( options.footerOptions.group == false ) {
        return QRect();
    }
    int height = headerFooterHeight( options.footerOptions );
    QRect r = const_cast<PrintingDialog*>( this )->printer().pageRect();
    return QRect( 0, r.height() - height, r.width(), height );
}

int PrintingDialog::headerFooterHeight( const PrintingOptions::Data &options ) const
{
    int height = 0.0;
    if ( options.page == Qt::Checked || options.project == Qt::Checked || options.manager == Qt::Checked || options.date == Qt::Checked ) {
        height += m_textheight * 1.5;
    }
    if (  options.project == Qt::Checked && options.manager == Qt::Checked && ( options.date == Qt::Checked || options.page == Qt::Checked ) ) {
       height *= 2.0;
    }
    kDebug(planDbg())<<height;
    return height;
}

void PrintingDialog::paintHeaderFooter( QPainter &p, const PrintingOptions &options, int pageNumber, const Project &project )
{
    if ( options.headerOptions.group == true ) {
        paint( p, options.headerOptions, headerRect(), pageNumber, project );
    }
    if ( options.footerOptions.group == true ) {
        paint( p, options.footerOptions, footerRect(), pageNumber, project );
    }
}

void PrintingDialog::paint( QPainter &p, const PrintingOptions::Data &options, const QRect &rect,  int pageNumber, const Project &project )
{
    p.save();

    p.setPen( Qt::black );
    p.drawRect( rect );
    QRect projectRect;
    QString projectName = project.name();
    QRect pageRect;
    QString page = i18nc( "1=page number, 2=last page number", "%1 (%2)", pageNumber, documentLastPage() );
    QRect managerRect;
    QString manager = project.leader();
    QRect dateRect;
    QString date = KGlobal::locale()->formatDate( QDate::currentDate() );

    QRect rect_1 = rect;
    QRect rect_2 = rect;
    if ( options.project == Qt::Checked ) {
        rect_2.setHeight( rect.height() / 2 );
        rect_2.translate( 0, rect_2.height() );
    }
    if ( ( options.project == Qt::Checked && options.manager == Qt::Checked ) && ( options.date == Qt::Checked || options.page == Qt::Checked ) ) {
        rect_1.setHeight( rect.height() / 2 );
        p.drawLine( rect_1.bottomLeft(), rect_1.bottomRight() );
    }

    if ( options.page == Qt::Checked ) {
        pageRect = p.boundingRect( rect_1, Qt::AlignRight|Qt::AlignTop, page );
        pageRect.setHeight( rect_1.height() );
        rect_1.setRight( pageRect.left() - 2 );
        if ( options.project == Qt::Checked || options.manager == Qt::Checked || options.date == Qt::Checked ) {
            p.drawLine( rect_1.topRight(), rect_1.bottomRight() );
        }
    }
    if ( options.date == Qt::Checked ) {
        if ( ( options.project == Qt::Checked && options.manager != Qt::Checked ) ||
             ( options.project != Qt::Checked && options.manager == Qt::Checked ) )
        {
            dateRect = p.boundingRect( rect_1, Qt::AlignRight|Qt::AlignTop, date );
            dateRect.setHeight( rect_1.height() );
            rect_1.setRight( dateRect.left() - 2 );
            p.drawLine( rect_1.topRight(), rect_1.bottomRight() );
        } else if ( options.project == Qt::Checked && options.manager == Qt::Checked ) {
            dateRect = p.boundingRect( rect_2, Qt::AlignRight|Qt::AlignTop, date );
            dateRect.setHeight( rect_2.height() );
            rect_2.setRight( dateRect.left() - 2 );
            p.drawLine( rect_2.topRight(), rect_2.bottomRight() );
        } else {
            dateRect = p.boundingRect( rect_2, Qt::AlignLeft|Qt::AlignTop, date );
            dateRect.setHeight( rect_2.height() );
            rect_2.setRight( dateRect.left() - 2 );
            if ( rect_2.left() != rect.left() ) {
                p.drawLine( rect_2.topRight(), rect_2.bottomRight() );
            }
        }
    }
    if ( options.manager == Qt::Checked ) {
        if ( options.project != Qt::Checked ) {
            managerRect = p.boundingRect( rect_1, Qt::AlignLeft|Qt::AlignTop, manager );
            managerRect.setHeight( rect_1.height() );
        } else if ( options.date != Qt::Checked && options.page != Qt::Checked ) {
            managerRect = p.boundingRect( rect_1, Qt::AlignRight|Qt::AlignTop, manager );
            managerRect.setHeight( rect_1.height() );
            rect_1.setRight( managerRect.left() - 2 );
            p.drawLine( rect_1.topRight(), rect_1.bottomRight() );
        } else {
            managerRect = p.boundingRect( rect_2, Qt::AlignLeft|Qt::AlignTop, manager );
            managerRect.setHeight( rect_2.height() );
        }
    }
    if ( options.project == Qt::Checked ) {
        projectRect = p.boundingRect( rect_1, Qt::AlignLeft|Qt::AlignTop, projectName );
        projectRect.setHeight( rect_1.height() );
    }

    if ( options.page == Qt::Checked ) {
        p.drawText( pageRect, Qt::AlignHCenter|Qt::AlignBottom, page );
    }
    if ( options.project == Qt::Checked ) {
        p.drawText( projectRect, Qt::AlignLeft|Qt::AlignBottom, projectName );
    }
    if ( options.date == Qt::Checked ) {
        p.drawText( dateRect, Qt::AlignHCenter|Qt::AlignBottom, date );
    }
    if ( options.manager == Qt::Checked ) {
        p.drawText( managerRect, Qt::AlignLeft|Qt::AlignBottom, manager );
    }
    QFont f = p.font();
    f.setPointSize( f.pointSize() * 0.5 );
    p.setFont( f );
    if ( options.page == Qt::Checked ) {
        p.drawText( pageRect, Qt::AlignTop|Qt::AlignLeft, i18n( "Page:" ) );
    }
    if ( options.project == Qt::Checked ) {
        p.drawText( projectRect, Qt::AlignTop|Qt::AlignLeft, i18n( "Project:" ) );
    }
    if ( options.date == Qt::Checked ) {
        p.drawText( dateRect, Qt::AlignTop|Qt::AlignLeft, i18n( "Date:" ) );
    }
    if ( options.manager == Qt::Checked ) {
        p.drawText( managerRect, Qt::AlignTop|Qt::AlignLeft, i18n( "Manager:" ) );
    }
    p.restore();
}

//--------------
ViewBase::ViewBase(KoPart *part, KoDocument *doc, QWidget *parent)
    : KoView(part, doc, parent),
    m_readWrite( false ),
    m_proj( 0 ),
    m_schedulemanager( 0 )
{
}

ViewBase::~ViewBase()
{
    if ( koDocument() ) {
        //HACK to avoid ~View to access koDocument()
        setDocumentDeleted();
    }
}

void ViewBase::setProject( Project *project )
{
    m_proj = project;
    emit projectChanged( project );
}

KoDocument *ViewBase::part() const
{
     return koDocument();
}

KoPageLayout ViewBase::pageLayout() const
{
    return m_pagelayout;
}

void ViewBase::setPageLayout( const KoPageLayout &layout )
{
    m_pagelayout = layout;
}

bool ViewBase::isActive() const
{
    if ( hasFocus() ) {
        return true;
    }
    foreach ( QWidget *v, findChildren<QWidget*>() ) {
        if ( v->hasFocus() ) {
            return true;
        }
    }
    return false;
}

void ViewBase::updateReadWrite( bool readwrite )
{
    m_readWrite = readwrite;
    emit readWriteChanged( readwrite );
}

void ViewBase::setGuiActive( bool active ) // virtual slot
{
    //kDebug(planDbg())<<active;
    emit guiActivated( this, active );
}

void ViewBase::slotUpdateReadWrite( bool rw )
{
    updateReadWrite( rw );
}

KoPrintJob *ViewBase::createPrintJob()
{
    KMessageBox::sorry(this, i18n("This view does not support printing."));

    return 0;
}

/*static*/
QWidget *ViewBase::createPageLayoutWidget( ViewBase *view )
{
    QWidget *widget = new QWidget();
    widget->setWindowTitle( i18nc( "@title:tab", "Page Layout" ) );

    QHBoxLayout *lay = new QHBoxLayout(widget);

    KoPageLayoutWidget *w = new KoPageLayoutWidget( widget, view->pageLayout() );
    w->showPageSpread( false );
    lay->addWidget( w, 1 );

    KoPagePreviewWidget *prev = new KoPagePreviewWidget( widget );
    prev->setPageLayout( view->pageLayout() );
    lay->addWidget( prev, 1 );

    connect (w, SIGNAL(layoutChanged(const KoPageLayout&)), prev, SLOT(setPageLayout(const KoPageLayout&)));

    return widget;
}

/*static*/
PrintingHeaderFooter *ViewBase::createHeaderFooterWidget( ViewBase *view )
{
    PrintingHeaderFooter *widget = new PrintingHeaderFooter( view->printingOptions() );
    widget->setWindowTitle( i18nc( "@title:tab", "Header and Footer" ) );
    widget->setOptions( view->printingOptions() );

    return widget;
}

void ViewBase::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug(planDbg());
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos, lst.first() );
    }
}

void ViewBase::createOptionAction()
{
    actionOptions = new KAction(koIcon("configure"), i18n("Configure View..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void ViewBase::slotOptionsFinished( int result )
{
    if ( result == QDialog::Accepted ) {
        emit optionsModified();
    }
    if ( sender() ) {
        sender()->deleteLater();
    }
}

bool ViewBase::loadContext( const KoXmlElement &context )
{
    KoXmlElement me;
    forEachElement( me, context ) {
        if ( me.tagName() == "page-layout" ) {
            m_pagelayout.format = KoPageFormat::formatFromString( me.attribute( "format" ) );
            m_pagelayout.orientation = me.attribute( "orientation" ) == "landscape" ? KoPageFormat::Landscape : KoPageFormat::Portrait;
            m_pagelayout.width = me.attribute( "width", "0.0" ).toDouble();
            m_pagelayout.height = me.attribute( "height", "0.0" ).toDouble();
            m_pagelayout.leftMargin = me.attribute( "left-margin", QString::number( MM_TO_POINT( 20.0 ) ) ).toDouble();
            m_pagelayout.rightMargin = me.attribute( "right-margin", QString::number( MM_TO_POINT( 20.0 ) ) ).toDouble();
            m_pagelayout.topMargin = me.attribute( "top-margin", QString::number( MM_TO_POINT( 20.0 ) ) ).toDouble();
            m_pagelayout.bottomMargin = me.attribute( "bottom-margin", QString::number( MM_TO_POINT( 20.0 ) ) ).toDouble();
        } else if ( me.tagName() == "printing-options" ) {
            m_printingOptions.loadXml( me );
        } else if ( me.tagName() == "dockers" ) {
            KoXmlElement e;
            forEachElement ( e, me ) {
                DockWidget *ds = findDocker( e.attribute( "id" ) );
                if ( ds ) {
                    ds->loadXml( e );
                }
            }
        }
    }
    return true;
}

void ViewBase::saveContext( QDomElement &context ) const
{
    QDomElement me = context.ownerDocument().createElement( "page-layout" );
    context.appendChild( me );
    me.setAttribute( "format", KoPageFormat::formatString( m_pagelayout.format ) );
    me.setAttribute( "orientation", m_pagelayout.orientation == KoPageFormat::Portrait ? "portrait" : "landscape" );
    me.setAttribute( "width", m_pagelayout.width );
    me.setAttribute( "height",m_pagelayout. height );
    me.setAttribute( "left-margin", m_pagelayout.leftMargin );
    me.setAttribute( "right-margin", m_pagelayout.rightMargin );
    me.setAttribute( "top-margin", m_pagelayout.topMargin );
    me.setAttribute( "bottom-margin", m_pagelayout.bottomMargin );

    m_printingOptions.saveXml( context );

    if ( ! m_dockers.isEmpty() ) {
        QDomElement e = context.ownerDocument().createElement( "dockers" );
        context.appendChild( e );
        foreach ( const DockWidget *ds, m_dockers ) {
            ds->saveXml( e );
        }
    }
}

void ViewBase::addDocker( DockWidget *ds )
{
    addAction( "view_docker_list", ds->toggleViewAction() );
    m_dockers << ds;
}

QList<DockWidget*> ViewBase::dockers() const
{
    return m_dockers;
}

DockWidget* ViewBase::findDocker( const QString &id ) const
{
    foreach ( DockWidget *ds, m_dockers ) {
        if ( ds->id == id ) {
            return ds;
        }
    }
    return 0;
}

//----------------------
TreeViewPrintingDialog::TreeViewPrintingDialog( ViewBase *view, TreeViewBase *treeview, Project *project )
    : PrintingDialog( view ),
    m_tree( treeview ),
    m_project( project ),
    m_firstRow( -1 )
{
    printer().setFromTo( documentFirstPage(), documentLastPage() );
}

int TreeViewPrintingDialog::documentLastPage() const
{
    int page = documentFirstPage();
    while ( firstRow( page ) != -1 ) {
        ++page;
    }
    if ( page > documentFirstPage() ) {
        --page;
    }
    return page;
}

int TreeViewPrintingDialog::firstRow( int page ) const
{
    kDebug(planDbg())<<page;
    int pageNumber = page - documentFirstPage();
    QHeaderView *mh = m_tree->header();
    int height = mh->height();
    int hHeight = headerRect().height();
    int fHeight = footerRect().height();
    QRect pageRect = const_cast<TreeViewPrintingDialog*>( this )->printer().pageRect();

    int gap = 8;
    int pageHeight = pageRect.height() - height;
    if ( hHeight > 0 ) {
        pageHeight -= ( hHeight + gap );
    }
    if ( fHeight > 0 ) {
        pageHeight -= ( fHeight + gap );
    }
    int rowsPrPage = pageHeight / height;

    int rows = m_tree->model()->rowCount();
    int row = -1;
    for ( int i = 0; i < rows; ++i ) {
        if ( ! m_tree->isRowHidden( i, QModelIndex() ) ) {
            row = i;
            break;
        }
    }
    if ( row != -1 ) {
        QModelIndex idx = m_tree->model()->index( row, 0, QModelIndex() );
        row = 0;
        while ( idx.isValid() ) {
            if ( row >= rowsPrPage * pageNumber ) {
                kDebug(planDbg())<<page<<pageNumber;
                break;
            }
            ++row;
            idx = m_tree->indexBelow( idx );
        }
        if ( ! idx.isValid() ) {
            row = -1;
        }
    }
    kDebug(planDbg())<<"Page"<<page<<":"<<(row==-1?"empty":"first row=")<<row<<"("<<rowsPrPage<<")";
    return row;
}

QList<QWidget*> TreeViewPrintingDialog::createOptionWidgets() const
{
    QList<QWidget*> lst;
    lst << createPageLayoutWidget();
    lst += PrintingDialog::createOptionWidgets();
    return  lst;
}

void TreeViewPrintingDialog::printPage( int page, QPainter &painter )
{
    m_firstRow = firstRow( page );

    QHeaderView *mh = m_tree->header();
    int length = mh->length();
    int height = mh->height();
    QRect hRect = headerRect();
    QRect fRect = footerRect();
    QRect pageRect = printer().pageRect();
    pageRect.moveTo( 0, 0 );
    QRect paperRect = printer().paperRect();

    QAbstractItemModel *model = m_tree->model();

    kDebug(planDbg())<<pageRect<<paperRect;

    painter.translate( pageRect.topLeft() );

    painter.setClipping( true );

    if ( m_project ) {
        paintHeaderFooter( painter, m_view->printingOptions(), page, *(m_project) );
    }

    int gap = 8;
    int pageHeight = pageRect.height() - height;
    if ( hRect.isValid() ) {
        pageHeight -= ( hRect.height() + gap );
    }
    if ( fRect.isValid() ) {
        pageHeight -= ( fRect.height() + gap );
    }
    int rowsPrPage = pageHeight / height;

    double sx = pageRect.width() > length ? 1.0 : (double)pageRect.width() / (double)length;
    double sy = 1.0;
    painter.scale( sx, sy );

    int h = 0;

    painter.translate( 0, hRect.height() + gap );
    h = hRect.height() + gap;

    painter.setPen(Qt::black);
    painter.setBrush( Qt::lightGray );
    for ( int i = 0; i < mh->count(); ++i ) {
        QString text = model->headerData( i, Qt::Horizontal ).toString();
        QVariant a = model->headerData( i, Qt::Horizontal, Qt::TextAlignmentRole );
        int align = a.isValid() ? a.toInt() : (int)(Qt::AlignLeft|Qt::AlignVCenter);
        if ( ! mh->isSectionHidden( i ) ) {
            QRect r( mh->sectionPosition( i ), 0, mh->sectionSize( i ), height );
            painter.drawRect( r );
            painter.drawText( r, align, text );
            //kDebug(planDbg())<<text<<r<<r.left() * sx<<align;
        }
        //kDebug(planDbg())<<text<<"hidden="<<h->isSectionHidden( i )<<h->sectionPosition( i );
    }
    if ( m_firstRow == -1 ) {
        kDebug(planDbg())<<"No data";
        return;
    }
    painter.setBrush( QBrush() );
    QModelIndex idx = model->index( m_firstRow, 0, QModelIndex() );
    int numRows = 0;
    //kDebug(planDbg())<<page<<rowsPrPage;
    while ( idx.isValid() && numRows < rowsPrPage ) {
        painter.translate( 0, height );
        h += height;
        for ( int i = 0; i < mh->count(); ++i ) {
            if ( mh->isSectionHidden( i ) ) {
                continue;
            }
            QModelIndex index = model->index( idx.row(), i, idx.parent() );
            QString text = model->data( index ).toString();
            QVariant a = model->data( index, Qt::TextAlignmentRole );
            int align = a.isValid() ? a.toInt() : (int)(Qt::AlignLeft|Qt::AlignVCenter);
            QRect r( mh->sectionPosition( i ),  0, mh->sectionSize( i ), height );
            drawBottomRect( painter, r );
            painter.drawText( r, align, text );
        }
        ++numRows;
        idx = m_tree->indexBelow( idx );
    }
}

/**
 * TreeViewBase is a QTreeView adapted for operation by keyboard only and as components in DoubleTreeViewBase.
 * Note that keyboard navigation and selection behavior may not be fully compliant with QTreeView.
 * If you use other settings than  QAbstractItemView::ExtendedSelection and QAbstractItemView::SelectRows,
 * you should have a look at the implementation keyPressEvent() and updateSelection().
 */

TreeViewBase::TreeViewBase( QWidget *parent )
    : QTreeView( parent ),
    m_arrowKeyNavigation( true ),
    m_acceptDropsOnView( false ),
    m_readWrite( false )

{
    setDefaultDropAction( Qt::MoveAction );
    setItemDelegate( new ItemDelegate( this ) );
    setAlternatingRowColors ( true );

    header()->setContextMenuPolicy( Qt::CustomContextMenu );

    connect( header(), SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

void TreeViewBase::dropEvent( QDropEvent *e )
{
    kDebug(planDbg());
    QTreeView::dropEvent( e );
}

KoPrintJob * TreeViewBase::createPrintJob( ViewBase *parent )
{
    TreeViewPrintingDialog *dia = new TreeViewPrintingDialog( parent, this, parent->project() );
    dia->printer().setCreator( QString( "Plan %1" ).arg( CALLIGRA_VERSION_STRING ) );
//    dia->printer().setFullPage(true); // ignore printer margins
    return dia;
}

void TreeViewBase::setReadWrite( bool rw )
{
    m_readWrite = rw;
    if ( model() ) {
        model()->setData( QModelIndex(), rw, Role::ReadWrite );
    }
}

void TreeViewBase::createItemDelegates( ItemModelBase *model )
{
    for ( int c = 0; c < model->columnCount(); ++c ) {
        QAbstractItemDelegate *delegate = model->createDelegate( c, this );
        if ( delegate ) {
            setItemDelegateForColumn( c, delegate );
        }
    }
}

void TreeViewBase::slotHeaderContextMenuRequested( const QPoint& pos )
{
    //kDebug(planDbg());
    emit headerContextMenuRequested( header()->mapToGlobal( pos ) );
}

void TreeViewBase::setColumnsHidden( const QList<int> &lst )
{
    //kDebug(planDbg())<<m_hideList;
    int prev = -1;
    QList<int> xlst;
    foreach ( int c, lst ) {
        if ( c == -1 ) {
            // hide rest
            for ( int i = prev+1; i < model()->columnCount(); ++i ) {
                if ( ! lst.contains( i ) ) {
                    xlst << i;
                }
            }
            break;
        }
        xlst << c;
        prev = c;
    }
    for ( int c = 0; c < model()->columnCount(); ++c ) {
        setColumnHidden( c, xlst.contains( c ) );
    }
}

QModelIndex TreeViewBase::firstColumn( int row, const QModelIndex &parent )
{
    int s;
    for ( s = 0; s < header()->count(); ++s ) {
        if ( ! header()->isSectionHidden( header()->logicalIndex( s ) ) ) {
            break;
        }
    }
    if ( s == -1 ) {
        return QModelIndex();
    }
    return model()->index( row, header()->logicalIndex( s ), parent );
}

QModelIndex TreeViewBase::lastColumn( int row, const QModelIndex &parent )
{
    int s;
    for ( s = header()->count() - 1; s >= 0; --s ) {
        if ( ! header()->isSectionHidden( header()->logicalIndex( s ) ) ) {
            break;
        }
    }
    if ( s == -1 ) {
        return QModelIndex();
    }
    return model()->index( row, header()->logicalIndex( s ), parent );
}

QModelIndex TreeViewBase::nextColumn( const QModelIndex &curr )
{
    return moveCursor( curr, QAbstractItemView::MoveRight );
}

QModelIndex TreeViewBase::previousColumn( const QModelIndex &curr )
{
    return moveCursor( curr, QAbstractItemView::MoveLeft );
}

QModelIndex TreeViewBase::firstEditable( int row, const QModelIndex &parent )
{
    QModelIndex index = firstColumn( row, parent );
    if ( model()->flags( index ) & Qt::ItemIsEditable ) {
        return index;
    }
    return moveToEditable( index, QAbstractItemView::MoveRight );
}

QModelIndex TreeViewBase::lastEditable( int row, const QModelIndex &parent )
{
    QModelIndex index = lastColumn( row, parent );
    if ( model()->flags( index ) & Qt::ItemIsEditable ) {
        return index;
    }
    return moveToEditable( index, QAbstractItemView::MoveLeft );
}

// Reimplemented to fix qt bug 160083: Doesn't scroll horisontally.
void TreeViewBase::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    //kDebug(planDbg())<<objectName()<<index<<hint;
    if ( ! hasFocus() ) {
        return;
    }
    QTreeView::scrollTo( index, hint ); // scrolls vertically
    if ( ! index.isValid() ) {
        return;
    }
    // horizontal
    int viewportWidth = viewport()->width();
    int horizontalOffset = header()->offset();
    int horizontalPosition = header()->sectionPosition(index.column());
    int cellWidth = header()->sectionSize(index.column());

    if (hint == PositionAtCenter) {
        horizontalScrollBar()->setValue(horizontalPosition - ((viewportWidth - cellWidth) / 2));
    } else {
        if (horizontalPosition - horizontalOffset < 0 || cellWidth > viewportWidth)
            horizontalScrollBar()->setValue(horizontalPosition);
        else if (horizontalPosition - horizontalOffset + cellWidth > viewportWidth)
            horizontalScrollBar()->setValue(horizontalPosition - viewportWidth + cellWidth);
    }
}

void TreeViewBase::focusInEvent(QFocusEvent *event)
{
    //kDebug(planDbg())<<event->reason();
    QAbstractScrollArea::focusInEvent(event); //NOTE: not QTreeView
    if ( event->reason() == Qt::MouseFocusReason ) {
        return;
    }
    QModelIndex curr = currentIndex();
    if ( ! curr.isValid() || ! isIndexHidden( curr ) ) {
        return;
    }
    QModelIndex idx = curr;
    for ( int s = 0; s < header()->count(); ++s) {
        idx = model()->index( curr.row(), header()->logicalIndex( s ), curr.parent() );
        if ( ! isIndexHidden( idx ) ) {
            selectionModel()->setCurrentIndex(idx, QItemSelectionModel::NoUpdate);
            scrollTo( idx );
            break;
        }
    }
}

/*!
    \reimp
 */
void TreeViewBase::keyPressEvent(QKeyEvent *event)
{
    //kDebug(planDbg())<<objectName()<<event->key()<<","<<m_arrowKeyNavigation;
    if ( !m_arrowKeyNavigation ) {
        QTreeView::keyPressEvent( event );
        return;
    }
    QModelIndex current = currentIndex();
    if ( current.isValid() ) {
        switch (event->key()) {
            case Qt::Key_Right: {
                QModelIndex nxt = moveCursor( MoveRight, Qt::NoModifier );
                if ( nxt.isValid() ) {
                    selectionModel()->setCurrentIndex( nxt, QItemSelectionModel::NoUpdate );
                } else {
                    emit moveAfterLastColumn( current );
                }
                event->accept();
                return;
                break;
            }
            case Qt::Key_Left: {
                QModelIndex prv = moveCursor( MoveLeft, Qt::NoModifier );
                if ( prv.isValid() ) {
                    selectionModel()->setCurrentIndex( prv, QItemSelectionModel::NoUpdate );
                } else {
                    emit moveBeforeFirstColumn( current );
                }
                event->accept();
                return;
                break;
            }
            case Qt::Key_Down: {
                QModelIndex i = moveCursor( MoveDown, Qt::NoModifier );
                updateSelection( current, i, event );
                event->accept();
                return;
                break;
            }
            case Qt::Key_Up: {
                QModelIndex i = moveCursor( MoveUp, Qt::NoModifier );
                updateSelection( current, i, event );
                event->accept();
                return;
                break;
            }
            default: break;
        }
    }
    QTreeView::keyPressEvent(event);
}

void TreeViewBase::updateSelection( const QModelIndex &oldidx, const QModelIndex &newidx, QKeyEvent *event )
{
    if ( newidx == oldidx || ! newidx.isValid() ) {
        return;
    }
    if ( !hasFocus() && QApplication::focusWidget() == indexWidget(oldidx) ) {
        setFocus();
    }
    QItemSelectionModel::SelectionFlags command;
    // NoUpdate on Key movement and Ctrl
    Qt::KeyboardModifiers modifiers = static_cast<const QKeyEvent*>(event)->modifiers();
    switch (static_cast<const QKeyEvent*>(event)->key()) {
    case Qt::Key_Backtab:
        modifiers = modifiers & ~Qt::ShiftModifier; // special case for backtab
    case Qt::Key_Down:
    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Right:
        if (modifiers & Qt::ControlModifier)
            command = QItemSelectionModel::NoUpdate;
        else if (modifiers & Qt::ShiftModifier)
            command = QItemSelectionModel::Select | selectionBehaviorFlags();
        else
            command = QItemSelectionModel::ClearAndSelect | selectionBehaviorFlags();
        break;
    default:
        break;
    }
    selectionModel()->setCurrentIndex( newidx, command );
}

void TreeViewBase::mousePressEvent(QMouseEvent *event)
{
    // If  the mouse is pressed outside any item, the current item should be/remain selected
    QPoint pos = event->pos();
    QModelIndex index = indexAt(pos);
    kDebug(planDbg())<<index<<event->pos();
    if ( ! index.isValid() ) {
        index = selectionModel()->currentIndex();
        if ( index.isValid() && ! selectionModel()->isSelected( index ) ) {
            pos = visualRect( index ).center();
            QMouseEvent e( event->type(), pos, mapToGlobal( pos ), event->button(), event->buttons(), event->modifiers() );
            QTreeView::mousePressEvent( &e );
            event->setAccepted( e.isAccepted() );
            kDebug(planDbg())<<index<<e.pos();
        }
        return;
    }
    QTreeView::mousePressEvent( event );
}

void TreeViewBase::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    //kDebug(planDbg())<<editor<<hint;
    ItemDelegate *delegate = ::qobject_cast<ItemDelegate*>( sender() );
    if ( delegate == 0 ) {
        kWarning()<<"Not a KPlato::ItemDelegate, try standard treatment"<<editor<<hint;
        return QTreeView::closeEditor( editor, hint );
    }
    // Hacky, if only hint was an int!
    Delegate::EndEditHint endHint = delegate->endEditHint();
    // Close editor, do nothing else
    QTreeView::closeEditor( editor, QAbstractItemDelegate::NoHint );


    QModelIndex index;
    switch ( endHint ) {
        case Delegate::EditLeftItem:
            index = moveToEditable( currentIndex(), MoveLeft );
            break;
        case Delegate::EditRightItem:
            index = moveToEditable( currentIndex(), MoveRight );
            break;
        case Delegate::EditDownItem:
            index = moveToEditable( currentIndex(), MoveDown );
            break;
        case Delegate::EditUpItem:
            index = moveToEditable( currentIndex(), MoveUp );
            break;
        default:
            //kDebug(planDbg())<<"Standard treatment"<<editor<<hint;
            return QTreeView::closeEditor( editor, hint ); // standard treatment
    }
    if (index.isValid()) {
        QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect | selectionBehaviorFlags();
        //kDebug(planDbg())<<flags;
        QPersistentModelIndex persistent(index);
        selectionModel()->setCurrentIndex(persistent, flags);
        // currentChanged signal would have already started editing
        if (!(editTriggers() & QAbstractItemView::CurrentChanged)) {
            edit(persistent);
        }
    }
}

QModelIndex TreeViewBase::moveToEditable( const QModelIndex &index, CursorAction cursorAction )
{
    QModelIndex ix = index;
    do {
        ix = moveCursor( ix, cursorAction );
    } while ( ix.isValid() &&  ! ( model()->flags( ix ) & Qt::ItemIsEditable ) );
    //kDebug(planDbg())<<ix;
    if ( ! ix.isValid() ) {
        switch ( cursorAction ) {
            case MovePrevious:
            case MoveLeft: emit editBeforeFirstColumn( index ); break;
            case MoveNext:
            case MoveRight: emit editAfterLastColumn( index ); break;
            default: break;
        }
    }
    return ix;
}


/*
    Reimplemented from QTreeView to make tab/backtab in editor work reasonably well.
    Move the cursor in the way described by \a cursorAction, *not* using the
    information provided by the button \a modifiers.
 */

QModelIndex TreeViewBase::moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers )
{
    QModelIndex current = currentIndex();
    //kDebug(planDbg())<<cursorAction<<current;
    if (!current.isValid()) {
        return QTreeView::moveCursor( cursorAction, modifiers );
    }
    return moveCursor( current, cursorAction, modifiers );
}

QModelIndex TreeViewBase::moveCursor( const QModelIndex &index, CursorAction cursorAction, Qt::KeyboardModifiers modifiers )
{
    executeDelayedItemsLayout();
    QModelIndex current = index;
    int col = current.column();
    QModelIndex ix;
    switch (cursorAction) {
        case MoveDown: {
            // TODO: span

            // Fetch the index below current.
            // This should be the next non-hidden row, same column as current,
            // that has a column in current.column()
            ix = indexBelow( current );
            while ( ix.isValid() && col >= model()->columnCount(ix.parent()) ) {
                //kDebug(planDbg())<<col<<model()->columnCount(ix.parent())<<ix;
                ix = indexBelow( ix );
            }
            if ( ix.isValid() ) {
                ix = model()->index( ix.row(), col, ix.parent() );
            } // else Here we could go to the top
            return ix;
        }
        case MoveUp: {
            // TODO: span

            // Fetch the index above current.
            // This should be the previous non-hidden row, same column as current,
            // that has a column in current.column()
            ix = indexAbove( current );
            while ( ix.isValid() && col >= model()->columnCount(ix.parent()) ) {
                ix = indexAbove( ix );
            }
            if ( ix.isValid() ) {
                ix = model()->index( ix.row(), col, ix.parent() );
            } // else Here we could go to the bottom
            return ix;
        }
        case MovePrevious:
        case MoveLeft: {
            for ( int s = header()->visualIndex( col ) - 1; s >= 0; --s ) {
                if ( ! header()->isSectionHidden( header()->logicalIndex( s ) ) ) {
                    ix = model()->index( current.row(), header()->logicalIndex( s ), current.parent() );
                    break;
                }
            }
            return ix;
        }
        case MoveNext:
        case MoveRight: {
            for ( int s = header()->visualIndex( col ) + 1; s < header()->count(); ++s ) {
                if ( ! header()->isSectionHidden( header()->logicalIndex( s ) ) ) {
                    ix = model()->index( current.row(), header()->logicalIndex( s ), current.parent() );
                    break;
                }
            }
            return ix;
        }
        case MovePageUp:
        case MovePageDown: {
            ix = QTreeView::moveCursor( cursorAction, modifiers );
            // Now we are at the correct row, so move to correct column
            if ( ix.isValid() ) {
                ix = model()->index( ix.row(), col, ix.parent() );
            } // else Here we could go to the bottom
            return ix;
        }
        case MoveHome: {
            if ( ( modifiers & Qt::ControlModifier ) == 0 ) {
                ix = QTreeView::moveCursor( cursorAction, modifiers ); // move to first row
            } else { //stay at this row
                ix = current;
            }
            for ( int s = 0; s < header()->count(); ++s ) {
                int logicalIndex = header()->logicalIndex( s );
                if ( ! isColumnHidden( logicalIndex ) ) {
                    ix = model()->index( ix.row(), header()->logicalIndex( s ), ix.parent() );
                    break;
                }
            }
            return ix;
        }
        case MoveEnd: {
            if ( ( modifiers & Qt::ControlModifier ) == 0 ) {
                ix = QTreeView::moveCursor( cursorAction, modifiers ); // move to last row
            } else { //stay at this row
                ix = current;
            }
            for ( int s = header()->count() - 1; s >= 0; --s ) {
                int logicalIndex = header()->logicalIndex( s );
                if ( ! isColumnHidden( logicalIndex ) ) {
                    ix = model()->index( ix.row(), logicalIndex, ix.parent() );
                    break;
                }
            }
            return ix;
        }
        default: break;
    }
    return ix;
}

void TreeViewBase::contextMenuEvent ( QContextMenuEvent *event )
{
    //kDebug(planDbg());
    emit contextMenuRequested( indexAt(event->pos()), event->globalPos() );
}

void TreeViewBase::slotCurrentChanged( const QModelIndex &current, const QModelIndex & )
{
    if ( current.isValid() ) {
        scrollTo( current );
    }
}

void TreeViewBase::setModel( QAbstractItemModel *model )
{
    if ( selectionModel() ) {
        disconnect( selectionModel(), SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotCurrentChanged( const QModelIndex&, const QModelIndex& ) ) );
    }
    QTreeView::setModel( model );
    if ( selectionModel() ) {
        connect( selectionModel(), SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotCurrentChanged( const QModelIndex&, const QModelIndex& ) ) );
    }
    setReadWrite( m_readWrite );
}

void TreeViewBase::setSelectionModel( QItemSelectionModel *model )
{
    if ( selectionModel() ) {
        disconnect( selectionModel(), SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotCurrentChanged( const QModelIndex&, const QModelIndex& ) ) );
    }
    QTreeView::setSelectionModel( model );
    if ( selectionModel() ) {
        connect( selectionModel(), SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotCurrentChanged( const QModelIndex&, const QModelIndex& ) ) );
    }
}

void TreeViewBase::setStretchLastSection( bool mode )
{
    header()->setStretchLastSection( mode );
}

void TreeViewBase::mapToSection( int col, int section )
{
    header()->moveSection( header()->visualIndex( col ), section );
}

int TreeViewBase::section( int col ) const
{
    return header()->visualIndex( col );
}

void TreeViewBase::dragMoveEvent(QDragMoveEvent *event)
{
    //kDebug(planDbg());
    if (dragDropMode() == InternalMove
        && (event->source() != this || !(event->possibleActions() & Qt::MoveAction))) {
        //kDebug(planDbg())<<"Internal:"<<event->isAccepted();
        return;
    }
    QTreeView::dragMoveEvent( event );
    if ( dropIndicatorPosition() == QAbstractItemView::OnViewport ) {
        if ( ! m_acceptDropsOnView ) {
            event->ignore();
        }
        kDebug(planDbg())<<"On viewport:"<<event->isAccepted();
    } else {
        QModelIndex index = indexAt( event->pos() );
        if ( index.isValid() ) {
            emit dropAllowed( index, dropIndicatorPosition(), event );
        } else {
            event->ignore();
            kDebug(planDbg())<<"Invalid index:"<<event->isAccepted();
        }
    }
    if ( event->isAccepted() ) {
        if ( viewport()->cursor().shape() == Qt::ForbiddenCursor ) {
            viewport()->unsetCursor();
        }
    } else if ( viewport()->cursor().shape() != Qt::ForbiddenCursor ) {
        viewport()->setCursor( Qt::ForbiddenCursor );
    }
    kDebug(planDbg())<<event->isAccepted()<<viewport()->cursor().shape();
}

QModelIndex TreeViewBase::firstVisibleIndex( const QModelIndex &idx ) const
{
    int count = model()->columnCount();
    for ( int c = 0; c < count; ++c ) {
        if ( ! isColumnHidden( c ) ) {
            return model()->index( idx.row(), c, model()->parent( idx ) );
        }
    }
    return QModelIndex();
}


bool TreeViewBase::loadContext( const QMetaEnum &map, const KoXmlElement &element )
{
    //kDebug(planDbg())<<objectName();
    header()->setStretchLastSection( (bool)( element.attribute( "stretch-last-column", "1" ).toInt() ) );
    KoXmlElement e = element.namedItem( "columns" ).toElement();
    if ( ! e.isNull() ) {
        if ( ! map.isValid() ) {
            // try numbers
            kDebug(planDbg())<<"invalid map";
            for ( int i = model()->columnCount() - 1; i >= 0; --i ) {
                QString s = e.attribute( QString( "column-%1" ).arg( i ), "" );
                if ( s == "hidden" ) {
                    hideColumn( i );
                } else if ( s == "shown" ) {
                    showColumn( i );
                } else kDebug(planDbg())<<objectName()<<"Unknown column:"<<s;
            }
        } else {
            for ( int i = model()->columnCount() - 1; i >= 0; --i ) {
                QString n = map.key( i );
                //kDebug(planDbg())<<i<<"="<<n;
                if ( ! n.isEmpty() ) {
                    QString s = e.attribute( n, "" );
                    if ( s == "hidden" ) {
                        hideColumn( i );
                    } else if ( s == "shown" ) {
                        showColumn( i );
                    } else kDebug(planDbg())<<objectName()<<"Unknown column:"<<s;
                } else kDebug(planDbg())<<"Column not in enum:"<<i;
            }
        }
    }
    e = element.namedItem( "sections" ).toElement();
    if ( ! e.isNull() ) {
        QHeaderView *h = header();
        QString s( "section-%1" );
        if ( ! map.isValid() ) {
            // try numbers
            for ( int i = 0; i < h->count(); ++i ) {
                if ( e.hasAttribute( s.arg( i ) ) ) {
                    int index = e.attribute( s.arg( i ), "-1" ).toInt();
                    if ( index >= 0 && index < h->count() ) {
                        header()->moveSection( h->visualIndex( index ), i );
                    }
                }
            }
        } else {
            QMap<int, int > m; // QMap<destination, column>
            for ( int i = 0; i < h->count(); ++i ) {
                QString n = e.attribute( s.arg( i ) );
                if ( n.isEmpty() ) {
                    continue;
                }
                int col = map.keyToValue( n.toUtf8() );
                if ( col >= 0 && col < h->count() ) {
                    m.insert( i, col );
                }
            }
            for ( QMap<int, int>::const_iterator it = m.constBegin(); it != m.constEnd(); ++it ) {
                QString n = e.attribute( s.arg( it.key() ) );
                int current = h->visualIndex( it.value() );
                header()->moveSection( current, it.key() );
            }
        }
    }
    return true;
}

void TreeViewBase::saveContext( const QMetaEnum &map, QDomElement &element ) const
{
    //kDebug(planDbg())<<objectName();
    element.setAttribute( "stretch-last-column", header()->stretchLastSection() );
    QDomElement e = element.ownerDocument().createElement( "columns" );
    element.appendChild( e );
    for ( int i = 0; i < model()->columnCount(); ++i ) {
        bool h = isColumnHidden( i );
        if ( ! map.isValid() ) {
            kDebug(planDbg())<<"invalid map";
            e.setAttribute( QString( "column-%1" ).arg( i ), h ? "hidden" : "shown" );
        } else {
            QString n = map.key( i );
            //kDebug(planDbg())<<i<<"="<<n;
            if ( ! n.isEmpty() ) {
                e.setAttribute( n, h ? "hidden" : "shown" );
            }
        }
    }
    e = element.ownerDocument().createElement( "sections" );
    element.appendChild( e );
    QHeaderView *h = header();
    for ( int i = 0; i < h->count(); ++i ) {
        if ( ! isColumnHidden( h->logicalIndex( i ) ) ) {
            if ( ! map.isValid() ) {
                e.setAttribute( QString( "section-%1" ).arg( i ), h->logicalIndex( i ) );
            } else {
                QString n = map.key( h->logicalIndex( i ) );
                if ( ! n.isEmpty() ) {
                    e.setAttribute( QString( "section-%1" ).arg( i ), n );
                }
            }
        }
    }
}

ItemModelBase *TreeViewBase::itemModel() const
{
    QAbstractItemModel *m = model();
    QAbstractProxyModel *p = qobject_cast<QAbstractProxyModel*>( m );
    while ( p ) {
        m = p->sourceModel();
        p = qobject_cast<QAbstractProxyModel*>( m );
    }
    return qobject_cast<ItemModelBase*>( m );
}

//----------------------
DoubleTreeViewPrintingDialog::DoubleTreeViewPrintingDialog( ViewBase *view, DoubleTreeViewBase *treeview, Project *project )
    : PrintingDialog( view ),
    m_tree( treeview ),
    m_project( project ),
    m_firstRow( -1 )
{
    printer().setFromTo( documentFirstPage(), documentLastPage() );
}

int DoubleTreeViewPrintingDialog::documentLastPage() const
{
    kDebug(planDbg())<<KoPageFormat::formatString( m_view->pageLayout().format );
    int page = documentFirstPage();
    while ( firstRow( page ) != -1 ) {
        ++page;
    }
    if ( page > documentFirstPage() ) {
        --page;
    }
    return page;
}

int DoubleTreeViewPrintingDialog::firstRow( int page ) const
{
    kDebug(planDbg())<<page;
    int pageNumber = page - documentFirstPage();
    QHeaderView *mh = m_tree->masterView()->header();
    QHeaderView *sh = m_tree->slaveView()->header();
    int height = mh->height() > sh->height() ? mh->height() : sh->height();
    int hHeight = headerRect().height();
    int fHeight = footerRect().height();
    QRect pageRect = const_cast<DoubleTreeViewPrintingDialog*>( this )->printer().pageRect();

    int gap = 8;
    int pageHeight = pageRect.height() - height;
    if ( hHeight > 0 ) {
        pageHeight -= ( hHeight + gap );
    }
    if ( fHeight > 0 ) {
        pageHeight -= ( fHeight + gap );
    }
    int rowsPrPage = pageHeight / height;

    kDebug(planDbg())<<"rowsPrPage"<<rowsPrPage;
    Q_ASSERT( rowsPrPage > 0 );

    int rows = m_tree->model()->rowCount();
    int row = -1;
    for ( int i = 0; i < rows; ++i ) {
        if ( ! m_tree->masterView()->isRowHidden( i, QModelIndex() ) ) {
            row = i;
            break;
        }
    }
    if ( row != -1 ) {
        QModelIndex idx = m_tree->model()->index( row, 0, QModelIndex() );
        row = 0;
        while ( idx.isValid() ) {
            if ( row >= rowsPrPage * pageNumber ) {
                kDebug(planDbg())<<page<<pageNumber;
                break;
            }
            ++row;
            idx = m_tree->masterView()->indexBelow( idx );
        }
        if ( ! idx.isValid() ) {
            row = -1;
        }
    }
    kDebug(planDbg())<<"Page"<<page<<":"<<(row==-1?"empty":"first row=")<<row<<"("<<rowsPrPage<<")";
    return row;
}

QList<QWidget*> DoubleTreeViewPrintingDialog::createOptionWidgets() const
{
    QList<QWidget*> lst;
    lst << createPageLayoutWidget();
    lst += PrintingDialog::createOptionWidgets();
    return  lst;
}

void DoubleTreeViewPrintingDialog::printPage( int page, QPainter &painter )
{
    kDebug(planDbg())<<page<<"paper size:"<<printer().paperSize()<<"---------------------------";
    setPrinterPageLayout( m_view->pageLayout() );
    qreal t, l, b, r; printer().getPageMargins( &l, &t, &r, &b, QPrinter::Point );
    kDebug(planDbg())<<page<<"paper size:"<<printer().paperSize()<<printer().pageRect()<<l<<t<<r<<b;
    painter.save();

    m_firstRow = firstRow( page );

    QHeaderView *mh = m_tree->masterView()->header();
    QHeaderView *sh = m_tree->slaveView()->header();
    int length = mh->length() + sh->length();
    int height = mh->height() > sh->height() ? mh->height() : sh->height();
    QRect hRect = headerRect();
    QRect fRect = footerRect();
    QRect pageRect = printer().pageRect();
    pageRect.moveTo( 0, 0 );
    QRect paperRect = printer().paperRect();

    QAbstractItemModel *model = m_tree->model();
    Q_ASSERT( model != 0 );
    kDebug(planDbg())<<pageRect<<paperRect;

    painter.translate( pageRect.topLeft() );

    painter.setClipping( true );

    if ( m_project ) {
        paintHeaderFooter( painter, printingOptions(), page, *(m_project) );
    }
    int gap = 8;
    int pageHeight = pageRect.height() - height;
    if ( hRect.isValid() ) {
        pageHeight -= ( hRect.height() + gap );
    }
    if ( fRect.isValid() ) {
        pageHeight -= ( fRect.height() + gap );
    }
    int rowsPrPage = pageHeight / height;

    double sx = pageRect.width() > length ? 1.0 : (double)pageRect.width() / (double)length;
    double sy = 1.0;
    painter.scale( sx, sy );

    int h = 0;

    painter.translate( 0, hRect.height() + gap );
    h = hRect.height() + gap;

    painter.setPen(Qt::black);
    painter.setBrush( Qt::lightGray );
    for ( int i = 0; i < mh->count(); ++i ) {
        QString text = model->headerData( i, Qt::Horizontal ).toString();
        QVariant a = model->headerData( i, Qt::Horizontal, Qt::TextAlignmentRole );
        int align = a.isValid() ? a.toInt() : (int)(Qt::AlignLeft|Qt::AlignVCenter);
        if ( ! mh->isSectionHidden( i ) ) {
            QRect r( mh->sectionPosition( i ), 0, mh->sectionSize( i ), height );
            painter.drawRect( r );
            painter.drawText( r, align, text );
        }
        if ( ! sh->isSectionHidden( i ) ) {
            QRect r( sh->sectionPosition( i ) + mh->length(), 0, sh->sectionSize( i ), height );
            painter.drawRect( r );
            painter.drawText( r, align, text );
        }
        //kDebug(planDbg())<<text<<"hidden="<<h->isSectionHidden( i )<<h->sectionPosition( i );
    }
    if ( m_firstRow == -1 || model->rowCount() == 0 ) {
        kDebug(planDbg())<<"No data";
        painter.restore();
        return;
    }
    painter.setBrush( QBrush() );

    QModelIndex idx = model->index( 0, 0 );
    for ( int r = 0; r < m_firstRow && idx.isValid(); ++r ) {
        idx = m_tree->masterView()->indexBelow( idx );
    }
    int numRows = 0;
    //kDebug(planDbg())<<page<<rowsPrPage;
    while ( idx.isValid() && numRows < rowsPrPage ) {
        kDebug(planDbg())<<"print:"<<idx;
        painter.translate( 0, height );
        h += height;
        for ( int i = 0; i < mh->count(); ++i ) {
            if ( mh->isSectionHidden( i ) &&  sh->isSectionHidden( i ) ) {
                continue;
            }
            QModelIndex index = model->index( idx.row(), i, idx.parent() );
            QString text = model->data( index ).toString();
            QVariant a = model->data( index, Qt::TextAlignmentRole );
            int align = a.isValid() ? a.toInt() : (int)(Qt::AlignLeft|Qt::AlignVCenter);
            if ( ! mh->isSectionHidden( i ) ) {
                QRect r( mh->sectionPosition( i ),  0, mh->sectionSize( i ), height );
                drawBottomRect( painter, r );
                painter.drawText( r, align, text );
            }
            if ( ! sh->isSectionHidden( i ) ) {
                QRect r( sh->sectionPosition( i ) + mh->length(), 0, sh->sectionSize( i ), height );
                drawBottomRect( painter, r );
                painter.drawText( r, align, text );
            }
        }
        ++numRows;
        idx = m_tree->masterView()->indexBelow( idx );
    }
    painter.restore();
}


/**
 * DoubleTreeViewBase is a QSplitter contaning two treeviews.
 * This makes it possible to keep columns visible in one view when scrolling the other view horizontally.
 */

DoubleTreeViewBase::DoubleTreeViewBase( bool /*mode*/, QWidget *parent )
    : QSplitter( parent ),
    m_rightview( 0 ),
    m_selectionmodel( 0 ),
    m_readWrite( false ),
    m_mode( false )
{
    init();
}

DoubleTreeViewBase::DoubleTreeViewBase( QWidget *parent )
    : QSplitter( parent ),
    m_rightview( 0 ),
    m_selectionmodel( 0 ),
    m_mode( false )
{
    init();
}

DoubleTreeViewBase::~DoubleTreeViewBase()
{
}

KoPrintJob *DoubleTreeViewBase::createPrintJob( ViewBase *parent )
{
    DoubleTreeViewPrintingDialog *dia = new DoubleTreeViewPrintingDialog( parent, this, parent->project() );
    dia->printer().setCreator( QString( "Plan %1" ).arg( CALLIGRA_VERSION_STRING ) );
//    dia->printer().setFullPage(true); // ignore printer margins
    return dia;
}

void DoubleTreeViewBase::expandAll()
{
    m_leftview->expandAll();
}

void DoubleTreeViewBase::setParentsExpanded( const QModelIndex &idx, bool expanded )
{
    //kDebug(planDbg())<<idx<<m_leftview->isExpanded( idx )<<m_rightview->isExpanded( idx );
    QModelIndex p = model()->parent( idx );
    QList<QModelIndex> lst;
    while ( p.isValid() ) {
        lst << p;
        p = model()->parent( p );
    }
    while ( ! lst.isEmpty() ) {
        p = lst.takeLast();
        m_leftview->setExpanded( p, expanded );
        m_rightview->setExpanded( m_rightview->firstVisibleIndex( p ), expanded ); //HACK: qt can't handle that column 0 is hidden!
        //kDebug(planDbg())<<p<<m_leftview->isExpanded( p )<<m_rightview->isExpanded( p );
    }
}

void DoubleTreeViewBase::init()
{
    setOrientation( Qt::Horizontal );
    setHandleWidth( 3 );
    m_leftview = new TreeViewBase();
    m_leftview->setObjectName("Left view");
    addWidget( m_leftview );
    setStretchFactor( 0, 1 );
    m_rightview = new TreeViewBase();
    m_rightview->setObjectName("Right view");
    addWidget( m_rightview );
    setStretchFactor( 1, 1 );


    connect( m_leftview, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ) );
    connect( m_leftview, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotLeftHeaderContextMenuRequested( const QPoint& ) ) );

    connect( m_rightview, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ) );
    connect( m_rightview, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotRightHeaderContextMenuRequested( const QPoint& ) ) );

    connect( m_leftview->verticalScrollBar(), SIGNAL( valueChanged( int ) ), m_rightview->verticalScrollBar(), SLOT( setValue( int ) ) );

    connect( m_rightview->verticalScrollBar(), SIGNAL( valueChanged( int ) ), m_leftview->verticalScrollBar(), SLOT( setValue( int ) ) );

    connect( m_leftview, SIGNAL( moveAfterLastColumn( const QModelIndex & ) ), this, SLOT( slotToRightView( const QModelIndex & ) ) );
    connect( m_rightview, SIGNAL( moveBeforeFirstColumn( const QModelIndex & ) ), this, SLOT( slotToLeftView( const QModelIndex & ) ) );

    connect( m_leftview, SIGNAL( editAfterLastColumn( const QModelIndex & ) ), this, SLOT( slotEditToRightView( const QModelIndex & ) ) );
    connect( m_rightview, SIGNAL( editBeforeFirstColumn( const QModelIndex & ) ), this, SLOT( slotEditToLeftView( const QModelIndex & ) ) );

    connect( m_leftview, SIGNAL( expanded( const QModelIndex & ) ), m_rightview, SLOT( expand( const QModelIndex & ) ) );
    connect( m_leftview, SIGNAL( collapsed( const QModelIndex & ) ), m_rightview, SLOT( collapse( const QModelIndex & ) ) );

    connect( m_rightview, SIGNAL( expanded( const QModelIndex & ) ), m_leftview, SLOT( expand( const QModelIndex & ) ) );
    connect( m_rightview, SIGNAL( collapsed( const QModelIndex & ) ), m_leftview, SLOT( collapse( const QModelIndex & ) ) );

    connect( m_leftview, SIGNAL( dropAllowed( const QModelIndex&, int, QDragMoveEvent* ) ), this, SIGNAL( dropAllowed( const QModelIndex&, int, QDragMoveEvent* ) ) );
    connect( m_rightview, SIGNAL( dropAllowed( const QModelIndex&, int, QDragMoveEvent* ) ), this, SIGNAL( dropAllowed( const QModelIndex&, int, QDragMoveEvent* ) ) );

    m_actionSplitView = new KAction(koIcon("view-split-left-right"), QString(), this);
    setViewSplitMode( true );

    connect( m_leftview->header(), SIGNAL( sortIndicatorChanged( int, Qt::SortOrder ) ), SLOT( slotLeftSortIndicatorChanged( int, Qt::SortOrder ) ) );

    connect( m_rightview->header(), SIGNAL( sortIndicatorChanged( int, Qt::SortOrder ) ), SLOT( slotRightSortIndicatorChanged( int, Qt::SortOrder ) ) );
}

void DoubleTreeViewBase::slotLeftSortIndicatorChanged( int logicalIndex, Qt::SortOrder /*order*/ )
{
    QSortFilterProxyModel *sf = qobject_cast<QSortFilterProxyModel*>( model() );
    if ( sf ) {
        ItemModelBase *m = m_rightview->itemModel();
        if ( m ) {
            sf->setSortRole( m->sortRole( logicalIndex ) );
        }
    }
    m_leftview->header()->setSortIndicatorShown( true );
    // sorting controlled by left treeview, turn right off
    m_rightview->header()->setSortIndicatorShown( false );
}

void DoubleTreeViewBase::slotRightSortIndicatorChanged( int logicalIndex, Qt::SortOrder /*order*/ )
{
    QSortFilterProxyModel *sf = qobject_cast<QSortFilterProxyModel*>( model() );
    if ( sf ) {
        ItemModelBase *m = m_rightview->itemModel();
        if ( m ) {
            sf->setSortRole( m->sortRole( logicalIndex ) );
        }
    }
    m_rightview->header()->setSortIndicatorShown( true );
    // sorting controlled by right treeview, turn left off
    m_leftview->header()->setSortIndicatorShown( false );
}


QList<int> DoubleTreeViewBase::expandColumnList( const QList<int> lst ) const
{
    QList<int> mlst = lst;
    if ( ! mlst.isEmpty() ) {
        int v = 0;
        if ( mlst.last() == -1 && mlst.count() > 1 ) {
            v = mlst[ mlst.count() - 2 ] + 1;
            mlst.removeLast();
        }
        for ( int c = v; c < model()->columnCount(); ++c ) {
            mlst << c;
        }
    }
    return mlst;
}

void DoubleTreeViewBase::hideColumns( TreeViewBase *view, const QList<int> &list )
{
    view->setColumnsHidden( list );
}

void DoubleTreeViewBase::hideColumns( const QList<int> &masterList, const QList<int> slaveList )
{
    m_leftview->setColumnsHidden( masterList );
    m_rightview->setColumnsHidden( slaveList );
    if ( m_rightview->isHidden() ) {
        QList<int> mlst = expandColumnList( masterList );
        QList<int> slst = expandColumnList( slaveList );
        QList<int> lst;
        for ( int c = 0; c < model()->columnCount(); ++c ) {
            // only hide columns hidden in *both* views
            //kDebug(planDbg())<<c<<(mlst.indexOf( c ))<<(slst.indexOf( c ));
            if ( (mlst.indexOf( c ) >= 0) && (slst.indexOf( c ) >= 0) ) {
                lst << c;
            }
        }
        //kDebug(planDbg())<<lst;
        m_leftview->setColumnsHidden( lst );
    } else {
        setStretchFactors();
    }
}

void DoubleTreeViewBase::slotToRightView( const QModelIndex &index )
{
    //kDebug(planDbg())<<index.column();
    QModelIndex nxt = m_rightview->firstColumn( index.row(), model()->parent( index ) );
    m_rightview->setFocus();
    if ( nxt.isValid() ) {
        m_selectionmodel->setCurrentIndex( nxt, QItemSelectionModel::NoUpdate );
    }
}

void DoubleTreeViewBase::slotToLeftView( const QModelIndex &index )
{
    //kDebug(planDbg())<<index.column();
    QModelIndex prv = m_leftview->lastColumn( index.row(), model()->parent( index ) );
    m_leftview->setFocus();
    if ( prv.isValid() ) {
        m_selectionmodel->setCurrentIndex( prv, QItemSelectionModel::NoUpdate );
    }
}

void DoubleTreeViewBase::slotEditToRightView( const QModelIndex &index )
{
    //kDebug(planDbg())<<index.column()<<endl;
    if ( m_rightview->isHidden() ) {
        return;
    }
    m_rightview->setFocus();
    QModelIndex nxt = m_rightview->firstEditable( index.row(), model()->parent ( index ) );
    if ( nxt.isValid() && ( model()->flags( nxt ) & Qt::ItemIsEditable ) ) {
        m_selectionmodel->setCurrentIndex( nxt, QItemSelectionModel::NoUpdate );
        m_rightview->edit( nxt );
    } else {
        slotToRightView( index );
    }
}

void DoubleTreeViewBase::slotEditToLeftView( const QModelIndex &index )
{
    //kDebug(planDbg())<<index.column()<<endl;
    if ( m_leftview->isHidden() ) {
        return;
    }
    m_leftview->setFocus();
    QModelIndex nxt = m_leftview->lastEditable( index.row(), model()->parent ( index ) );
    if ( nxt.isValid() && ( model()->flags( nxt ) & Qt::ItemIsEditable ) ) {
        m_selectionmodel->setCurrentIndex( nxt, QItemSelectionModel::NoUpdate );
        m_leftview->edit( nxt );
    } else {
        slotToLeftView( index );
    }
}


void DoubleTreeViewBase::setReadWrite( bool rw )
{
    m_readWrite = rw;
    m_leftview->setReadWrite( rw );
    m_rightview->setReadWrite( rw );
}

void DoubleTreeViewBase::closePersistentEditor( const QModelIndex &index )
{
    m_leftview->closePersistentEditor( index );
    m_rightview->closePersistentEditor( index );
}

void DoubleTreeViewBase::setModel( QAbstractItemModel *model )
{
    m_leftview->setModel( model );
    m_rightview->setModel( model );
    if ( m_selectionmodel ) {
        disconnect( m_selectionmodel, SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection & ) ), this, SLOT( slotSelectionChanged( const QItemSelection &, const QItemSelection & ) ) );

        disconnect( m_selectionmodel, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SIGNAL( currentChanged ( const QModelIndex &, const QModelIndex & ) ) );
    }
    m_selectionmodel = m_leftview->selectionModel();
    m_rightview->setSelectionModel( m_selectionmodel );

    connect( m_selectionmodel, SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection & ) ), this, SLOT( slotSelectionChanged( const QItemSelection &, const QItemSelection & ) ) );

    connect( m_selectionmodel, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SIGNAL( currentChanged ( const QModelIndex &, const QModelIndex & ) ) );

    setReadWrite( m_readWrite );
}

QAbstractItemModel *DoubleTreeViewBase::model() const
{
    return m_leftview->model();
}

void DoubleTreeViewBase::slotSelectionChanged( const QItemSelection &sel, const QItemSelection & )
{
    emit selectionChanged( sel.indexes() );
}

void DoubleTreeViewBase::setSelectionModel( QItemSelectionModel *model )
{
    m_leftview->setSelectionModel( model );
    m_rightview->setSelectionModel( model );
}

void DoubleTreeViewBase::setSelectionMode( QAbstractItemView::SelectionMode mode )
{
    m_leftview->setSelectionMode( mode );
    m_rightview->setSelectionMode( mode );
}

void DoubleTreeViewBase::setSelectionBehavior( QAbstractItemView::SelectionBehavior mode )
{
    m_leftview->setSelectionBehavior( mode );
    m_rightview->setSelectionBehavior( mode );
}

void DoubleTreeViewBase::setItemDelegateForColumn( int col, QAbstractItemDelegate * delegate )
{
    m_leftview->setItemDelegateForColumn( col, delegate );
    m_rightview->setItemDelegateForColumn( col, delegate );
}

void DoubleTreeViewBase::createItemDelegates( ItemModelBase *model )
{
    m_leftview->createItemDelegates( model );
    m_rightview->createItemDelegates( model );
}

void DoubleTreeViewBase::setEditTriggers( QAbstractItemView::EditTriggers mode )
{
    m_leftview->setEditTriggers( mode );
    m_rightview->setEditTriggers( mode );
}

QAbstractItemView::EditTriggers DoubleTreeViewBase::editTriggers() const
{
    return m_leftview->editTriggers();
}

void DoubleTreeViewBase::setStretchLastSection( bool mode )
{
    m_rightview->header()->setStretchLastSection( mode );
    if ( m_rightview->isHidden() ) {
        m_leftview->header()->setStretchLastSection( mode );
    }
}

void DoubleTreeViewBase::edit( const QModelIndex &index )
{
    if ( ! m_leftview->isColumnHidden( index.column() ) ) {
        m_leftview->edit( index );
    } else if ( ! m_rightview->isHidden() && ! m_rightview->isColumnHidden( index.column() ) ) {
        m_rightview->edit( index );
    }
}

void DoubleTreeViewBase::setDragDropMode( QAbstractItemView::DragDropMode mode )
{
    m_leftview->setDragDropMode( mode );
    m_rightview->setDragDropMode( mode );
}

void DoubleTreeViewBase::setDragDropOverwriteMode( bool mode )
{
    m_leftview->setDragDropOverwriteMode( mode );
    m_rightview->setDragDropOverwriteMode( mode );
}

void DoubleTreeViewBase::setDropIndicatorShown( bool mode )
{
    m_leftview->setDropIndicatorShown( mode );
    m_rightview->setDropIndicatorShown( mode );
}

void DoubleTreeViewBase::setDragEnabled ( bool mode )
{
    m_leftview->setDragEnabled( mode );
    m_rightview->setDragEnabled( mode );
}

void DoubleTreeViewBase::setAcceptDrops( bool mode )
{
    m_leftview->setAcceptDrops( mode );
    m_rightview->setAcceptDrops( mode );
}

void DoubleTreeViewBase::setAcceptDropsOnView( bool mode )
{
    m_leftview->setAcceptDropsOnView( mode );
    m_rightview->setAcceptDropsOnView( mode );
}

void DoubleTreeViewBase::setDefaultDropAction( Qt::DropAction action )
{
    m_leftview->setDefaultDropAction( action );
    m_rightview->setDefaultDropAction( action );
}

void DoubleTreeViewBase::slotRightHeaderContextMenuRequested( const QPoint &pos )
{
    //kDebug(planDbg());
    emit slaveHeaderContextMenuRequested( pos );
    emit headerContextMenuRequested( pos );
}

void DoubleTreeViewBase::slotLeftHeaderContextMenuRequested( const QPoint &pos )
{
    //kDebug(planDbg());
    emit masterHeaderContextMenuRequested( pos );
    emit headerContextMenuRequested( pos );
}

void DoubleTreeViewBase::setStretchFactors()
{
    int lc = m_leftview->header()->count() - m_leftview->header()->hiddenSectionCount();
    int rc = m_rightview->header()->count() - m_rightview->header()->hiddenSectionCount();
    setStretchFactor( indexOf( m_rightview ), qMax( 1, qMin( 4, rc / qMax( 1, lc ) ) ) );
    //kDebug(planDbg())<<this<<"set stretch factor="<<qMax( 1, qMin( 4, rc / lc ) );
}

bool DoubleTreeViewBase::loadContext( const QMetaEnum &map, const KoXmlElement &element )
{
    //kDebug(planDbg());
    QList<int> lst1;
    QList<int> lst2;
    KoXmlElement e = element.namedItem( "slave" ).toElement();
    if ( ! e.isNull() ) {
        if ( e.attribute( "hidden", "false" ) == "true" ) {
            setViewSplitMode( false );
        } else {
            setStretchFactors();
        }
        m_rightview->loadContext( map, e );
    }
    e = element.namedItem( "master" ).toElement();
    if ( ! e.isNull() ) {
        m_leftview->loadContext( map, e );
    }
    return true;
}

void DoubleTreeViewBase::saveContext( const QMetaEnum &map, QDomElement &element ) const
{
    //kDebug(planDbg())<<objectName();
    QDomElement e = element.ownerDocument().createElement( "master" );
    element.appendChild( e );
    m_leftview->saveContext( map, e );
    e = element.ownerDocument().createElement( "slave" );
    element.appendChild( e );
    if ( m_rightview->isHidden() ) {
        e.setAttribute( "hidden", "true" );
    }
    m_rightview->saveContext( map, e );
}

void DoubleTreeViewBase::setViewSplitMode( bool split )
{
    if ( split ) {
        m_actionSplitView->setText( i18n( "Unsplit View" ) );
        m_actionSplitView->setIcon(koIcon("view-close"));
    } else {
        m_actionSplitView->setText( i18n( "Split View" ) );
        m_actionSplitView->setIcon(koIcon("view-split-left-right"));
    }

    if ( m_mode == split ) {
        return;
    }

    m_mode = split;
    if ( split ) {
        m_leftview->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        m_leftview->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
        if ( model() ) {
            m_rightview->setColumnHidden( 0, true );
            m_leftview->resizeColumnToContents( 0 );
            for ( int c = 1; c < m_rightview->model()->columnCount(); ++c ) {
                if ( m_leftview->isColumnHidden( c ) ) {
                    m_rightview->setColumnHidden( c, true );
                } else {
                    m_rightview->setColumnHidden( c, false );
                    m_rightview->mapToSection( c, m_leftview->section( c ) );
                    m_leftview->setColumnHidden( c, true );
                    m_rightview->resizeColumnToContents( c );
                }
            }
        }
        m_rightview->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
        m_rightview->show();
    } else {
        m_rightview->hide();
        if ( model() ) {
            int offset = m_rightview->isColumnHidden( 0 ) ? 1 : 0;
            for ( int c = 0; c < model()->columnCount(); ++c ) {
                if ( ! m_rightview->isColumnHidden( c ) ) {
                    m_leftview->setColumnHidden( c, false );
                    m_leftview->mapToSection( c, m_rightview->section( c ) + offset );
                    m_leftview->resizeColumnToContents( c );
                }
            }
        }
        m_leftview->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
        m_leftview->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    }
}

void DoubleTreeViewBase::setRootIsDecorated ( bool show )
{
    m_leftview->setRootIsDecorated( show );
    m_rightview->setRootIsDecorated( show );
}

QModelIndex DoubleTreeViewBase::indexAt( const QPoint &pos ) const
{
    QModelIndex idx = m_leftview->indexAt( pos );
    if ( ! idx.isValid() ) {
        idx = m_rightview->indexAt( pos );
    }
    return idx;
}

} // namespace KPlato

#include "kptviewbase.moc"
