/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "BibliographyConfigureDialog.h"
#include "ui_BibliographyConfigureDialog.h"

#include <KoStyleManager.h>
#include <KoTextDocument.h>

#include <QAbstractButton>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QRadioButton>

BibliographyConfigureDialog::BibliographyConfigureDialog(const QTextDocument *document, QWidget *parent)
    : QDialog(parent)
    , m_document(document)
    , m_bibConfiguration(KoTextDocument(m_document).styleManager()->bibliographyConfiguration())
{
    dialog.setupUi(this);
    dialog.prefix->setText(m_bibConfiguration->prefix());
    dialog.suffix->setText(m_bibConfiguration->suffix());
    dialog.numberedEntries->setChecked(m_bibConfiguration->numberedEntries());
    dialog.sortAlgorithm->setCurrentIndex(dialog.sortAlgorithm->findText(m_bibConfiguration->sortAlgorithm(), Qt::MatchFixedString));

    dialog.sortByPosition->setChecked(m_bibConfiguration->sortByPosition());

    connect(dialog.buttonBox, &QDialogButtonBox::clicked, this, &BibliographyConfigureDialog::save);
    connect(dialog.addSortKeyButton, &QAbstractButton::clicked, this, &BibliographyConfigureDialog::addSortKey);
    connect(dialog.sortByPosition, &QAbstractButton::clicked, this, &BibliographyConfigureDialog::sortMethodChanged);

    dialog.sortKeyGroupBox->setDisabled(m_bibConfiguration->sortByPosition());

    if (m_bibConfiguration->sortKeys().isEmpty()) {
        m_bibConfiguration->setSortKeys(m_bibConfiguration->sortKeys() << QPair<QString, Qt::SortOrder>("identifier", Qt::AscendingOrder));
    }

    foreach (const SortKeyPair &key, m_bibConfiguration->sortKeys()) {
        dialog.sortKeyGroupBox->layout()->addWidget(new SortKeyWidget(key.first, key.second, dialog.sortKeyGroupBox));
    }

    show();
}

void BibliographyConfigureDialog::save(QAbstractButton *button)
{
    if (dialog.buttonBox->standardButton(button) == dialog.buttonBox->Apply) {
        m_bibConfiguration->setPrefix(dialog.prefix->text());
        m_bibConfiguration->setSuffix(dialog.suffix->text());
        m_bibConfiguration->setSortAlgorithm(dialog.sortAlgorithm->currentText());
        m_bibConfiguration->setSortByPosition(dialog.sortByPosition->isChecked());
        m_bibConfiguration->setNumberedEntries(dialog.numberedEntries->isChecked());

        QVector<SortKeyPair> sortKeys;

        foreach (QObject *o, dialog.sortKeyGroupBox->children()) {
            SortKeyWidget *widget = dynamic_cast<SortKeyWidget *>(o);
            if (widget) {
                sortKeys << SortKeyPair(widget->sortKey(), widget->sortOrder());
            }
        }
        m_bibConfiguration->setSortKeys(sortKeys);

        KoTextDocument(m_document).styleManager()->setBibliographyConfiguration(m_bibConfiguration);
    }
    accept();
}

void BibliographyConfigureDialog::addSortKey()
{
    dialog.sortKeyGroupBox->layout()->addWidget(new SortKeyWidget("identifier", Qt::AscendingOrder, dialog.sortKeyGroupBox));
}

void BibliographyConfigureDialog::sortMethodChanged(bool sortByPosition)
{
    m_bibConfiguration->setSortByPosition(sortByPosition);

    if (!sortByPosition && m_bibConfiguration->sortKeys().isEmpty()) {
        m_bibConfiguration->setSortKeys(m_bibConfiguration->sortKeys() << QPair<QString, Qt::SortOrder>("identifier", Qt::AscendingOrder));
    }
}

SortKeyWidget::SortKeyWidget(const QString &sortKey, Qt::SortOrder order, QWidget *parent)
    : QWidget(parent)
    , m_dataFields(new QComboBox)
    , m_ascButton(new QRadioButton(i18n("Ascending")))
    , m_dscButton(new QRadioButton(i18n("Descending")))
    , m_layout(new QHBoxLayout)
{
    setLayout(m_layout);
    m_dataFields->addItems(KoOdfBibliographyConfiguration::bibDataFields);
    setSortKey(sortKey);
    setSortOrder(order);

    m_layout->addWidget(m_dataFields);
    m_layout->addWidget(m_ascButton);
    m_layout->addWidget(m_dscButton);
}

void SortKeyWidget::setSortKey(const QString &sortKey)
{
    int sortKeyIndex = KoOdfBibliographyConfiguration::bibDataFields.indexOf(sortKey);
    if (sortKeyIndex != -1) {
        m_dataFields->setCurrentIndex(sortKeyIndex);
    }
}

void SortKeyWidget::setSortOrder(Qt::SortOrder order)
{
    if (order == Qt::DescendingOrder)
        m_dscButton->setChecked(true);
    else
        m_ascButton->setChecked(true);
}

QString SortKeyWidget::sortKey() const
{
    return m_dataFields->currentText();
}

Qt::SortOrder SortKeyWidget::sortOrder() const
{
    return (m_ascButton->isChecked()) ? Qt::AscendingOrder : Qt::DescendingOrder;
}
