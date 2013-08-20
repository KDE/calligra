/* This file is part of the KDE libraries
  Copyright (c) 2003 Scott Wheeler <wheeler@kde.org>
  Copyright (c) 2004 Gustavo Sverzut Barbieri <gsbarbieri@users.sourceforge.net>

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

#include "klistwidgetsearchline.h"

#include <QLineEdit>
#include <QListWidget>
#include <QApplication>
#include <QKeyEvent>
#include <QtCore/QEvent>
#include <QtCore/QTimer>


class KListWidgetSearchLine::KListWidgetSearchLinePrivate
{
public:
    KListWidgetSearchLinePrivate(KListWidgetSearchLine *parent) :
            q( parent ),
            listWidget( 0 ),
            caseSensitivity( Qt::CaseInsensitive ),
            activeSearch( false ),
            queuedSearches( 0 )
    {}

    void _k_listWidgetDeleted();
    void _k_queueSearch(const QString&);
    void _k_activateSearch();
    void _k_rowsInserted(const QModelIndex&, int, int);
    void _k_dataChanged(const QModelIndex&, const QModelIndex&);

    void init( QListWidget *listWidget = 0 );
    void updateHiddenState( int start, int end );

    KListWidgetSearchLine *q;
    QListWidget *listWidget;
    Qt::CaseSensitivity caseSensitivity;
    bool activeSearch;
    QString search;
    int queuedSearches;
};

/******************************************************************************
 * Public Methods                                                             *
 *****************************************************************************/
KListWidgetSearchLine::KListWidgetSearchLine( QWidget *parent, QListWidget *listWidget ) :
        QLineEdit( parent ),
        d( new KListWidgetSearchLinePrivate(this) )

{
    d->init( listWidget );
}

KListWidgetSearchLine::~KListWidgetSearchLine()
{
    clear(); // returning items back to listWidget
    delete d;
}

Qt::CaseSensitivity KListWidgetSearchLine::caseSensitive() const
{
    return d->caseSensitivity;
}

QListWidget *KListWidgetSearchLine::listWidget() const
{
    return d->listWidget;
}

/******************************************************************************
 * Public Slots                                                               *
 *****************************************************************************/
void KListWidgetSearchLine::updateSearch( const QString &s )
{
    d->search = s.isNull() ? text() : s;
    if( d->listWidget ) {
        d->updateHiddenState( 0, d->listWidget->count() - 1 );
    }
}

void KListWidgetSearchLine::clear()
{
    // Show items back to QListWidget
    if ( d->listWidget != 0 ) {
        for (int i = 0 ; i < d->listWidget->count(); ++i) {
            d->listWidget->item( i )->setHidden( false );
        }
    }

    d->search = QString();
    d->queuedSearches = 0;
    QLineEdit::clear();
}

void KListWidgetSearchLine::setCaseSensitivity( Qt::CaseSensitivity cs )
{
    d->caseSensitivity = cs;
}

void KListWidgetSearchLine::setListWidget( QListWidget *lw )
{
    if ( d->listWidget != 0 ) {
        disconnect( d->listWidget, SIGNAL(destroyed()),
                    this, SLOT(_k_listWidgetDeleted()) );
        d->listWidget->model()->disconnect( this );
    }

    d->listWidget = lw;

    if ( lw != 0 ) {
        connect( d->listWidget, SIGNAL(destroyed()),
                 this, SLOT(_k_listWidgetDeleted()) );
        connect( d->listWidget->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                 this, SLOT(_k_rowsInserted(QModelIndex,int,int)) );
        connect( d->listWidget->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                 this, SLOT(_k_dataChanged(QModelIndex,QModelIndex)) );
        setEnabled( true );
    } else
        setEnabled( false );
}

/******************************************************************************
 * Protected Methods                                                          *
 *****************************************************************************/
bool KListWidgetSearchLine::itemMatches( const QListWidgetItem *item,
        const QString &s ) const
{
    if ( s.isEmpty() )
        return true;

    if ( item == 0 )
        return false;

    return ( item->text().indexOf( s, 0,
                                   caseSensitive() ? Qt::CaseSensitive : Qt::CaseInsensitive ) >= 0 );
}

