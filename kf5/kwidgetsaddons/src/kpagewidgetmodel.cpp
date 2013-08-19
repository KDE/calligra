/*
    This file is part of the KDE Libraries

    Copyright (C) 2006 Tobias Koenig (tokoe@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB. If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kpagewidgetmodel.h"
#include "kpagewidgetmodel_p.h"

#include <QPointer>
#include <QWidget>

#include <QIcon>

class KPageWidgetItem::Private
{
  public:
    Private()
            : checkable(false), checked(false), enabled(true)
    {
    }

    ~Private()
    {
      delete widget;
      widget = 0;
    }

    QString name;
    QString header;
    QIcon icon;
    QPointer<QWidget> widget;
    bool checkable : 1;
    bool checked : 1;
    bool enabled : 1;
};

KPageWidgetItem::KPageWidgetItem( QWidget *widget )
  : QObject( 0 ), d( new Private )
{
  d->widget = widget;

  /**
   * Hide the widget, otherwise when the widget has this KPageView as
   * parent the widget is shown outside the QStackedWidget if the page
   * was not selected ( and reparented ) yet.
   */
  if ( d->widget )
    d->widget->hide();
}

KPageWidgetItem::KPageWidgetItem( QWidget *widget, const QString &name )
  : QObject( 0 ), d( new Private )
{
  d->widget = widget;
  d->name = name;

  /**
   * Hide the widget, otherwise when the widget has this KPageView as
   * parent the widget is shown outside the QStackedWidget if the page
   * was not selected ( and reparented ) yet.
   */
  if ( d->widget )
    d->widget->hide();
}

KPageWidgetItem::~KPageWidgetItem()
{
  delete d;
}

void KPageWidgetItem::setEnabled(bool enabled)
{
    d->enabled = enabled;
    if (d->widget) {
        d->widget->setEnabled(enabled);
    }
    emit changed();
}

bool KPageWidgetItem::isEnabled() const
{
    return d->enabled;
}

QWidget* KPageWidgetItem::widget() const
{
  return d->widget;
}

void KPageWidgetItem::setName( const QString &name )
{
  d->name = name;

  emit changed();
}

QString KPageWidgetItem::name() const
{
  return d->name;
}

void KPageWidgetItem::setHeader( const QString &header )
{
  d->header = header;

  emit changed();
}

QString KPageWidgetItem::header() const
{
  return d->header;
}

void KPageWidgetItem::setIcon( const QIcon &icon )
{
  d->icon = icon;

  emit changed();
}

QIcon KPageWidgetItem::icon() const
{
  return d->icon;
}

void KPageWidgetItem::setCheckable( bool checkable )
{
  d->checkable = checkable;

  emit changed();
}

bool KPageWidgetItem::isCheckable() const
{
  return d->checkable;
}

void KPageWidgetItem::setChecked( bool checked )
{
  d->checked = checked;

  emit toggled( checked );
  emit changed();
}

bool KPageWidgetItem::isChecked() const
{
  return d->checked;
}

PageItem::PageItem( KPageWidgetItem *pageWidgetItem, PageItem *parent )
  : mPageWidgetItem( pageWidgetItem ), mParentItem( parent )
{
}

PageItem::~PageItem()
{
  delete mPageWidgetItem;
  mPageWidgetItem = 0;

  qDeleteAll(mChildItems);
}

void PageItem::appendChild( PageItem *item )
{
  mChildItems.append( item );
}

void PageItem::insertChild( int row, PageItem *item )
{
  mChildItems.insert( row, item );
}

void PageItem::removeChild( int row )
{
  mChildItems.removeAt( row );
}

PageItem *PageItem::child( int row )
{
  return mChildItems.value( row );
}

int PageItem::childCount() const
{
  return mChildItems.count();
}

int PageItem::columnCount() const
{
  return 1;
}

PageItem *PageItem::parent()
{
  return mParentItem;
}

int PageItem::row() const
{
  if ( mParentItem )
    return mParentItem->mChildItems.indexOf( const_cast<PageItem*>(this) );

  return 0;
}

KPageWidgetItem* PageItem::pageWidgetItem() const
{
  return mPageWidgetItem;
}

PageItem *PageItem::findChild( const KPageWidgetItem *item )
{
  if ( mPageWidgetItem == item )
    return this;

  for ( int i = 0; i < mChildItems.count(); ++i ) {
    PageItem *pageItem = mChildItems[ i ]->findChild( item );
    if ( pageItem )
      return pageItem;
  }

  return 0;
}

