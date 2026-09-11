/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#pragma once

#include "koodf_export.h"

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QString>

class KoXmlElement;
class KoXmlWriter;

class KOODF_EXPORT KoOdfScript
{
public:
    struct Script {
        QString language;
        QString name;
        QString content;
    };
    using Scripts = QList<Script>;

    using EventHandlers = QMap<QString, QString>;

    static Scripts loadScripts(const KoXmlElement &parent);
    static void saveScripts(KoXmlWriter &writer, const Scripts &scripts);

    static EventHandlers loadEventListeners(const KoXmlElement &parent);
    static void saveEventListeners(KoXmlWriter &writer, const EventHandlers &events);
};
