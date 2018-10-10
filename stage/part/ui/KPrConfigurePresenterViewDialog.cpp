/* This file is part of the KDE project
 * Copyright (C) 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrConfigurePresenterViewDialog.h"

#include <QDesktopWidget>

#include <klocalizedstring.h>

#include "StageDebug.h"
#include "KPrDocument.h"

KPrConfigurePresenterViewDialog::KPrConfigurePresenterViewDialog( KPrDocument *document, QWidget *parent )
    : KoDialog( parent )
    , m_document( document )
{
    QWidget *widget = new QWidget( this );

    ui.setupUi( widget );

    QDesktopWidget desktop;
    int numScreens = desktop.numScreens();
    int primaryScreen = desktop.primaryScreen();
    for ( int i = 0; i < numScreens; i++ ) {
        if ( i == primaryScreen ) {
            ui.monitorComboBox->addItem( i18n( "Monitor %1 (primary)", i + 1 ) );
        }
        else {
            ui.monitorComboBox->addItem( i18n( "Monitor %1", i + 1 ) );
        }
    }

    if ( numScreens <= 1 )
        ui.presenterViewCheckBox->setEnabled( false );

    setMainWidget( widget );

    setCaption( i18n( "Configure Presenter View" ) );

    ui.monitorComboBox->setCurrentIndex( document->presentationMonitor() );
    ui.presenterViewCheckBox->setChecked(document->isPresenterViewEnabled());
}

int KPrConfigurePresenterViewDialog::presentationMonitor()
{
    return ui.monitorComboBox->currentIndex();
}

bool KPrConfigurePresenterViewDialog::presenterViewEnabled()
{
    return ( ui.presenterViewCheckBox->checkState() == Qt::Checked );
}
