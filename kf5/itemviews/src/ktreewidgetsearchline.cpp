/* This file is part of the KDE libraries
   Copyright (c) 2003 Scott Wheeler <wheeler@kde.org>
   Copyright (c) 2005 Rafal Rzepecki <divide@users.sourceforge.net>
   Copyright (c) 2006 Hamish Rodda <rodda@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "ktreewidgetsearchline.h"

#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QApplication>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QToolButton>
#include <QTreeWidget>

class KTreeWidgetSearchLine::Private
{
  public:
    Private( KTreeWidgetSearchLine *_q )
      : q( _q ),
        caseSensitive( Qt::CaseInsensitive ),
        keepParentsVisible( true ),
        canChooseColumns( true ),
        queuedSearches( 0 )
    {
    }

    KTreeWidgetSearchLine *q;
    QList<QTreeWidget *> treeWidgets;
    Qt::CaseSensitivity caseSensitive;
    bool keepParentsVisible;
    bool canChooseColumns;
    QString search;
    int queuedSearches;
    QList<int> searchColumns;

    void _k_rowsInserted(const QModelIndex & parent, int start, int end) const;
    void _k_treeWidgetDeleted( QObject *treeWidget );
    void _k_slotColumnActivated(QAction* action);
    void _k_slotAllVisibleColumns();
    void _k_queueSearch(const QString&);
    void _k_activateSearch();

    void checkColumns();
    void checkItemParentsNotVisible(QTreeWidget *treeWidget);
    bool checkItemParentsVisible(QTreeWidgetItem* item);
};

////////////////////////////////////////////////////////////////////////////////
// private slots
////////////////////////////////////////////////////////////////////////////////

// Hack to make a protected method public
class QTreeWidgetWorkaround : public QTreeWidget
{
  public:
    QTreeWidgetItem *itemFromIndex( const QModelIndex &index ) const
    {
      return QTreeWidget::itemFromIndex( index );
    }
};

void KTreeWidgetSearchLine::Private::_k_rowsInserted( const QModelIndex & parentIndex, int start, int end ) const
{
  QAbstractItemModel* model = qobject_cast<QAbstractItemModel*>( q->sender() );
  if ( !model )
    return;

  QTreeWidget* widget = 0L;
  foreach ( QTreeWidget* tree, treeWidgets )
    if ( tree->model() == model ) {
      widget = tree;
      break;
    }

  if ( !widget )
    return;

  QTreeWidgetWorkaround* widgetW = static_cast<QTreeWidgetWorkaround *>(widget);
  for (int i = start; i <= end; ++i) {
      if (QTreeWidgetItem *item = widgetW->itemFromIndex(model->index(i, 0, parentIndex))) {
          bool newHidden = !q->itemMatches(item, q->text());
          if (item->isHidden() != newHidden) {
              item->setHidden(newHidden);
              emit q->hiddenChanged(item, newHidden);
          }
      }
  }
}

void KTreeWidgetSearchLine::Private::_k_treeWidgetDeleted( QObject *object )
{
  treeWidgets.removeAll( static_cast<QTreeWidget *>( object ) );
  q->setEnabled( treeWidgets.isEmpty() );
}

void KTreeWidgetSearchLine::Private::_k_slotColumnActivated( QAction *action )
{
  if ( !action )
    return;

  bool ok;
  int column = action->data().toInt( &ok );

  if ( !ok )
    return;

  if ( action->isChecked() ) {
    if ( !searchColumns.isEmpty() ) {
      if ( !searchColumns.contains( column ) )
        searchColumns.append( column );

      if ( searchColumns.count() == treeWidgets.first()->header()->count() - treeWidgets.first()->header()->hiddenSectionCount() )
        searchColumns.clear();

    } else {
      searchColumns.append( column );
    }
  } else {
    if ( searchColumns.isEmpty() ) {
      QHeaderView* const header = treeWidgets.first()->header();

      for ( int i = 0; i < header->count(); i++ ) {
        if ( i != column && !header->isSectionHidden( i ) )
          searchColumns.append( i );
      }

    } else if ( searchColumns.contains( column ) ) {
      searchColumns.removeAll( column );
    }
  }

  q->updateSearch();
}

void KTreeWidgetSearchLine::Private::_k_slotAllVisibleColumns()
{
  if ( searchColumns.isEmpty() )
    searchColumns.append( 0 );
  else
    searchColumns.clear();

  q->updateSearch();
}

////////////////////////////////////////////////////////////////////////////////
// private methods
////////////////////////////////////////////////////////////////////////////////


void KTreeWidgetSearchLine::Private::checkColumns()
{
  canChooseColumns = q->canChooseColumnsCheck();
}

void KTreeWidgetSearchLine::Private::checkItemParentsNotVisible(QTreeWidget *treeWidget)
{
    for (QTreeWidgetItemIterator it(treeWidget); *it; ++it) {
        QTreeWidgetItem *item = *it;
        bool newHidden = !q->itemMatches(item, search);
        if (item->isHidden() != newHidden) {
            item->setHidden(newHidden);
            emit q->hiddenChanged(item, newHidden);
        }
    }
}

/** Check whether \p item, its siblings and their descendants should be shown. Show or hide the items as necessary.
 *
 *  \p item  The list view item to start showing / hiding items at. Typically, this is the first child of another item, or
 *              the first child of the list view.
 *  \return \c true if an item which should be visible is found, \c false if all items found should be hidden. If this function
 *             returns true and \p highestHiddenParent was not 0, highestHiddenParent will have been shown.
 */
