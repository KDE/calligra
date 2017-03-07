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
#include "MctStaticData.h"

#include <QMutableListIterator>

MctChangeset::MctChangeset(const QDomElement &changeset, ulong parentId, MctAuthor* author, const QDateTime &date, const QString &comment, ulong id)
{
    m_parents = new QList<ulong>();
    m_childs = new QList<ulong>();
    m_changelist = new QList<MctChange*>();

    m_changeset = changeset;
    m_id = id;
    if(parentId != 0) {
        addParentId(parentId);
    }
    m_author = author;
    m_date = date;
    m_comment = comment;

    //adatok beállítása az xml-ben
    setDate(date);
    setAuthor(author);
    setComment(comment);
    setId(id); // Overrides the id if it was set in the changeset before

    m_corrected = false;
}

MctChangeset::MctChangeset(const QDomElement &changeset)
{
    m_changeset = changeset;
    m_parents = new QList<ulong>();
    m_childs = new QList<ulong>();
    m_changelist = new QList<MctChange*>();
    m_author = new MctAuthor(changeset.attribute(MctStaticData::attributeNS(MctStaticData::AUTHOR, MctStaticData::NS_DC), "Undefined"));
    m_id = 0;

    QString datestring = changeset.attribute(MctStaticData::attributeNS(MctStaticData::DATE, MctStaticData::NS_DC));
    datestring.replace("T", " ");
    m_date = QDateTime::fromString(datestring, "yyyy-MM-dd hh:mm:ss");

    m_corrected = false;
}

MctChangeset::MctChangeset()
{
    m_parents = new QList<ulong>();
    m_childs = new QList<ulong>();
    m_changelist = new QList<MctChange*>();
    m_changeset.parentNode().removeChild(m_changeset);
    m_corrected = false;
    setAuthor(new MctAuthor("Undefined_0"));
    m_id = 0;
}

MctChangeset::MctChangeset(const MctChangeset& other)
{
    setId(other.id());
    m_changeset = other.changeset();
    m_parents = new QList<ulong>(*other.parents());
    m_childs = new QList<ulong>(*other.childs());
    setAuthor(other.author());
    setDate(other.date());
    setComment(other.comment());
    m_changelist = new QList<MctChange*>(*other.changes());
    m_corrected = other.isCorrected();
}

MctChangeset::~MctChangeset()
{
    delete m_parents;
    delete m_childs;
    delete m_changelist;
}

QList<ulong>* MctChangeset::parents() const
{
    return m_parents;
}

void MctChangeset::addParentId(ulong pid)
{
    if(!m_parents->contains(pid)) {
        m_parents->append(pid);
        m_changeset.setAttribute(MctStaticData::attributeNS(MctStaticData::PARENT + QString::number(m_parents->size()), MctStaticData::NS_C) , QString::number(pid));
    }
}

void MctChangeset::clearParents()
{
    /*foreach (ulong parent, *parents) {
        QString name = "parent" + QString::number(parent);
        changeset.removeAttribute(name);
    }*/
    for(int i=0; i < m_parents->length(); i++) {
        QString name = "parent" + QString::number(i+1);
        m_changeset.removeAttribute(name);
    }
    m_parents->clear();
}

int MctChangeset::parentId()
{
    QString parent = m_changeset.attribute("parent1", "0");
    return parent.toInt();
}

QList<ulong>* MctChangeset::childs() const
{
    return m_childs;
}

void MctChangeset::removeChild(ulong id)
{
    QMutableListIterator<ulong> it(*m_childs);
    while(it.hasNext()) {
        if(it.next() == id) {
            it.remove();
            break;
        }
    }
}

void MctChangeset::addChild(ulong id)
{
    if(!m_childs->contains(id)) {
        m_childs->append(id);
    }
}

/**
 * @brief This clear the list of children.
 */
void MctChangeset::clearChilds()
{
    m_childs->clear();
}

MctAuthor* MctChangeset::author() const
{
    if(m_author == NULL) {
        QString name = m_changeset.attribute(MctStaticData::attributeNS(MctStaticData::AUTHOR, MctStaticData::NS_DC), "Undefined");
        return new MctAuthor(name);
    }
    return m_author;
}

