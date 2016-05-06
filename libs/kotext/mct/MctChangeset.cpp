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
/*
 * This file is part of the EuroOffice for Android project
 *
 * Copyright (C) 2015 MultiRacio Ltd. <support@multiracio.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "MctChangeset.h"
#include "MctChange.h"
#include "MctAuthor.h"
#include "MctPosition.h"

#include <QMutableListIterator>


MctChangeset::MctChangeset(QDomElement changeset, ulong parentId, MctAuthor* author, QDateTime date, QString comment, ulong id)
{
    parents = new QList<ulong>();
    childs = new QList<ulong>();
    changelist = new QList<MctChange*>();

    this->changeset = changeset;
    this->id = id;
    if(parentId != 0) {
        addParentId(parentId);
    }
    this->author = author;
    this->date = date;
    this->comment = comment;

    //adatok beállítása az xml-ben
    setDate(date);
    setAuthor(author);
    setComment(comment);
    setId(id); // Overrides the id if it was set in the changeset before

    corrected = false;
}

MctChangeset::MctChangeset(QDomElement changeset)
{
    this->changeset = changeset;
    parents = new QList<ulong>();
    childs = new QList<ulong>();
    changelist = new QList<MctChange*>();
    setAuthor(new MctAuthor("Undefined"));
    id = 0;

    QString datestring = changeset.attribute("date");
    datestring.replace("T", " ");
    date = QDateTime::fromString(datestring, "yyyy-MM-dd hh:mm:ss");

    corrected = false;
}

MctChangeset::MctChangeset()
{
    parents = new QList<ulong>();
    childs = new QList<ulong>();
    changelist = new QList<MctChange*>();    
    changeset.parentNode().removeChild(changeset);
    corrected = false;
    setAuthor(new MctAuthor("Undefined"));
    id = 0;
}

MctChangeset::MctChangeset(const MctChangeset& other)
{
    setId(other.getId());
    changeset = other.getChangeSetNode();
    parents = new QList<ulong>(*other.getParents());
    childs = new QList<ulong>(*other.getChilds());
    setAuthor(other.getAuthor());
    setDate(other.getDate());
    setComment(other.getComment());
    changelist = new QList<MctChange*>(*other.getChanges());
    corrected = other.isCorrected();
}

MctChangeset::~MctChangeset()
{
    delete parents;
    delete childs;
    delete changelist;
}

/**
 * @brief This gets the list of parent Id's
 * @return Returns with the list of parent Id's
 */
QList<ulong>* MctChangeset::getParents() const
{
    return parents;
}

/**
 * @brief This add new Id to the list of parent Id's
 * @param pid The ID number of the parent changeset node
 */
void MctChangeset::addParentId(ulong pid)
{
    if(!parents->contains(pid)) {
        parents->append(pid);
        changeset.setAttribute("parent" + QString::number(parents->size()), QString::number(pid));
    }
}

/**
 * @brief This clear the list of parents
 */
void MctChangeset::clearParents()
{
    /*foreach (ulong parent, *parents) {
        QString name = "parent" + QString::number(parent);
        changeset.removeAttribute(name);
    }*/
    for(int i=0; i < parents->length(); i++) {
        QString name = "parent" + QString::number(i+1);
        changeset.removeAttribute(name);
    }
    parents->clear();
}

/**
 * @brief This gets the first parentID of the changeset --- OBSOLETE
 * @return Returns with the author name
 */
int MctChangeset::getParentId()
{
    QString parent = changeset.attribute("parent1", "0");
    return parent.toInt();
}

/**
 * @brief This gets the list is child Id's
 * @return Returns with the list of child Id's
 */
QList<ulong>* MctChangeset::getChilds() const
{
    return childs;
}

/**
 * @brief This removes one child from the child list
 * @param id child Id to be removed
 */
void MctChangeset::removeChild(ulong id)
{
    QMutableListIterator<ulong> it(*childs);
    while(it.hasNext()) {
        if(it.next() == id) {
            it.remove();
            break;
        }
    }
}

/**
 * @brief This add new Id to the list of parent Id's
 * @param id The ID number of the child changeset node.
 */
void MctChangeset::addChild(ulong id)
{
    if(!childs->contains(id)) {
        childs->append(id);
    }
}

/**
 * @brief This clear the list of children.
 */
void MctChangeset::clearChilds()
{
    childs->clear();
}

/**
 * @brief This removes given child Id from the list of childs
 * @param id Child id
 */
void MctChangeset::removeChildId(ulong id)
{
    for(int i = 0; i < childs->length(); i++) {
        if(childs->at(i) == id) {
            childs->removeAt(i);
            break;
        }
    }
}

