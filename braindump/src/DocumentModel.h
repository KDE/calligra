/*
 *  SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _DOCUMENT_MODEL_H_
#define _DOCUMENT_MODEL_H_

#include "KoDocumentSectionModel.h"

class RootSection;
class Section;
class SectionGroup;

class DocumentModel : public KoDocumentSectionModel
{
    Q_OBJECT
public:
    enum Role { SectionPtr = 0x1252BAD };

public:
    DocumentModel(QObject *parent, RootSection *document);
    ~DocumentModel();
    // from QAbstractItemModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

public:
    void removeSection(Section *);
    void insertSection(Section *, SectionGroup *parent, Section *before);
    void insertSection(Section *, SectionGroup *parent, int _idx);
    void changeSectionName(Section *_section, const QString &_name);

public:
    QModelIndex index(Section *);
    QModelIndex index(SectionGroup *);
    Section *dataFromIndex(const QModelIndex &index) const;
    void *dataToIndex(Section *section) const;
Q_SIGNALS:
    /**
     * This signal is emitted when the active section is changed.
     */
    void activeSectionChanged(Section *section);

private:
    RootSection *m_document;
};

#endif
