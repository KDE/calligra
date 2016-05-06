/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
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

#include "mct/MctWidget.h"
#include "ui_MctWidget.h"

#include "MctRevisionManager.h"
#include "mctmergemanagerdialog.h"

#include <QMessageBox>
#include <KWDocument.h>
#include <changetracker/KoChangeTracker.h>
#include <QDebug>
#include "MctMain.h"
#include "MctStaticData.h"

MctWidget::MctWidget(QWidget *parent, KWDocument *document, QString fileUrl) :
    QWidget(parent),
    ui(new Ui::MctWidget),
    document(document),
    fileUrl(fileUrl)
{
    ui->setupUi(this);

    ui->labelRevision->setText("0");

    setWindowModality(Qt::WindowModal);

    //Change Tracking kapcsolas
    connect(this, SIGNAL(start()), this, SLOT(startChangeTracking()));

    // Managerek megnyitasa
    connect(this, SIGNAL(openMergeManager()), this, SLOT(mergeManager()));
    connect(this, SIGNAL(openRevisionManager()), this, SLOT(revisionManager()));

    connect(this, SIGNAL(clearComboBox()), ui->comboRevision, SLOT(clear()));
    connect(this, SIGNAL(sendMctStatus(bool)), this, SLOT(updateMctStatus(bool)));

    mct = NULL;

    if (this->fileUrl != "" && MctStaticData::hasMctSupport(this->fileUrl)){
        isFileSaved = true;
        startChangeTracking();
    }
}

MctWidget::~MctWidget()
{
    delete ui;
    if(mct) {
        delete mct;
    }
}

void MctWidget::on_buttonClose_clicked()
{
    this->hide();
}

void MctWidget::enableUi(bool enable)
{
    isEnabled = enable;

    ui->buttonStart->setEnabled(!enable);
    ui->buttonStop->setEnabled(enable);

    ui->buttonCreate->setEnabled(enable);
    ui->buttonRedo->setEnabled(enable);
    ui->comboRevision->setEnabled(enable);
    ui->buttonRevisionManager->setEnabled(enable);
    ui->buttonMergeManager->setEnabled(enable);

    ui->lineAuthor->setEnabled(enable);
    ui->lineComment->setEnabled(enable);
}

bool MctWidget::status()
{
    return isEnabled;
}

void MctWidget::on_buttonStart_clicked()
{
    fileUrl = document->localFilePath();
    isFileSaved = (fileUrl != "");

    if (!isFileSaved) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("Warning"), tr("Document need to be saved first"));
        if (reply == QMessageBox::Ok) {
            this->hide();
            emit save();
        }
    } else if (checkExtension()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("Warning"), tr("Document extension need to be odt for start mct"));
        if (reply == QMessageBox::Ok) {
            this->hide();
            emit saveAs(true);
        }
    } else {
        emit start();   // Start Change Tracking
    }
}

void MctWidget::startChangeTracking()
{
    KoChangeTracker *ct = document->resourceManager()->resource(KoText::ChangeTracker).value<KoChangeTracker*>();
    //ct->setRecordChanges(true);
    //ct->setDisplayChanges(true);

    enableUi(true);

    if(!mct) {
        mct = new MctMain(document, fileUrl);
    }

    emit sendMctStatus(status());
    connect(mct, SIGNAL(adjustListOfRevisions()), this, SLOT(adjustListOfRevisions()));
    adjustListOfRevisions();

    //ui->labelRevision->setStyleSheet("QLabel { color : green; }");

    qDebug() << "ON";
}

void MctWidget::on_buttonStop_clicked()
{
    // TODO
    emit stopChangTracking();
    // backup keszites
}

void MctWidget::stopChangTracking()
{
    KoChangeTracker *ct = document->resourceManager()->resource(KoText::ChangeTracker).value<KoChangeTracker*>();
    //ct->setRecordChanges(false);
    //ct->setDisplayChanges(false);

    enableUi(false);
    emit sendMctStatus(status());

    //ui->labelRevision->setStyleSheet("QLabel { color : white; }");

    qDebug() << "OFF";
}

void MctWidget::updateFileStatus(bool enabled)
{
    this->isFileSaved = enabled;
}

void MctWidget::setFileUrl(QString url)
{
    fileUrl = url;
}

KWDocument *MctWidget::getKoDoc()
{
    return document;
}

bool MctWidget::checkExtension()
{
    QString ext = fileUrl;
    ext.remove(0, fileUrl.lastIndexOf('.') + 1);
    return (ext == "odt" ? false : true);
}

bool MctWidget::hasMct()
{
    return (mct) ? (true) : (false);
}

QString MctWidget::getFileUrl() const
{
    return fileUrl;
}

void MctWidget::on_buttonCreate_clicked()
{
    if(!mct) {
        return;
    }
    mct->createRevision(ui->lineAuthor->text(), ui->lineComment->text());
}

void MctWidget::on_buttonRedo_clicked()
{
    if(!mct) {
        return;
    }
    redoRevision(ui->comboRevision->currentText());
}

