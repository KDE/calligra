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

#include "kptdebug.h"

namespace KPlato
{

ReportSourceEditor::ReportSourceEditor( QWidget *parent )
    : QWidget( parent )
{
    setupUi( this );

    connect(ui_source, SIGNAL(currentIndexChanged(int)), SLOT(slotCurrentIndexChanged()));
}

void ReportSourceEditor::setModel( QAbstractItemModel *model )
{
    ui_source->setModel( model );
    ui_source->setCurrentIndex( 0 );
}

void ReportSourceEditor::slotCurrentIndexChanged()
{
    emit selectFromChanged( selectFromTag() );
}

QString ReportSourceEditor::selectFromTag() const
{
    QString tag;
    if ( ui_source->currentIndex() >= 0 ) {
        QAbstractItemModel *m = ui_source->model();
        tag = m->index( ui_source->currentIndex(), 0 ).data( Reports::TagRole ).toString();
    }
    return tag;
}

void ReportSourceEditor::setSourceData( const QDomElement &element )
{
    if ( element.tagName() != "data-source" ) {
        kDebug(planDbg())<<"no source element";
        ui_source->setCurrentIndex( 0 );
        return;
    }
    QString selectfrom = element.attribute( "select-from" );
    QAbstractItemModel *m = ui_source->model();
    for ( int row = 0; row < m->rowCount(); ++row ) {
        QString name = m->index( row, 0 ).data( Reports::TagRole ).toString();
        if ( ! name.isEmpty() && name == selectfrom ) {
            ui_source->setCurrentIndex( row );
            return;
        }
    }
    kDebug(planDbg())<<"no source";
    ui_source->setCurrentIndex( 0 );
}

void ReportSourceEditor::sourceData( QDomElement &element ) const
{
    QDomElement e = element.ownerDocument().createElement( "data-source" );
    element.appendChild( e );
    int row = ui_source->currentIndex();
    QAbstractItemModel *m = ui_source->model();
    e.setAttribute( "select-from", m->index( row, 0 ).data( Reports::TagRole ).toString() );
}

} //namespace KPlato

