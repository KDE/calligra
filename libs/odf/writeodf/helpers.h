/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2013 Jos van den Oever <jos@vandenoever.info>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef WRITEODF_HELPERS_H
#define WRITEODF_HELPERS_H

#include "writeodf/writeodfconfig.h"
#include "writeodf/writeodftext.h"

#include "koodf_export.h"

namespace writeodf
{

template<class T>
void addConfigItem(T &config, const QString &configName, const QString &value)
{
    config_config_item item(config.add_config_config_item(configName, "string"));
    item.addTextNode(value);
}

template<class T>
void addConfigItem(T &config, const QString &configName, bool value)
{
    config_config_item item(config.add_config_config_item(configName, "boolean"));
    item.addTextNode(value ? "true" : "false");
}

template<class T>
void addConfigItem(T &config, const QString &configName, int value)
{
    config_config_item item(config.add_config_config_item(configName, "int"));
    item.addTextNode(QString::number(value));
}

template<class T>
void addConfigItem(T &config, const QString &configName, double value)
{
    config_config_item item(config.add_config_config_item(configName, "double"));
    item.addTextNode(QString::number(value));
}

template<class T>
void addConfigItem(T &config, const QString &configName, float value)
{
    config_config_item item(config.add_config_config_item(configName, "double"));
    item.addTextNode(QString::number(value));
}

template<class T>
void addConfigItem(T &config, const QString &configName, long value)
{
    config_config_item item(config.add_config_config_item(configName, "long"));
    item.addTextNode(QString::number(value));
}

template<class T>
void addConfigItem(T &config, const QString &configName, short value)
{
    config_config_item item(config.add_config_config_item(configName, "short"));
    item.addTextNode(QString::number(value));
}

/**
 * Overloaded version of addTextSpan which takes an additional tabCache map.
 * @param content the content of the paragraph group
 * @param text the text to write
 * @param tabCache optional map allowing to find a tab for a given character index
 */
void KOODF_EXPORT addTextSpan(group_paragraph_content &content, const QString &text, const QMap<int, int> &tabCache);

/**
 * @brief Adds a text span as nodes of the current odf paragraph.
 *
 * It handles tabulations, linebreaks, and multiple spaces by using the
 * appropriate OASIS tags.
 *
 * @param e the content of the paragraph group
 * @param text the text to write
 */
inline void addTextSpan(group_paragraph_content &e, const QString &text)
{
    QMap<int, int> tabCache;
    addTextSpan(e, text, tabCache);
}

}

#endif
