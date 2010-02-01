/*
 * KPlato Report Plugin
 * Copyright (C) 2010 by Dag Andersen <danders@get2net.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#ifndef KPLATO_REPORTSOURCEEDITORTOR_H
#define KPLATO_REPORTSOURCEEDITORTOR_H


#include "ui_reportsourceeditor.h"

#include <qwidget.h>

class QAbstractItemModel;
class QDomElement;

namespace KPlato
{

class ReportSourceEditor : public QWidget, public Ui::ReportSourceEditor
{
    Q_OBJECT
public:
    ReportSourceEditor( QWidget *parent );

    void setModel( QAbstractItemModel *model );
    
    void setSourceData( const QDomElement &element );
    void sourceData( QDomElement &element ) const;
    
protected:
    void setSourceData( const QDomElement &element, const QModelIndex &parent );
    void sourceData( QDomElement &element, const QModelIndex &parent ) const;

signals:
    void sourceChanged( int );
    void selectFromChanged( int );
    
private slots:
    void slotSourceChanged( int index );
    void slotSelectFromChanged( int );
};

}

#endif

