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

    MctChangeset(QDomElement changeset, ulong parentId, MctAuthor* author, QDateTime date, QString comment, ulong id);
    MctChangeset(QDomElement changeset);
    MctChangeset();

    MctChangeset(const MctChangeset &other);

    ~MctChangeset();    

    void setId(unsigned long id);
    unsigned long getId() const;

    QList<ulong>* getParents() const;
    void addParentId(ulong pid);
    void clearParents();
    int getParentId();

    QList<ulong>* getChilds() const;
    void removeChild(ulong id);
    void addChild(ulong id);
    void clearChilds();
    void removeChildId(ulong id);

    MctAuthor* getAuthor() const;
    void setAuthor(MctAuthor *author);
    QString getComment() const;
    void setComment(QString comment);
    QDateTime getDate() const;
    void setDate(QDateTime date);

    void addChange(MctChange * change);
    QList<MctChange*>* getChanges() const;
    void clearChangeset();

    QString getDateInString();

    QDomElement getChangeSetNode() const;
    void setChangeSet(QDomElement changeset);

    void setCorrected(bool value);
    bool isCorrected() const;

    void printChangeset();

private:

    QDomElement changeset;
    QDateTime date;
    QString comment;    
    MctAuthor *author;
    QList<ulong> *parents;
    QList<ulong> *childs;
    QList<MctChange*> *changelist;

    bool corrected;

};

#endif
