/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
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

#ifndef MERGEENABLEDCHANGETRACKING_H
#define MERGEENABLEDCHANGETRACKING_H

#include "kotext_export.h"

#include "MctNode.h"

#include <QDateTime>
#include <QDomElement>

class MctChange;
class MctAuthor;

/**
 * Represent an entire revision
 *
 * A Changeset could consist of many individual changes,
 * it has an author, parent revision(s) and a timestamp.
 * This class often access Dom tree.
 *
 * @todo this class could be rename (eg. Revision would be proper)
 */
class KOTEXT_EXPORT MctChangeset : public MctNode
{
public:
    /// constructor
    MctChangeset(const QDomElement &changeset, ulong parentId, MctAuthor* author, const QDateTime &date, const QString &comment, ulong m_id);
    MctChangeset(const QDomElement &changeset);
    MctChangeset();

    /// copy constructor
    MctChangeset(const MctChangeset &other);

    ~MctChangeset();    

    /// setter
    void setId(unsigned long id);
    /// getter
    unsigned long id() const;
    /// getter of parent revisions list
    QList<ulong>* parents() const;
    /// append new parent revision id
    void addParentId(ulong pid);
    /// clear parent revision list
    void clearParents();
    /// getter @todo remove this obsolote function
    int parentId();
    /// getter of child revisions list
    QList<ulong>* childs() const;
    /// remove child revision
    void removeChild(ulong id);
    /// add child revision
    void addChild(ulong id);
    /// clear all child revision from list
    void clearChilds();
    /// getter
    MctAuthor* author() const;
    /// setter
    void setAuthor(MctAuthor *author);
    /// getter
    QString comment() const;
    /// setter
    void setComment(const QString &comment);
    /// getter
    QDateTime date() const;
    /// setter
    void setDate(const QDateTime &date);

    /**
     * add new change node to the revision
     * @param change change node representing the change entity.
     */
    void addChange(MctChange * change);
    /// getter of change list
    QList<MctChange*>* changes() const;
    /// clear change list
    void clearChangeset();

    /// print author and date as a string
    QString dateInString();

    /// getter
    QDomElement changeset() const;
    /// setter
    void setChangeset(const QDomElement &changeset);
    /// getter
    bool isCorrected() const;
    /// setter
    void setCorrected(bool value);
    /// debug toString method
    void printChangeset();

private:

    QDomElement m_changeset;    ///< node of changeset in
    QDateTime m_date;           ///< timestamp
    QString m_comment;          ///< optional comment
    MctAuthor *m_author;        ///< author of the changeset
    QList<ulong> *m_parents;    ///< list of parent ids
    QList<ulong> *m_childs;     ///< list of child ids
    QList<MctChange*> *m_changelist;    ///< list of changes in the set

    bool m_corrected;   ///< are the positions corrected?

};

#endif