void PageItem::dump( int indent )
{
  QString prefix;
  for ( int i = 0; i < indent; ++i )
    prefix.append( QStringLiteral(" ") );

  const QString name = ( mPageWidgetItem ? mPageWidgetItem->name() : QStringLiteral("root") );
  qDebug( "%s (%p)", qPrintable( QString( QStringLiteral("%1%2") ).arg( prefix, name ) ), (void*)this );
  for ( int i = 0; i < mChildItems.count(); ++i )
    mChildItems[ i ]->dump( indent + 2 );
}

KPageWidgetModel::KPageWidgetModel( QObject *parent )
    : KPageModel(*new KPageWidgetModelPrivate, parent)
{
}

KPageWidgetModel::~KPageWidgetModel()
{
}

int KPageWidgetModel::columnCount( const QModelIndex& ) const
{
  return 1;
}

QVariant KPageWidgetModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  PageItem *item = static_cast<PageItem*>( index.internalPointer() );

  if ( role == Qt::DisplayRole )
    return QVariant( item->pageWidgetItem()->name() );
  else if ( role == Qt::DecorationRole )
    return QVariant( item->pageWidgetItem()->icon() );
  else if ( role == HeaderRole )
    return QVariant( item->pageWidgetItem()->header() );
  else if ( role == WidgetRole )
    return QVariant::fromValue( item->pageWidgetItem()->widget() );
  else if ( role == Qt::CheckStateRole ) {
    if ( item->pageWidgetItem()->isCheckable() ) {
      return ( item->pageWidgetItem()->isChecked() ? Qt::Checked : Qt::Unchecked );
    } else
      return QVariant();
  } else
    return QVariant();
}

bool KPageWidgetModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
  if ( !index.isValid() )
    return false;

  if ( role != Qt::CheckStateRole )
    return false;

  PageItem *item = static_cast<PageItem*>( index.internalPointer() );
  if ( !item )
    return false;

  if ( !item->pageWidgetItem()->isCheckable() )
    return false;

  if ( value.toInt() == Qt::Checked )
    item->pageWidgetItem()->setChecked( true );
  else
    item->pageWidgetItem()->setChecked( false );

  return true;
}

Qt::ItemFlags KPageWidgetModel::flags( const QModelIndex &index ) const
{
  if ( !index.isValid() )
    return 0;

  Qt::ItemFlags flags = Qt::ItemIsSelectable;

  PageItem *item = static_cast<PageItem*>( index.internalPointer() );
  if ( item->pageWidgetItem()->isCheckable() )
    flags |= Qt::ItemIsUserCheckable;
  if (item->pageWidgetItem()->isEnabled()) {
      flags |= Qt::ItemIsEnabled;
  }

  return flags;
}

QModelIndex KPageWidgetModel::index( int row, int column, const QModelIndex &parent ) const
{
  PageItem *parentItem;

  if ( parent.isValid() )
    parentItem = static_cast<PageItem*>( parent.internalPointer() );
  else
        parentItem = d_func()->rootItem;

  PageItem *childItem = parentItem->child( row );
  if ( childItem )
    return createIndex( row, column, childItem );
  else
    return QModelIndex();
}

QModelIndex KPageWidgetModel::parent( const QModelIndex &index ) const
{
  if ( !index.isValid() )
    return QModelIndex();

  PageItem *item = static_cast<PageItem*>( index.internalPointer() );
  PageItem *parentItem = item->parent();

    if ( parentItem == d_func()->rootItem )
    return QModelIndex();
  else
    return createIndex( parentItem->row(), 0, parentItem );
}

int KPageWidgetModel::rowCount( const QModelIndex &parent ) const
{
  PageItem *parentItem;

  if ( !parent.isValid() )
        parentItem = d_func()->rootItem;
  else
    parentItem = static_cast<PageItem*>( parent.internalPointer() );

  return parentItem->childCount();
}

KPageWidgetItem* KPageWidgetModel::addPage( QWidget *widget, const QString &name )
{
  KPageWidgetItem *item = new KPageWidgetItem( widget, name );

  addPage( item );

  return item;
}

void KPageWidgetModel::addPage( KPageWidgetItem *item )
{
  emit layoutAboutToBeChanged();

    Q_D(KPageWidgetModel);
    connect(item, SIGNAL(changed()), this, SLOT(_k_itemChanged()));
    connect(item, SIGNAL(toggled(bool)), this, SLOT(_k_itemToggled(bool)));

  // The row to be inserted
  int row = d->rootItem->childCount();

  beginInsertRows(QModelIndex(), row, row);

  PageItem *pageItem = new PageItem( item, d->rootItem );
  d->rootItem->appendChild( pageItem );

  endInsertRows();

  emit layoutChanged();
}

