/* This file is part of the KDE project
 * Copyright (C) 2009 Yannick Motta <yannick.motta@gmail.com>
 * Copyright (C) 2009-2010 Benjamin Port <port.benjamin@gmail.com>
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

#include "KPrHtmlExportDialog.h"

#include <QMessageBox>
#include <QPainter>
#include <QDir>
#include <QWebFrame>

#include <KLocale>
#include <KStandardDirs>
#include <KFileDialog>
#include <KArchiveDirectory>
#include <KZip>

#include "KPrHtmlExport.h"

KPrHtmlExportDialog::KPrHtmlExportDialog(const QList<KoPAPageBase*> &slides, const QString &title, const QString &author, QWidget *parent)
: KDialog(parent)
, m_allSlides(slides)
, m_title(title)
{
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    setMainWidget(widget);
    setCaption( i18n("Html Export"));
    setButtonText(Ok, i18n("Export"));
    ui.klineedit_title->setText(m_title);
    ui.klineedit_author->setText(author);

    connect(ui.kpushbuttonBrowseTemplate, SIGNAL(clicked()), this, SLOT(browserAction()));

    connect(&preview, SIGNAL(loadFinished(bool)), this, SLOT(renderPreview()));
    connect(ui.klineedit_title, SIGNAL(editingFinished()), this, SLOT(generatePreview()));
    connect(ui.klineedit_author, SIGNAL(editingFinished()), this, SLOT(generatePreview()));
    connect(ui.kListBox_slides, SIGNAL(currentRowChanged(int)), this, SLOT(generatePreview(int)));
    connect(ui.kcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(generatePreview()));
    connect(ui.kPushButton_selectAll, SIGNAL(clicked()), this, SLOT(checkAllItems()));
    connect(ui.kPushButton_deselectAll, SIGNAL(clicked()), this, SLOT(uncheckAllItems()));
    connect(ui.toolButton_previous, SIGNAL(clicked()), this, SLOT(generatePrevious()));
    connect(ui.toolButton_next, SIGNAL(clicked()), this, SLOT(generateNext()));
    connect(ui.kPushButton_Favorite, SIGNAL(clicked()), this, SLOT(favoriteAction()));
    connect(ui.kcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFavoriteButton()));

    this->updateFavoriteButton();
    this->frameToRender = 0;
    this->generateSlidesNames(slides);
    this->loadTemplatesList();;
    this->generatePreview();
}

QList<KoPAPageBase*> KPrHtmlExportDialog::checkedSlides()
{
    QList<KoPAPageBase*> selectedSlides;
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
    for (int i = 0; i < countItems; ++i ){
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

QStringList KPrHtmlExportDialog::slidesNames(){
    QStringList names;
    int countItems = ui.kListBox_slides->count();
    for (int i = 0; i < countItems; ++i) {
        if (ui.kListBox_slides->item(i)->checkState() == Qt::Checked) {
            names.append(ui.kListBox_slides->item(i)->text());
        }
    }
    return names;
}

KUrl KPrHtmlExportDialog::templateUrl()
{
    return KUrl(ui.kcombobox->itemData(ui.kcombobox->currentIndex()).toString());
}

void KPrHtmlExportDialog::generateSlidesNames(const QList<KoPAPageBase*> &slides)
{
    QString slideName;
    for (int i = 0; i < slides.count(); ++i) {
        if (slides.at(i)->name().isEmpty()) {
            slideName = i18n("Slide %1", QString::number(i + 1));
        }
        else {
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
    KStandardDirs std;
    QStringList dirs = std.findDirs("data", "kpresenter/templates/exportHTML/templates");
    for (QStringList::ConstIterator path=dirs.begin(); path!=dirs.end(); ++path) {
        QDir dir(*path);
        dir.setFilter(QDir::Files);
        QStringList entries = dir.entryList();
        for (QStringList::ConstIterator entry=entries.begin(); entry!=entries.end(); ++entry) {
            if (*entry != "." && *entry != "..") {
                QString name = *entry;
                if(name.endsWith(".zip", Qt::CaseInsensitive)){
                        name.resize(name.size() - 4);
                }
                ui.kcombobox->addItem(name, QVariant(QString(*path + *entry)));
            }
        }
    }
    ui.kcombobox->insertSeparator(ui.kcombobox->count());
}

void KPrHtmlExportDialog::addSelectedTemplateToFavorite()
{
    QString savePath = KStandardDirs::locateLocal("data", "kpresenter/templates/exportHTML/templates/");
    KUrl templatePath(ui.kcombobox->itemData(ui.kcombobox->currentIndex()).toString());
    savePath += templatePath.fileName();
    if(!(QFile::copy(templatePath.toLocalFile(), savePath))){
        QMessageBox::information(this, i18n("Error"), i18n("There is already a favorite file with this name"));
    }
    else {
        // Update list
        QString name(ui.kcombobox->itemText(ui.kcombobox->currentIndex()));
        // deactivate preview
        disconnect(ui.kcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(generatePreview()));
        ui.kcombobox->removeItem(ui.kcombobox->currentIndex());
        ui.kcombobox->insertItem(0, name, savePath);
        ui.kcombobox->setCurrentIndex(0);
        //reactivate preview
        connect( ui.kcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(generatePreview()));
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
    KFileDialog dialog(KUrl(), QString("*.zip"), this);
    if (dialog.exec() == QDialog::Accepted) {
        if (verifyZipFile(dialog.selectedFile())) {
            QString name (dialog.selectedUrl().fileName());
            if (name.endsWith(".zip", Qt::CaseInsensitive)) {
                    name.resize(name.size()-4);
            }
            ui.kcombobox->addItem(name, dialog.selectedFile());
            ui.kcombobox->setCurrentIndex(ui.kcombobox->count() - 1);
        }
        this->updateFavoriteButton();
    }
}

bool KPrHtmlExportDialog::verifyZipFile(const QString &zipLocalPath){
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
    if(!message.isEmpty()){
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
    QList<KoPAPageBase*> slides;
    QStringList slidesNames;
    slides.append(this->m_allSlides.at(frameToRender));
    slidesNames.append(ui.kListBox_slides->item(frameToRender)->text());

    KUrl url = previewGenerator.exportPreview(KPrHtmlExport::Parameter(this->templateUrl(), (KPrView*)this->parentWidget(), slides, KUrl(),
                                              this->author(), ui.klineedit_title->text(), slidesNames, false));
    preview.mainFrame()->load(url);
}

void KPrHtmlExportDialog::renderPreview()
{
    preview.currentFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    preview.currentFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    preview.setViewportSize(preview.currentFrame()->contentsSize());
    QImage image(preview.currentFrame()->contentsSize(), QImage::Format_ARGB32);
    QPainter painter(&image);

    preview.mainFrame()->render(&painter);
    painter.end();

    QImage thumbnail = image.scaled(ui.qLabel_preview->size(), Qt::KeepAspectRatio);
    ui.qLabel_preview->setPixmap(QPixmap::fromImage(thumbnail));
}

bool KPrHtmlExportDialog::selectedTemplateIsFavorite()
{
    QString templatePath(ui.kcombobox->itemData(ui.kcombobox->currentIndex()).toString());
    return templatePath.contains(KStandardDirs::locateLocal("data","kpresenter/templates/exportHTML"));
}

bool KPrHtmlExportDialog::selectedTemplateIsSystemFavorite()
{
    QString templatePath(ui.kcombobox->itemData(ui.kcombobox->currentIndex()).toString());
    QString dir;

    QStringList dirs(KStandardDirs().findDirs("data", "kpresenter/templates/exportHTML"));
    for (QStringList::ConstIterator path=dirs.begin(); path!=dirs.end(); ++path) {
        if (!path->contains(KStandardDirs::locateLocal("data","kpresenter/templates/exportHTML"))) {
            dir = *path;
        }
    }

    return (!dir.isNull()) && templatePath.contains(dir);
}

void KPrHtmlExportDialog::updateFavoriteButton()
{
    if (this->selectedTemplateIsFavorite() || this->selectedTemplateIsSystemFavorite()) {
        ui.kPushButton_Favorite->setText(i18n("Delete Template"));
        if (this->selectedTemplateIsSystemFavorite()) {
            ui.kPushButton_Favorite->setEnabled(false);
            ui.kPushButton_Favorite->setToolTip(i18n("You may not remove the templates provided with the application"));
        }
        else {
            ui.kPushButton_Favorite->setEnabled(true);
            ui.kPushButton_Favorite->setToolTip("");
        }
    }
    else {
        ui.kPushButton_Favorite->setText(i18n("Add Template"));
        ui.kPushButton_Favorite->setEnabled(true);
        ui.kPushButton_Favorite->setToolTip("");
    }
}

void KPrHtmlExportDialog::favoriteAction()
{
   if (this->selectedTemplateIsFavorite()) {
        int button = QMessageBox::question(this, i18n("Confirm remove"), i18n("Are you sure you want to remove \"%1\"?",
            ui.kcombobox->currentText()), QMessageBox::Yes | QMessageBox::No);

        if (button == QMessageBox::Yes){
            this->delSelectedTemplateFromFavorite();
        }
    }
    else {
        this->addSelectedTemplateToFavorite();
    }
}

#include "KPrHtmlExportDialog.moc"
