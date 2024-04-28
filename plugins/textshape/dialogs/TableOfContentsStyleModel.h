/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TABLEOFCONTENTSSTYLEMODEL_H
#define TABLEOFCONTENTSSTYLEMODEL_H

#include <QAbstractTableModel>

class KoStyleManager;
class KoStyleThumbnailer;
class KoTableOfContentsGeneratorInfo;

class TableOfContentsStyleModel : public QAbstractTableModel
{
public:
    TableOfContentsStyleModel(const KoStyleManager *manager, KoTableOfContentsGeneratorInfo *info);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void saveData();

protected:
    QList<int> m_styleList; // list of style IDs
    QList<int> m_outlineLevel;

private:
    const KoStyleManager *m_styleManager;
    KoStyleThumbnailer *m_styleThumbnailer;
    KoTableOfContentsGeneratorInfo *m_tocInfo;

    int getOutlineLevel(int styleId);
    void setOutlineLevel(int styleId, int outLineLevel);
};

#endif // TABLEOFCONTENTSSTYLEMODEL_H