bool KTreeWidgetSearchLine::Private::checkItemParentsVisible(QTreeWidgetItem *item)
{
    bool childMatch = false;
    for (int i = 0; i < item->childCount(); ++i) {
        childMatch |= checkItemParentsVisible(item->child(i));
    }

    // Should this item be shown? It should if any children should be, or if it matches.
    bool newHidden = !childMatch && !q->itemMatches(item, search);
    if (item->isHidden() != newHidden) {
        item->setHidden(newHidden);
        emit q->hiddenChanged(item, newHidden);
    }

    return !newHidden;
}


////////////////////////////////////////////////////////////////////////////////
// public methods
////////////////////////////////////////////////////////////////////////////////

KTreeWidgetSearchLine::KTreeWidgetSearchLine( QWidget *q, QTreeWidget *treeWidget )
  : QLineEdit( q ), d( new Private( this ) )
{
  connect( this, SIGNAL(textChanged(QString)),
           this, SLOT(_k_queueSearch(QString)) );

  #pragma message("KF5 TODO: enable clear button in QLineEdit once available")
  //setClearButtonShown( true );
  setTreeWidget( treeWidget );

  if ( !treeWidget ) {
      setEnabled( false );
  }
}

KTreeWidgetSearchLine::KTreeWidgetSearchLine( QWidget *q,
                                              const QList<QTreeWidget *> &treeWidgets )
  : QLineEdit( q ), d( new Private( this ) )
{
  connect( this, SIGNAL(textChanged(QString)),
           this, SLOT(_k_queueSearch(QString)) );

  #pragma message("KF5 TODO: enable clear button in QLineEdit once available")
  //setClearButtonShown( true );
  setTreeWidgets( treeWidgets );
}

KTreeWidgetSearchLine::~KTreeWidgetSearchLine()
{
  delete d;
}

Qt::CaseSensitivity KTreeWidgetSearchLine::caseSensitivity() const
{
  return d->caseSensitive;
}

QList<int> KTreeWidgetSearchLine::searchColumns() const
{
  if ( d->canChooseColumns )
    return d->searchColumns;
  else
    return QList<int>();
}

bool KTreeWidgetSearchLine::keepParentsVisible() const
{
  return d->keepParentsVisible;
}

QTreeWidget *KTreeWidgetSearchLine::treeWidget() const
{
  if ( d->treeWidgets.count() == 1 )
    return d->treeWidgets.first();
  else
    return 0;
}

