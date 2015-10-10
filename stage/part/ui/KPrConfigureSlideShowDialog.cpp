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

#include "KPrConfigureSlideShowDialog.h"

#include <QDesktopWidget>

#include <klocalizedstring.h>

#include "StageDebug.h"
#include "KPrDocument.h"
#include "KPrView.h"
#include "KPrCustomSlideShows.h"
#include "KPrViewModeSlidesSorter.h"

KPrConfigureSlideShowDialog::KPrConfigureSlideShowDialog( KPrDocument *document, KPrView *parent )
    : KoDialog( parent )
    , m_document( document )
    , m_view(parent)
{
    QWidget *widget = new QWidget( this );

    ui.setupUi( widget );

    ui.slidesComboBox->addItem( i18n( "All slides" ) );
    KPrCustomSlideShows *customSlideShows = document->customSlideShows();
    ui.slidesComboBox->addItems( customSlideShows->names() );

    setMainWidget( widget );

    setCaption( i18n( "Configure Slide Show" ) );

    QString activeCustomSlideShow = document->activeCustomSlideShow();
    if ( activeCustomSlideShow.isEmpty() ) {
        ui.slidesComboBox->setCurrentIndex( 0 );
    }
    else {
        QList<QString> customSlideShows = document->customSlideShows()->names();
        int index = customSlideShows.indexOf( activeCustomSlideShow ) + 1;
        Q_ASSERT( index < ui.slidesComboBox->count() );
        ui.slidesComboBox->setCurrentIndex( index );
    }

    connect( ui.editSlidesButton, SIGNAL(clicked()), this, SLOT(editCustomSlideShow()) );
}

QString KPrConfigureSlideShowDialog::activeCustomSlideShow() const
{
    if ( ui.slidesComboBox->currentIndex() != 0 ) {
        return ui.slidesComboBox->currentText();
    }

    return QString();
}

void KPrConfigureSlideShowDialog::editCustomSlideShow()
{
    m_view->slidesSorter()->setActiveCustomSlideShow(ui.slidesComboBox->currentIndex());
    m_view->showSlidesSorter();
    accept();
}
