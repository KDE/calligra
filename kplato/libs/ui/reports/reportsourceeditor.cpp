/* This file is part of the KDE project
  Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

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

#include "reportsourceeditor.h"
#include "report.h"

#include "kptnodeitemmodel.h"

#include <QDomElement>
#include <QTimer>

#include <kdebug.h>

namespace KPlato
{

ReportSourceEditor::ReportSourceEditor( QWidget *parent )
    : QWidget( parent )
{
    setupUi( this );

    QTimer::singleShot( 0, ui_source, SLOT( expandAll() ) ); // HACK how else?

}

void ReportSourceEditor::setModel( QAbstractItemModel *model )
{
    ui_source->setModel( model );
}

void ReportSourceEditor::slotSourceChanged( int index )
{
    emit sourceChanged( index );
}

void ReportSourceEditor::slotSelectFromChanged( int index )
{
    emit selectFromChanged( index );
}

QString ReportSourceEditor::selectFromTag( const QModelIndex &parent ) const
{
    QString tag;
    QAbstractItemModel *m = ui_source->model();
    for ( int row = 0; row < m->rowCount( parent ); ++row ) {
        QModelIndex idx = m->index( row, 0, parent );
        tag =  idx.data( Reports::TagRole ).toString();
        if ( tag == "select-from" ) {
            tag = checkedTag( idx );
        } else {
            // in case select-from is a child
            tag = selectFromTag( idx );
        }
        if ( ! tag.isEmpty() ) {
            break;
        }
    }
    return tag;
}

QString ReportSourceEditor::checkedTag( const QModelIndex &parent ) const
{
    QAbstractItemModel *m = ui_source->model();
    for ( int row = 0; row < m->rowCount( parent ); ++row ) {
        int r = m->index( row, 1, parent ).data( Qt::CheckStateRole ).toInt();
        if ( r == Qt::Checked ) {
            return m->index( row, 0, parent ).data( Reports::TagRole ).toString();
        }
    }
    return QString();
}

void ReportSourceEditor::setSourceData( const QDomElement &element )
{
    if ( element.tagName() != "data-source" ) {
        return;
    }
    QAbstractItemModel *m = ui_source->model();
    for ( int row = 0; row < m->rowCount(); ++row ) {
        QString name = m->index( row, 0 ).data( Reports::TagRole ).toString();
        if ( ! name.isEmpty() && element.hasAttribute( name ) ) {
            QModelIndex value = m->index( row, 1 );
            m->setData( value, element.attribute( name ) );
            setSourceData( element.firstChildElement( name ), m->index( row, 0 ) );
        }
    }
}

void ReportSourceEditor::setSourceData( const QDomElement &element, const QModelIndex &parent )
{
    if ( element.isNull() ) {
        return;
    }
    QAbstractItemModel *m = ui_source->model();
    for ( int row = 0; row < m->rowCount( parent ); ++row ) {
        QString name = m->index( row, 0, parent ).data( Reports::TagRole ).toString();
        if ( ! name.isEmpty() && element.hasAttribute( name ) ) {
            QModelIndex value = m->index( row, 1, parent );
            if ( m->flags( value ) & Qt::ItemIsUserCheckable ) {
                if ( element.attribute( name ) == "checked" ) {
                    m->setData( value, Qt::Checked, Qt::CheckStateRole );
                } // else nothing
            } else {
                m->setData( value, element.attribute( name ) );
            }
            setSourceData( element.firstChildElement( name ), m->index( row, 0, parent ) );
        }
    }
}

void ReportSourceEditor::sourceData( QDomElement &element ) const
{
    QDomElement e = element.ownerDocument().createElement( "data-source" );
    element.appendChild( e );
    QAbstractItemModel *m = ui_source->model();
    for ( int row = 0; row < m->rowCount(); ++row ) {
        QString attr = m->index( row, 0 ).data( Reports::TagRole ).toString();
        QString value = m->index( row, 1 ).data( Reports::TagRole ).toString();
        if ( ! attr.isEmpty() ) {
            e.setAttribute( attr, value );
            sourceData( e, m->index( row, 0 ) );
        }
    }
}

void ReportSourceEditor::sourceData( QDomElement &element, const QModelIndex &parent ) const
{
    QAbstractItemModel *m = ui_source->model();
    if ( ! parent.isValid() || ! m->hasChildren( parent ) ) {
        return;
    }
    QString tag = parent.data(  Reports::TagRole ).toString();
    if ( tag.isEmpty() ) {
        return;
    }
    QDomElement e = element.ownerDocument().createElement( tag );
    element.appendChild( e );
    for ( int row = 0; row < m->rowCount( parent ); ++row ) {
        QString attr = m->index( row, 0, parent ).data( Reports::TagRole ).toString();
        QModelIndex idx = m->index( row, 1, parent );
        QString value = m->index( row, 1, parent ).data( Reports::TagRole ).toString();
        e.setAttribute( attr, value );
    }
}

} //namespace KPlato

