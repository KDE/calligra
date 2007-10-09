/* This file is part of the KDE project
  Copyright (C) 2006 - 2007 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation;
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "kptitemmodelbase.h"

#include "kptproject.h"
#include "kptdurationwidget.h"
#include "kptdurationspinbox.h"

#include "KoDocument.h"

#include <QComboBox>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QTimeEdit>

#include <klineedit.h>
#include <kdebug.h>

namespace KPlato
{

// Hmmm, a bit hacky, but this makes it possible to use index specific editors...
SelectorDelegate::SelectorDelegate( QObject *parent )
: QItemDelegate( parent )
{
}

QWidget *SelectorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index ) const
{
    switch ( index.model()->data( index, Role::EditorType ).toInt() ) {
        case Delegate::EnumEditor: {
            QComboBox *editor = new QComboBox(parent);
            editor->installEventFilter(const_cast<SelectorDelegate*>(this));
            return editor;
        }
        case Delegate::TimeEditor: {
            QTimeEdit *editor = new QTimeEdit(parent);
            editor->installEventFilter(const_cast<SelectorDelegate*>(this));
            return editor;
        }
    }
    return 0; // FIXME: What to do?
}

void SelectorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    switch ( index.model()->data( index, Role::EditorType ).toInt() ) {
        case Delegate::EnumEditor: {
            QStringList lst = index.model()->data( index, Role::EnumList ).toStringList();
            int value = index.model()->data(index, Role::EnumListValue).toInt();
            QComboBox *box = static_cast<QComboBox*>(editor);
            box->addItems( lst );
            box->setCurrentIndex( value );
            return;
        }
        case Delegate::TimeEditor:
            QTime value = index.model()->data(index, Qt::EditRole).toTime();
            QTimeEdit *e = static_cast<QTimeEdit*>(editor);
            e->setMinimumTime( index.model()->data( index, Role::Minimum ).toTime() );
            e->setMaximumTime( index.model()->data( index, Role::Maximum ).toTime() );
            e->setTime( value );
            return;
    }
}

void SelectorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    switch ( index.model()->data( index, Role::EditorType ).toInt() ) {
        case Delegate::EnumEditor: {
            QComboBox *box = static_cast<QComboBox*>(editor);
            int value = box->currentIndex();
            model->setData( index, value, Qt::EditRole );
            return;
        }
        case Delegate::TimeEditor: {
            QTimeEdit *e = static_cast<QTimeEdit*>(editor);
            model->setData( index, e->time(), Qt::EditRole );
            return;
        }
    }
}

void SelectorDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    QRect r = option.rect;
    editor->setGeometry(r);
}

EnumDelegate::EnumDelegate( QObject *parent )
: QItemDelegate( parent )
{
}

QWidget *EnumDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->installEventFilter(const_cast<EnumDelegate*>(this));
    return editor;
}

void EnumDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStringList lst = index.model()->data( index, Role::EnumList ).toStringList();
    int value = index.model()->data(index, Role::EnumListValue).toInt();

    QComboBox *box = static_cast<QComboBox*>(editor);
    box->addItems( lst );
    box->setCurrentIndex( value );
}

void EnumDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    QComboBox *box = static_cast<QComboBox*>(editor);
    int value = box->currentIndex();
    model->setData( index, value, Qt::EditRole );
}

void EnumDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    kDebug()<<editor<<":"<<option.rect<<","<<editor->sizeHint();
    QRect r = option.rect;
    //r.setHeight(r.height() 50);
    editor->setGeometry(r);
}

DurationDelegate::DurationDelegate( QObject *parent )
    : QItemDelegate( parent )
{
}

QWidget *DurationDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    DurationWidget *editor = new DurationWidget(parent);
    editor->installEventFilter(const_cast<DurationDelegate*>(this));
    return editor;
}

void DurationDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    DurationWidget *dw = static_cast<DurationWidget*>(editor);
    Duration value = Duration( index.model()->data(index, Qt::EditRole).toLongLong() );
    QVariantList scales = index.model()->data(index, Role::DurationScales ).value<QVariantList>();
    for(int i = 0; i < scales.count(); ++i ) {
        dw->setFieldScale( i, scales[ i ].toDouble() );
        dw->setFieldRightscale(i, scales[ i ].toDouble());
        dw->setFieldLeftscale(i+1, scales[ i ].toDouble());
    }
    dw->setVisibleFields( DurationWidget::Days | DurationWidget::Hours | DurationWidget::Minutes );
    dw->setValue( value );
}

void DurationDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    DurationWidget *dw = static_cast<DurationWidget*>(editor);
    qint64 value = dw->value().seconds();
    model->setData( index, value, Qt::EditRole );
}

void DurationDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    kDebug()<<editor<<":"<<option.rect<<","<<editor->sizeHint();
    QRect r = option.rect;
    //r.setHeight(r.height() + 50);
    editor->setGeometry(r);
}

//----------------------
DurationSpinBoxDelegate::DurationSpinBoxDelegate( QObject *parent )
    : QItemDelegate( parent )
{
}

QWidget *DurationSpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    DurationSpinBox *editor = new DurationSpinBox(parent);
    editor->installEventFilter(const_cast<DurationSpinBoxDelegate*>(this));
    return editor;
}

void DurationSpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    DurationSpinBox *dsb = static_cast<DurationSpinBox*>(editor);
    dsb->setScales( index.model()->data( index, Role::DurationScales ) );
    dsb->setUnit( (Duration::Unit)( index.model()->data( index, Role::DurationUnit ).toInt() ) );
    dsb->setValue( index.model()->data( index, Qt::EditRole ).toLongLong() );
}

void DurationSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    DurationSpinBox *dsb = static_cast<DurationSpinBox*>(editor);
    QVariantList lst;
    lst << QVariant( dsb->value() ) << QVariant( (int)( dsb->unit() ) );
    model->setData( index, QVariant( lst ), Qt::EditRole );
}

void DurationSpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    kDebug()<<editor<<":"<<option.rect<<","<<editor->sizeHint();
    QRect r = option.rect;
    //r.setHeight(r.height() + 50);
    editor->setGeometry(r);
}

//---------------------------
SpinBoxDelegate::SpinBoxDelegate( QObject *parent )
    : QItemDelegate( parent )
{
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    QSpinBox *editor = new QSpinBox(parent);
    editor->installEventFilter(const_cast<SpinBoxDelegate*>(this));
    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();
    int min = index.model()->data(index, Role::Minimum).toInt();
    int max = index.model()->data(index, Role::Maximum).toInt();

    QSpinBox *box = static_cast<QSpinBox*>(editor);
    box->setRange( min, max );
    box->setValue( value );
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    QSpinBox *box = static_cast<QSpinBox*>(editor);
    model->setData( index, box->value(), Qt::EditRole );
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    kDebug()<<editor<<":"<<option.rect<<","<<editor->sizeHint();
    QRect r = option.rect;
    //r.setHeight(r.height() + 50);
    editor->setGeometry(r);
}

//---------------------------
DoubleSpinBoxDelegate::DoubleSpinBoxDelegate( QObject *parent )
    : QItemDelegate( parent )
{
}

QWidget *DoubleSpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->installEventFilter(const_cast<DoubleSpinBoxDelegate*>(this));
    return editor;
}

void DoubleSpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    double value = index.model()->data(index, Qt::EditRole).toDouble();
    double min = 0.0;//index.model()->data(index, Role::Minimum).toInt();
    double max = 24.0;//index.model()->data(index, Role::Maximum).toInt();

    QDoubleSpinBox *box = static_cast<QDoubleSpinBox*>(editor);
    box->setDecimals( 1 );
    box->setRange( min, max );
    box->setValue( value );
    box->selectAll();
}

void DoubleSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    QDoubleSpinBox *box = static_cast<QDoubleSpinBox*>(editor);
    model->setData( index, box->value(), Qt::EditRole );
}

void DoubleSpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    QRect r = option.rect;
    editor->setGeometry(r);
}

//---------------------------
MoneyDelegate::MoneyDelegate( QObject *parent )
    : QItemDelegate( parent )
{
}

QWidget *MoneyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    KLineEdit *editor = new KLineEdit(parent);
    //TODO: validator
    editor->installEventFilter(const_cast<MoneyDelegate*>(this));
    return editor;
}

void MoneyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    KLineEdit *e = static_cast<KLineEdit*>(editor);
    e->setText( value );
}

void MoneyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    KLineEdit *e = static_cast<KLineEdit*>(editor);
    model->setData( index, e->text(), Qt::EditRole );
}

void MoneyDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    QRect r = option.rect;
    editor->setGeometry(r);
}

//---------------------------
TimeDelegate::TimeDelegate( QObject *parent )
    : QItemDelegate( parent )
{
}

QWidget *TimeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    QTimeEdit *editor = new QTimeEdit(parent);
    editor->installEventFilter(const_cast<TimeDelegate*>(this));
    return editor;
}

void TimeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QTime value = index.model()->data(index, Qt::EditRole).toTime();
    QTimeEdit *e = static_cast<QTimeEdit*>(editor);
    e->setMinimumTime( index.model()->data( index, Role::Minimum ).toTime() );
    e->setMaximumTime( index.model()->data( index, Role::Maximum ).toTime() );
    e->setTime( value );
}

void TimeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    QTimeEdit *e = static_cast<QTimeEdit*>(editor);
    model->setData( index, e->time(), Qt::EditRole );
}

void TimeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    QRect r = option.rect;
    editor->setGeometry(r);
}

//--------------------------
ItemModelBase::ItemModelBase( KoDocument *part, QObject *parent )
    : QAbstractItemModel( parent ),
    m_part(part),
    m_project(0),
    m_readWrite( false )//part->isReadWrite() )
{
}

ItemModelBase::~ItemModelBase()
{
}

void ItemModelBase::setProject( Project *project )
{
    m_project = project;
}

void ItemModelBase::slotLayoutChanged()
{
    kDebug();
    emit layoutChanged();
}

void ItemModelBase::slotLayoutToBeChanged()
{
    kDebug();
    emit layoutAboutToBeChanged();
}

bool ItemModelBase::dropAllowed( const QModelIndex &index, int, const QMimeData *data )
{
    if ( flags( index ) & Qt::ItemIsDropEnabled ) {
        foreach ( QString s, data->formats() ) {
            if ( mimeTypes().contains( s ) ) {
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------

TreeViewBase::TreeViewBase( QWidget *parent )
    : QTreeView( parent ),
    m_arrowKeyNavigation( true ),
    m_acceptDropsOnView( false )
{
    setAlternatingRowColors ( true );

    header()->setContextMenuPolicy( Qt::CustomContextMenu );

    connect( header(), SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

void TreeViewBase::slotHeaderContextMenuRequested( const QPoint& pos )
{
    kDebug();
    emit headerContextMenuRequested( header()->mapToGlobal( pos ) );
}

void TreeViewBase::setColumnsHidden( const QList<int> &list )
{
    m_hideList = list;
}

void TreeViewBase::updateColumnsHidden()
{
    //kDebug()<<m_hideList<<endl;
    int prev = 0;
    foreach ( int c, m_hideList ) {
        if ( c == -1 ) {
            // hide rest
            for ( int i = prev+1; i < model()->columnCount(); ++i ) {
                if ( ! m_hideList.contains( i ) ) {
                    hideColumn( i );
                }
            }
            break;
        }
        hideColumn( c );
        prev = c;
    }
}

QModelIndex TreeViewBase::nextColumn( const QModelIndex &curr ) const
{
    int s = header()->visualIndex( curr.column() ) + 1;
    while ( isColumnHidden( header()->logicalIndex( s ) ) ) {
        if ( s >= header()->count() ) {
            kDebug()<<curr.column()<<": -1";
            return QModelIndex();
        }
        ++s;
    }
    kDebug()<<curr.column()<<": next="<<header()->logicalIndex( s )<<","<<s;
    return model()->index( curr.row(), header()->logicalIndex( s ), curr.parent() );
}

QModelIndex TreeViewBase::previousColumn( const QModelIndex &curr ) const
{
    int s = header()->visualIndex( curr.column() ) - 1;
    while ( isColumnHidden( header()->logicalIndex( s ) ) ) {
        if ( s < 0 ) {
            kDebug()<<curr.column()<<": -1";
            return QModelIndex();
        }
        --s;
    }
    kDebug()<<curr.column()<<": prev="<<header()->logicalIndex( s )<<","<<s;
    return model()->index( curr.row(), header()->logicalIndex( s ), curr.parent() );
}

/*!
    \reimp
 */
