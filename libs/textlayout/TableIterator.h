/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann KO GmbH <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TABLEITERATOR_H
#define TABLEITERATOR_H

#include <QString>
#include <QVector>

class FrameIterator;
class KoTextLayoutArea;
class QTextTable;

/**
 * Convenience cursor class used during table layout.
 *
 * The class holds information about a table and the current row. Can also
 * provide a FrameIterator for a given column in the table.
 */
class TableIterator
{
public:
    /**
     * Constructs a new iterator for the given table.
     *
     * @param table table to use.
     */
    explicit TableIterator(QTextTable *table);

    /**
     * Constructs a new iterator initialized from another.
     *
     * @param other iterator to initialize the iterator from.
     */
    explicit TableIterator(TableIterator *other);

    /// Destructor.
    ~TableIterator();

    /// Compare this iterator to another.
    bool operator==(const TableIterator &other) const;

    /**
     * Returns a frame iterator that iterates over the frames in a given column.
     *
     * @param column column for which and iterator should be returned.
     */
    FrameIterator *frameIterator(int column);

    /// Table being iterated over.
    QTextTable *table;
    /// Current row.
    int row;
    /// Number of header rows.
    int headerRows;
    /// X position of header.
    qreal headerPositionX;
    /// Frame iterators, one for each column.
    QVector<FrameIterator *> frameIterators;
    /// Header row positions.
    QVector<qreal> headerRowPositions; // we will only fill those of header rows
    /// Rows of header cell areas.
    QVector<QVector<KoTextLayoutArea *>> headerCellAreas;
    /// The name of the master-page that is used for this table.
    QString masterPageName;
};

#endif
