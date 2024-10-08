/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Peter Simonsson <psn@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoTemplatesPane.h"

#include "KoTemplate.h"
#include "KoTemplateGroup.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QStandardItemModel>
#include <QUrl>

class KoTemplatesPanePrivate
{
public:
    KoTemplatesPanePrivate()
        : m_selected(false)
    {
    }

    bool m_selected;
    QString m_alwaysUseTemplate;
};

KoTemplatesPane::KoTemplatesPane(QWidget *parent, const QString &header, KoTemplateGroup *group, KoTemplate *defaultTemplate)
    : KoDetailsPane(parent, header)
    , d(new KoTemplatesPanePrivate)
{
    setFocusProxy(m_documentList);

    m_openButton->setText(i18n("Use This Template"));
    KConfigGroup cfgGrp(KSharedConfig::openConfig(), "TemplateChooserDialog");
    QString fullTemplateName = cfgGrp.readPathEntry("FullTemplateName", QString());
    d->m_alwaysUseTemplate = cfgGrp.readPathEntry("AlwaysUseTemplate", QString());
    connect(m_alwaysUseCheckBox, &QAbstractButton::clicked, this, &KoTemplatesPane::alwaysUseClicked);

    QStandardItem *selectItem = nullptr;
    QStandardItem *rootItem = model()->invisibleRootItem();
    QStandardItem *defaultItem = nullptr;

    foreach (KoTemplate *t, group->templates()) {
        if (t->isHidden())
            continue;

        QPixmap preview = t->loadPicture();
        QImage icon = preview.toImage();
        icon = icon.scaled(IconExtent, IconExtent, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        icon = icon.convertToFormat(QImage::Format_ARGB32);
        icon = icon.copy((icon.width() - IconExtent) / 2, (icon.height() - IconExtent) / 2, IconExtent, IconExtent);
        QStandardItem *item = new QStandardItem(QPixmap::fromImage(icon), t->name());
        item->setEditable(false);
        item->setData(t->description(), Qt::UserRole);
        item->setData(t->file(), Qt::UserRole + 1);
        item->setData(preview, Qt::UserRole + 2);
        rootItem->appendRow(item);

        if (d->m_alwaysUseTemplate == t->file()) {
            selectItem = item;
        } else if (!selectItem && (t->file() == fullTemplateName)) {
            selectItem = item;
        }

        if (defaultTemplate && (t->file() == defaultTemplate->file())) {
            defaultItem = item;
        }
    }

    QModelIndex selectedIndex;

    if (selectItem) {
        selectedIndex = model()->indexFromItem(selectItem);
        d->m_selected = true;
    } else if (defaultItem) {
        selectedIndex = model()->indexFromItem(defaultItem);
    } else {
        selectedIndex = model()->indexFromItem(model()->item(0));
    }

    m_documentList->selectionModel()->select(selectedIndex, QItemSelectionModel::Select);
    m_documentList->selectionModel()->setCurrentIndex(selectedIndex, QItemSelectionModel::Select);
}

KoTemplatesPane::~KoTemplatesPane()
{
    delete d;
}

void KoTemplatesPane::selectionChanged(const QModelIndex &index)
{
    if (index.isValid()) {
        QStandardItem *item = model()->itemFromIndex(index);
        m_openButton->setEnabled(true);
        m_alwaysUseCheckBox->setEnabled(true);
        m_titleLabel->setText(item->data(Qt::DisplayRole).toString());
        m_previewLabel->setPixmap(item->data(Qt::UserRole + 2).value<QPixmap>());
        m_detailsLabel->setHtml(item->data(Qt::UserRole).toString());
        m_alwaysUseCheckBox->setChecked(item->data(Qt::UserRole + 1).toString() == d->m_alwaysUseTemplate);
    } else {
        m_openButton->setEnabled(false);
        m_alwaysUseCheckBox->setEnabled(false);
        m_alwaysUseCheckBox->setChecked(false);
        m_titleLabel->clear();
        m_previewLabel->setPixmap(QPixmap());
        m_detailsLabel->clear();
    }
}

void KoTemplatesPane::openFile()
{
    KoDetailsPane::openFile();
}

void KoTemplatesPane::openFile(const QModelIndex &index)
{
    if (index.isValid()) {
        QStandardItem *item = model()->itemFromIndex(index);
        KConfigGroup cfgGrp(KSharedConfig::openConfig(), "TemplateChooserDialog");
        cfgGrp.writePathEntry("FullTemplateName", item->data(Qt::UserRole + 1).toString());
        cfgGrp.writeEntry("LastReturnType", "Template");
        cfgGrp.writeEntry("AlwaysUseTemplate", d->m_alwaysUseTemplate);
        Q_EMIT openUrl(QUrl::fromLocalFile(item->data(Qt::UserRole + 1).toString()));
    }
}

bool KoTemplatesPane::isSelected()
{
    return d->m_selected;
}

void KoTemplatesPane::alwaysUseClicked()
{
    QStandardItem *item = model()->itemFromIndex(m_documentList->selectionModel()->currentIndex());

    if (!m_alwaysUseCheckBox->isChecked()) {
        d->m_alwaysUseTemplate.clear();
    } else {
        d->m_alwaysUseTemplate = item->data(Qt::UserRole + 1).toString();
    }

    KConfigGroup cfgGrp(KSharedConfig::openConfig(), "TemplateChooserDialog");
    cfgGrp.writeEntry("AlwaysUseTemplate", d->m_alwaysUseTemplate);
    cfgGrp.sync();
    Q_EMIT alwaysUseChanged(this, d->m_alwaysUseTemplate);
}

void KoTemplatesPane::changeAlwaysUseTemplate(KoTemplatesPane *sender, const QString &alwaysUse)
{
    if (this == sender)
        return;

    QStandardItem *item = model()->itemFromIndex(m_documentList->selectionModel()->currentIndex());

    // If the old always use template is selected uncheck the checkbox
    if (item && (item->data(Qt::UserRole + 1).toString() == d->m_alwaysUseTemplate)) {
        m_alwaysUseCheckBox->setChecked(false);
    }

    d->m_alwaysUseTemplate = alwaysUse;
}
