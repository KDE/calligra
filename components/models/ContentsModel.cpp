/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "ContentsModel.h"

#include <QDebug>

#include "Document.h"
#include <part/Doc.h>
#include <Map.h>
#include <Sheet.h>
#include <KPrDocument.h>
#include <KoPAPageBase.h>

using namespace Calligra::Components;

struct ContentsEntry
{
    ContentsEntry() : level{0}, index{0}
    { }

    QString title;
    int level;
    int index;
};

class ContentsModel::Private
{
public:
    Private() : document{nullptr}
    { }

    Document* document;

    QList<ContentsEntry> contents;
};

ContentsModel::ContentsModel(QObject* parent)
    : QAbstractListModel{parent}, d{new Private}
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(TitleRole, "contentTitle");
    roleNames.insert(IndexRole, "contentIndex");
    roleNames.insert(LevelRole, "contentLevel");
    setRoleNames(roleNames);
}

ContentsModel::~ContentsModel()
{
    delete d;
}

QVariant ContentsModel::data(const QModelIndex& index, int role) const
{
    if(!d->document || !index.isValid())
        return QVariant();

    switch(role) {
        case TitleRole:
            return d->contents.at(index.row()).title;
        case IndexRole:
            return d->contents.at(index.row()).index;
        case LevelRole:
            return d->contents.at(index.row()).level;
        default:
            return QVariant();
    }
}

int ContentsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return d->contents.count();
}

Document* ContentsModel::document() const
{
    return d->document;
}

void ContentsModel::setDocument(Document* newDocument)
{
    if(newDocument != d->document) {
        if(d->document) {
            disconnect(d->document, &Document::statusChanged, this, &ContentsModel::listContents);
        }

        d->document = newDocument;
        connect(d->document, &Document::statusChanged, this, &ContentsModel::listContents);
        listContents();

        emit documentChanged();
    }
}

void ContentsModel::listContents()
{
    if(!d->document || !d->document->status() == DocumentStatus::Loaded) {
        d->contents = QList<ContentsEntry>{};
        return;
    }

    beginRemoveRows(QModelIndex(), 0, d->contents.count() - 1);
    d->contents.clear();
    endRemoveRows();

    switch(d->document->documentType()) {
        case DocumentType::TextDocument:
            break;
        case DocumentType::Spreadsheet: {
            Calligra::Sheets::Doc* doc = static_cast<Calligra::Sheets::Doc*>(d->document->koDocument());
            Calligra::Sheets::Map* map = doc->map();

            for(Calligra::Sheets::Sheet* sheet : map->sheetList()) {
                ContentsEntry newEntry;
                newEntry.index = map->indexOf(sheet);
                newEntry.title = sheet->sheetName();
                d->contents.append(newEntry);
            }
            break;
        }
        case DocumentType::Presentation: {
            KPrDocument* doc = static_cast<KPrDocument*>(d->document->koDocument());

            for(int i = 0; i < doc->pageCount(); ++i) {
                KoPAPageBase* page = doc->pageByIndex(i, false);

                ContentsEntry newEntry;
                newEntry.title = QString("%1: %2").arg(i + 1).arg(page->name());
                newEntry.index = i;
                d->contents.append(newEntry);
            }
            break;
        }
        default:
            qWarning() << "Unknown document type, unable to list contents.";
            d->contents = QList<ContentsEntry>{};
            break;
    }

    beginInsertRows(QModelIndex(), 0, d->contents.count() - 1);
    endInsertRows();
}


