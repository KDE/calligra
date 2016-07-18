/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "TemplatesModel.h"
#include "TemplateVariantsModel.h"
#include <KoTemplateTree.h>
#include <KoTemplateGroup.h>
#include <KoTemplate.h>
#include <KWFactory.h>
#include <KWDocument.h>
#include <part/KPrFactory.h>
#include <part/KPrDocument.h>

struct TemplateEntry {
    TemplateEntry()
        : variants(new TemplateVariantsModel())
    {}
    ~TemplateEntry()
    {
        variants->deleteLater();
    }

    QString title;
    QString description;
    TemplateVariantsModel* variants;
};

class TemplatesModel::Private {
public:
    Private()
        : showWide(false)
    {}

    QString templateType;
    bool showWide;
    QList<TemplateEntry*> entries;

    void refresh()
    {
        qDeleteAll(entries);
        entries.clear();

        KoTemplateTree* tree(0);
        if(templateType == WORDS_MIME_TYPE)
            tree = new KoTemplateTree("calligrawords/templates/", true);
        else if(templateType == STAGE_MIME_TYPE)
            tree = new KoTemplateTree("calligrastage/templates/", true);
        if(!tree)
            return;

        Q_FOREACH(const KoTemplateGroup* group , tree->groups()) {
            Q_FOREACH(const KoTemplate* tmplate, group->templates()) {
                if(tmplate->wide() != showWide)
                    continue;

                QString title = tmplate->name();

                TemplateEntry* found(0);
                Q_FOREACH(TemplateEntry* otherEntry, entries) {
                    if(otherEntry->title == title) {
                        found = otherEntry;
                        break;
                    }
                }
                TemplateEntry* entry(0);
                if(found) {
                    entry = found;
                }
                else {
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

TemplatesModel::TemplatesModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private())
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "text";
    roles[DescriptionRole] = "description";
    roles[ColorRole] = "color";
    roles[ThumbnailRole] = "thumbnail";
    roles[UrlRole] = "url";
    roles[VariantCountRole] = "variantCount";
    roles[VariantsRole] = "variants";
    setRoleNames(roles);
}

TemplatesModel::~TemplatesModel()
{
    delete d;
}

QVariant TemplatesModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if(index.isValid() && index.row() > -1 && index.row() < d->entries.count())
    {
        TemplateEntry* entry = d->entries.at(index.row());
        switch(role)
        {
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
                result = QVariant::fromValue<QObject*>(entry->variants);
                break;
            default:
                break;
        }
    }

    return result;
}

int TemplatesModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->entries.count();
}

QString TemplatesModel::templateType() const
{
    return d->templateType;
}

void TemplatesModel::setTemplateType(const QString& newType)
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

void TemplatesModel::setShowWide(const bool& newValue)
{
    d->showWide = newValue;
    emit showWideChanged();
    qApp->processEvents();
    beginResetModel();
    d->refresh();
    endResetModel();
}