QList<QTreeWidget *> KTreeWidgetSearchLine::treeWidgets() const
{
  return d->treeWidgets;
}


////////////////////////////////////////////////////////////////////////////////
// public slots
////////////////////////////////////////////////////////////////////////////////

void KTreeWidgetSearchLine::addTreeWidget( QTreeWidget *treeWidget )
{
  if ( treeWidget ) {
    connectTreeWidget( treeWidget );

    d->treeWidgets.append( treeWidget );
    setEnabled( !d->treeWidgets.isEmpty() );

    d->checkColumns();
  }
}

void KTreeWidgetSearchLine::removeTreeWidget( QTreeWidget *treeWidget )
{
  if ( treeWidget ) {
    int index = d->treeWidgets.indexOf( treeWidget );

    if ( index != -1 ) {
      d->treeWidgets.removeAt( index );
      d->checkColumns();

      disconnectTreeWidget( treeWidget );

      setEnabled( !d->treeWidgets.isEmpty() );
    }
  }
}

void KTreeWidgetSearchLine::updateSearch( const QString &pattern )
{
  d->search = pattern.isNull() ? text() : pattern;

  foreach ( QTreeWidget* treeWidget, d->treeWidgets )
    updateSearch( treeWidget );
}

void KTreeWidgetSearchLine::updateSearch( QTreeWidget *treeWidget )
{
  if ( !treeWidget || !treeWidget->topLevelItemCount() )
    return;


  // If there's a selected item that is visible, make sure that it's visible
  // when the search changes too (assuming that it still matches).

  QTreeWidgetItem *currentItem = treeWidget->currentItem();

  if ( d->keepParentsVisible )
    for ( int i = 0; i < treeWidget->topLevelItemCount(); ++i )
      d->checkItemParentsVisible( treeWidget->topLevelItem( i ) );
  else
    d->checkItemParentsNotVisible( treeWidget );

  if ( currentItem )
    treeWidget->scrollToItem( currentItem );

  emit searchUpdated(d->search);
}

void KTreeWidgetSearchLine::setCaseSensitivity( Qt::CaseSensitivity caseSensitive )
{
  if ( d->caseSensitive != caseSensitive ) {
    d->caseSensitive = caseSensitive;
    updateSearch();
  }
}

void KTreeWidgetSearchLine::setKeepParentsVisible( bool visible )
{
  if ( d->keepParentsVisible != visible ) {
    d->keepParentsVisible = visible;
    updateSearch();
  }
}

void KTreeWidgetSearchLine::setSearchColumns( const QList<int> &columns )
{
  if ( d->canChooseColumns )
    d->searchColumns = columns;
}

void KTreeWidgetSearchLine::setTreeWidget( QTreeWidget *treeWidget )
{
  setTreeWidgets( QList<QTreeWidget *>() );
  addTreeWidget( treeWidget );
}

void KTreeWidgetSearchLine::setTreeWidgets( const QList<QTreeWidget *> &treeWidgets )
{
  foreach ( QTreeWidget* treeWidget, d->treeWidgets )
    disconnectTreeWidget( treeWidget );

  d->treeWidgets = treeWidgets;

  foreach ( QTreeWidget* treeWidget, d->treeWidgets )
    connectTreeWidget( treeWidget );

  d->checkColumns();

  setEnabled( !d->treeWidgets.isEmpty() );
}

////////////////////////////////////////////////////////////////////////////////
// protected members
////////////////////////////////////////////////////////////////////////////////

