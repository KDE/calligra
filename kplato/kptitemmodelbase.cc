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

#include "kptpart.h"
#include "kptproject.h"
#include "kptdurationwidget.h"

#include <QAbstractItemModel>
#include <QItemDelegate>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QTreeWidgetItem>

#include <kdebug.h>

namespace KPlato
{

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
    kDebug()<<k_funcinfo<<editor<<": "<<option.rect<<", "<<editor->sizeHint()<<endl;
    QRect r = option.rect;
    //r.setHeight(r.height() + 50);
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
    Duration value = Duration( index.model()->data(index, Role::DurationValue).toLongLong() / 1000 ); //FIXME??
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
    kDebug()<<k_funcinfo<<editor<<": "<<option.rect<<", "<<editor->sizeHint()<<endl;
    QRect r = option.rect;
    //r.setHeight(r.height() + 50);
    editor->setGeometry(r);
}

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
    kDebug()<<k_funcinfo<<editor<<": "<<option.rect<<", "<<editor->sizeHint()<<endl;
    QRect r = option.rect;
    //r.setHeight(r.height() + 50);
    editor->setGeometry(r);
}

//--------------------------
ItemModelBase::ItemModelBase( Part *part, QObject *parent )
    : QAbstractItemModel( parent ),
    m_part(part),
    m_project(0),
    m_readWrite( part->isReadWrite() )
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
    kDebug()<<k_funcinfo<<endl;
    emit layoutChanged();
}

void ItemModelBase::slotLayoutToBeChanged()
{
    kDebug()<<k_funcinfo<<endl;
    emit layoutAboutToBeChanged();
}

TreeViewBase::TreeViewBase( QWidget *parent )
    : QTreeView( parent ),
    m_arrowKeyNavigation( true )
{
}

int TreeViewBase::nextColumn( int col ) const
{
    int s = header()->visualIndex( col ) + 1;
    while ( isColumnHidden( header()->logicalIndex( s ) ) ) {
        if ( s >= header()->count() ) {
            kDebug()<<col<<": -1"<<endl;
            return -1;
        }
        ++s;
    }
    //kDebug()<<k_funcinfo<<col<<": next="<<header()->logicalIndex( s )<<", "<<s<<endl;
    return header()->logicalIndex( s );
}

int TreeViewBase::previousColumn( int col ) const
{
    int s = header()->visualIndex( col ) - 1;
    while ( isColumnHidden( header()->logicalIndex( s ) ) ) {
        if ( s < 0 ) {
            return -1;
        }
        --s;
    }
    //kDebug()<<k_funcinfo<<col<<": prev="<<header()->logicalIndex( s )<<", "<<s<<endl;
    return header()->logicalIndex( s );
}

/*!
    \reimp
 */
void TreeViewBase::keyPressEvent(QKeyEvent *event)
{
    kDebug()<<k_funcinfo<<event->key()<<", "<<m_arrowKeyNavigation<<endl;
    if ( !m_arrowKeyNavigation ) {
        QTreeView::keyPressEvent( event );
        return;
    }
    QModelIndex current = currentIndex();
    if ( current.isValid() ) {
        switch (event->key()) {
            case Qt::Key_Right: {
                int nxt = nextColumn( current.column() );
                if ( nxt != -1 ) {
                    QModelIndex i = model()->index( current.row(), nxt, current.parent() );
                    selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
                }
                event->accept();
                return;
                break;
            }
            case Qt::Key_Left: {
                int prv = previousColumn( current.column() );
                if ( prv != -1 ) {
                    QModelIndex i = model()->index( current.row(), prv, current.parent() );
                    selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
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


} //namespace KPlato

#include "kptitemmodelbase.moc"

