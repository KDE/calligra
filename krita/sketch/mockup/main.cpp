/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <QtDeclarative>
#include <QDeclarativeView>
#include <QDeclarativeError>
#include <QApplication>
#include <QDesktopServices>
#include <QDebug>

#include "constants.h"

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    QDeclarativeView* view = new QDeclarativeView();
    view->rootContext()->setContextProperty("Constants", new Constants( view ) );
    view->setSource( QUrl( "qrc:/main.qml" ) );
    view->setResizeMode( QDeclarativeView::SizeRootObjectToView );
    view->show();

    if( view->errors().size() > 0 )
    {
        return 1;
    }
    else
    {
        return app.exec();
    }
}