/* This file is part of the KDE project
  Copyright (C) 2006 - 2007 Dag Andersen <danders@get2net.dk>

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
* Boston, MA 02110-1301, USA.
*/

#include "kptitemmodelbase.h"

#include "kptproject.h"
#include "kptdurationspinbox.h"

#include <QApplication>
#include <QComboBox>
#include <QKeyEvent>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QTimeEdit>
#include <QPainter>
#include <QToolTip>

#include <KComboBox>
#include <klineedit.h>
#include <kdebug.h>

namespace KPlato
{

//--------------------------------------
bool ItemDelegate::eventFilter(QObject *object, QEvent *event)
{
    QWidget *editor = ::qobject_cast<QWidget*>(object);
    if (!editor) {
        return false;
    }
    m_lastHint = Delegate::NoHint;
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *e = static_cast<QKeyEvent *>(event);
        if ( e->modifiers() & Qt::AltModifier && e->modifiers() & Qt::ControlModifier ) {
            switch ( e->key() ) {
                case Qt::Key_Left:
                    m_lastHint = Delegate::EditLeftItem;
                    emit commitData(editor);
                    emit closeEditor(editor, QAbstractItemDelegate::NoHint );
                    return true;
                case Qt::Key_Right:
                    m_lastHint = Delegate::EditRightItem;
                    emit commitData(editor);
                    emit closeEditor( editor, QAbstractItemDelegate::NoHint );
                    return true;
                case Qt::Key_Down:
                    m_lastHint = Delegate::EditDownItem;
                    emit commitData(editor);
                    emit closeEditor(editor, QAbstractItemDelegate::NoHint );
                    return true;
                case Qt::Key_Up:
                    m_lastHint = Delegate::EditUpItem;
                    emit commitData(editor);
                    emit closeEditor(editor, QAbstractItemDelegate::NoHint );
                    return true;
                default:
                    break;
            }
        }
    }
    return QStyledItemDelegate::eventFilter( object, event );
}

QSize ItemDelegate::sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    // 18 is a bit arbitrary, it gives (most?) editors a usable size
    QSize s = QStyledItemDelegate::sizeHint( option, index );
    return QSize( s.width(), qMax( s.height(), 18 ) );
}

//-----------------------------
ProgressBarDelegate::ProgressBarDelegate( QObject *parent )
 : ItemDelegate( parent )
{
}

ProgressBarDelegate::~ProgressBarDelegate()
{
}

void ProgressBarDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option,
 const QModelIndex &index ) const
{
    QStyle *style;

    QStyleOptionViewItemV4 opt = option;
    initStyleOption( &opt, index );

    style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawPrimitive( QStyle::PE_PanelItemViewItem, &opt, painter );

    if ( !( opt.state & QStyle::State_Editing ) ) {
        QStyleOptionProgressBar pbOption;
        pbOption.QStyleOption::operator=( option );
        initStyleOptionProgressBar( &pbOption, index );

        style->drawControl( QStyle::CE_ProgressBar, &pbOption, painter );
        // Draw focus, copied from qt
        if (opt.state & QStyle::State_HasFocus) {
            painter->save();
            QStyleOptionFocusRect o;
            o.QStyleOption::operator=( opt );
            o.rect = style->subElementRect( QStyle::SE_ItemViewItemFocusRect, &opt, opt.widget );
            o.state |= QStyle::State_KeyboardFocusChange;
            o.state |= QStyle::State_Item;
            QPalette::ColorGroup cg = ( opt.state & QStyle::State_Enabled )
                            ? QPalette::Normal : QPalette::Disabled;
            o.backgroundColor = opt.palette.color( cg, ( opt.state & QStyle::State_Selected )
                                            ? QPalette::Highlight : QPalette::Window );
            style->drawPrimitive( QStyle::PE_FrameFocusRect, &o, painter, opt.widget );
            //kDebug()<<"Focus"<<o.rect<<opt.rect<<pbOption.rect;
            painter->restore();
        }
    }
}