void TreeViewBase::keyPressEvent(QKeyEvent *event)
{
    kDebug()<<event->key()<<","<<m_arrowKeyNavigation;
    if ( !m_arrowKeyNavigation ) {
        QTreeView::keyPressEvent( event );
        return;
    }
    QModelIndex current = currentIndex();
    if ( current.isValid() ) {
        switch (event->key()) {
            case Qt::Key_Right: {
                QModelIndex nxt = nextColumn( current );
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
                QModelIndex prv = previousColumn( current );
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
                QModelIndex i = indexBelow( current );
                if ( i.isValid() ) {
                    i = model()->index( i.row(), current.column(), i.parent() );
                    selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
                }
                event->accept();
                return;
                break;
            }
            case Qt::Key_Up: {
                QModelIndex i = indexAbove( current );
                if ( i.isValid() ) {
                    i = model()->index( i.row(), current.column(), i.parent() );
                    selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
                }
                event->accept();
                return;
                break;
            }
            case Qt::Key_Plus:
                if ( itemsExpandable()) {
                    if ( model()->hasChildren( current ) ) {
                        QTreeView::keyPressEvent( event );
                    //HACK: Bug in qt??
                        selectionModel()->setCurrentIndex(current, QItemSelectionModel::NoUpdate);
                    }
                    event->accept();
                    return;
                }
                break;
            case Qt::Key_Minus:
                if ( itemsExpandable() ) {
                    if ( model()->hasChildren( current ) ) {
                        QTreeView::keyPressEvent( event );
                    //HACK: Bug in qt??
                        selectionModel()->setCurrentIndex(current, QItemSelectionModel::NoUpdate);
                    }
                    event->accept();
                    return;
                }
                break;
        }
    }
    QTreeView::keyPressEvent(event);
}