KPageWidgetItem* KPageWidgetModel::insertPage( KPageWidgetItem *before, QWidget *widget, const QString &name )
{
  KPageWidgetItem *item = new KPageWidgetItem( widget, name );

  insertPage( before, item );

  return item;
}

void KPageWidgetModel::insertPage( KPageWidgetItem *before, KPageWidgetItem *item )
{
    PageItem *beforePageItem = d_func()->rootItem->findChild(before);
  if ( !beforePageItem ) {
    qDebug( "Invalid KPageWidgetItem passed!" );
    return;
  }

  emit layoutAboutToBeChanged();

    connect(item, SIGNAL(changed()), this, SLOT(_k_itemChanged()));
    connect(item, SIGNAL(toggled(bool)), this, SLOT(_k_itemToggled(bool)));

  PageItem *parent = beforePageItem->parent();
  // The row to be inserted
  int row = beforePageItem->row();

  QModelIndex index;
  if (parent != d_func()->rootItem) {
      index = createIndex( parent->row(), 0, parent );
  }

  beginInsertRows(index, row, row);

  PageItem *newPageItem = new PageItem( item, parent );
  parent->insertChild( row, newPageItem );

  endInsertRows();

  emit layoutChanged();
}

KPageWidgetItem* KPageWidgetModel::addSubPage( KPageWidgetItem *parent, QWidget *widget, const QString &name )
{
  KPageWidgetItem *item = new KPageWidgetItem( widget, name );

  addSubPage( parent, item );

  return item;
}

void KPageWidgetModel::addSubPage( KPageWidgetItem *parent, KPageWidgetItem *item )
{
    PageItem *parentPageItem = d_func()->rootItem->findChild(parent);
  if ( !parentPageItem ) {
    qDebug( "Invalid KPageWidgetItem passed!" );
    return;
  }

  emit layoutAboutToBeChanged();

    connect(item, SIGNAL(changed()), this, SLOT(_k_itemChanged()));
    connect(item, SIGNAL(toggled(bool)), this, SLOT(_k_itemToggled(bool)));

  // The row to be inserted
  int row = parentPageItem->childCount();

  QModelIndex index;
  if (parentPageItem != d_func()->rootItem) {
      index = createIndex( parentPageItem->row(), 0, parentPageItem );
  }

  beginInsertRows(index, row, row);

  PageItem *newPageItem = new PageItem( item, parentPageItem );
  parentPageItem->appendChild( newPageItem );

  endInsertRows();

  emit layoutChanged();
}

void KPageWidgetModel::removePage( KPageWidgetItem *item )
{
  if ( !item )
    return;

  Q_D(KPageWidgetModel);

  PageItem *pageItem = d->rootItem->findChild( item );
  if ( !pageItem ) {
    qDebug( "Invalid KPageWidgetItem passed!" );
    return;
  }

  emit layoutAboutToBeChanged();

    disconnect(item, SIGNAL(changed()), this, SLOT(_k_itemChanged()));
    disconnect(item, SIGNAL(toggled(bool)), this, SLOT(_k_itemToggled(bool)));

  PageItem *parentPageItem = pageItem->parent();
  int row = parentPageItem->row();

  QModelIndex index;
  if ( parentPageItem != d->rootItem )
    index = createIndex( row, 0, parentPageItem );

    beginRemoveRows(index, pageItem->row(), pageItem->row());

  parentPageItem->removeChild( pageItem->row() );
  delete pageItem;

  endRemoveRows();

  emit layoutChanged();
}

KPageWidgetItem *KPageWidgetModel::item(const QModelIndex &index) const
{
  if ( !index.isValid() )
    return 0;

  PageItem *item = static_cast<PageItem*>( index.internalPointer() );
  if ( !item )
    return 0;

  return item->pageWidgetItem();
}

QModelIndex KPageWidgetModel::index( const KPageWidgetItem *item ) const
{
  if ( !item )
    return QModelIndex();

    const PageItem *pageItem = d_func()->rootItem->findChild(item);
  if ( !pageItem ) {
    return QModelIndex();
  }

  return createIndex( pageItem->row(), 0, (void*)pageItem );
}

#include "moc_kpagewidgetmodel.cpp"
