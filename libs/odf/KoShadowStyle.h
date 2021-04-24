/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHADOWSTYLE_H
#define KOSHADOWSTYLE_H

#include "koodf_export.h"

#include <QColor>
#include <QMetaType>
#include <QPointF>
#include <QSharedData>

class KoShadowStylePrivate;

/**
 * A container and parser for shadows as defined in the
 * OpenDocument specification.
 * Applies to at least :
 * - graphic elements,
 * - headers-footers,
 * - pages,
 * - paragraphs,
 * - tables and table cells.
 */
class KOODF_EXPORT KoShadowStyle
{
public:
    /// Default constructor, constructs an empty shadow
    KoShadowStyle();
    /// Copy constructor
    KoShadowStyle(const KoShadowStyle &other);
    ~KoShadowStyle();


    // Holds data about one of the shadow this shadow contains
    struct KOODF_EXPORT ShadowData {
        ShadowData();
        bool operator==(const ShadowData &other) const;
        QColor color;
        QPointF offset;
        qreal radius;
    };


    bool operator==(const KoShadowStyle &other) const;
    bool operator!=(const KoShadowStyle &other) const;

    /**
     * Loads the given OpenDocument-defined shadow
     * in this KoShadow object.
     * @param shadow the shadow to parse
     * @return true when the parsing was successful
     */
    bool loadOdf(const QString &shadow);

    /**
     * Returns this shadow as a string formatted like an
     * OpenDocument-defined shadow.
     */
    QString saveOdf() const;

    /**
     * Returns the number of shadows that are contained in this shadow
     */
    int shadowCount() const;


private:
    QSharedDataPointer<KoShadowStylePrivate> d;
};

Q_DECLARE_TYPEINFO(KoShadowStyle::ShadowData, Q_MOVABLE_TYPE);

Q_DECLARE_METATYPE(KoShadowStyle)

#endif