bool KTreeWidgetSearchLine::itemMatches( const QTreeWidgetItem *item, const QString &pattern ) const
{
  if ( pattern.isEmpty() )
    return true;

  // If the search column list is populated, search just the columns
  // specified.  If it is empty default to searching all of the columns.

  if ( !d->searchColumns.isEmpty() ) {
    QList<int>::ConstIterator it = d->searchColumns.constBegin();
    for ( ; it != d->searchColumns.constEnd(); ++it ) {
      if ( *it < item->treeWidget()->columnCount() &&
           item->text( *it ).indexOf( pattern, 0, d->caseSensitive ) >= 0 )
        return true;
    }
  } else {
    for ( int i = 0; i < item->treeWidget()->columnCount(); i++) {
      if ( item->treeWidget()->columnWidth(i) > 0 &&
           item->text( i ).indexOf( pattern, 0, d->caseSensitive ) >= 0 )
        return true;
    }
  }

  return false;
}

void KTreeWidgetSearchLine::contextMenuEvent( QContextMenuEvent *event )
{
  QMenu *popup = QLineEdit::createStandardContextMenu();

  if ( d->canChooseColumns ) {
    popup->addSeparator();
    QMenu *subMenu = popup->addMenu( tr("Search Columns") );

    QAction* allVisibleColumnsAction = subMenu->addAction( tr("All Visible Columns"),
                                                           this, SLOT(_k_slotAllVisibleColumns()) );
    allVisibleColumnsAction->setCheckable( true );
    allVisibleColumnsAction->setChecked( !d->searchColumns.count() );
    subMenu->addSeparator();

    bool allColumnsAreSearchColumns = true;

    QActionGroup* group = new QActionGroup( popup );
    group->setExclusive( false );
    connect( group, SIGNAL(triggered(QAction*)), SLOT(_k_slotColumnActivated(QAction*)) );

    QHeaderView* const header = d->treeWidgets.first()->header();
    for ( int j = 0; j < header->count(); j++ ) {
      int i = header->logicalIndex( j );

      if ( header->isSectionHidden( i ) )
        continue;

      QString columnText = d->treeWidgets.first()->headerItem()->text( i );
      QAction* columnAction = subMenu->addAction( d->treeWidgets.first()->headerItem()->icon( i ), columnText );
      columnAction->setCheckable( true );
      columnAction->setChecked( d->searchColumns.isEmpty() || d->searchColumns.contains( i ) );
      columnAction->setData( i );
      columnAction->setActionGroup( group );

      if ( d->searchColumns.isEmpty() || d->searchColumns.indexOf( i ) != -1 )
        columnAction->setChecked( true );
      else
        allColumnsAreSearchColumns = false;
    }

    allVisibleColumnsAction->setChecked( allColumnsAreSearchColumns );

    // searchColumnsMenuActivated() relies on one possible "all" representation
    if ( allColumnsAreSearchColumns && !d->searchColumns.isEmpty() )
      d->searchColumns.clear();
  }

  popup->exec( event->globalPos() );
  delete popup;
}

void KTreeWidgetSearchLine::connectTreeWidget( QTreeWidget *treeWidget )
{
  connect( treeWidget, SIGNAL(destroyed(QObject*)),
           this, SLOT(_k_treeWidgetDeleted(QObject*)) );

  connect( treeWidget->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
           this, SLOT(_k_rowsInserted(QModelIndex,int,int)) );
}

void KTreeWidgetSearchLine::disconnectTreeWidget( QTreeWidget *treeWidget )
{
  disconnect( treeWidget, SIGNAL(destroyed(QObject*)),
              this, SLOT(_k_treeWidgetDeleted(QObject*)) );

  disconnect( treeWidget->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
              this, SLOT(_k_rowsInserted(QModelIndex,int,int)) );
}