QItemSelectionModel::SelectionFlags TreeViewBase::selectionCommand(const QModelIndex &index, const QEvent *event) const
{
    /*    if ( event && event->type() == QEvent::KeyPress && selectionMode() == QAbstractItemView::ExtendedSelection ) {
        if ( static_cast<const QKeyEvent*>(event)->key() == Qt::Key_Space ) {
        Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
        QItemSelectionModel::SelectionFlags bflags = QItemSelectionModel::Rows;
        // 
        if ( modifiers && Qt::ShiftModifier ) {
        return QItemSelectionModel::SelectCurrent|bflags;
    }
    // Toggle on Ctrl-Qt::Key_Space
        if ( modifiers & Qt::ControlModifier ) {
        return QItemSelectionModel::Toggle|bflags;
    }
    // Select on Space alone
        return QItemSelectionModel::ClearAndSelect|bflags;
    }
    }*/
    return QTreeView::selectionCommand( index, event );
}

void TreeViewBase::contextMenuEvent ( QContextMenuEvent *event )
{
    kDebug();
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


ItemModelBase *TreeViewBase::itemModel() const
{
    return static_cast<ItemModelBase*>( model() );
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
    if ( itemModel()->dropAllowed( index, dropIndicatorPosition(), event->mimeData() ) ) {
        event->accept();
    }
    //kDebug()<<event->isAccepted();
}

bool TreeViewBase::loadContext( const KoXmlElement &element )
{
    kDebug()<<objectName()<<endl;
    KoXmlElement e = element.namedItem( "columns" ).toElement();
    if ( ! e.isNull() ) {
        for ( int i = model()->columnCount() - 1; i >= 0; --i ) {
            if ( e.attribute( QString( "column-%1" ).arg( i ), "" ) == "hidden" ) {
                hideColumn( i );
            } else {
                showColumn( i );
            }
        }
    }
    e = element.namedItem( "sections" ).toElement();
    if ( ! e.isNull() ) {
        QHeaderView *h = header();
        QString s( "section-%1" );
        for ( int i = 0; i < h->count(); ++i ) {
            if ( e.hasAttribute( s.arg( i ) ) ) {
                int pos = h->visualIndex( e.attribute( s.arg( i ), "-1" ).toInt() );
                header()->moveSection( pos, i );
            }
        }
    }
    return true;
}

void TreeViewBase::saveContext( QDomElement &element ) const
{
    QDomElement e = element.ownerDocument().createElement( "columns" );
    element.appendChild( e );
    for ( int i = 0; i < model()->columnCount(); ++i ) {
        if ( isColumnHidden( i ) ) {
            e.setAttribute( QString( "column-%1" ).arg( i ), "hidden" );
        }
    }
    e = element.ownerDocument().createElement( "sections" );
    element.appendChild( e );
    QHeaderView *h = header();
    for ( int i = 0; i < h->count(); ++i ) {
        if ( ! isColumnHidden( h->logicalIndex( i ) ) ) {
            e.setAttribute( QString( "section-%1" ).arg( i ), h->logicalIndex( i ) );
        }
    }
}


//--------------------------------
DoubleTreeViewBase::DoubleTreeViewBase( bool mode, QWidget *parent )
    : QSplitter( parent ),
    m_rightview( 0 ),
    m_model( 0 ),
    m_selectionmodel( 0 )
{
    init( mode );
}

  DoubleTreeViewBase::DoubleTreeViewBase( QWidget *parent )
    : QSplitter( parent ),
    m_rightview( 0 ),
    m_model( 0 ),
    m_selectionmodel( 0 )
{
    init( true );
}

DoubleTreeViewBase::~DoubleTreeViewBase()
{
}

void DoubleTreeViewBase::init( bool mode )
{
    setOrientation( Qt::Horizontal );
    setHandleWidth( 2 );
    m_leftview = new TreeViewBase( this );
    if ( mode ) {
        m_leftview->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        m_leftview->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
        
        m_rightview = new TreeViewBase( this );
        m_rightview->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    }

    connect( m_leftview, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ) );
    connect( m_leftview, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotLeftHeaderContextMenuRequested( const QPoint& ) ) );
    if ( m_rightview ) {
        connect( m_rightview, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ) );
        connect( m_rightview, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotRightHeaderContextMenuRequested( const QPoint& ) ) );

        connect( m_rightview->verticalScrollBar(), SIGNAL( valueChanged( int ) ), m_leftview->verticalScrollBar(), SLOT( setValue( int ) ) );
    
        connect( m_leftview, SIGNAL( moveAfterLastColumn( const QModelIndex & ) ), this, SLOT( slotToRightView( const QModelIndex & ) ) );
        connect( m_rightview, SIGNAL( moveBeforeFirstColumn( const QModelIndex & ) ), this, SLOT( slotToLeftView( const QModelIndex & ) ) );
    
        connect( m_leftview, SIGNAL( expanded( const QModelIndex & ) ), m_rightview, SLOT( expand( const QModelIndex & ) ) );
        connect( m_leftview, SIGNAL( collapsed( const QModelIndex & ) ), m_rightview, SLOT( collapse( const QModelIndex & ) ) );
    }
}
void DoubleTreeViewBase::updateColumnsHidden()
{
    m_leftview->updateColumnsHidden();
    if ( m_rightview ) {
        m_rightview->updateColumnsHidden();
    }
}
void DoubleTreeViewBase::hideColumns( const QList<int> &masterList, QList<int> slaveList )
{
    m_leftview->setColumnsHidden( masterList );
    if ( m_rightview ) {
        m_rightview->setColumnsHidden( slaveList );
    }
    // HACK: Bug in qt
    QTimer::singleShot( 100, this, SLOT( updateColumnsHidden() ) );
}

