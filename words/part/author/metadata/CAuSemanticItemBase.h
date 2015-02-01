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

#ifndef CAUSEMANTICITEMBASE_H
#define CAUSEMANTICITEMBASE_H

#include <KoRdfBasicSemanticItem.h>

/**
 * Base class for all Author semantic items.
 *
 * This implements "declare on subclassing" method to
 * store of QString and Int properties.
 *
 * That allows to automatically generate soprano queries
 * and hold RDF triples and the actual data in sync.
 */
class CAuSemanticItemBase : public KoRdfBasicSemanticItem
{
    Q_OBJECT

public:
    explicit CAuSemanticItemBase(QObject *parent, const KoDocumentRdf *rdf);

    void init(Soprano::QueryResultIterator &it);

    virtual Soprano::Node context() const;
    virtual Soprano::Node linkingSubject() const;
    virtual void updateFromEditorData();

protected:
    /**
     * Call this after updating from UI editors
     * to notify system about the changes in semantic items.
     */
    void finishUpdateFromEditorData();

    /**
     * You MUST call this function in constructor of subclass
     * to setup properties properly.
     */
    void setupProps();

    /**
     * This function should return the list of string properties
     * of subclass item.
     */
    virtual QList<QString> stringProps() = 0;

    /**
     * Returns string property by @p propName.
     */
    QString stringProp(const QString &propName);

    /**
     * Sets string property with name @p propName to @p value
     */
    void setStringProp(const QString &propName, const QString &value);

    /**
     * This function should return the list of integer properties
     * of subclass item.
     */
    virtual QList<QString> intProps() = 0;

    /**
     * Returns integer property by @p propName.
     */
    int intProp(const QString &propName);

    /**
     * Sets integer property with name @p propName to @p value
     */
    void setIntProp(const QString &propName, const int &value);

    /**
     * Forms Soprano query that can be used to retrieve semantic
     * items of the class. Recommended to use this in factories.
     */
    QString formQuery();

private:
    QString m_uri;
    bool m_isTypeSet;

    QMap<QString, QString> m_stringProps;
    QMap<QString, int> m_intProps;

};

#endif //CAUSEMANTICITEMBASE_H
