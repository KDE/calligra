/*
 *  SPDX-FileCopyrightText: 2011-2012 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOELEMENTREFERENCE_H
#define KOELEMENTREFERENCE_H

#include <QSharedDataPointer>
#include <QSharedData>
#include <QUuid>

#include "KoXmlReaderForward.h"

#include "koodf_export.h"

class KoXmlWriter;

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

    enum GenerationOption {
        UUID = 0,
        Counter = 1
    };

    enum SaveOption {
        XmlId = 0x0,
        DrawId = 0x1,
        TextId = 0x2
    };
    Q_DECLARE_FLAGS(SaveOptions, SaveOption)

    KoElementReference();
    explicit KoElementReference(const QString &prefix);
    KoElementReference(const QString &prefix, int counter);
    KoElementReference(const KoElementReference &other);
    KoElementReference &operator=(const KoElementReference &rhs);
    bool operator==(const KoElementReference &other) const;
    bool operator!=(const KoElementReference &other) const;

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
     * @param saveOption determines which attributes we save. We always save the xml:id.
     */
    void saveOdf(KoXmlWriter *writer, SaveOption saveOption = XmlId) const;

    /**
     * @brief toString creates a QString from the element reference
     * @return a string that represents the element. Can be used in maps etc.
     */
    QString toString() const;

    /**
     * Invalidate the reference
     */
    void invalidate();


private:

    QSharedDataPointer<KoElementReferenceData> d;
};


Q_DECLARE_OPERATORS_FOR_FLAGS(KoElementReference::SaveOptions)

#endif // KOELEMENTREFERENCE_H