bool KTreeWidgetSearchLine::canChooseColumnsCheck()
{
  // This is true if either of the following is true:

  // there are no listviews connected
  if ( d->treeWidgets.isEmpty() )
    return false;

  const QTreeWidget *first = d->treeWidgets.first();

  const unsigned int numcols = first->columnCount();
  // the listviews have only one column,
  if ( numcols < 2 )
    return false;

  QStringList headers;
  for ( unsigned int i = 0; i < numcols; ++i )
    headers.append( first->headerItem()->text( i ) );

  QList<QTreeWidget *>::ConstIterator it = d->treeWidgets.constBegin();
  for ( ++it /* skip the first one */; it != d->treeWidgets.constEnd(); ++it ) {
    // the listviews have different numbers of columns,
    if ( (unsigned int) (*it)->columnCount() != numcols )
      return false;

    // the listviews differ in column labels.
    QStringList::ConstIterator jt;
    unsigned int i;
    for ( i = 0, jt = headers.constBegin(); i < numcols; ++i, ++jt ) {
      Q_ASSERT( jt != headers.constEnd() );

      if ( (*it)->headerItem()->text( i ) != *jt )
        return false;
    }
  }

  return true;
}

bool KTreeWidgetSearchLine::event(QEvent *event) {

	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if(keyEvent->matches(QKeySequence::MoveToNextLine) || keyEvent->matches(QKeySequence::SelectNextLine) ||
		   keyEvent->matches(QKeySequence::MoveToPreviousLine) || keyEvent->matches(QKeySequence::SelectPreviousLine) ||
		   keyEvent->matches(QKeySequence::MoveToNextPage) ||  keyEvent->matches(QKeySequence::SelectNextPage) ||
		   keyEvent->matches(QKeySequence::MoveToPreviousPage) ||  keyEvent->matches(QKeySequence::SelectPreviousPage) ||
		   keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
		{
			QTreeWidget *first = d->treeWidgets.first();
			if(first) {
				QApplication::sendEvent(first, event);
				return true;
			}
		}
	}
	return QLineEdit::event(event);
}

////////////////////////////////////////////////////////////////////////////////
// protected slots
////////////////////////////////////////////////////////////////////////////////

void KTreeWidgetSearchLine::Private::_k_queueSearch( const QString &_search )
{
  queuedSearches++;
  search = _search;

  QTimer::singleShot( 200, q, SLOT(_k_activateSearch()) );
}

void KTreeWidgetSearchLine::Private::_k_activateSearch()
{
  --queuedSearches;

  if ( queuedSearches == 0 )
    q->updateSearch( search );
}

////////////////////////////////////////////////////////////////////////////////
// KTreeWidgetSearchLineWidget
////////////////////////////////////////////////////////////////////////////////

class KTreeWidgetSearchLineWidget::Private
{
  public:
    Private()
      : treeWidget( 0 ),
        searchLine( 0 )
    {
    }

    QTreeWidget *treeWidget;
    KTreeWidgetSearchLine *searchLine;
};

KTreeWidgetSearchLineWidget::KTreeWidgetSearchLineWidget( QWidget *parent, QTreeWidget *treeWidget )
  : QWidget( parent ), d( new Private )
{
  d->treeWidget = treeWidget;

  // can't call createWidgets directly because it calls virtual functions
  // that might not work if called directly from here due to how inheritance works
  QMetaObject::invokeMethod(this, "createWidgets", Qt::QueuedConnection);
}

KTreeWidgetSearchLineWidget::~KTreeWidgetSearchLineWidget()
{
  delete d;
}

KTreeWidgetSearchLine *KTreeWidgetSearchLineWidget::createSearchLine( QTreeWidget *treeWidget ) const
{
  return new KTreeWidgetSearchLine( const_cast<KTreeWidgetSearchLineWidget*>(this), treeWidget );
}

void KTreeWidgetSearchLineWidget::createWidgets()
{
  QLabel *label = new QLabel( tr("S&earch:"), this );

  searchLine()->show();

  label->setBuddy( d->searchLine );
  label->show();

  QHBoxLayout* layout = new QHBoxLayout( this );
  layout->setMargin( 0 );
  layout->addWidget( label );
  layout->addWidget( d->searchLine );
  setFocusProxy( searchLine() );
}

KTreeWidgetSearchLine *KTreeWidgetSearchLineWidget::searchLine() const
{
  if ( !d->searchLine )
    d->searchLine = createSearchLine( d->treeWidget );

  return d->searchLine;
}

#include "moc_ktreewidgetsearchline.cpp"
