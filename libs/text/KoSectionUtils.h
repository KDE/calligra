/*
 *  SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KOSECTIONUTILS_H
#define KOSECTIONUTILS_H

#include <KoSection.h>
#include <KoSectionEnd.h>

#include <QString>
#include <QTextCursor>
#include <QVariant>

namespace KoSectionUtils
{
/**
 * Moves the cursors to the next block within the same QTextFrame.
 * @param cur cursor to move, modified during call
 * @return @c false if there is no next block, @c true otherwise
 */
bool getNextBlock(QTextCursor &cur);

/**
 * Convenient function to set a list of startings to QTextBlockFormat.
 * This checks that list is empty.
 *
 * @param fmt QTextBlockFormat reference to set startings.
 * @param list QList<KoSection *> is a list to set.
 */
KOTEXT_EXPORT void setSectionStartings(QTextBlockFormat &fmt, const QList<KoSection *> &list);

/**
 * Convenient function to set a list of endings to QTextBlockFormat.
 * This checks that list is empty.
 *
 * @param fmt QTextBlockFormat reference to set endings.
 * @param list QList<KoSectionEnd *> is a list to set.
 */
KOTEXT_EXPORT void setSectionEndings(QTextBlockFormat &fmt, const QList<KoSectionEnd *> &list);

/**
 * Convenient function to get section startings from QTextBlockFormat.
 * @param fmt QTextBlockFormat format to retrieve section startings from.
 * @return QList<KoSection *> that contains pointers to sections that start
 *                            according to QTextBlockFormat.
 */
KOTEXT_EXPORT QList<KoSection *> sectionStartings(const QTextBlockFormat &fmt);

/**
 * Convenient function to get section endings from QTextBlockFormat.
 * @param fmt QTextBlockFormat format to retrieve section startings from.
 * @return QList<KoSectionEnd *> that contains pointers to sections that end
 *                               according to QTextBlockFormat.
 */
KOTEXT_EXPORT QList<KoSectionEnd *> sectionEndings(const QTextBlockFormat &fmt);

}

#endif // KOSECTIONUTILS_H
