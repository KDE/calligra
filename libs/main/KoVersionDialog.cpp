/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2006 Fredrik Edemar <f_edemar@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoVersionDialog.h"

#include "KoDocumentEntry.h"
#include "KoMainWindow.h"
#include "KoPart.h"

#include <QFile>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QList>
#include <QPushButton>
#include <QToolButton>
#include <QTreeWidget>

#include <KLocalizedString>
#include <KMessageBox>
#include <MainDebug.h>
#include <QTemporaryFile>

#include <QTextEdit>

KoVersionDialog::KoVersionDialog(QWidget *parent, KoDocument *doc)
    : KoDialog(parent)
{
    setCaption(i18n("Version"));
    setButtons(Close);
    setDefaultButton(Close);
    m_doc = doc;

    QWidget *page = new QWidget(this);
    setMainWidget(page);
    setModal(true);

    QGridLayout *grid1 = new QGridLayout(page);

    list = new QTreeWidget(page);
    list->setColumnCount(3);
    QStringList h;
    h.append(i18n("Date & Time"));
    h.append(i18n("Saved By"));
    h.append(i18n("Comment"));
    list->setHeaderLabels(h);

    updateVersionList();

    grid1->addWidget(list, 0, 0, 9, 1);

    m_pAdd = new QPushButton(i18n("&Add"), page);
    grid1->addWidget(m_pAdd, 1, 2);

    m_pRemove = new QPushButton(i18n("&Remove"), page);
    grid1->addWidget(m_pRemove, 2, 2);

    m_pModify = new QPushButton(i18n("&Modify"), page);
    grid1->addWidget(m_pModify, 3, 2);

    m_pOpen = new QPushButton(i18n("&Open"), page);
    grid1->addWidget(m_pOpen, 4, 2);

    connect(m_pRemove, &QAbstractButton::clicked, this, &KoVersionDialog::slotRemove);
    connect(m_pAdd, &QAbstractButton::clicked, this, &KoVersionDialog::slotAdd);
    connect(m_pOpen, &QAbstractButton::clicked, this, &KoVersionDialog::slotOpen);
    connect(m_pModify, &QAbstractButton::clicked, this, &KoVersionDialog::slotModify);

    updateButton();

    resize(600, 250);
}

KoVersionDialog::~KoVersionDialog() = default;

void KoVersionDialog::updateVersionList()
{
    list->clear();
    // add all versions to the tree widget
    QList<KoVersionInfo> versions = m_doc->versionList();
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < versions.size(); ++i) {
        QStringList l;
        l.append(versions.at(i).date.toString());
        l.append(versions.at(i).saved_by);
        l.append(versions.at(i).comment);
        items.append(new QTreeWidgetItem(l));
    }
    list->insertTopLevelItems(0, items);
}

void KoVersionDialog::updateButton()
{
#if 0
    bool state = (list->currentItem() >= 0);
    m_pRemove->setEnabled(state);
#endif
}

void KoVersionDialog::slotAdd()
{
    KoVersionModifyDialog *dlg = new KoVersionModifyDialog(this, nullptr);
    if (!dlg->exec()) {
        delete dlg;
        return;
    }

    if (!m_doc->addVersion(dlg->comment()))
        KMessageBox::error(this, i18n("A new version could not be added"));

    delete dlg;

    updateVersionList();
}

void KoVersionDialog::slotRemove()
{
    if (!list->currentItem())
        return;

    for (int i = 0; i < m_doc->versionList().size(); ++i) {
        if (m_doc->versionList().at(i).date.toString() == list->currentItem()->text(0)) {
            m_doc->versionList().takeAt(i);
            delete list->currentItem();
            return;
        }
    }
}

void KoVersionDialog::slotModify()
{
    if (!list->currentItem())
        return;

    KoVersionInfo *version = nullptr;
    for (int i = 0; i < m_doc->versionList().size(); ++i) {
        if (m_doc->versionList().at(i).date.toString() == list->currentItem()->text(0)) {
            version = &m_doc->versionList()[i];
            break;
        }
    }
    if (!version)
        return;

    KoVersionModifyDialog *dlg = new KoVersionModifyDialog(this, version);
    if (dlg->exec()) {
        version->comment = dlg->comment();
        list->currentItem()->setText(2, version->comment);
    }
    delete dlg;
}

void KoVersionDialog::slotOpen()
{
    if (!list->currentItem())
        return;

    KoVersionInfo *version = nullptr;
    for (int i = 0; i < m_doc->versionList().size(); ++i) {
        if (m_doc->versionList().at(i).date.toString() == list->currentItem()->text(0)) {
            version = &m_doc->versionList()[i];
            break;
        }
    }
    if (!version)
        return;

    QTemporaryFile tmp;
    tmp.setAutoRemove(false);
    tmp.open();
    tmp.write(version->data);
    tmp.flush();
    tmp.setPermissions(QFile::ReadUser);
    tmp.flush();

    if (!m_doc->documentPart()->mainWindows().isEmpty()) { // open the version in a new window if possible
        KoDocumentEntry entry = KoDocumentEntry::queryByMimeType(m_doc->nativeOasisMimeType());
        if (entry.isEmpty()) {
            entry = KoDocumentEntry::queryByMimeType(m_doc->nativeFormatMimeType());
        }
        Q_ASSERT(!entry.isEmpty());
        QString errorMsg;
        KoPart *part = entry.createKoPart(&errorMsg);
        if (!part) {
            if (!errorMsg.isEmpty())
                KMessageBox::error(nullptr, errorMsg);
            return;
        }
        KoMainWindow *mainWindow = part->createMainWindow();
        mainWindow->openDocument(QUrl::fromLocalFile(tmp.fileName()));
        mainWindow->show();
    } else {
        m_doc->openUrl(QUrl::fromUserInput(tmp.fileName()));
    }

    tmp.setAutoRemove(true);
    slotButtonClicked(Close);
}

KoVersionModifyDialog::KoVersionModifyDialog(QWidget *parent, KoVersionInfo *info)
    : KoDialog(parent)
{
    setCaption(i18n("Comment"));
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    setModal(true);

    QWidget *page = new QWidget(this);
    setMainWidget(page);

    QVBoxLayout *grid1 = new QVBoxLayout(page);

    QLabel *l = new QLabel(page);
    if (info)
        l->setText(i18n("Date: %1", info->date.toString()));
    else
        l->setText(i18n("Date: %1", QDateTime::currentDateTime().toString(Qt::ISODate)));
    grid1->addWidget(l);

    m_textEdit = new QTextEdit(page);
    if (info)
        m_textEdit->setText(info->comment);
    grid1->addWidget(m_textEdit);
}

QString KoVersionModifyDialog::comment() const
{
    return m_textEdit->toPlainText();
}
