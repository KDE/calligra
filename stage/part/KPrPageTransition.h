/* This file is part of the KDE project
 * Copyright (C) 2020 Dag Andersen <danders@get2net.dk>
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

#ifndef KPRPAGETRANSITION_H
#define KPRPAGETRANSITION_H

#include "stage_export.h"

#include <QMap>
#include <QDebug>

class KoShapeLoadingContext;
class KoXmlElement;
class KoGenStyle;

class STAGE_EXPORT KPrPageTransition
{
public:
    KPrPageTransition();

    /// Odf 20.233 presentation:transition-type
    /// The presentation:transition-type attribute specifies the mode of a transition.
    enum Type {
        Manual,         /// Slide transition and shape effects are started separately by the user
        Automatic,      /// Slide transition and shape effects start automatically
        SemiAutomatic   /// Slide transition starts automatically, shape effects are started by the user.
    }; // NOTE: If this is changed, also update KPrPageEffectDocker

    /// Return the transition type
    Type type() const;
    /// Set the transition type to @p type
    void setType(Type type);
    /// Return the odf name of the type
    QString odfName() const;

    /// Return the presentation duration
    qreal duration() const;
    /// Set presentation duration to @p duration
    /// @p duration must be in seconds, e.g. 3.5
    void setDuration(qreal duration);
    /// Return presentation duration in milliseconds
    int milliseconds() const;

    void saveOdfAttributes(KoGenStyle &style) const;
    bool loadOdfAttributes(const KoXmlElement &element, KoShapeLoadingContext &context);

private:
    Type m_type;
    qreal m_duration; // duration in seconds
    QMap<Type, QString> m_odfNames;
};
STAGE_EXPORT QDebug operator<<(QDebug dbg, const KPrPageTransition &t);

#endif
