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
#include "kptdurationspinbox.h"

#include <QAbstractItemModel>
#include <QItemDelegate>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QTreeWidgetItem>

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
    kDebug()<<k_funcinfo<<editor<<": "<<option.rect<<", "<<editor->sizeHint()<<endl;
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
    kDebug()<<k_funcinfo<<editor<<": "<<option.rect<<", "<<editor->sizeHint()<<endl;
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
    kDebug()<<k_funcinfo<<editor<<": "<<option.rect<<", "<<editor->sizeHint()<<endl;
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

