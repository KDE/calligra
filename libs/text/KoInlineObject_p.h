/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOINLINEOBJECT_P_H
#define KOINLINEOBJECT_P_H

#include "TextDebug.h"
class KoTextInlineRdf;

class KoInlineObjectPrivate
{
public:
    KoInlineObjectPrivate()
        : manager(nullptr)
        , id(-1)
        , propertyChangeListener(0)
        , rdf(nullptr)
    {
    }
    virtual ~KoInlineObjectPrivate();

    KoInlineTextObjectManager *manager;
    int id;
    bool propertyChangeListener;
    KoTextInlineRdf *rdf; //< An inline object might have RDF, we own it.

    virtual QDebug printDebug(QDebug dbg) const;
};

#endif
