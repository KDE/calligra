/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "exportdialog.h"

// Kasten gui
#include <abstractmodelexporterconfigeditor.h>
#include <abstractselectionview.h>
// KDE
#include <KGlobal>
#include <KLocale>
// Qt
#include <QtGui/QSplitter>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QLayout>


namespace Kasten2
{

ExportDialog::ExportDialog( const QString& remoteTypeName,
                            AbstractModelExporterConfigEditor* configEditor,
                            QWidget* parent )
  : KDialog( parent ),
    mConfigEditor( configEditor )
{
    setCaption( i18nc("@title:window","Export") );
    setButtons( Ok | Cancel );
    setButtonGuiItem( Ok, KGuiItem(i18nc("@action:button","&Export to File..."), QLatin1String("document-export"),
                      i18nc("@info:tooltip","Export the selected data to a file."),
                      i18nc("@info:whatsthis","If you press the <interface>Export to file</interface> "
                            "button, the selected data will be copied to a file "
                            "with the settings you entered above.")) );
    setDefaultButton( Ok );

    QSplitter* splitter = new QSplitter( this );

    setMainWidget( splitter );

    // config editor
    QWidget* editorPage = new QWidget( splitter );
    QVBoxLayout* editorPageLayout = new QVBoxLayout( editorPage );
    QLabel* editorLabel = new QLabel( remoteTypeName );
    QFont font = editorLabel->font();
    font.setBold( true );
    editorLabel->setFont( font );
    editorPageLayout->addWidget( editorLabel );
    editorPageLayout->addWidget( mConfigEditor );
    editorPageLayout->addStretch();

    splitter->addWidget( editorPage );
    splitter->setCollapsible( 0, false );

    mPreviewView = configEditor->createPreviewView();

    if( mPreviewView )
    {
        QGroupBox* previewBox = new QGroupBox( i18nc("@title:group","Preview"), this );
        splitter->addWidget( previewBox );

        QHBoxLayout* previewBoxLayout = new QHBoxLayout( previewBox );

        previewBoxLayout->addWidget( mPreviewView->widget() );
    }

    enableButtonOk( configEditor->isValid() );
    connect( configEditor, SIGNAL(validityChanged(bool)), SLOT(enableButtonOk(bool)) );
}

void ExportDialog::setData( AbstractModel* model, const AbstractModelSelection* selection )
{
    if( mPreviewView )
        mPreviewView->setData( model, selection );
}

ExportDialog::~ExportDialog()
{
    delete mPreviewView;
}

}
