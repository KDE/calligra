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

class KOTEXT_EXPORT MctChangeset : public MctNode
{
public:

    MctChangeset(const QDomElement &changeset, ulong parentId, MctAuthor* author, const QDateTime &date, const QString &comment, ulong m_id);
    MctChangeset(const QDomElement &changeset);
    MctChangeset();

    MctChangeset(const MctChangeset &other);

    ~MctChangeset();    

    void setId(unsigned long id);
    unsigned long id() const;

    QList<ulong>* parents() const;
    void addParentId(ulong pid);
    void clearParents();
    int parentId();

    QList<ulong>* childs() const;
    void removeChild(ulong id);
    void addChild(ulong id);
    void clearChilds();
    void removeChildId(ulong id);

    MctAuthor* author() const;
    void setAuthor(MctAuthor *author);
    QString comment() const;
    void setComment(const QString &comment);
    QDateTime date() const;
    void setDate(const QDateTime &date);

    void addChange(MctChange * change);
    QList<MctChange*>* changes() const;
    void clearChangeset();

    QString dateInString();

    QDomElement changeset() const;
    void setChangeset(const QDomElement &changeset);

    void setCorrected(bool value);
    bool isCorrected() const;

    void printChangeset();

private:

    QDomElement m_changeset;
    QDateTime m_date;
    QString m_comment;
    MctAuthor *m_author;
    QList<ulong> *m_parents;
    QList<ulong> *m_childs;
    QList<MctChange*> *m_changelist;

    bool m_corrected;

};

#endif
