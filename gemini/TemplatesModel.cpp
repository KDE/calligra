/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "TemplatesModel.h"
#include "TemplateVariantsModel.h"

#include <QApplication>

#include <KWDocument.h>
#include <KWFactory.h>
#include <KoTemplate.h>
#include <KoTemplateGroup.h>
#include <KoTemplateTree.h>
#include <part/KPrDocument.h>
#include <part/KPrFactory.h>

struct TemplateEntry {
    TemplateEntry()
        : variants(new TemplateVariantsModel())
    {
    }
    ~TemplateEntry()
    {
        variants->deleteLater();
    }

    QString title;
    QString description;
    TemplateVariantsModel *variants;
};

class TemplatesModel::Private
{
public:
    Private()
        : showWide(false)
    {
    }

    QString templateType;
    bool showWide;
    QList<TemplateEntry *> entries;

    void refresh()
    {
        qDeleteAll(entries);
        entries.clear();

        KoTemplateTree *tree(nullptr);
        if (templateType == WORDS_MIME_TYPE)
            tree = new KoTemplateTree("calligrawords/templates/", true);
        else if (templateType == STAGE_MIME_TYPE)
            tree = new KoTemplateTree("calligrastage/templates/", true);
        if (!tree)
            return;

        Q_FOREACH (const KoTemplateGroup *group, tree->groups()) {
            Q_FOREACH (const KoTemplate *tmplate, group->templates()) {
                if (tmplate->wide() != showWide)
                    continue;

                QString title = tmplate->name();

                TemplateEntry *found(nullptr);
                Q_FOREACH (TemplateEntry *otherEntry, entries) {
                    if (otherEntry->title == title) {
                        found = otherEntry;
                        break;
                    }
                }
                TemplateEntry *entry(nullptr);
                if (found) {
                    entry = found;
                } else {
                    entry = new TemplateEntry();
                    entry->title = title;
                    entry->description = tmplate->description();
                    entries.append(entry);
                }

                entry->variants->addVariant(tmplate->variantName(),
                                            tmplate->color(),
                                            tmplate->swatch(),
                                            tmplate->thumbnail().isEmpty() ? tmplate->picture() : tmplate->thumbnail(),
                                            tmplate->file());
            }
        }
    }
};

TemplatesModel::TemplatesModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private())
{
}

TemplatesModel::~TemplatesModel()
{
    delete d;
}

QHash<int, QByteArray> TemplatesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "text";
    roles[DescriptionRole] = "description";
    roles[ColorRole] = "color";
    roles[ThumbnailRole] = "thumbnail";
    roles[UrlRole] = "url";
    roles[VariantCountRole] = "variantCount";
    roles[VariantsRole] = "variants";
    return roles;
}

QVariant TemplatesModel::data(const QModelIndex &index, int role) const
{
    QVariant result;

    if (index.isValid() && index.row() > -1 && index.row() < d->entries.count()) {
        TemplateEntry *entry = d->entries.at(index.row());
        switch (role) {
        case TitleRole:
            result = entry->title;
            break;
        case DescriptionRole:
            result = entry->description;
            break;
        case ColorRole:
            result = entry->variants->data(entry->variants->firstIndex(), TemplateVariantsModel::ColorRole);
            break;
        case ThumbnailRole:
            result = entry->variants->data(entry->variants->firstIndex(), TemplateVariantsModel::ThumbnailRole);
            break;
        case UrlRole:
            result = entry->variants->data(entry->variants->firstIndex(), TemplateVariantsModel::UrlRole);
            break;
        case VariantCountRole:
            result = entry->variants->rowCount();
            break;
        case VariantsRole:
            result = QVariant::fromValue<QObject *>(entry->variants);
            break;
        default:
            break;
        }
    }

    return result;
}

int TemplatesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return d->entries.count();
}

QString TemplatesModel::templateType() const
{
    return d->templateType;
}

void TemplatesModel::setTemplateType(const QString &newType)
{
    d->templateType = newType;
    emit templateTypeChanged();
    qApp->processEvents();
    beginResetModel();
    d->refresh();
    endResetModel();
}

bool TemplatesModel::showWide() const
{
    return d->showWide;
}

void TemplatesModel::setShowWide(const bool &newValue)
{
    d->showWide = newValue;
    emit showWideChanged();
    qApp->processEvents();
    beginResetModel();
    d->refresh();
    endResetModel();
}
