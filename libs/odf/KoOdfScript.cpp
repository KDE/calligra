/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoOdfScript.h"

#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <QDomDocument>

#include <utility>

using namespace Qt::StringLiterals;

KoOdfScript::Scripts KoOdfScript::loadScripts(const KoXmlElement &parent)
{
    for (KoXmlNode node = parent.firstChild(); !node.isNull(); node = node.nextSibling()) {
        if (!node.isElement()) {
            continue;
        }
        const auto element = node.toElement();
        if (element.localName() != "scripts"_L1 && element.tagName() != "office:scripts"_L1) {
            continue;
        }
        Scripts scripts;
        for (KoXmlNode scriptNode = element.firstChild(); !scriptNode.isNull(); scriptNode = scriptNode.nextSibling()) {
            if (!scriptNode.isElement()) {
                continue;
            }
            const auto scriptElement = scriptNode.toElement();
            if (scriptElement.localName() != "script"_L1 && scriptElement.tagName() != "script:script"_L1) {
                continue;
            }
            QDomDocument document;
            KoXml::asQDomElement(document, scriptElement);
            Script script;
            script.language = scriptElement.attributeNS(KoXmlNS::script, u"language"_s, scriptElement.attribute(u"script:language"_s));
            script.name = scriptElement.attributeNS(KoXmlNS::script, u"name"_s, scriptElement.attribute(u"script:name"_s));
            script.content = scriptElement.text();
            scripts.append(std::move(script));
        }
        return scripts;
    }
    return {};
}

void KoOdfScript::saveScripts(KoXmlWriter &writer, const Scripts &scripts)
{
    if (scripts.isEmpty()) {
        return;
    }
    writer.startElement("office:scripts");
    for (const auto &script : scripts) {
        writer.startElement("script:script");
        if (!script.language.isEmpty()) {
            writer.addAttribute("script:language", script.language);
        }
        if (!script.name.isEmpty()) {
            writer.addAttribute("script:name", script.name);
        }
        writer.addTextNode(script.content);
        writer.endElement();
    }
    writer.endElement();
}

KoOdfScript::EventHandlers KoOdfScript::loadEventListeners(const KoXmlElement &parent)
{
    EventHandlers events;
    for (KoXmlNode node = parent.firstChild(); !node.isNull(); node = node.nextSibling()) {
        if (!node.isElement()) {
            continue;
        }
        const auto listeners = node.toElement();
        if (listeners.localName() != "event-listeners"_L1 && listeners.tagName() != "office:event-listeners"_L1) {
            continue;
        }
        for (KoXmlNode listenerNode = listeners.firstChild(); !listenerNode.isNull(); listenerNode = listenerNode.nextSibling()) {
            if (!listenerNode.isElement()) {
                continue;
            }
            const auto listener = listenerNode.toElement();
            if (listener.localName() != "event-listener"_L1 && listener.tagName() != "script:event-listener"_L1) {
                continue;
            }
            const QString event = listener.attributeNS(KoXmlNS::script, u"event-name"_s, listener.attribute(u"script:event-name"_s));
            if (!event.isEmpty()) {
                events.insert(event, listener.attributeNS(KoXmlNS::xlink, u"href"_s, listener.attribute(u"xlink:href"_s)));
            }
        }
    }
    return events;
}

void KoOdfScript::saveEventListeners(KoXmlWriter &writer, const EventHandlers &events)
{
    if (events.isEmpty()) {
        return;
    }
    writer.startElement("office:event-listeners");
    for (auto it = events.cbegin(); it != events.cend(); ++it) {
        writer.startElement("script:event-listener");
        writer.addAttribute("script:event-name", it.key());
        writer.addAttribute("xlink:href", it.value());
        writer.addAttribute("xlink:type", "simple");
        writer.endElement();
    }
    writer.endElement();
}
