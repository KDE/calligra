/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2002, 2003 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2003 Nicolas GOUTTE <goutte@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOGLOBAL_H
#define KOGLOBAL_H

#include <QFont>
#include <QMap>
#include <QStringList>

#include "kowidgets_export.h"

class KConfig;

class KOWIDGETS_EXPORT KoGlobal
{
public:
    KoGlobal();

    /// For KoApplication
    static void initialize()
    {
        (void)self(); // I don't want to make KGlobal instances public, so self() is private
    }
    /**
     * Return the default font for Calligra programs.
     * This is (currently) the same as the KDE-global default font,
     * except that it is guaranteed to have a point size set,
     * never a pixel size (see @ref QFont).
     */
    static QFont defaultFont()
    {
        return self()->_defaultFont();
    }

    /**
     * @return the global KConfig object around calligrarc.
     * calligrarc is used for Calligra-wide settings, from totally unrelated classes,
     * so this is the centralization of the KConfig object so that the file is
     * parsed only once
     */
    static KConfig *calligraConfig()
    {
        return self()->_calligraConfig();
    }

    /// Return the list of available languages, in their displayable form
    /// (translated names)
    static QStringList listOfLanguages()
    {
        return self()->_listOfLanguages();
    }
    /// Return the list of available languages, in their internal form
    /// e.g. "fr" or "en_US", here called "tag"
    static QStringList listOfLanguageTags()
    {
        return self()->_listOfLanguageTags();
    }
    /// For a given language display name, return its tag
    static QString tagOfLanguage(const QString &_lang);
    /// For a given language tag, return its display name
    static QString languageFromTag(const QString &_lang);

    ~KoGlobal();

private:
    static KoGlobal *self();

    QFont _defaultFont();
    QStringList _listOfLanguages();
    QStringList _listOfLanguageTags();
    KConfig *_calligraConfig();
    void createListOfLanguages();

    int m_pointSize;
    typedef QMap<QString, QString> LanguageMap;
    LanguageMap m_langMap; // display-name -> language tag
    KConfig *m_calligraConfig;
    // No BC problem here, constructor is private, feel free to add members

    friend class this_is_a_singleton; // work around gcc warning
};

#endif // KOGLOBAL
