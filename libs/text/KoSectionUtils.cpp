/*
 *  SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <KoParagraphStyle.h>
#include <KoSectionUtils.h>

bool KoSectionUtils::getNextBlock(QTextCursor &cur)
{
    QTextCursor next = cur;
    bool ok = next.movePosition(QTextCursor::NextBlock);

    while (ok && next.currentFrame() != cur.currentFrame()) {
        ok = next.movePosition(QTextCursor::NextBlock);
    }

    if (!ok || next.currentFrame() != cur.currentFrame()) {
        // There is no previous block.
        return false;
    }
    cur = next;
    return true;
}

void KoSectionUtils::setSectionStartings(QTextBlockFormat &fmt, const QList<KoSection *> &list)
{
    if (list.empty()) {
        fmt.clearProperty(KoParagraphStyle::SectionStartings);
    } else {
        fmt.setProperty(KoParagraphStyle::SectionStartings, QVariant::fromValue<QList<KoSection *>>(list));
    }
}

void KoSectionUtils::setSectionEndings(QTextBlockFormat &fmt, const QList<KoSectionEnd *> &list)
{
    if (list.empty()) {
        fmt.clearProperty(KoParagraphStyle::SectionEndings);
    } else {
        fmt.setProperty(KoParagraphStyle::SectionEndings, QVariant::fromValue<QList<KoSectionEnd *>>(list));
    }
}

QList<KoSection *> KoSectionUtils::sectionStartings(const QTextBlockFormat &fmt)
{
    if (!fmt.hasProperty(KoParagraphStyle::SectionStartings)) {
        return QList<KoSection *>();
    } else {
        return fmt.property(KoParagraphStyle::SectionStartings).value<QList<KoSection *>>();
    }
}

QList<KoSectionEnd *> KoSectionUtils::sectionEndings(const QTextBlockFormat &fmt)
{
    if (!fmt.hasProperty(KoParagraphStyle::SectionEndings)) {
        return QList<KoSectionEnd *>();
    } else {
        return fmt.property(KoParagraphStyle::SectionEndings).value<QList<KoSectionEnd *>>();
    }
}
