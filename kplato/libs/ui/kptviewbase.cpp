/* This file is part of the KDE project
  Copyright (C) 2006 Dag Andersen <danders@get2net.dk>

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

#include <kaction.h>
#include <kicon.h>
#include <kparts/event.h>
#include <kxmlguifactory.h>
#include <kmessagebox.h>

#include <KoDocument.h>
#include <KoShape.h>

#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QHeaderView>
#include <QPoint>
#include <QScrollBar>
#include <QAbstractScrollArea>
#include <QMetaEnum>
#include <QStyleOption>
#include <QPainter>
#include <QMenu>

namespace KPlato
{
    
PrintingHeaderFooter::PrintingHeaderFooter( const PrintingOptions &opt, QWidget *parent )
    : QWidget( parent )
{
    setupUi( this );
    setWindowTitle( i18n("Header and Footer" ));
    setOptions( opt );
}

PrintingHeaderFooter::~PrintingHeaderFooter()
{
    //kDebug();
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
    //kDebug();
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
}

PrintingOptions PrintingDialog::printingOptions() const
{
    return m_view->printingOptions();
}

void PrintingDialog::setPrintingOptions( const PrintingOptions &opt )
{
    return m_view->setPrintingOptions( opt );
}

void PrintingDialog::startPrinting(RemovePolicy removePolicy )
{
    PrintingOptions opt;
    if ( m_widget ) {
        opt = m_widget->options();
    }
    setPrintingOptions( opt );
    KoPrintingDialog::startPrinting( removePolicy );
}

QList<QWidget*> PrintingDialog::createOptionWidgets() const
{
    //kDebug();
    PrintingHeaderFooter *w = new PrintingHeaderFooter( printingOptions() );
    const_cast<PrintingDialog*>( this )->m_widget = w;
    
    return QList<QWidget*>() << m_widget;
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
    int height = 0;
    if ( options.page == Qt::Checked || options.project == Qt::Checked || options.manager == Qt::Checked || options.date == Qt::Checked ) {
        height += painter().boundingRect( const_cast<PrintingDialog*>( this )->printer().pageRect(), Qt::AlignTop, "Aj" ).height();
        height *= 1.5;
    }
    if (  options.project == Qt::Checked && options.manager == Qt::Checked && ( options.date == Qt::Checked || options.page == Qt::Checked ) ) {
        height *= 2.0;
    }
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
ViewBase::ViewBase(KoDocument *doc, QWidget *parent)
    : KoView( doc, parent ),
    m_readWrite( false )
{
}
    
ViewBase::~ViewBase()
{
    if ( koDocument() ) {
        //HACK to avoid ~View to access koDocument()
        setDocumentDeleted();
    }
}

KoDocument *ViewBase::part() const
{
     return koDocument();
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
}

void ViewBase::setGuiActive( bool active ) // virtual slot
{
    //kDebug()<<active;
    emit guiActivated( this, active );
}

ViewBase *ViewBase::hitView( const QPoint &glpos )
{
    //kDebug()<<glpos;
    return this;
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

void ViewBase::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos, lst.first() );
    }
}

void ViewBase::createOptionAction()
{
    actionOptions = new KAction(KIcon("configure"), i18n("Configure View..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

//----------------------
TreeViewPrintingDialog::TreeViewPrintingDialog( ViewBase *view, TreeViewBase *treeview, Project *project )
    : PrintingDialog( view ),
    m_tree( treeview ),
    m_project( project ),
    m_firstRow( -1 )
{
}

int TreeViewPrintingDialog::documentLastPage() const
{
    int page = documentFirstPage();
    while ( firstRow( page ) != -1 ) { ++page; }
    if ( page > documentFirstPage() ) {
        --page;
    }
    return page;
}

int TreeViewPrintingDialog::firstRow( int page ) const
{
    kDebug()<<page;
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
                kDebug()<<page<<pageNumber;
                break;
            }
            ++row;
            idx = m_tree->indexBelow( idx );
        }
        if ( ! idx.isValid() ) {
            row = -1;
        }
    }
    kDebug()<<row<<rowsPrPage;
    return row;
}

void TreeViewPrintingDialog::printPage( int page, QPainter &painter )
{
    painter.save();
    
    m_firstRow = firstRow( page );
    
    QHeaderView *mh = m_tree->header();
    int length = mh->length();;
    int height = mh->height();
    QRect hRect = headerRect();
    QRect fRect = footerRect();
    QRect pageRect = printer().pageRect();
    QRect paperRect = printer().paperRect();
    
    QAbstractItemModel *model = m_tree->model();
    
    kDebug()<<pageRect<<paperRect;

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
            //kDebug()<<text<<r<<r.left() * sx<<align;
        }
        //kDebug()<<text<<"hidden="<<h->isSectionHidden( i )<<h->sectionPosition( i );
    }
    if ( m_firstRow == -1 ) {
        kDebug()<<"No data";
        painter.restore();
        return;
    }
    painter.setBrush( QBrush() );
    QModelIndex idx = model->index( m_firstRow, 0, QModelIndex() );
    int numRows = 0;
    //kDebug()<<page<<rowsPrPage;
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
    painter.restore();
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
    setItemDelegate( new ItemDelegate( this ) );
    setAlternatingRowColors ( true );

    header()->setContextMenuPolicy( Qt::CustomContextMenu );

    connect( header(), SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

KoPrintJob * TreeViewBase::createPrintJob( ViewBase *parent )
{
    TreeViewPrintingDialog *dia = new TreeViewPrintingDialog( parent, this, parent->project() );
    dia->printer().setCreator("KPlato 0.7");
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
    //kDebug();
    emit headerContextMenuRequested( header()->mapToGlobal( pos ) );
}

void TreeViewBase::setColumnsHidden( const QList<int> &lst )
{
    //kDebug()<<m_hideList;
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
    //kDebug()<<objectName()<<index<<hint;
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
    //kDebug()<<event->reason();
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
    //kDebug()<<objectName()<<event->key()<<","<<m_arrowKeyNavigation;
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
            command = QItemSelectionModel::Clear | selectionBehaviorFlags();
        break;
    default:
        break;
    }
    selectionModel()->setCurrentIndex( newidx, command );
}

void TreeViewBase::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    //kDebug()<<editor<<hint;
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
            //kDebug()<<"Standard treatment"<<editor<<hint;
            return QTreeView::closeEditor( editor, hint ); // standard treatment
    }
    if (index.isValid()) {
        QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect | selectionBehaviorFlags();
        //kDebug()<<flags;
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
    //kDebug()<<ix;
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
    //kDebug()<<cursorAction<<current;
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
                //kDebug()<<col<<model()->columnCount(ix.parent())<<ix;
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
                if ( ! header()->isSectionHidden( s ) ) {
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
                if ( ! header()->isSectionHidden( s ) ) {
                    ix = model()->index( ix.row(), header()->logicalIndex( s ), ix.parent() );
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
    //kDebug();
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
    //kDebug();
    if (dragDropMode() == InternalMove
        && (event->source() != this || !(event->possibleActions() & Qt::MoveAction))) {
        //kDebug()<<"Internal:"<<event->isAccepted();
        return;
    }
    QTreeView::dragMoveEvent( event );
    event->ignore();
    if ( dropIndicatorPosition() == QAbstractItemView::OnViewport ) {
        if ( m_acceptDropsOnView ) {
            event->accept();
        }
        //kDebug()<<"On viewport:"<<event->isAccepted();
        return;
    }
    QModelIndex index = indexAt( event->pos() );
    if ( ! index.isValid() ) {
        //kDebug()<<"Invalid index:"<<event->isAccepted();
        return;
    }
    emit dropAllowed( index, dropIndicatorPosition(), event );
    //kDebug()<<event->isAccepted();
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
    //kDebug()<<objectName();
    KoXmlElement e = element.namedItem( "columns" ).toElement();
    if ( ! e.isNull() ) {
        if ( ! map.isValid() ) {
            // try numbers
            kDebug()<<"invalid map";
            for ( int i = model()->columnCount() - 1; i >= 0; --i ) {
                QString s = e.attribute( QString( "column-%1" ).arg( i ), "" );
                if ( s == "hidden" ) {
                    hideColumn( i );
                } else if ( s == "shown" ) {
                    showColumn( i );
                } else kDebug()<<objectName()<<"Unknown column:"<<s;
            }
        } else {
            for ( int i = model()->columnCount() - 1; i >= 0; --i ) {
                QString n = map.key( i );
                //kDebug()<<i<<"="<<n;
                if ( ! n.isEmpty() ) {
                    QString s = e.attribute( n, "" );
                    if ( s == "hidden" ) {
                        hideColumn( i );
                    } else if ( s == "shown" ) {
                        showColumn( i );
                    } else kDebug()<<objectName()<<"Unknown column:"<<s;
                } else kDebug()<<"Column not in enum:"<<i;
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
            for ( int i = 0; i < h->count(); ++i ) {
                QString n = map.key( i );
                if ( ! n.isEmpty() ) {
                    int col = map.keyToValue( e.attribute( s.arg( i ), "" ).toUtf8() );
                    if ( col >= 0 && col < h->count() ) {
                        header()->moveSection( h->visualIndex( col ), i );
                    }
                }
            }
        }
    }
    return true;
}

void TreeViewBase::saveContext( const QMetaEnum &map, QDomElement &element ) const
{
    //kDebug()<<objectName();
    QDomElement e = element.ownerDocument().createElement( "columns" );
    element.appendChild( e );
    for ( int i = 0; i < model()->columnCount(); ++i ) {
        bool h = isColumnHidden( i );
        if ( ! map.isValid() ) {
            kDebug()<<"invalid map";
            e.setAttribute( QString( "column-%1" ).arg( i ), h ? "hidden" : "shown" );
        } else {
            QString n = map.key( i );
            //kDebug()<<i<<"="<<n;
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

//----------------------
DoubleTreeViewPrintingDialog::DoubleTreeViewPrintingDialog( ViewBase *view, DoubleTreeViewBase *treeview, Project *project )
    : PrintingDialog( view ),
    m_tree( treeview ),
    m_project( project ),
    m_firstRow( -1 )
{
}

int DoubleTreeViewPrintingDialog::documentLastPage() const
{
    int page = documentFirstPage();
    while ( firstRow( page ) != -1 ) { ++page; }
    if ( page > documentFirstPage() ) {
        --page;
    }
    return page;
}

int DoubleTreeViewPrintingDialog::firstRow( int page ) const
{
    kDebug()<<page;
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
                kDebug()<<page<<pageNumber;
                break;
            }
            ++row;
            idx = m_tree->masterView()->indexBelow( idx );
        }
        if ( ! idx.isValid() ) {
            row = -1;
        }
    }
    kDebug()<<row<<rowsPrPage;
    return row;
}

void DoubleTreeViewPrintingDialog::printPage( int page, QPainter &painter )
{
    painter.save();
    
    m_firstRow = firstRow( page );
    
    QHeaderView *mh = m_tree->masterView()->header();
    QHeaderView *sh = m_tree->slaveView()->header();
    int length = mh->length() + sh->length();
    int height = mh->height() > sh->height() ? mh->height() : sh->height();
    QRect hRect = headerRect();
    QRect fRect = footerRect();
    QRect pageRect = printer().pageRect();
    QRect paperRect = printer().paperRect();
    
    QAbstractItemModel *model = m_tree->model();
    Q_ASSERT( model != 0 );
    kDebug()<<pageRect<<paperRect;

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
        //kDebug()<<text<<"hidden="<<h->isSectionHidden( i )<<h->sectionPosition( i );
    }
    if ( m_firstRow == -1 ) {
        kDebug()<<"No data";
        painter.restore();
        return;
    }
    painter.setBrush( QBrush() );
    QModelIndex idx = model->index( m_firstRow, 0, QModelIndex() );
    int numRows = 0;
    //kDebug()<<page<<rowsPrPage;
    while ( idx.isValid() && numRows < rowsPrPage ) {
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

DoubleTreeViewBase::DoubleTreeViewBase( bool mode, QWidget *parent )
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
    dia->printer().setCreator("KPlato 0.7");
//    dia->printer().setFullPage(true); // ignore printer margins
    return dia;
}

void DoubleTreeViewBase::setParentsExpanded( const QModelIndex &idx, bool expanded )
{
    //kDebug()<<idx<<m_leftview->isExpanded( idx )<<m_rightview->isExpanded( idx );
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
        //kDebug()<<p<<m_leftview->isExpanded( p )<<m_rightview->isExpanded( p );
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

    m_actionSplitView = new KAction(KIcon("view-split-left-right"), "", this);
    setViewSplitMode( true );

    connect( m_leftview->header(), SIGNAL( sortIndicatorChanged( int, Qt::SortOrder ) ), SLOT( slotLeftSortIndicatorChanged( int, Qt::SortOrder ) ) );
    
    connect( m_rightview->header(), SIGNAL( sortIndicatorChanged( int, Qt::SortOrder ) ), SLOT( slotRightSortIndicatorChanged( int, Qt::SortOrder ) ) );
}

void DoubleTreeViewBase::slotLeftSortIndicatorChanged( int logicalIndex, Qt::SortOrder order )
{
    m_leftview->header()->setSortIndicatorShown( true );
    // sorting controlled by left treeview, turn right off
    m_rightview->header()->setSortIndicatorShown( false );
}

void DoubleTreeViewBase::slotRightSortIndicatorChanged( int logicalIndex, Qt::SortOrder order )
{
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

void DoubleTreeViewBase::hideColumns( const QList<int> &masterList, QList<int> slaveList )
{
    m_leftview->setColumnsHidden( masterList );
    m_rightview->setColumnsHidden( slaveList );
    if ( m_rightview->isHidden() ) {
        QList<int> mlst = expandColumnList( masterList );
        QList<int> slst = expandColumnList( slaveList );
        QList<int> lst;
        for ( int c = 0; c < model()->columnCount(); ++c ) {
            // only hide columns hidden in *both* views
            //kDebug()<<c<<(mlst.indexOf( c ))<<(slst.indexOf( c ));
            if ( (mlst.indexOf( c ) >= 0) && (slst.indexOf( c ) >= 0) ) {
                lst << c;
            }
        }
        //kDebug()<<lst;
        m_leftview->setColumnsHidden( lst );
    } else {
        setStretchFactors();
    }
}

void DoubleTreeViewBase::slotToRightView( const QModelIndex &index )
{
    //kDebug()<<index.column();
    QModelIndex nxt = m_rightview->firstColumn( index.row(), model()->parent( index ) );
    m_rightview->setFocus();
    if ( nxt.isValid() ) {
        m_selectionmodel->setCurrentIndex( nxt, QItemSelectionModel::NoUpdate );
    }
}

void DoubleTreeViewBase::slotToLeftView( const QModelIndex &index )
{
    //kDebug()<<index.column();
    QModelIndex prv = m_leftview->lastColumn( index.row(), model()->parent( index ) );
    m_leftview->setFocus();
    if ( prv.isValid() ) {
        m_selectionmodel->setCurrentIndex( prv, QItemSelectionModel::NoUpdate );
    }
}

void DoubleTreeViewBase::slotEditToRightView( const QModelIndex &index )
{
    //kDebug()<<index.column()<<endl;
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
    //kDebug()<<index.column()<<endl;
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
}

void DoubleTreeViewBase::setDropIndicatorShown( bool mode )
{
    m_leftview->setDropIndicatorShown( mode );
}

void DoubleTreeViewBase::setDragEnabled ( bool mode )
{
    m_leftview->setDragEnabled( mode );
}

void DoubleTreeViewBase::setAcceptDrops( bool mode )
{
    m_leftview->setAcceptDrops( mode );
}

void DoubleTreeViewBase::setAcceptDropsOnView( bool mode )
{
    m_leftview->setAcceptDropsOnView( mode );
}

void DoubleTreeViewBase::slotRightHeaderContextMenuRequested( const QPoint &pos )
{
    //kDebug();
    emit slaveHeaderContextMenuRequested( pos );
    emit headerContextMenuRequested( pos );
}

void DoubleTreeViewBase::slotLeftHeaderContextMenuRequested( const QPoint &pos )
{
    //kDebug();
    emit masterHeaderContextMenuRequested( pos );
    emit headerContextMenuRequested( pos );
}

void DoubleTreeViewBase::setStretchFactors()
{
    int lc = m_leftview->header()->count() - m_leftview->header()->hiddenSectionCount();
    int rc = m_rightview->header()->count() - m_rightview->header()->hiddenSectionCount();
    setStretchFactor( indexOf( m_rightview ), qMax( 1, qMin( 4, rc / lc ) ) );
    //kDebug()<<this<<"set stretch factor="<<qMax( 1, qMin( 4, rc / lc ) );
}

bool DoubleTreeViewBase::loadContext( const QMetaEnum &map, const KoXmlElement &element )
{
    //kDebug();
    QList<int> lst1;
    QList<int> lst2;
    KoXmlElement e = element.namedItem( "master" ).toElement();
    if ( ! e.isNull() ) {
        m_leftview->loadContext( map, e );
    }
    e = element.namedItem( "slave" ).toElement();
    if ( ! e.isNull() ) {
        m_rightview->loadContext( map, e );
        if ( e.attribute( "hidden", "false" ) == "true" ) {
            setViewSplitMode( false );
        } else {
            setStretchFactors();
        }
    }
    return true;
}

void DoubleTreeViewBase::saveContext( const QMetaEnum &map, QDomElement &element ) const
{
    //kDebug()<<objectName();
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
        m_actionSplitView->setIcon( KIcon( "view-close" ) );
    } else {
        m_actionSplitView->setText( i18n( "Split View" ) );
        m_actionSplitView->setIcon( KIcon( "view-split-left-right" ) );
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
