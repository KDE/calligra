/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "createdialog.h"

// Kasten gui
#include <abstractmodeldatageneratorconfigeditor.h>
// KDE
#include <KLocale>
// Qt
#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QFont>


namespace Kasten2
{

CreateDialog::CreateDialog( AbstractModelDataGeneratorConfigEditor* configEditor, QWidget* parent )
  : KDialog( parent ),
    mConfigEditor( configEditor )
{
    setCaption( i18nc("@title:window","Create") );
    setButtons( Ok | Cancel );
    setButtonGuiItem( Ok, KGuiItem(i18nc("@action:button create the new document","&Create"), QLatin1String("document-new"),
                      i18nc("@info:tooltip",
                            "Create a new document with the generated data."),
                      i18nc("@info:whatsthis",
                            "If you press the <interface>Create</interface> button, "
                            "the data will be generated with the settings you entered above "
                            "and inserted in a new document.")) );
    setDefaultButton( Ok );

    QWidget* page = new QWidget( this );
    setMainWidget( page );

    QVBoxLayout* layout = new QVBoxLayout( page );
    layout->setMargin( 0 );

    QLabel* editorLabel = new QLabel( mConfigEditor->name() );
    QFont font = editorLabel->font();
    font.setBold( true );
    editorLabel->setFont( font );
    layout->addWidget( editorLabel );
    layout->addWidget( mConfigEditor );
    layout->addStretch();

    enableButtonOk( configEditor->isValid() );
    connect( configEditor, SIGNAL(validityChanged(bool)), SLOT(enableButtonOk(bool)) );
}

CreateDialog::~CreateDialog()
{
}

}
