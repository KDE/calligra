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

#include "mainwindow.h"

#include <QApplication>
#include <QResizeEvent>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>

#include "colorimageprovider.h"
#include "constants.h"
#include "settings.h"
#include "palettecolorsmodel.h"
#include "palettemodel.h"
#include "presetmodel.h"
#include "presetimageprovider.h"

class MainWindow::Private
{
    public:
        QDeclarativeView* view;
        Constants* constants;
};

MainWindow::MainWindow( QWidget* parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags ), d( new Private )
{
    qApp->setActiveWindow( this );

    setAttribute( Qt::WA_AcceptTouchEvents );

    d->constants = new Constants( this );

    QDeclarativeView* view = new QDeclarativeView();
    view->engine()->addImageProvider(QLatin1String("presetthumb"), new PresetImageProvider);
    view->engine()->addImageProvider(QLatin1String("color"), new ColorImageProvider);
    view->rootContext()->setContextProperty( "PaletteModel", new PaletteModel( this ) );
    view->rootContext()->setContextProperty( "PaletteColorsModel", new PaletteColorsModel( this ) );
    view->rootContext()->setContextProperty( "PresetsModel", new PresetModel( this ) );
    view->rootContext()->setContextProperty( "Constants", d->constants );
    view->rootContext()->setContextProperty( "Settings", new Settings( this ) );
    view->setSource( QUrl( "qrc:/main.qml" ) );
    view->setResizeMode( QDeclarativeView::SizeRootObjectToView );
    setCentralWidget( view );
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    d->constants->setGridWidth( event->size().width() / d->constants->gridColumns() );
    d->constants->setGridHeight( event->size().height() / d->constants->gridRows() );
    QWidget::resizeEvent(event);
}

MainWindow::~MainWindow()
{
    delete d;
}

#include "mainwindow.moc"
