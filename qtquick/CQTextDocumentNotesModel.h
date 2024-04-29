/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#ifndef CQTEXTDOCUMENTNOTESMODEL_H
#define CQTEXTDOCUMENTNOTESMODEL_H

#include <KoShape.h>
#include <QAbstractListModel>

class CQTextDocumentNotesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum NoteRoles { Text = Qt::UserRole + 1, Image, Color, ColorCount, CategoryName, FirstOfThisColor, Position, Expanded };
    explicit CQTextDocumentNotesModel(QObject *parent = nullptr);
    virtual ~CQTextDocumentNotesModel();

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual int rowCount(const QModelIndex &parent) const;
    int count() const;

    Q_INVOKABLE void toggleExpanded(int index);
    void addEntry(const QString &text, const QString &image, const QString &color, KoShape *shape);

Q_SIGNALS:
    void countChanged();

private:
    class Private;
    Private *const d;
};

#endif // CQTEXTDOCUMENTNOTESMODEL_H