QSize ProgressBarDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QStyleOptionViewItemV4 opt = option;
    //  initStyleOption( &opt, index );

    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();

    QStyleOptionProgressBar pbOption;
    pbOption.QStyleOption::operator=( option );
    initStyleOptionProgressBar( &pbOption, index );

    return style->sizeFromContents( QStyle::CT_ProgressBar, &pbOption, QSize(), opt.widget );
}

void ProgressBarDelegate::initStyleOptionProgressBar( QStyleOptionProgressBar *option, const QModelIndex &index ) const
{
    option->rect.adjust( 0, 1, 0, -1 );
    option->maximum = 100;
    option->minimum = 0;
    option->progress = index.data().toInt();
    option->text = index.data().toString() + QChar::fromAscii( '%' );
    option->textAlignment = Qt::AlignCenter;
    option->textVisible = true;
}

QWidget *ProgressBarDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &, const QModelIndex & ) const
{
    Slider *slider = new Slider( parent );
    slider->setRange( 0, 100 );
    slider->setOrientation( Qt::Horizontal );
    //kDebug()<<slider->minimumSizeHint()<<slider->minimumSize();
    return slider;
}

void ProgressBarDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const
{
    QSlider *slider = static_cast<QSlider *>( editor );
    slider->setValue( index.data( Qt::EditRole ).toInt() );
}

void ProgressBarDelegate::setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    QSlider *slider = static_cast<QSlider *>( editor );
    model->setData( index, slider->value() );
}

void ProgressBarDelegate::updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & ) const
{
    editor->setGeometry( option.rect );
    //kDebug()<<editor->minimumSizeHint()<<editor->minimumSize()<<editor->geometry()<<editor->size();
}

Slider::Slider( QWidget *parent )
  : QSlider( parent )
{
    connect( this, SIGNAL(valueChanged(int)), this, SLOT(updateTip(int)) );
}

void Slider::updateTip( int value )
{
    QPoint p;
    p.setY( height() / 2 );
    p.setX( style()->sliderPositionFromValue ( minimum(), maximum(), value, width() ) );

    QString text = QString::fromAscii( "%1%" ).arg( value );
    QToolTip::showText( mapToGlobal( p ), text, this );
}

//--------------------------------------
// Hmmm, a bit hacky, but this makes it possible to use index specific editors...
SelectorDelegate::SelectorDelegate( QObject *parent )
    : ItemDelegate( parent )
{
}

QWidget *SelectorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index ) const
{
    switch ( index.model()->data( index, Role::EditorType ).toInt() ) {
        case Delegate::EnumEditor: {
            QComboBox *editor = new KComboBox(parent);
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
    : ItemDelegate( parent )
{
}

QWidget *EnumDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    QComboBox *editor = new KComboBox(parent);
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

//----------------------
DurationSpinBoxDelegate::DurationSpinBoxDelegate( QObject *parent )
    : ItemDelegate( parent )
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
//    dsb->setScales( index.model()->data( index, Role::DurationScales ) );
    dsb->setMinimumUnit( (Duration::Unit)(index.data( Role::Minimum ).toInt()) );
    dsb->setMaximumUnit( (Duration::Unit)(index.data( Role::Maximum ).toInt()) );
    dsb->setUnit( (Duration::Unit)( index.model()->data( index, Role::DurationUnit ).toInt() ) );
    dsb->setValue( index.model()->data( index, Qt::EditRole ).toDouble() );
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
    : ItemDelegate( parent )
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
    : ItemDelegate( parent )
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
    : ItemDelegate( parent )
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
    : ItemDelegate( parent )
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
ItemModelBase::ItemModelBase( QObject *parent )
    : QAbstractItemModel( parent ),
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
        foreach ( const QString &s, data->formats() ) {
            if ( mimeTypes().contains( s ) ) {
                return true;
            }
        }
    }
    return false;
}

bool ItemModelBase::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( role == Role::ReadWrite ) {
        setReadWrite( value.toBool() );
        return true;
    }
    return false;
}

} //namespace KPlato

#include "kptitemmodelbase.moc"

