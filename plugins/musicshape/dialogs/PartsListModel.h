/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_PARTSLISTMODEL_H
#define MUSIC_PARTSLISTMODEL_H

#include <QAbstractListModel>

namespace MusicCore
{
class Sheet;
class Part;
}

class PartsListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit PartsListModel(MusicCore::Sheet *sheet);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
private Q_SLOTS:
    void partAdded(int index, MusicCore::Part *part);
    void partRemoved(int index, MusicCore::Part *part);

private:
    MusicCore::Sheet *m_sheet;
};

#endif // MUSIC_PARTSLISTMODEL_H
