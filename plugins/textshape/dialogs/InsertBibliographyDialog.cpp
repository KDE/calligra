/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "InsertBibliographyDialog.h"

#include <BibliographyGenerator.h>
#include <KoBibliographyInfo.h>
#include <KoOdfBibliographyConfiguration.h>
#include <KoParagraphStyle.h>
#include <ToCBibGeneratorInfo.h>

#include <QDebug>
#include <QListWidgetItem>
#include <QMessageBox>

InsertBibliographyDialog::InsertBibliographyDialog(KoTextEditor *editor, QWidget *parent)
    : QDialog(parent)
    , m_editor(editor)
    , m_bibInfo(new KoBibliographyInfo())
{
    dialog.setupUi(this);

    connect(dialog.bibTypes, &QListWidget::currentTextChanged, this, &InsertBibliographyDialog::updateFields);
    connect(dialog.buttonBox, &QDialogButtonBox::accepted, this, &InsertBibliographyDialog::insert);
    connect(dialog.add, &QAbstractButton::clicked, this, &InsertBibliographyDialog::addField);
    connect(dialog.remove, &QAbstractButton::clicked, this, &InsertBibliographyDialog::removeField);
    connect(dialog.span, &QAbstractButton::clicked, this, &InsertBibliographyDialog::addSpan);
    connect(dialog.addedFields, &QListWidget::itemChanged, this, &InsertBibliographyDialog::spanChanged);

    /*  To do : handle tab stops
     */
    // connect(dialog.addTabStop,SIGNAL(clicked()),this,SLOT(insertTabStop()));
    // connect(dialog.removeTabStop,SIGNAL(clicked()),this,SLOT(removeTabStop()));

    dialog.addedFields->clear();
    dialog.availableFields->clear();
    m_bibInfo->m_entryTemplate = BibliographyGenerator::defaultBibliographyEntryTemplates();
    dialog.bibTypes->setCurrentRow(0, QItemSelectionModel::Select);
    show();
}

QString InsertBibliographyDialog::bibliographyType()
{
    return dialog.bibTypes->currentItem()->text().remove(' ').toLower();
}

void InsertBibliographyDialog::insert()
{
    m_bibInfo->m_indexTitleTemplate.text = dialog.title->text();
    m_editor->insertBibliography(m_bibInfo);
}

void InsertBibliographyDialog::updateFields()
{
    dialog.availableFields->clear();
    dialog.addedFields->clear();

    QSet<QString> addedFields;
    foreach (IndexEntry *entry, m_bibInfo->m_entryTemplate[bibliographyType()].indexEntries) {
        if (entry->name == IndexEntry::BIBLIOGRAPHY) {
            IndexEntryBibliography *bibEntry = static_cast<IndexEntryBibliography *>(entry);
            QListWidgetItem *bibItem = new QListWidgetItem(bibEntry->dataField, dialog.addedFields);
            addedFields.insert(bibEntry->dataField);
            bibItem->setData(Qt::UserRole, QVariant::fromValue<IndexEntry::IndexEntryName>(IndexEntry::BIBLIOGRAPHY));
        } else if (entry->name == IndexEntry::SPAN) {
            IndexEntrySpan *span = static_cast<IndexEntrySpan *>(entry);
            QListWidgetItem *spanField = new QListWidgetItem(span->text, dialog.addedFields);
            addedFields.insert(span->text);
            spanField->setData(Qt::UserRole, QVariant::fromValue<IndexEntry::IndexEntryName>(IndexEntry::SPAN));
            spanField->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
    }
    QSet<QString> availableFields =
        QSet<QString>(KoOdfBibliographyConfiguration::bibDataFields.begin(), KoOdfBibliographyConfiguration::bibDataFields.end()) - addedFields;

    foreach (const QString &field, availableFields) {
        new QListWidgetItem(field, dialog.availableFields);
    }
    dialog.availableFields->sortItems();
}

void InsertBibliographyDialog::addField()
{
    int row = dialog.availableFields->currentRow();

    if (row != -1) {
        disconnect(dialog.addedFields, &QListWidget::itemChanged, this, &InsertBibliographyDialog::spanChanged);

        QString newDataField = dialog.availableFields->takeItem(row)->text();
        QListWidgetItem *bibField = new QListWidgetItem(newDataField, dialog.addedFields);
        bibField->setData(Qt::UserRole, QVariant::fromValue<IndexEntry::IndexEntryName>(IndexEntry::BIBLIOGRAPHY));

        IndexEntryBibliography *newEntry = new IndexEntryBibliography(QString());
        newEntry->dataField = newDataField;

        m_bibInfo->m_entryTemplate[bibliographyType()].indexEntries.append(static_cast<IndexEntry *>(newEntry));
        connect(dialog.addedFields, &QListWidget::itemChanged, this, &InsertBibliographyDialog::spanChanged);
    }
}

void InsertBibliographyDialog::removeField()
{
    int row = dialog.addedFields->currentRow();

    if (row != -1) {
        if (dialog.addedFields->currentItem()->data(Qt::UserRole).value<IndexEntry::IndexEntryName>() == IndexEntry::BIBLIOGRAPHY) {
            new QListWidgetItem(dialog.addedFields->takeItem(row)->text(), dialog.availableFields);
            dialog.availableFields->sortItems();
        } else {
            dialog.availableFields->removeItemWidget(dialog.addedFields->takeItem(row));
        }

        m_bibInfo->m_entryTemplate[bibliographyType()].indexEntries.removeAt(row);
    }
}

void InsertBibliographyDialog::addSpan()
{
    QString spanText = (dialog.addedFields->count() == 1) ? QString(":") : QString(",");
    QListWidgetItem *spanField = new QListWidgetItem(spanText, dialog.addedFields);
    spanField->setData(Qt::UserRole, QVariant::fromValue<IndexEntry::IndexEntryName>(IndexEntry::SPAN));
    spanField->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    IndexEntrySpan *span = new IndexEntrySpan(QString());
    span->text = spanText;

    m_bibInfo->m_entryTemplate[bibliographyType()].indexEntries.append(static_cast<IndexEntry *>(span));
}

void InsertBibliographyDialog::spanChanged(QListWidgetItem *item)
{
    int row = dialog.addedFields->currentRow();

    if (row != -1) {
        IndexEntrySpan *span = static_cast<IndexEntrySpan *>(m_bibInfo->m_entryTemplate[bibliographyType()].indexEntries.at(row));
        span->text = item->text();
    }
}

void InsertBibliographyDialog::insertTabStop()
{
    /*QListWidgetItem *tabStopItem = new QListWidgetItem(QString("Tab stop"),dialog.availableFields);

    IndexEntryTabStop *tabStop = new IndexEntryTabStop(QString());*/
}

void InsertBibliographyDialog::removeTabStop()
{
    /*int row = dialog.addedFields->row(dialog.addedFields->currentItem());

    if (row != -1 && dialog.addedFields->takeItem(row)->text() == "Tab stop") {
        dialog.addedFields->removeItemWidget(dialog.addedFields->takeItem(row));
    }*/
}