void KListWidgetSearchLine::KListWidgetSearchLinePrivate::init( QListWidget *_listWidget )
{
    listWidget = _listWidget;

    connect( q, SIGNAL(textChanged(QString)),
             q, SLOT(_k_queueSearch(QString)) );

    if ( listWidget != 0 ) {
        connect( listWidget, SIGNAL(destroyed()),
                 q, SLOT(_k_listWidgetDeleted()) );
        connect( listWidget->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                 q, SLOT(_k_rowsInserted(QModelIndex,int,int)) );
        connect( listWidget->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                 q, SLOT(_k_dataChanged(QModelIndex,QModelIndex)) );
        q->setEnabled( true );
    } else {
        q->setEnabled( false );
    }
    #pragma message("KF5 TODO: enable clear button in QLineEdit once available")
    //q->setClearButtonShown(true);
}

void KListWidgetSearchLine::KListWidgetSearchLinePrivate::updateHiddenState( int start, int end ) {
    if ( !listWidget ) {
        return;
    }

    QListWidgetItem *currentItem = listWidget->currentItem();

    // Remove Non-Matching items
    for( int index = start; index <= end; ++index ) {
        QListWidgetItem *item = listWidget->item(index);
        if ( ! q->itemMatches( item, search ) ) {
            item->setHidden( true );

            if ( item == currentItem ) {
                currentItem = 0; // It's not in listWidget anymore.
            }
        } else if ( item->isHidden() ) {
            item->setHidden( false );
        }
    }

    if ( listWidget->isSortingEnabled() ) {
        listWidget->sortItems();
    }

    if ( currentItem != 0 ) {
        listWidget->scrollToItem( currentItem );
    }
}

bool KListWidgetSearchLine::event(QEvent *event) {

	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if(keyEvent->matches(QKeySequence::MoveToNextLine) || keyEvent->matches(QKeySequence::SelectNextLine) ||
		   keyEvent->matches(QKeySequence::MoveToPreviousLine) || keyEvent->matches(QKeySequence::SelectPreviousLine) ||
		   keyEvent->matches(QKeySequence::MoveToNextPage) ||  keyEvent->matches(QKeySequence::SelectNextPage) ||
		   keyEvent->matches(QKeySequence::MoveToPreviousPage) ||  keyEvent->matches(QKeySequence::SelectPreviousPage)
		   )
		{
			if(d->listWidget) {
				QApplication::sendEvent(d->listWidget, event);
				return true;
			}
		}
		else if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return ) {

                     if(d->listWidget) {
                            QApplication::sendEvent(d->listWidget, event);
                            return true;
                     }
		}
	}
	return QLineEdit::event(event);
}
/******************************************************************************
 * Protected Slots                                                            *
 *****************************************************************************/
void KListWidgetSearchLine::KListWidgetSearchLinePrivate::_k_queueSearch( const QString &s )
{
    queuedSearches++;
    search = s;
    QTimer::singleShot( 200, q, SLOT(_k_activateSearch()) );
}

void KListWidgetSearchLine::KListWidgetSearchLinePrivate::_k_activateSearch()
{
    queuedSearches--;

    if ( queuedSearches <= 0 ) {
        q->updateSearch( search );
        queuedSearches = 0;
    }
}

/******************************************************************************
 * Private Slots                                                              *
 *****************************************************************************/
void KListWidgetSearchLine::KListWidgetSearchLinePrivate::_k_listWidgetDeleted()
{
    listWidget = 0;
    q->setEnabled( false );
}


void KListWidgetSearchLine::KListWidgetSearchLinePrivate::_k_rowsInserted( const QModelIndex &parent, int start, int end )
{
    if( parent.isValid() ) {
        return;
    }

    updateHiddenState( start, end );
}

void KListWidgetSearchLine::KListWidgetSearchLinePrivate::_k_dataChanged( const QModelIndex & topLeft, const QModelIndex & bottomRight )
{
    if( topLeft.parent().isValid() ) {
        return;
    }

    updateHiddenState( topLeft.row(), bottomRight.row() );
}



#include "moc_klistwidgetsearchline.cpp"
