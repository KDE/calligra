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

#ifndef CQTEXTDOCUMENTNOTESMODEL_H
#define CQTEXTDOCUMENTNOTESMODEL_H

#include <QAbstractListModel>
#include <KoShape.h>

class CQTextDocumentNotesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum NoteRoles {
        Text = Qt::UserRole + 1,
        Image,
        Color,
        ColorCount,
        CategoryName,
        FirstOfThisColor,
        Position,
        Expanded
    };
    explicit CQTextDocumentNotesModel(QObject* parent = 0);
    virtual ~CQTextDocumentNotesModel();

    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual int rowCount(const QModelIndex& parent) const;
    int count() const;

    Q_INVOKABLE void toggleExpanded(int index);
    void addEntry(const QString& text, const QString& image, const QString& color, KoShape* shape);

Q_SIGNALS:
    void countChanged();

private:
    class Private;
    Private* const d;
};

#endif // CQTEXTDOCUMENTNOTESMODEL_H
