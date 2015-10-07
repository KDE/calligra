/*
This file is part of the KDE project
Copyright (C) 2013 Jeremy Bourdiol <jerem.dante@gmail.com>

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
Boston, MA 02110-1301, USA
*/

//Author includes:
#include "CoverSelectionDialog.h"
#include "ui_CoverSelectionDialog.h"
#include "CoverImage.h"

#include <KoIcon.h>

//KDE includes:
#include <kmessagebox.h>

//QT includes
#include <QPushButton>
#include <QDebug>
#include <QDesktopServices>

// Calligra includes
#include <KoFileDialog.h>

CoverSelectionDialog::CoverSelectionDialog(CAuView *au, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CoverSelectionDialog)
{
    ui->setupUi(this);
    createActions();

    view = au;
    img = view->getCurrentCoverImage();
    refresh();
}

CoverSelectionDialog::~CoverSelectionDialog()
{
    delete ui;
}

void CoverSelectionDialog::createActions()
{
    //need to add that 'open..' button manualy, behaviour is not standard
    QPushButton *b = ui->coverSelectionButtonBox->addButton(i18n("Open..."),QDialogButtonBox::ActionRole);
    b->setIcon(koIcon("document-open"));
    connect(b, SIGNAL(clicked()), this, SLOT(open()));

    connect(ui->coverSelectionButtonBox->button(QDialogButtonBox::Reset),
            SIGNAL(clicked()), this, SLOT(reset()));

    connect(ui->coverSelectionButtonBox->button(QDialogButtonBox::Ok),
            SIGNAL(clicked()), this, SLOT(ok()));

}

void CoverSelectionDialog::open()
{
    //Here filter could be change if new extension(s) have to be added
    KoFileDialog dialog(this, KoFileDialog::OpenFile, "OpenDocument");
    dialog.setCaption(i18n("Open Cover"));
    dialog.setDefaultDir(QDesktopServices::storageLocation(QDesktopServices::PicturesLocation));
    dialog.setImageFilters();

    QString fileName = dialog.filename();

    if (!fileName.isEmpty()) {
        CoverImage cover;
        QPair<QString, QByteArray> tmp_img = cover.readCoverImage(fileName);
        if (tmp_img.second.isEmpty()) {
            KMessageBox::error(0, i18n("Import problem"), i18n("Import problem"));
            return;
        }
        img = tmp_img;
        refresh();
    }
}

void CoverSelectionDialog::resizeEvent(QResizeEvent*) {
    refresh();
}

void CoverSelectionDialog::refresh(){
    if(!img.second.isNull()) {
        QPixmap p = QPixmap::fromImage(QImage::fromData(img.second));
        ui->coverSelectLabel->setPixmap(p.scaled(ui->coverSelectLabel->size(),Qt::KeepAspectRatio));
    }
    /*
    else {
        qDebug() << "AUTHOR : nothing to display in cover preview";
    }*/
}

void CoverSelectionDialog::reset()
{
    ui->coverSelectLabel->setText("No cover selected yet");
    img = QPair<QString, QByteArray>();
}

void CoverSelectionDialog::ok()
{
    view->setCurrentCoverImage(img);
    //qDebug() << "AUTHOR : cover image saved";
}

