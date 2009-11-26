/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#include "kpttreecombobox.h"

#include <klocale.h>
#include <kdebug.h>

#include <QModelIndex>
#include <QTreeView>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QStylePainter>

namespace KPlato
{


//----------------------
TreeComboBox::TreeComboBox( QWidget *parent )
    : KComboBox( parent ),
    m_selectionmode( QAbstractItemView::ExtendedSelection )
{
    m_showcolumns << 0;
    m_showheader = false;
    
    updateView();
    
    connect( this, SIGNAL( activated( int ) ), SLOT( slotSelectionChanged() ) );
}

void TreeComboBox::updateView()
{
    QTreeView *v = new QTreeView();
    setView( v );
    v->setSelectionMode( m_selectionmode );
    // don't want to have mouseover select an item
    v->disconnect(SIGNAL(entered(QModelIndex)));

    QHeaderView *h = v->header();
    for ( int i = 0; i < h->count(); ++i ) {
        h->setSectionHidden( i, ! m_showcolumns.contains( i ) );
    }
    h->setVisible( m_showheader );
    v->setRootIsDecorated( false );
}

QTreeView *TreeComboBox::view() const
{
    return static_cast<QTreeView*>( KComboBox::view() );
}

void TreeComboBox::setModel( QAbstractItemModel *model )
{
    KComboBox::setModel( model );
    updateView();
}

QAbstractItemModel *TreeComboBox::model() const
{
    return KComboBox::model();
}

void TreeComboBox::setSelectionMode( QAbstractItemView::SelectionMode mode )
{
    m_selectionmode = mode;
    view()->setSelectionMode( mode );
}

void TreeComboBox::slotSelectionChanged()
{
    updateCurrentIndexes( view()->selectionModel()->selectedRows() );
}

void TreeComboBox::showPopup()
{
    QComboBox::showPopup();
    // now clean up things we want different
    QItemSelectionModel *sm = view()->selectionModel();
    sm->clearSelection();
    view()->setSelectionMode( m_selectionmode );
    view()->setSelectionBehavior( QAbstractItemView::SelectRows );
    foreach ( const QModelIndex &i, m_currentIndexes ) {
        if ( i.isValid() ) {
            sm->select( i, QItemSelectionModel::Select | QItemSelectionModel::Rows );
        }
    }
    if ( ! sm->selectedRows().contains( sm->currentIndex() ) ) {
        sm->setCurrentIndex( sm->selectedRows().value( 0 ), QItemSelectionModel::NoUpdate );
    }
}

void TreeComboBox::paintEvent( QPaintEvent *event )
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    // draw the combobox frame, focusrect and selected etc.
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    QStringList lst;
    foreach ( const QPersistentModelIndex &idx, m_currentIndexes ) {
        if ( idx.isValid() ) {
            lst << idx.data().toString();
        }
    }
    opt.currentText = lst.isEmpty() ? i18n( "None" ) : lst.join( "," );
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    // draw the icon and text
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

void TreeComboBox::setCurrentIndexes( const QModelIndexList &lst )
{
    m_currentIndexes.clear();
    foreach ( const QModelIndex &idx, lst ) {
        m_currentIndexes << QPersistentModelIndex( idx );
    }
}

void TreeComboBox::setCurrentIndexes( const QList<QPersistentModelIndex> &lst )
{
    m_currentIndexes = lst;
}

void TreeComboBox::updateCurrentIndexes( const QModelIndexList &lst )
{
    QList<QPersistentModelIndex> x;
    foreach ( const QModelIndex &idx, lst ) {
        x << QPersistentModelIndex( idx );
    }
    if ( x == m_currentIndexes ) {
        return;
    }
    m_currentIndexes = x;
    emit changed();
}

} //namespace KPlato

#include "kpttreecombobox.moc"

