/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Yannick Motta <yannick.motta@gmail.com>
 * SPDX-FileCopyrightText: 2009-2010 Benjamin Port <port.benjamin@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrHtmlExportDialog.h"

#include <QDir>
#include <QMessageBox>
#include <QPainter>
// #include <QWebFrame>
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>
#include <QUrl>

#include <KLocalizedString>
#include <kzip.h>

#include <KPrView.h>

#include "KPrHtmlExport.h"

KPrHtmlExportDialog::KPrHtmlExportDialog(const QList<KoPAPageBase *> &slides, const QString &title, const QString &author, QWidget *parent)
    : KoDialog(parent)
    , m_allSlides(slides)
    , m_title(title)
{
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    setMainWidget(widget);
    setCaption(i18n("Html Export"));
    setButtonText(Ok, i18n("Export"));
    ui.klineedit_title->setText(m_title);
    ui.klineedit_author->setText(author);

    connect(ui.pushbuttonBrowseTemplate, &QAbstractButton::clicked, this, &KPrHtmlExportDialog::browserAction);

    //     connect(&preview, SIGNAL(loadFinished(bool)), this, SLOT(renderPreview()));
    connect(ui.klineedit_title, &KLineEdit::editingFinished, this, [this]() {
        generatePreview();
    });
    connect(ui.klineedit_author, &KLineEdit::editingFinished, this, [this]() {
        generatePreview();
    });
    connect(ui.kListBox_slides, &QListWidget::currentRowChanged, this, &KPrHtmlExportDialog::generatePreview);
    connect(ui.kcombobox, QOverload<int>::of(&KComboBox::currentIndexChanged), this, &KPrHtmlExportDialog::generatePreview);
    connect(ui.pushButton_selectAll, &QAbstractButton::clicked, this, &KPrHtmlExportDialog::checkAllItems);
    connect(ui.pushButton_deselectAll, &QAbstractButton::clicked, this, &KPrHtmlExportDialog::uncheckAllItems);
    connect(ui.toolButton_previous, &QAbstractButton::clicked, this, &KPrHtmlExportDialog::generatePrevious);
    connect(ui.toolButton_next, &QAbstractButton::clicked, this, &KPrHtmlExportDialog::generateNext);
    connect(ui.pushButton_Favorite, &QAbstractButton::clicked, this, &KPrHtmlExportDialog::favoriteAction);
    connect(ui.kcombobox, QOverload<int>::of(&KComboBox::currentIndexChanged), this, &KPrHtmlExportDialog::updateFavoriteButton);

    this->updateFavoriteButton();
    this->frameToRender = 0;
    this->generateSlidesNames(slides);
    this->loadTemplatesList();
    this->generatePreview();
}

QList<KoPAPageBase *> KPrHtmlExportDialog::checkedSlides()
{
    QList<KoPAPageBase *> selectedSlides;
    int countItems = ui.kListBox_slides->count();
    for (int i = 0; i < countItems; ++i) {
        if (ui.kListBox_slides->item(i)->checkState() == Qt::Checked) {
            selectedSlides.append(this->m_allSlides.at(i));
        }
    }
    return selectedSlides;
}

void KPrHtmlExportDialog::checkAllItems()
{
    int countItems = ui.kListBox_slides->count();
    for (int i = 0; i < countItems; ++i) {
        ui.kListBox_slides->item(i)->setCheckState(Qt::Checked);
    }
}

void KPrHtmlExportDialog::uncheckAllItems()
{
    int countItems = ui.kListBox_slides->count();
    for (int i = 0; i < countItems; ++i) {
        ui.kListBox_slides->item(i)->setCheckState(Qt::Unchecked);
    }
}

QStringList KPrHtmlExportDialog::slidesNames()
{
    QStringList names;
    int countItems = ui.kListBox_slides->count();
    for (int i = 0; i < countItems; ++i) {
        if (ui.kListBox_slides->item(i)->checkState() == Qt::Checked) {
            names.append(ui.kListBox_slides->item(i)->text());
        }
    }
    return names;
}

QUrl KPrHtmlExportDialog::templateUrl()
{
    return QUrl::fromLocalFile(ui.kcombobox->itemData(ui.kcombobox->currentIndex()).toString());
}

