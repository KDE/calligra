/* This file is part of the KDE libraries
   Copyright (c) 2001 Hans Petter Bieker <bieker@kde.org>
   Copyright (c) 2012, 2013 Chusslove Illich <caslav.ilic@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KCATALOG_H
#define KCATALOG_H

#include <QtCore/QString>
#include <QtCore/QByteArray>

class KCatalogPrivate;

/**
 * This class abstracts a Gettext message catalog.
 * It takes care of needed Gettext bindings.
 *
 * @see KLocalizedString
 * @internal
 */
class KCatalog
{
public:
    /**
     * Constructor.
     *
     * @param name translation domain
     * @param language translation language
     */
    KCatalog(const QByteArray &domain, const QString &language);

    /**
     * Destructor.
     */
    ~KCatalog();

    /**
     * Get translation of the given message text.
     *
     * Do not pass empty message text.
     *
     * @param msgid message text
     *
     * @return translated message if found, <tt>QString()</tt> otherwise
     */
    QString translate(const QByteArray &msgid) const;

    /**
     * Get translation of the given message text with given context.
     *
     * Do not pass empty message text.
     *
     * @param msgctxt message context
     * @param msgid message text
     *
     * @return translated message if found, <tt>QString()</tt> otherwise
     */
    QString translate(const QByteArray &msgctxt, const QByteArray &msgid) const;

    /**
     * Get translation of given message with plural forms.
     *
     * Do not pass empty message text.
     *
     * @param msgid singular message text
     * @param msgid_plural plural message text
     * @param n number for which the plural form is needed
     *
     * @return translated message if found, <tt>QString()</tt> otherwise
     */
    QString translate(const QByteArray &msgid, const QByteArray &msgid_plural,
                      qulonglong n) const;

    /**
     * Get translation of given message with plural forms with given context.
     *
     * Do not pass empty message text.
     *
     * @param msgctxt message context
     * @param msgid singular message text
     * @param msgid_plural plural message text
     * @param n number for which the plural form is needed
     *
     * @return translated message if found, <tt>QString()</tt> otherwise
     */
    QString translate(const QByteArray &msgctxt, const QByteArray &msgid,
                      const QByteArray &msgid_plural, qulonglong n) const;

    /**
     * Find the locale directory for the given catalog in the given language.
     *
     * @param name name of the catalog
     * @param language language of the catalog
     *
     * @return the locale directory if found, <tt>QByteArray()</tt> otherwise.
     */
    static QString catalogLocaleDir(const QByteArray &domain,
                                    const QString &language);

private:
    Q_DISABLE_COPY(KCatalog);

    KCatalogPrivate* const d;
};

#endif
