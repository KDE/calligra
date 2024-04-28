/* This file is part of the KDE project
 * Copyright (C) Boudewijn Rempt <boud@valdyas.org>, (C) 2014
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFileDialogTester.h"
#include "ui_KoFileDialogTester.h"

#include <QApplication>
#include <QCheckBox>
#include <QListWidget>
#include <QRadioButton>
#include <QStandardPaths>

#include <KoFileDialog.h>

KoFileDialogTester::KoFileDialogTester(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::KoFileDialogTester)
{
    ui->setupUi(this);

    connect(ui->bnOpenFile, &QAbstractButton::clicked, this, &KoFileDialogTester::testOpenFile);
    connect(ui->bnOpenFiles, &QAbstractButton::clicked, this, &KoFileDialogTester::testOpenFiles);
    connect(ui->bnOpenDirectory, &QAbstractButton::clicked, this, &KoFileDialogTester::testOpenDirectory);
    connect(ui->bnImportFile, &QAbstractButton::clicked, this, &KoFileDialogTester::testImportFile);
    connect(ui->bnImportFiles, &QAbstractButton::clicked, this, &KoFileDialogTester::testImportFiles);
    connect(ui->bnImportDirectory, &QAbstractButton::clicked, this, &KoFileDialogTester::testImportDirectory);
    connect(ui->bnSaveFile, &QAbstractButton::clicked, this, &KoFileDialogTester::testSaveFile);

    m_nameFilters << QStringLiteral("Documents (*.odt *.doc *.txt)") << QStringLiteral("Images (*.png *.jpg *.jpeg)")
                  << QStringLiteral("Presentations (*.ppt *.odp)") << QStringLiteral("Patterns (*.pat *.jpg *.gif *.png *.tif *.xpm *.bmp)")
                  << QStringLiteral("Palettes (*.gpl *.pal *.act *.aco *.colors)");

    m_mimeFilter = QStringList() << QStringLiteral("image/x-exr") << QStringLiteral("image/openraster") << QStringLiteral("image/x-tga")
                                 << QStringLiteral("image/vnd.adobe.photoshop") << QStringLiteral("image/x-xcf") << QStringLiteral("image/x-portable-pixmap")
                                 << QStringLiteral("image/x-portable-graymap") << QStringLiteral("image/x-portable-bitmap") << QStringLiteral("image/png")
                                 << QStringLiteral("image/jp2") << QStringLiteral("image/tiff") << QStringLiteral("application/vnd.oasis.opendocument.graphics")
                                 << QStringLiteral("application/pdf") << QStringLiteral("image/jpeg") << QStringLiteral("image/bmp")
                                 << QStringLiteral("image/x-xpixmap") << QStringLiteral("image/gif") << QStringLiteral("image/x-xbitmap")
                                 << QStringLiteral("image/x-adobe-dng") << QStringLiteral("image/x-xfig") << QStringLiteral("image/svg+xml")
                                 << QStringLiteral("image/svg+xml-compressed") << QStringLiteral("image/x-eps") << QStringLiteral("image/eps")
                                 << QStringLiteral("application/eps") << QStringLiteral("application/x-eps") << QStringLiteral("application/postscript")
                                 << QStringLiteral("image/x-wmf") << QStringLiteral("application/x-karbon") << QStringLiteral("image/tiff")
                                 << QStringLiteral("application/vnd.oasis.opendocument.graphics");
}

KoFileDialogTester::~KoFileDialogTester()
{
    delete ui;
}

void KoFileDialogTester::testOpenFile()
{
    ui->listResults->clear();
    KoFileDialog dlg(this, KoFileDialog::OpenFile, ui->txtUniqueKey->text());
    dlg.setCaption(QStringLiteral("Testing: OpenFile"));
    dlg.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (ui->radioName->isChecked()) {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setNameFilters(m_nameFilters, m_nameFilters.last());
        } else {
            dlg.setNameFilters(m_nameFilters);
        }
    } else {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setMimeTypeFilters(m_mimeFilter, m_mimeFilter[4]);
        } else {
            dlg.setMimeTypeFilters(m_mimeFilter);
        }
    }

    if (ui->chkHideNameFilterDetailsOption->isChecked()) {
        dlg.setHideNameFilterDetailsOption();
    }

    QString url = dlg.filename();
    ui->listResults->addItem(url);
    ui->lblMime->setText(dlg.selectedMimeType());
    ui->txtFilter->setText(dlg.selectedNameFilter());
}

void KoFileDialogTester::testOpenFiles()
{
    ui->listResults->clear();
    KoFileDialog dlg(this, KoFileDialog::OpenFiles, ui->txtUniqueKey->text());
    dlg.setCaption(QStringLiteral("Testing: OpenFile"));
    dlg.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (ui->radioName->isChecked()) {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setNameFilters(m_nameFilters, m_nameFilters.last());
        } else {
            dlg.setNameFilters(m_nameFilters);
        }

    } else {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setMimeTypeFilters(m_mimeFilter, m_mimeFilter[4]);
        } else {
            dlg.setMimeTypeFilters(m_mimeFilter);
        }
    }

    if (ui->chkHideNameFilterDetailsOption->isChecked()) {
        dlg.setHideNameFilterDetailsOption();
    }

    QStringList urls = dlg.filenames();
    foreach (const QString &url, urls) {
        ui->listResults->addItem(url);
    }
    ui->lblMime->setText(dlg.selectedMimeType());
    ui->txtFilter->setText(dlg.selectedNameFilter());
}

void KoFileDialogTester::testOpenDirectory()
{
    ui->listResults->clear();
    KoFileDialog dlg(this, KoFileDialog::OpenDirectory, ui->txtUniqueKey->text());
    dlg.setCaption(QStringLiteral("Testing: OpenDirectory"));
    dlg.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (ui->radioName->isChecked()) {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setNameFilters(m_nameFilters, m_nameFilters.last());
        } else {
            dlg.setNameFilters(m_nameFilters);
        }

    } else {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setMimeTypeFilters(m_mimeFilter, m_mimeFilter[4]);
        } else {
            dlg.setMimeTypeFilters(m_mimeFilter);
        }
    }

    if (ui->chkHideNameFilterDetailsOption->isChecked()) {
        dlg.setHideNameFilterDetailsOption();
    }

    QString url = dlg.filename();
    ui->listResults->addItem(url);
    ui->lblMime->setText(dlg.selectedMimeType());
    ui->txtFilter->setText(dlg.selectedNameFilter());
}

void KoFileDialogTester::testImportFile()
{
    ui->listResults->clear();

    KoFileDialog dlg(this, KoFileDialog::ImportFile, ui->txtUniqueKey->text());
    dlg.setCaption(QStringLiteral("Testing: ImportFile"));
    dlg.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (ui->radioName->isChecked()) {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setNameFilters(m_nameFilters, m_nameFilters.last());
        } else {
            dlg.setNameFilters(m_nameFilters);
        }

    } else {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setMimeTypeFilters(m_mimeFilter, m_mimeFilter[4]);
        } else {
            dlg.setMimeTypeFilters(m_mimeFilter);
        }
    }

    if (ui->chkHideNameFilterDetailsOption->isChecked()) {
        dlg.setHideNameFilterDetailsOption();
    }

    QString url = dlg.filename();
    ui->listResults->addItem(url);
    ui->lblMime->setText(dlg.selectedMimeType());
    ui->txtFilter->setText(dlg.selectedNameFilter());
}

void KoFileDialogTester::testImportFiles()
{
    ui->listResults->clear();
    KoFileDialog dlg(this, KoFileDialog::ImportFiles, ui->txtUniqueKey->text());
    dlg.setCaption(QStringLiteral("Testing: ImportFiles"));
    dlg.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (ui->radioName->isChecked()) {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setNameFilters(m_nameFilters, m_nameFilters.last());
        } else {
            dlg.setNameFilters(m_nameFilters);
        }

    } else {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setMimeTypeFilters(m_mimeFilter, m_mimeFilter[4]);
        } else {
            dlg.setMimeTypeFilters(m_mimeFilter);
        }
    }

    if (ui->chkHideNameFilterDetailsOption->isChecked()) {
        dlg.setHideNameFilterDetailsOption();
    }

    QStringList urls = dlg.filenames();
    foreach (const QString &url, urls) {
        ui->listResults->addItem(url);
    }
    ui->lblMime->setText(dlg.selectedMimeType());
    ui->txtFilter->setText(dlg.selectedNameFilter());
}

void KoFileDialogTester::testImportDirectory()
{
    ui->listResults->clear();
    KoFileDialog dlg(this, KoFileDialog::ImportDirectory, ui->txtUniqueKey->text());
    dlg.setCaption(QStringLiteral("Testing: Import Directory"));
    dlg.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (ui->radioName->isChecked()) {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setNameFilters(m_nameFilters, m_nameFilters.last());
        } else {
            dlg.setNameFilters(m_nameFilters);
        }

    } else {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setMimeTypeFilters(m_mimeFilter, m_mimeFilter[4]);
        } else {
            dlg.setMimeTypeFilters(m_mimeFilter);
        }
    }

    if (ui->chkHideNameFilterDetailsOption->isChecked()) {
        dlg.setHideNameFilterDetailsOption();
    }

    QString url = dlg.filename();
    ui->listResults->addItem(url);
    ui->lblMime->setText(dlg.selectedMimeType());
    ui->txtFilter->setText(dlg.selectedNameFilter());
}

void KoFileDialogTester::testSaveFile()
{
    ui->listResults->clear();

    KoFileDialog dlg(this, KoFileDialog::SaveFile, ui->txtUniqueKey->text());
    dlg.setCaption(QStringLiteral("Testing: SaveFile"));
    dlg.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (ui->radioName->isChecked()) {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setNameFilters(m_nameFilters, m_nameFilters.last());
        } else {
            dlg.setNameFilters(m_nameFilters);
        }
    } else {
        if (ui->chkSetDefaultFilter->isChecked()) {
            dlg.setMimeTypeFilters(m_mimeFilter, m_mimeFilter[4]);
        } else {
            dlg.setMimeTypeFilters(m_mimeFilter);
        }
    }

    if (ui->chkHideNameFilterDetailsOption->isChecked()) {
        dlg.setHideNameFilterDetailsOption();
    }

    QString url = dlg.filename();
    ui->listResults->addItem(url);
    ui->lblMime->setText(dlg.selectedMimeType());
    ui->txtFilter->setText(dlg.selectedNameFilter());
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KoFileDialogTester w;
    w.show();

    return a.exec();
}