/**
 * @brief This gets the author of the changeset.
 * @return Returns with the author name.
 */
MctAuthor* MctChangeset::getAuthor() const
{
    if(author == NULL) {
        QString name = changeset.attribute("author");
        return new MctAuthor(name);
    }
    return author;
}

/**
 * @brief This sets the author of the changeset.
 * @param author The author
 */
void MctChangeset::setAuthor(MctAuthor *author)
{
    changeset.setAttribute("author", author->getName());
    this->author = author;
}

/**
 * @brief This gets the comment of the changeset
 * @return Returns with the comment.
 */
QString MctChangeset::getComment() const
{
    if(comment.isEmpty()) {
        return changeset.attribute("comment","");
    }
    return comment;
}

/**
 * @brief This sets the comment of the changeset
 * @param comment The comment to set
 */
void MctChangeset::setComment(QString comment)
{
    this->comment = comment;
    changeset.setAttribute("comment", comment);
}

/**
 * @brief This gets the date of the changeset
 * @return Returns with the date.
 */
QDateTime MctChangeset::getDate() const
{
    if(!date.isValid()) {
        QString datestring = changeset.attribute("date");
        return QDateTime::fromString(changeset.attribute("date").replace("T", " "), "yyyy-MM-dd hh:mm:ss");
        /*QString datestring = changeset.attribute("date");
        datestring.replace("T", " ");
        date = QDateTime::fromString(datestring, "yyyy-MM-dd hh:mm:ss");*/
    }
    return date;
}

/**
 * @brief This sets the date of the changeset
 * @param date The date to set
 */
void MctChangeset::setDate(QDateTime date)
{
    QString datestring = date.toString("yyyy-MM-dd hh:mm:ss");
    datestring.replace(" ", "T");
    changeset.setAttribute("date", datestring);
    this->date = date;
}

/**
 * @brief This gets the Id number of the changeset
 * @return Returns with the Id number.
 */
ulong MctChangeset::getId() const
{
    if(id == 0) {
        return changeset.attribute("id", "0").toULong();
    }
    return id;
}

/**
 * @brief This sets the Id number of the changeset
 * @param id The Id number.
 */
void MctChangeset::setId(unsigned long id)
{
    this->id = id;
    changeset.setAttribute("id", QString::number(id));
}

/**
 * @brief This adds new change node to the changeset node.
 * @param change The change node representing the change entity.
 */
void MctChangeset::addChange(MctChange *change)
{
    changelist->append(change);
}

/**
 * @brief This gets the changes related to the changeset node.
 * @return Returns with the list of change nodes.
 */
QList<MctChange*>* MctChangeset::getChanges() const
{
    return changelist;
}

/**
 * @brief This clears child changes under the changeset
 */
void MctChangeset::clearChangeset()
{
    foreach (MctChange* changeNode, *changelist) {
        changeset.removeChild(changeNode->getChangeNode());
    }
    changelist->clear();
}

/**
 * @brief This gets the date and author of the changeset in one string.
 * @return
 */
QString MctChangeset::getDateInString()
{
    QString author = this->author->getName();
    QString nodestring = getDate().toString("yyyy-MM-dd hh:mm:ss") + " by: " + author;
    return nodestring;
}

/**
 * @brief This gets the xml changeset node.
 * @return Returns with the changeset node.
 */
QDomElement MctChangeset::getChangeSetNode() const
{
    return changeset;
}

/**
 * @brief This sets the xml changeset node.
 * @param changeset the xml changeset node
 */
void MctChangeset::setChangeSet(QDomElement changeset)
{
    this->changeset = changeset;
}

void MctChangeset::setCorrected(bool value)
{
    corrected = value;
}

bool MctChangeset::isCorrected() const
{
    return corrected;
}

void MctChangeset::printChangeset()
{
    qDebug() << "Printing changeset:";
    QString dateString = date.toString("yyyy.M.d.");
    QString timeString = date.toString("(H:m:s)");
    qDebug() << dateString << " " << timeString << " by: " << author->getName();
    for (int i = 0; i < changelist->size(); i++){
        MctChange *change = changelist->at(i);
        qDebug() << change->getPosition()->toString();
        if (change->getPosition() == NULL)
            continue;
        if (change->getChangeType() == MovedString){
            qDebug() << change->getChangeType() << " " << change->getPosition()->getStartChar() << " " << change->getPosition()->getEndChar() <<
                        change->getMovedPosition()->getStartChar() << change->getMovedPosition()->getEndChar();
        } else {
            qDebug() << change->getChangeType() << " " << change->getPosition()->getStartChar() << " " << change->getPosition()->getEndChar();
        }
    }
    qDebug() << "-------------------";
}
