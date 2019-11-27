/* This file is part of the Calligra project, made within the KDE community.
 *
 * Copyright (C) 2013,2016 Friedrich W. H. Kossebau <friedrich@kogmbh.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef TEXTDOCUMENTSTRUCTUREMODEL_H
#define TEXTDOCUMENTSTRUCTUREMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QHash>
#include <QPointer>

class QTextDocument;
class QTextFrame;
class QTextBlock;
class QTextLine;

struct NodeData
{
    enum Type {Frame, Block, Line};

    Type type;
    union {
        QTextFrame *frame;
        int blockNumber;
    };
    int lineNumber;

    static NodeData fromFrame(QTextFrame *frame);
    static NodeData fromBlock(int blockNumber);
    static NodeData fromLine(int blockNumber, int lineNumber);
};
Q_DECLARE_TYPEINFO(NodeData, Q_MOVABLE_TYPE);

struct BlockData
{
    explicit BlockData(int _nodeIndex) : nodeIndex(_nodeIndex) {}

    int nodeIndex;
    QHash<int, int> lineNumberTable;
};
Q_DECLARE_TYPEINFO(BlockData, Q_MOVABLE_TYPE);


class TextDocumentStructureModel : public QAbstractItemModel
{
    Q_OBJECT

    enum Columns {
        nameColumn = 0,
        endColumn
    };

public:
    explicit TextDocumentStructureModel(QObject *parent = 0);
    ~TextDocumentStructureModel() override;

public: // QAbstractItemModel API
    QModelIndex index(int row, int column, const QModelIndex &parentIndex) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &index) const override;
    int columnCount(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool hasChildren(const QModelIndex &parent) const override;

public:
    void setTextDocument(QTextDocument *textDocument);

private Q_SLOTS:
    void onContentsChanged();
    void onModelReset();

private:
    int lineIndex(const QTextBlock &block, const QTextLine &line) const;
    int blockIndex(const QTextBlock &block) const;
    int frameIndex(QTextFrame *frame) const;

private:
    QPointer<QTextDocument> m_textDocument;

    mutable QVector<NodeData> m_nodeDataTable;

    mutable QHash<int, BlockData> m_blockNumberTable;
    mutable QHash<QTextFrame*, int> m_frameTable;
};

#endif // TEXTDOCUMENTSTRUCTUREMODEL_H
