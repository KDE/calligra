/*
 * This file is part of the KDE project
 *
 * Copyright 2014  Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "CQTextDocumentNotesModel.h"

#include <QColor>

struct Entry {
    Entry() : colorCount(1), shape(0), expanded(false) {};
    QString text;
    QString image;
    QString color;
    QString categoryName;
    int colorCount;
    KoShape* shape;
    bool expanded;
};

class CQTextDocumentNotesModel::Private {
public:
    Private() {}
    ~Private() {
        qDeleteAll(entries);
    }
    QList<Entry*> entries;
};

CQTextDocumentNotesModel::CQTextDocumentNotesModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
    QHash<int, QByteArray> roles;
    roles[Text] = "text";
    roles[Image] = "image";
    roles[Color] = "color";
    roles[ColorCount] = "colorCount";
    roles[CategoryName] = "categoryName";
    roles[FirstOfThisColor] = "firstOfThisColor";
    roles[Position] = "position";
    roles[Expanded] = "expanded";
    setRoleNames(roles);
}

CQTextDocumentNotesModel::~CQTextDocumentNotesModel()
{
    delete d;
}

QVariant CQTextDocumentNotesModel::data(const QModelIndex& index, int role) const
{
    QVariant data;
    if (index.isValid() && index.row() < d->entries.count()) {
        Entry* entry = d->entries.at(index.row());
        switch(role) {
            case Text:
                data = entry->text;
                break;
            case Image:
                data = entry->image;
                break;
            case Color:
                data = entry->color;
                break;
            case ColorCount:
                data = entry->colorCount;
                break;
            case CategoryName:
                data = entry->categoryName;
                break;
            case FirstOfThisColor:
                data = true;
                if (index.row() > 0 && d->entries.at(index.row() - 1)->color == entry->color) {
                    data = false;
                }
                break;
            case Position:
                data = entry->shape->absolutePosition();
                break;
            case Expanded:
                data = entry->expanded;
                break;
            default:
                data = QLatin1String("No such role. Supported roles are text, image, color, colorCount and position.");
                break;
        }
    }
    return data;
}

int CQTextDocumentNotesModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return d->entries.count();
}

int CQTextDocumentNotesModel::count() const
{
    return d->entries.count();
}

void CQTextDocumentNotesModel::toggleExpanded(int index)
{
    if (index > -1 && index < d->entries.count()) {
        QColor color = d->entries.at(index)->color;
        foreach(Entry* entry, d->entries) {
            if (color == entry->color) {
                entry->expanded = !entry->expanded;
            }
        }
    }
    dataChanged(this->index(0), this->index(d->entries.count() - 1));
}

void CQTextDocumentNotesModel::addEntry(const QString& text, const QString& image, const QString& color, KoShape* shape)
{
    Entry* entry = new Entry();
    entry->text = text;
    entry->image = image;
    entry->shape = shape;
    entry->color = color;
    entry->categoryName = "Others";
    if (color == "#fd5134") {
        entry->categoryName = "Major Errors";
    } else if (color == "#ffb20c") {
        entry->categoryName = "Minor Errors";
    } else if (color == "#29b618") {
        entry->categoryName = "Successes";
    }

    QList<Entry*>::iterator before = d->entries.begin();
    bool reachedColor = false;
    int colorCount = 0, position = 0;
    for(; before != d->entries.end(); ++before) {
        if ((*before)->color == entry->color)
        {
            // We are now in the current entry's section as defined by colour
            // and we grab the current colour count for that section from
            // this item. Could just increase, but that's cause a double-loop
            // to update all the other entries.
            reachedColor = true;
            colorCount = (*before)->colorCount + 1;
            (*before)->colorCount = colorCount;
            entry->colorCount = colorCount;
            entry->expanded = (*before)->expanded;
        }
        if (reachedColor)
        {
            // If we find a new colour, that means we're out of the current
            // section, and we break out. This also conveniently leaves us
            // with the entry we want to insert the item before.
            if ((*before)->color != entry->color) {
                break;
            }
            (*before)->colorCount = colorCount;
        }
        ++position;
    }

    // By default, the Neutral category is supposed to be expanded
    if (color == "Neutral" && colorCount == 1) {
        entry->expanded = true;
    }

    beginInsertRows(QModelIndex(), position, position);
    d->entries.insert(before, entry);
    endInsertRows();
    dataChanged(index(position - colorCount), index(position - 1));
    emit countChanged();
}