void MctWidget::on_buttonMergeManager_clicked()
{
    if (!isFileSaved) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("Warning"), tr("Document need to be saved first"));
        if (reply == QMessageBox::Ok)
            emit save();
    } else if (checkExtension()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("Warning"), tr("Document extension need to be odt"));
        if (reply == QMessageBox::Ok) {
            emit saveAs(true);
        }
    } else {
        emit mergeManager(); // Open Merge Manager
    }
}

void MctWidget::on_buttonRevisionManager_clicked()
{
    if (!isFileSaved) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("Warning"), tr("Document need to be saved first"));
        if (reply == QMessageBox::Ok)
            emit save();
    } else if (checkExtension()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("Warning"), tr("Document extension need to be odt"));
        if (reply == QMessageBox::Ok) {
            emit saveAs(true);
        }
    } else {
        emit revisionManager(); // Open Revision Manager
    }
}

void MctWidget::mergeManager()
{
    MctMergeManagerDialog* mergeMan = new MctMergeManagerDialog(nullptr, mct->getUndoop(), mct->getRedoop());
    connect(mergeMan, SIGNAL(adjustListOfRevisions()), this, SLOT(adjustListOfRevisions()));
    connect(mergeMan, SIGNAL(normalizeBuffer()), this, SLOT(normalizeBuffer()));
    mergeMan->show();
}

void MctWidget::revisionManager()
{
    MctRevisionManager *revman = new MctRevisionManager(nullptr, mct->getUndoop(), mct->getRedoop());
    connect(revman, SIGNAL(adjustListOfRevisions()), this, SLOT(adjustListOfRevisions()));
    connect(revman, SIGNAL(normalizeBuffer()), this, SLOT(normalizeBuffer()));
    connect(revman, SIGNAL(clearingProcess()), this, SLOT(clearRevisionHistory()));
    revman->show();

}

void MctWidget::adjustListOfRevisions()
{
    int undoRevCount = mct->getUndoRevCount();
    int redoRevCount = mct->getRedoRevCount();

    ui->labelRevision->setText(QString::number(undoRevCount));

     // reset the combobox
     emit clearComboBox();

    ui->comboRevision->setInsertPolicy(QComboBox::InsertAtBottom);
    ui->comboRevision->addItem(QString::number(0), 0);       // thats the baseline revision
    for (int i = 1; i <= undoRevCount; ++i) {
        ui->comboRevision->addItem(QString::number(i), i);
    }

    for (int i = 1; i <= redoRevCount; ++i) {
        ui->comboRevision->addItem(MctStaticData::REDOCHAR + QString::number(i), i);
    }
    ui->comboRevision->setCurrentIndex(ui->comboRevision->count() - 1);

    emit refreshRevisionManager();
}

void MctWidget::redoRevision(QString revisionIndex)
{
    mct->restoreRevision(revisionIndex);
}

void MctWidget::removeSelectedRevision(QString target)
{
    mct->removeRevision(target);
}

void MctWidget::clearRevisionHistory()
{
    qDebug() << "CLEAR ALL THE HISTORY!";
    mct->clearRevisionHistory();
}

void MctWidget::normalizeBuffer()
{
    mct->normailizeChangebuffer();
}

void MctWidget::updateMctStatus(bool enable)
{
    if (enable){;
        MctStaticData::instance()->setMctState(true);
        mct->connectSignals();
    }
    else {
        MctStaticData::instance()->setMctState(false);;
        mct->disconnectSignals();
    }
}

void MctWidget::createMctChange(KoShape &selection, MctChangeTypes changeType, const KUndo2MagicString title, QString fileUrl, ChangeAction action)
{
    if (!fileUrl.contains("/")){
        mct->createShapeMctChange(selection.shapeId(), selection.position(), selection, action);
    } else {
        mct->addGraphicMctChange(selection, changeType, title, fileUrl, action);
    }
}

void MctWidget::createRevisionOnSave()
{
    if(!mct || !isEnabled) {
        return;
    }

    QString fileUrlTmp = document->localFilePath();

    if (fileUrlTmp != fileUrl){
        fileUrl = fileUrlTmp;
        mct->documentSavedAs(fileUrl);
    }

    mct->createRevision("System", "revision on save");
}

/*void MctWidget::setBackgroundColor()
{
    int undoRevCount = mct->getUndoRevCount();
    int redoRevCount = mct->getRedoRevCount();

    if(undoRevCount == 0) {
        return;
    }

    QSettings settings("Multiracio","EuroOffice");
    int target = settings.value("revisionNumber", 1).toInt();

    int rev = undoRevCount - target;
    if(rev < 0 || target == -1) {
        rev = 0;
    }
    if(redoRevCount != 0) {
        mct->restoreRevision(QString::number(rev));
        if(rev == 0) {
            target = undoRevCount;
        }
        QString prev = MctStaticData::REDOCHAR + QString::number(target);
        mct->restoreRevision(prev);
    } else {
        mct->restoreRevision(QString::number(rev));
        mct->restoreRevision(ui->comboRevision->currentText());
    }
}*/