void MctChangeset::setAuthor(MctAuthor *author)
{
    m_changeset.setAttribute(MctStaticData::attributeNS(MctStaticData::AUTHOR, MctStaticData::NS_DC), author->name());
    this->m_author = author;
}

QString MctChangeset::comment() const
{
    if(m_comment.isEmpty()) {
        return m_changeset.attribute(MctStaticData::attributeNS(MctStaticData::COMMENT, MctStaticData::NS_DC),"");
    }
    return m_comment;
}

void MctChangeset::setComment(const QString &comment)
{
    this->m_comment = comment;
    m_changeset.setAttribute(MctStaticData::attributeNS(MctStaticData::COMMENT, MctStaticData::NS_DC), comment);
}

QDateTime MctChangeset::date() const
{
    if(!m_date.isValid()) {
        QString datestring = m_changeset.attribute(MctStaticData::attributeNS(MctStaticData::DATE, MctStaticData::NS_DC));
        return QDateTime::fromString(m_changeset.attribute(MctStaticData::attributeNS(MctStaticData::DATE, MctStaticData::NS_DC)).replace("T", " "), "yyyy-MM-dd hh:mm:ss");
        /*QString datestring = changeset.attribute("date");
        datestring.replace("T", " ");
        date = QDateTime::fromString(datestring, "yyyy-MM-dd hh:mm:ss");*/
    }
    return m_date;
}

void MctChangeset::setDate(const QDateTime &date)
{
    QString datestring = date.toString("yyyy-MM-dd hh:mm:ss");
    datestring.replace(" ", "T");
    m_changeset.setAttribute(MctStaticData::attributeNS(MctStaticData::DATE, MctStaticData::NS_DC), datestring);
    this->m_date = date;
}

ulong MctChangeset::id() const
{
    if(m_id == 0) {
        return m_changeset.attribute(MctStaticData::attributeNS(MctStaticData::ID, MctStaticData::NS_C), "0").toULong();
    }
    return m_id;
}

void MctChangeset::setId(unsigned long id)
{
    m_id = id;
    m_changeset.setAttribute(MctStaticData::attributeNS(MctStaticData::ID, MctStaticData::NS_C), QString::number(id));
}

void MctChangeset::addChange(MctChange *change)
{
    m_changelist->append(change);
}

QList<MctChange*>* MctChangeset::changes() const
{
    return m_changelist;
}

void MctChangeset::clearChangeset()
{
    foreach (MctChange* changeNode, *m_changelist) {
        m_changeset.removeChild(changeNode->changeNode());
    }
    m_changelist->clear();
}

QString MctChangeset::dateInString()
{
    QString author = this->m_author->name();
    QString nodestring = date().toString("yyyy-MM-dd hh:mm:ss") + " by: " + author;
    return nodestring;
}


QDomElement MctChangeset::changeset() const
{
    return m_changeset;
}

void MctChangeset::setChangeset(const QDomElement &changeset)
{
    this->m_changeset = changeset;
}

void MctChangeset::setCorrected(bool value)
{
    m_corrected = value;
}

bool MctChangeset::isCorrected() const
{
    return m_corrected;
}

void MctChangeset::printChangeset()
{
    qDebug() << "Printing changeset:";
    QString dateString = m_date.toString("yyyy.M.d.");
    QString timeString = m_date.toString("(H:m:s)");
    qDebug() << dateString << " " << timeString << " by: " << m_author->name();
    for (int i = 0; i < m_changelist->size(); i++){
        MctChange *change = m_changelist->at(i);
        qDebug() << change->position()->toString();
        if (change->position() == NULL)
            continue;
        if (change->changeType() == MovedString){
            qDebug() << change->changeType() << " " << change->position()->startChar() << " " << change->position()->endChar() <<
                        change->movedPosition()->startChar() << change->movedPosition()->endChar();
        } else {
            qDebug() << change->changeType() << " " << change->position()->startChar() << " " << change->position()->endChar();
        }
    }
    qDebug() << "-------------------";
}
