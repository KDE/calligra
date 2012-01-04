/*
 *  Copyright (c) 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOELEMENTREFERENCE_H
#define KOELEMENTREFERENCE_H

#include <QSharedDataPointer>
#include <QSharedData>
#include <QUuid>
#include <QWeakPointer>
#include <QMap>
#include <QSet>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>

#include "koodf_export.h"

class KoElementReferenceData : public QSharedData
{
public:

    KoElementReferenceData()
    {
        xmlid = QUuid::createUuid().toString();
        xmlid.remove('{');
        xmlid.remove('}');
    }

    KoElementReferenceData(const KoElementReferenceData &other)
        : QSharedData(other)
        , xmlid(other.xmlid)
    {
    }

    ~KoElementReferenceData() {}

    QString xmlid;
};

/**
 * KoElementReference is used to store unique identifiers for elements in an odf document.
 * Element references are saved as xml:id and optionally for compatibility also as draw:id
 * and text:id.
 *
 * You can use element references wherever you would have used a QString to refer to the id
 * of an object.
 *
 * Element references are implicitly shared, so you can and should pass them along by value.
 */
class KOODF_EXPORT KoElementReference
{
public:

    enum SaveOption {
        XMLID = 0x0,
        DRAWID = 0x1,
        TEXTID = 0x2
    };
    Q_DECLARE_FLAGS(SaveOptions, SaveOption)

    KoElementReference();
    KoElementReference(const QString &prefix);
    KoElementReference(const KoElementReference &other);
    KoElementReference &operator=(const KoElementReference &rhs);
    bool operator==(const KoElementReference &other);
    bool operator!=(const KoElementReference &other);

    /**
     * @return true if the xmlid is valid, i.e., not null
     */
    bool isValid() const;

    /**
     * @brief loadOdf creates a new KoElementReference from the given element. If the element
     *   does not have an xml:id, draw:id or text:id attribute, and invalid element reference
     *   is returned.
     * @param element the element that may contain xml:id, text:id or draw:id. xml:id has
     *    priority.
     * @return a new element reference
     */
    KoElementReference loadOdf(const KoXmlElement &element);

    /**
     * @brief saveOdf saves this element reference into the currently open element in the xml writer.
     * @param writer the writer we save to
     * @param saveOptions determins which attributes we save. We always save the xml:id.
     */
    void saveOdf(KoXmlWriter *writer, SaveOption saveOption = XMLID) const;


    /**
     * @brief toString creates a QString from the element reference
     * @return a string that represents the element. Can be used in maps etc.
     */
    QString toString() const;


private:

    QSharedDataPointer<KoElementReferenceData> d;
};

/**
 * Implement this class to be warned when the element reference you use
 * internally has changed in the map. This happens when more than one
 * element reference is relevant to a particular element on saving; all these
 * element references are then set to the same reference.
 */
class KoElementReferenceChangeListener : public QObject
{
    Q_OBJECT
public:
    virtual void elementReferenceChanged(KoElementReference oldReference, KoElementReference newReference) = 0;
};

/**
 * @brief The KoElementReferenceUpdateFacade class must be implemented by the object that is handed to the
 * ODF code for saving. The ODF code will for every element create a list of update facades and when the
 * element is closed, save a new element reference, then ask all update facades to update their internal
 * KoElementReferenceMaps, which will inform all KoElementReferenceChangeListener objects.
 */
class KoElementReferenceUpdateFacade
{
public:
    virtual ~KoElementReferenceUpdateFacade() {}
    virtual void updateReference(KoElementReference oldReference, KoElementReference newReference) = 0;
};

/**
 * This class is used in the situation where several parts of Calligra might be keeping
 * track of cross-references, but don't know about each other that they might be needing
 * that cross-reference about the same ODF element.
 *
 * It is internal to those parts; they only should expose a KoElementReferenceUpdateFacade to the
 * saving code.
 */
template<typename T>
class KoElementReferenceMap {

public:

    /**
     * @brief addListener adds a listener which will be informed when the reference changes
     *
     * Note that you don't need to manually remove the listener. If the listener object dies,
     * it will be removed from the set of listeners next time the references are updated.
     * @param listener the listener
     */
    void addListener(KoElementReferenceChangeListener *listener) {
        m_listeners.insert(listener);
    }

    /**
     * @brief updateReference is called whenever the given reference must be replaced everywhere
     *    by the new reference.
     * @param oldReference
     * @param newReference
     */
    void updateReference(KoElementReference oldReference, KoElementReference newReference) {
        // Update the maps
        if (m_referenceMap.contains(oldReference)) {
            T key = m_referenceMap[oldReference];
            m_referentMap[key] = newReference;
            m_referenceMap.remove(oldReference);
            m_referenceMap.insert(newReference, key);
        }
        // Update the listeners
        foreach(QWeakPointer<KoElementReferenceChangeListener> listener, m_listeners) {
            if (listener) {
                listener.data()->elementReferenceChanged(oldReference, newReference);
            }
            else {
                m_listeners.remove(listener);
            }
        }
    }

private:

    QMap<KoElementReference, T> m_referenceMap;
    QMap<T, KoElementReference> m_referentMap;
    QSet<QWeakPointer<KoElementReferenceChangeListener> > m_listeners;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KoElementReference::SaveOptions)

#endif // KOELEMENTREFERENCE_H