void KPrHtmlExportDialog::generateSlidesNames(const QList<KoPAPageBase *> &slides)
{
    QString slideName;
    for (int i = 0; i < slides.count(); ++i) {
        if (slides.at(i)->name().isEmpty()) {
            slideName = i18n("Slide %1", QString::number(i + 1));
        } else {
            slideName = slides.at(i)->name();
        }
        QListWidgetItem *listItem = new QListWidgetItem(slideName);
        listItem->setFlags(listItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
        listItem->setCheckState(Qt::Checked);
        ui.kListBox_slides->addItem(listItem);
    }
}

void KPrHtmlExportDialog::loadTemplatesList()
{
    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                       QStringLiteral("calligrastage/templates/exportHTML/templates"),
                                                       QStandardPaths::LocateDirectory);
    for (QStringList::ConstIterator path = dirs.constBegin(); path != dirs.constEnd(); ++path) {
        QDir dir(*path);
        dir.setFilter(QDir::Files);
        QStringList entries = dir.entryList();
        for (QStringList::ConstIterator entry = entries.constBegin(); entry != entries.constEnd(); ++entry) {
            if (*entry != "." && *entry != "..") {
                QString name = *entry;
                if (name.endsWith(QLatin1String(".zip"), Qt::CaseInsensitive)) {
                    name.chop(4);
                }
                ui.kcombobox->addItem(name, QVariant(QString(*path + *entry)));
            }
        }
    }
    ui.kcombobox->insertSeparator(ui.kcombobox->count());
}

void KPrHtmlExportDialog::addSelectedTemplateToFavorite()
{
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/stage/templates/exportHTML/templates/");
    QUrl templatePath = QUrl::fromLocalFile(ui.kcombobox->itemData(ui.kcombobox->currentIndex()).toString());
    savePath += templatePath.fileName();
    if (!(QFile::copy(templatePath.toLocalFile(), savePath))) {
        QMessageBox::information(this, i18n("Error"), i18n("There is already a favorite file with this name"));
    } else {
        // Update list
        QString name(ui.kcombobox->itemText(ui.kcombobox->currentIndex()));
        // deactivate preview
        disconnect(ui.kcombobox, QOverload<int>::of(&KComboBox::currentIndexChanged), this, &KPrHtmlExportDialog::generatePreview);
        ui.kcombobox->removeItem(ui.kcombobox->currentIndex());
        ui.kcombobox->insertItem(0, name, savePath);
        ui.kcombobox->setCurrentIndex(0);
        // reactivate preview
        connect(ui.kcombobox, QOverload<int>::of(&KComboBox::currentIndexChanged), this, &KPrHtmlExportDialog::generatePreview);
    }
}

void KPrHtmlExportDialog::delSelectedTemplateFromFavorite()
{
    QString templatePath(ui.kcombobox->itemData(ui.kcombobox->currentIndex()).toString());
    QFile::remove(templatePath);
    // Update list
    ui.kcombobox->removeItem(ui.kcombobox->currentIndex());
}

void KPrHtmlExportDialog::browserAction()
{
    QFileDialog dialog(this);
    dialog.setMimeTypeFilters(QStringList(QStringLiteral("application/zip")));
    if (dialog.exec() == QDialog::Accepted) {
        const QString filePath = dialog.selectedFiles().first();
        if (verifyZipFile(filePath)) {
            QString name = QFileInfo(filePath).fileName();
            if (name.endsWith(QLatin1String(".zip"), Qt::CaseInsensitive)) {
                name.chop(4);
            }
            ui.kcombobox->addItem(name, filePath);
            ui.kcombobox->setCurrentIndex(ui.kcombobox->count() - 1);
        }
        this->updateFavoriteButton();
    }
}

bool KPrHtmlExportDialog::verifyZipFile(const QString &zipLocalPath)
{
    QString message;
    KZip zip(zipLocalPath);
    if (!zip.open(QIODevice::ReadOnly)) {
        message = i18n("Incorrect file, .zip only or corrupted zip");
    } else {
        // verify the file contains style.css
        QStringList filenameList = zip.directory()->entries();
        if (!filenameList.contains("style.css")) {
            message = i18n("Zip file need to contain style.css");
        }
    }
    if (!message.isEmpty()) {
        QMessageBox::information(this, i18n("Error"), message);
    }
    return message.isEmpty();
}