void DoubleTreeViewBase::slotToRightView( const QModelIndex &index )
{
    kDebug()<<index.column()<<endl;
    if ( m_rightview == 0 ) {
        return;
    }
    m_rightview->setFocus();
    QModelIndex nxt = m_rightview->nextColumn( index );
    if ( nxt.isValid() ) {
        m_selectionmodel->setCurrentIndex( nxt, QItemSelectionModel::NoUpdate );
    }
}

void DoubleTreeViewBase::slotToLeftView( const QModelIndex &index )
{
    kDebug()<<index.column();
    m_leftview->setFocus();
    QModelIndex prv = m_leftview->previousColumn( index );
    if ( prv.isValid() ) {
        m_selectionmodel->setCurrentIndex( prv, QItemSelectionModel::NoUpdate );
    }
}

void DoubleTreeViewBase::setModel( ItemModelBase *model )
{
    m_model = model;
    m_leftview->setModel( model );
    if ( m_rightview ) {
        m_rightview->setModel( model );
    }
    if ( m_selectionmodel ) {
        disconnect( m_selectionmodel, SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection & ) ), this, SLOT( slotSelectionChanged( const QItemSelection &, const QItemSelection & ) ) );
    
        disconnect( m_selectionmodel, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SIGNAL( currentChanged ( const QModelIndex &, const QModelIndex & ) ) );
    }
    m_selectionmodel = m_leftview->selectionModel();
    if ( m_rightview ) {
        m_rightview->setSelectionModel( m_selectionmodel );
    }
    
    connect( m_selectionmodel, SIGNAL( selectionChanged ( const QItemSelection &, const QItemSelection & ) ), this, SLOT( slotSelectionChanged( const QItemSelection &, const QItemSelection & ) ) );

    connect( m_selectionmodel, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SIGNAL( currentChanged ( const QModelIndex &, const QModelIndex & ) ) );
    
}

