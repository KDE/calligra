/* This file is part of the KDE project
   Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

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

#include "ConfigProjectPanel.h"

#include "calligraplansettings.h"


#include <kactioncollection.h>

#include <QFileDialog>

namespace KPlato
{

ConfigProjectPanel::ConfigProjectPanel( QWidget *parent )
    : ConfigProjectPanelImpl( parent )
{
}

//-----------------------------
ConfigProjectPanelImpl::ConfigProjectPanelImpl(QWidget *p )
    : QWidget(p)
{

    setupUi(this);

    initDescription();

    connect(resourceFileBrowseBtn, SIGNAL(clicked()), this, SLOT(resourceFileBrowseBtnClicked()));
    connect(projectsPlaceBrowseBtn, SIGNAL(clicked()), this, SLOT(projectsPlaceBrowseBtnClicked()));

}

void ConfigProjectPanelImpl::resourceFileBrowseBtnClicked()
{
    QFileDialog dialog(this, tr("Shared resources file"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilters(QStringList()<<tr("Plan file (*.plan)"));
    if (dialog.exec()) {
        kcfg_SharedResourcesFile->setText(dialog.selectedFiles().value(0));
    }
}

void ConfigProjectPanelImpl::projectsPlaceBrowseBtnClicked()
{
    QFileDialog dialog(this, tr("Shared projects place"));
    dialog.setFileMode(QFileDialog::Directory);
    if (dialog.exec()) {
        kcfg_SharedProjectsPlace->setText(dialog.directory().absolutePath());
    }
}

void ConfigProjectPanelImpl::initDescription()
{
    toolbar->setToolButtonStyle( Qt::ToolButtonIconOnly );

    KActionCollection *collection = new KActionCollection( this ); //krazy:exclude=tipsandthis
    kcfg_ProjectDescription->setRichTextSupport( KRichTextWidget::SupportBold |
                                            KRichTextWidget::SupportItalic |
                                            KRichTextWidget::SupportUnderline |
                                            KRichTextWidget::SupportStrikeOut |
                                            KRichTextWidget::SupportChangeListStyle |
                                            KRichTextWidget::SupportAlignment |
                                            KRichTextWidget::SupportFormatPainting );

    collection->addActions(kcfg_ProjectDescription->createActions());

    toolbar->addAction( collection->action( "format_text_bold" ) );
    toolbar->addAction( collection->action( "format_text_italic" ) );
    toolbar->addAction( collection->action( "format_text_underline" ) );
    toolbar->addAction( collection->action( "format_text_strikeout" ) );
    toolbar->addSeparator();

    toolbar->addAction( collection->action( "format_list_style" ) );
    toolbar->addSeparator();

    toolbar->addAction( collection->action( "format_align_left" ) );
    toolbar->addAction( collection->action( "format_align_center" ) );
    toolbar->addAction( collection->action( "format_align_right" ) );
    toolbar->addAction( collection->action( "format_align_justify" ) );
    toolbar->addSeparator();

//    toolbar->addAction( collection->action( "format_painter" ) );

    kcfg_ProjectDescription->append( "" );
    kcfg_ProjectDescription->setReadOnly( false );
    kcfg_ProjectDescription->setOverwriteMode( false );
    kcfg_ProjectDescription->setLineWrapMode( KTextEdit::WidgetWidth );
    kcfg_ProjectDescription->setTabChangesFocus( true );

}


}  //KPlato namespace
