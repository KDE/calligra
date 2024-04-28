/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TABLEOFCONTENTSENTYMODEL_H
#define TABLEOFCONTENTSENTYMODEL_H

#include <QAbstractTableModel>
#include <QPair>
#include <QStringList>

class KoStyleManager;
class KoTableOfContentsGeneratorInfo;

class TableOfContentsEntryModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum ModelColumns { Levels = 0, Styles = 1 };
    TableOfContentsEntryModel(KoStyleManager *manager, KoTableOfContentsGeneratorInfo *info);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void saveData();

Q_SIGNALS:
    void tocEntryDataChanged();

private:
    QList<QPair<QString, int>> m_tocEntries; // first contains the text that will appear in table view, and second one is the styleId
    KoStyleManager *m_styleManager;
    KoTableOfContentsGeneratorInfo *m_tocInfo;
};

#endif // TABLEOFCONTENTSENTYMODEL_H
