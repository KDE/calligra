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

MctWidget::MctWidget(QWidget *parent, KWDocument *document, QString fileUrl)
    : QWidget(parent)
    , m_ui(new Ui::MctWidget)
    , m_document(document)
    , m_fileUrl(fileUrl)
{
    m_ui->setupUi(this);

    m_ui->labelRevision->setText("0");

    setWindowModality(Qt::WindowModal);

    //Change Tracking kapcsolas
    connect(this, SIGNAL(start()), this, SLOT(startChangeTracking()));

    // Managerek megnyitasa
    connect(this, SIGNAL(openMergeManager()), this, SLOT(mergeManager()));
    connect(this, SIGNAL(openRevisionManager()), this, SLOT(revisionManager()));

    connect(this, SIGNAL(clearComboBox()), m_ui->comboRevision, SLOT(clear()));
    connect(this, SIGNAL(sendMctStatus(bool)), this, SLOT(updateMctStatus(bool)));

    m_mct = NULL;

    if (m_fileUrl != "" && MctStaticData::hasMctSupport(m_fileUrl)){
        m_isFileSaved = true;
        startChangeTracking();
    }
}

MctWidget::~MctWidget()
{
    delete m_ui;
    if(m_mct) {
        delete m_mct;
    }
}

void MctWidget::on_buttonClose_clicked()
{
    hide();
}

void MctWidget::enableUi(bool enable)
{
    m_isEnabled = enable;

    m_ui->buttonStart->setEnabled(!enable);
    m_ui->buttonStop->setEnabled(enable);

    m_ui->buttonCreate->setEnabled(enable);
    m_ui->buttonRedo->setEnabled(enable);
    m_ui->comboRevision->setEnabled(enable);
    m_ui->buttonRevisionManager->setEnabled(enable);
    m_ui->buttonMergeManager->setEnabled(enable);

    m_ui->lineAuthor->setEnabled(enable);
    m_ui->lineComment->setEnabled(enable);
}

bool MctWidget::status()
{
    return m_isEnabled;
}

void MctWidget::on_buttonStart_clicked()
{
    m_fileUrl = m_document->localFilePath();
    m_isFileSaved = (m_fileUrl != "");

    if (!m_isFileSaved) {
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
    KoChangeTracker *ct = m_document->resourceManager()->resource(KoText::ChangeTracker).value<KoChangeTracker*>();
    //ct->setRecordChanges(true);
    //ct->setDisplayChanges(true);

    enableUi(true);

    if(!m_mct) {
        m_mct = new MctMain(m_document, m_fileUrl);
    }

    emit sendMctStatus(status());
    connect(m_mct, SIGNAL(adjustListOfRevisions()), this, SLOT(adjustListOfRevisions()));
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
    KoChangeTracker *ct = m_document->resourceManager()->resource(KoText::ChangeTracker).value<KoChangeTracker*>();
    //ct->setRecordChanges(false);
    //ct->setDisplayChanges(false);

    enableUi(false);
    emit sendMctStatus(status());

    //ui->labelRevision->setStyleSheet("QLabel { color : white; }");

    qDebug() << "OFF";
}

void MctWidget::updateFileStatus(bool enabled)
{
    m_isFileSaved = enabled;
}

void MctWidget::setFileUrl(QString url)
{
    m_fileUrl = url;
}

KWDocument *MctWidget::getKoDoc()
{
    return m_document;
}

bool MctWidget::checkExtension()
{
    QString ext = m_fileUrl;
    ext.remove(0, m_fileUrl.lastIndexOf('.') + 1);
    return (ext == "odt" ? false : true);
}

bool MctWidget::hasMct()
{
    return (m_mct) ? (true) : (false);
}

QString MctWidget::getFileUrl() const
{
    return m_fileUrl;
}

void MctWidget::on_buttonCreate_clicked()
{
    if(!m_mct) {
        return;
    }
    m_mct->createRevision(m_ui->lineAuthor->text(), m_ui->lineComment->text());
}

void MctWidget::on_buttonRedo_clicked()
{
    if(!m_mct) {
        return;
    }
    redoRevision(m_ui->comboRevision->currentText());
}

void MctWidget::on_buttonMergeManager_clicked()
{
    if (!m_isFileSaved) {
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
    if (!m_isFileSaved) {
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
    MctMergeManagerDialog* mergeMan = new MctMergeManagerDialog(nullptr, m_mct->undoop(), m_mct->redoop());
    connect(mergeMan, SIGNAL(adjustListOfRevisions()), this, SLOT(adjustListOfRevisions()));
    connect(mergeMan, SIGNAL(normalizeBuffer()), this, SLOT(normalizeBuffer()));
    mergeMan->show();
}

void MctWidget::revisionManager()
{
    MctRevisionManager *revman = new MctRevisionManager(nullptr, m_mct->undoop(), m_mct->redoop());
    connect(revman, SIGNAL(adjustListOfRevisions()), this, SLOT(adjustListOfRevisions()));
    connect(revman, SIGNAL(normalizeBuffer()), this, SLOT(normalizeBuffer()));
    connect(revman, SIGNAL(clearingProcess()), this, SLOT(clearRevisionHistory()));
    revman->show();

}

void MctWidget::adjustListOfRevisions()
{
    int undoRevCount = m_mct->undoRevCount();
    int redoRevCount = m_mct->redoRevCount();

    m_ui->labelRevision->setText(QString::number(undoRevCount));

     // reset the combobox
     emit clearComboBox();

    m_ui->comboRevision->setInsertPolicy(QComboBox::InsertAtBottom);
    m_ui->comboRevision->addItem(QString::number(0), 0);       // thats the baseline revision
    for (int i = 1; i <= undoRevCount; ++i) {
        m_ui->comboRevision->addItem(QString::number(i), i);
    }

    for (int i = 1; i <= redoRevCount; ++i) {
        m_ui->comboRevision->addItem(MctStaticData::REDOCHAR + QString::number(i), i);
    }
    m_ui->comboRevision->setCurrentIndex(m_ui->comboRevision->count() - 1);

    emit refreshRevisionManager();
}

void MctWidget::redoRevision(QString revisionIndex)
{
    m_mct->restoreRevision(revisionIndex);
}

void MctWidget::removeSelectedRevision(QString target)
{
    m_mct->removeRevision(target);
}

void MctWidget::clearRevisionHistory()
{
    qDebug() << "CLEAR ALL THE HISTORY!";
    m_mct->clearRevisionHistory();
}

void MctWidget::normalizeBuffer()
{
    m_mct->normailizeChangebuffer();
}

void MctWidget::updateMctStatus(bool enable)
{
    if (enable){;
        MctStaticData::instance()->setMctState(true);
        m_mct->connectSignals();
    }
    else {
        MctStaticData::instance()->setMctState(false);;
        m_mct->disconnectSignals();
    }
}

void MctWidget::createMctChange(KoShape &selection, MctChangeTypes changeType, const KUndo2MagicString title, QString fileUrl, ChangeAction action)
{
    if (!fileUrl.contains("/")){
        m_mct->createShapeMctChange(selection.shapeId(), selection.position(), selection, action);
    } else {
        m_mct->addGraphicMctChange(selection, changeType, title, fileUrl, action);
    }
}

void MctWidget::createRevisionOnSave()
{
    if(!m_mct || !m_isEnabled) {
        return;
    }

    QString fileUrlTmp = m_document->localFilePath();

    if (fileUrlTmp != m_fileUrl){
        m_fileUrl = fileUrlTmp;
        m_mct->documentSavedAs(m_fileUrl);
    }

    m_mct->createRevision("System", "revision on save");
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
