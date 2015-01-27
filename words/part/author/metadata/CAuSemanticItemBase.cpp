/* This file is part of the KDE project
 *   Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "CAuSemanticItemBase.h"

#include <author/metadata/CAuMetaDataManager.h>
#include <KoDocumentRdf.h>

#include <QUuid>

#include <QDebug>

using namespace Soprano;

CAuSemanticItemBase::CAuSemanticItemBase(QObject *parent, const KoDocumentRdf *rdf)
    : KoRdfBasicSemanticItem(parent, rdf)
    , m_uri(QUuid::createUuid().toString())
    , m_isTypeSet(false)
{
}

void CAuSemanticItemBase::init(QueryResultIterator &it)
{
    m_uri = it.binding("uri").toString();
    m_isTypeSet = true;
    foreach (const QString &propName, stringProps()) {
        m_stringProps[propName] = it.binding(propName).toString();
    }

    foreach (const QString &propName, intProps()) {
        m_intProps[propName] = it.binding(propName).toString().toInt();
    }
}


void CAuSemanticItemBase::setupProps()
{
    foreach (const QString &propName, stringProps()) {
        m_stringProps[propName] = "";
    }

    foreach (const QString &propName, intProps()) {
        m_intProps[propName] = 0;
    }
}

QString CAuSemanticItemBase::formQuery()
{
    QString authorPrefix = CAuMetaDataManager::AUTHOR_PREFIX;
    QString typePrefix = CAuMetaDataManager::AUTHOR_PREFIX + className() + "#";

    QString bindingsList, queryList;
    foreach (const QString &propName, stringProps()) {
        bindingsList += " ?" + propName;
        queryList += QString("?uri type:%1 ?%1 . \n").arg(propName);
    }

    foreach (const QString &propName, intProps()) {
        bindingsList += " ?" + propName;
        queryList += QString("?uri type:%1 ?%1 . \n").arg(propName);
    }

    QString result = QString(
        "PREFIX rdf:  <http://www.w3.org/1999/02/22-rdf-syntax-ns#> \n"
        "PREFIX cau:  <%1> \n"
        "PREFIX type: <%2> \n"
        "SELECT DISTINCT ?graph ?uri ?magicid%3\n"
        "WHERE { \n"
        "  GRAPH ?graph { \n"
        "    ?uri rdf:type cau:%4 . \n"
        "    ?uri type:magicid ?magicid . \n"
        "%5"
        "    }\n"
        "}\n").arg(authorPrefix, typePrefix, bindingsList, className(), queryList);

   return result;
}

void CAuSemanticItemBase::updateFromEditorData()
{
    if (!m_isTypeSet) {
        setRdfType(CAuMetaDataManager::AUTHOR_PREFIX + className());
        m_isTypeSet = true;
    }

    updateTriple(m_uri, m_uri, CAuMetaDataManager::AUTHOR_PREFIX + className() + "#magicid");
}

Node CAuSemanticItemBase::linkingSubject() const
{
    return Node::createResourceNode(m_uri);
}

Node CAuSemanticItemBase::context() const
{
    return (m_context.isValid() ? m_context : CAuMetaDataManager::authorContext());
}

QString CAuSemanticItemBase::stringProp(const QString &propName)
{
    return m_stringProps[propName];
}

void CAuSemanticItemBase::setStringProp(const QString &propName, QString value)
{
    updateTriple(
        m_stringProps[propName],
        value,
        CAuMetaDataManager::AUTHOR_PREFIX + className() + "#" + propName
    );
}

int CAuSemanticItemBase::intProp(const QString &propName)
{
    return m_intProps[propName];
}

void CAuSemanticItemBase::setIntProp(const QString &propName, int value)
{
    updateTriple(
        m_stringProps[propName],
        QString::number(value),
        CAuMetaDataManager::AUTHOR_PREFIX + className() + "#" + propName
    );
}

void CAuSemanticItemBase::finishUpdateFromEditorData()
{
    if (documentRdf()) {
        const_cast<KoDocumentRdf*>(documentRdf())->emitSemanticObjectUpdated(hKoRdfBasicSemanticItem(this));
    }
}