ItemModelBase *DoubleTreeViewBase::model() const
{
    return m_model;
}

void DoubleTreeViewBase::slotSelectionChanged( const QItemSelection &sel, const QItemSelection & )
{
    emit selectionChanged( sel.indexes() );
}

void DoubleTreeViewBase::setSelectionModel( QItemSelectionModel *model )
{
    m_leftview->setSelectionModel( model );
    if ( m_rightview ) {
        m_rightview->setSelectionModel( model );
    }
}

void DoubleTreeViewBase::setSelectionMode( QAbstractItemView::SelectionMode mode )
{
    m_leftview->setSelectionMode( mode );
    if ( m_rightview ) {
        m_rightview->setSelectionMode( mode );
    }
}

void DoubleTreeViewBase::setSelectionBehavior( QAbstractItemView::SelectionBehavior mode )
{
    m_leftview->setSelectionBehavior( mode );
    if ( m_rightview ) {
        m_rightview->setSelectionBehavior( mode );
    }
}

void DoubleTreeViewBase::setItemDelegateForColumn( int col, QAbstractItemDelegate * delegate )
{
    m_leftview->setItemDelegateForColumn( col, delegate );
    if ( m_rightview ) {
        m_rightview->setItemDelegateForColumn( col, delegate );
    }
}

