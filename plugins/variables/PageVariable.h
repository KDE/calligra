/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Pierre Ducroquet <pinaraf@gmail.com>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008 Sebastian Sauer <mail@dipe.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef PAGEVARIABLE_H
#define PAGEVARIABLE_H

#include <KoOdfNumberDefinition.h>
#include <KoTextPage.h>
#include <KoVariable.h>

class KoShapeSavingContext;

/**
 * This is a KoVariable for page numbers.
 */
class PageVariable : public KoVariable
{
public:
    enum PageType { PageCount, PageNumber, PageContinuation };

    /**
     * Constructor.
     */
    PageVariable();

    void readProperties(const KoProperties *props);

    void propertyChanged(Property property, const QVariant &value) override;

    /// reimplemented
    void saveOdf(KoShapeSavingContext &context) override;

    /// reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

private:
    void resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd) override;

    PageType m_type;

    KoTextPage::PageSelection m_pageselect;
    int m_pageadjust;
    bool m_fixed;
    KoOdfNumberDefinition m_numberFormat;

    QString m_continuation;
};

#endif