QString KPrHtmlExportDialog::title()
{
    return ui.klineedit_title->text();
}

QString KPrHtmlExportDialog::author()
{
    return ui.klineedit_author->text();
}

bool KPrHtmlExportDialog::openBrowser()
{
    return ui.checkBox_browser->checkState();
}

void KPrHtmlExportDialog::generatePrevious()
{
    generatePreview(--frameToRender);
}

void KPrHtmlExportDialog::generateNext()
{
    generatePreview(++frameToRender);
}

void KPrHtmlExportDialog::generatePreview(int item)
{
    if (item >= 0) {
        frameToRender = item;
    }

    ui.kListBox_slides->setCurrentRow(frameToRender);
    // refresh button status
    ui.toolButton_previous->setEnabled(frameToRender > 0);
    ui.toolButton_next->setEnabled(frameToRender < (m_allSlides.size() - 1));

    KPrHtmlExport previewGenerator;
    QList<KoPAPageBase *> slides;
    QStringList slidesNames;
    slides.append(this->m_allSlides.at(frameToRender));
    slidesNames.append(ui.kListBox_slides->item(frameToRender)->text());

    previewGenerator.exportPreview(KPrHtmlExport::Parameter(templateUrl(),
                                                            static_cast<KPrView *>(parentWidget()),
                                                            slides,
                                                            QUrl(),
                                                            this->author(),
                                                            ui.klineedit_title->text(),
                                                            slidesNames,
                                                            false));
    //     preview.mainFrame()->load(url);
}

void KPrHtmlExportDialog::renderPreview()
{
    //     preview.currentFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    //     preview.currentFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    //     preview.setViewportSize(preview.currentFrame()->contentsSize());
    //     QImage image(preview.currentFrame()->contentsSize(), QImage::Format_ARGB32);
    //     QPainter painter(&image);

    //     preview.mainFrame()->render(&painter);
    //     painter.end();

    //     QImage thumbnail = image.scaled(ui.qLabel_preview->size(), Qt::KeepAspectRatio);
    //     ui.qLabel_preview->setPixmap(QPixmap::fromImage(thumbnail));
}

bool KPrHtmlExportDialog::selectedTemplateIsFavorite()
{
    QString templatePath(ui.kcombobox->itemData(ui.kcombobox->currentIndex()).toString());
    return templatePath.startsWith(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
}

bool KPrHtmlExportDialog::selectedTemplateIsSystemFavorite()
{
    QString templatePath(ui.kcombobox->itemData(ui.kcombobox->currentIndex()).toString());
    QString dir;

    const QString writablePath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    const QStringList paths =
        QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("calligrastage/templates/exportHTML"), QStandardPaths::LocateDirectory);
    foreach (const QString &path, paths) {
        if (!path.startsWith(writablePath)) {
            dir = path;
        }
    }

    return (!dir.isNull()) && templatePath.contains(dir);
}

void KPrHtmlExportDialog::updateFavoriteButton()
{
    if (this->selectedTemplateIsFavorite() || this->selectedTemplateIsSystemFavorite()) {
        ui.pushButton_Favorite->setText(i18n("Delete Template"));
        if (this->selectedTemplateIsSystemFavorite()) {
            ui.pushButton_Favorite->setEnabled(false);
            ui.pushButton_Favorite->setToolTip(i18n("You may not remove the templates provided with the application"));
        } else {
            ui.pushButton_Favorite->setEnabled(true);
            ui.pushButton_Favorite->setToolTip("");
        }
    } else {
        ui.pushButton_Favorite->setText(i18n("Add Template"));
        ui.pushButton_Favorite->setEnabled(true);
        ui.pushButton_Favorite->setToolTip("");
    }
}

void KPrHtmlExportDialog::favoriteAction()
{
    if (this->selectedTemplateIsFavorite()) {
        int button = QMessageBox::question(this,
                                           i18n("Confirm remove"),
                                           i18n("Are you sure you want to remove \"%1\"?", ui.kcombobox->currentText()),
                                           QMessageBox::Yes | QMessageBox::No);

        if (button == QMessageBox::Yes) {
            this->delSelectedTemplateFromFavorite();
        }
    } else {
        this->addSelectedTemplateToFavorite();
    }
}