void DoubleTreeViewBase::setEditTriggers( QAbstractItemView::EditTriggers mode )
{
    m_leftview->setEditTriggers( mode );
    if ( m_rightview ) {
        m_rightview->setEditTriggers( mode );
    }
}

QAbstractItemView::EditTriggers DoubleTreeViewBase::editTriggers() const
{
    return m_leftview->editTriggers();
}

void DoubleTreeViewBase::setStretchLastSection( bool mode )
{
    if ( m_rightview ) {
        m_rightview->header()->setStretchLastSection( mode );
    } else {
        m_leftview->header()->setStretchLastSection( mode );
    }
}

void DoubleTreeViewBase::edit( const QModelIndex &index )
{
    if ( ! m_leftview->isColumnHidden( index.column() ) ) {
        m_leftview->edit( index );
    } else if ( m_rightview && ! m_leftview->isColumnHidden( index.column() ) ) {
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
    kDebug();
    emit slaveHeaderContextMenuRequested( pos );
    emit headerContextMenuRequested( pos );
}

void DoubleTreeViewBase::slotLeftHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    emit masterHeaderContextMenuRequested( pos );
    emit headerContextMenuRequested( pos );
}

bool DoubleTreeViewBase::loadContext( const KoXmlElement &element )
{
    kDebug()<<endl;
    QList<int> lst1;
    QList<int> lst2;
    KoXmlElement e = element.namedItem( "master" ).toElement();
    if ( ! e.isNull() ) {
        m_leftview->loadContext( e );
/*        for ( int i = model()->columnCount() - 1; i >= 0; --i ) {
            if ( e.attribute( QString( "column-%1" ).arg( i ), "" ) == "hidden" ) {
                lst1 << i;
            }
        }*/
    }
    if ( m_rightview ) {
        e = element.namedItem( "slave" ).toElement();
        if ( ! e.isNull() ) {
            m_rightview->loadContext( e );
/*        for ( int i = model()->columnCount() - 1; i >= 0; --i ) {
            if ( e.attribute( QString( "column-%1" ).arg( i ), "" ) == "hidden" ) {
                lst2 << i;
            }
        }*/
        }
    }
//    hideColumns( lst1, lst2 );
    return true;
}

void DoubleTreeViewBase::saveContext( QDomElement &element ) const
{
    QDomElement e = element.ownerDocument().createElement( "master" );
    element.appendChild( e );
    m_leftview->saveContext( e );
    if ( m_rightview ) {
        e = element.ownerDocument().createElement( "slave" );
        element.appendChild( e );
        m_rightview->saveContext( e );
    }
}


} //namespace KPlato

#include "kptitemmodelbase.moc"

