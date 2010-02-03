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

#include <kdebug.h>

namespace KPlato
{

ReportSourceEditor::ReportSourceEditor( QWidget *parent )
    : QWidget( parent )
{
    setupUi( this );
}

void ReportSourceEditor::setModel( QAbstractItemModel *model )
{
    ui_source->setModel( model );
}

void ReportSourceEditor::slotSourceChanged( int index )
{
    qDebug()<<"ReportSourceEditor::slotSourceChanged:"<<index;
    emit sourceChanged( index );
}

void ReportSourceEditor::slotSelectFromChanged( int index )
{
    qDebug()<<"ReportSourceEditor::slotSelectFromChanged:"<<index;
    emit selectFromChanged( index );
}

void ReportSourceEditor::setSourceData( const QDomElement &element )
{
    qDebug()<<"ReportSourceEditor::setSourceData:"<<element.tagName();
    if ( element.tagName() != "connection" ) {
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
    qDebug()<<"ReportSourceEditor::setSourceData (child):"<<element.tagName();
    QAbstractItemModel *m = ui_source->model();
    for ( int row = 0; row < m->rowCount( parent ); ++row ) {
        QString name = m->index( row, 0, parent ).data( Reports::TagRole ).toString();
        qDebug()<<"ReportSourceEditor::setSourceData (child): tag name="<<name;
        if ( ! name.isEmpty() && element.hasAttribute( name ) ) {
            QModelIndex value = m->index( row, 1, parent );
            if ( m->flags( value ) & Qt::ItemIsUserCheckable ) {
                if ( element.attribute( name ) == "checked" ) {
                    qDebug()<<"ReportSourceEditor::setSourceData (child)"<<value<<"set checked";
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
    QDomElement e = element.ownerDocument().createElement( "connection" );
    element.appendChild( e );
    QAbstractItemModel *m = ui_source->model();
    for ( int row = 0; row < m->rowCount(); ++row ) {
        QString attr = m->index( row, 0 ).data( Reports::TagRole ).toString();
        QString value = m->index( row, 1 ).data( Reports::TagRole ).toString();
        e.setAttribute( attr, value );
        sourceData( e, m->index( row, 0 ) );
    }
    qDebug()<<"ReportSourceEditor::sourceData:"<<e.text();
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

