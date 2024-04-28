/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOTEXTREFERENCE_H
#define KOTEXTREFERENCE_H

#include "KoVariable.h"

class KoTextLocator;

/**
 * This variable displays information about a text reference.
 * A user can insert characters that are called locators.  And are represented by a KoTextLocator
 * the user can then insert (several) KoTextReference variables that will update the textual description
 * of the locator whenever text is re-layouted.
 * This effectively means that the reference will print the page number (for example) of where the
 * locator is and keep it updated automatically.
 */
class KoTextReference : public KoVariable
{
    Q_OBJECT
public:
    /**
     * Constructor; please don't use directly as the KoInlineTextObjectManager will supply an action
     * to create one.
     * @param indexId the index of the inline object that is the locator.  See KoInlineObject::id()
     */
    explicit KoTextReference(int indexId);
    ~KoTextReference() override;

    void variableMoved(const QTextDocument *document, int posInDocument) override;
    void setup() override;
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    void saveOdf(KoShapeSavingContext &context) override;

private:
    KoTextLocator *locator();
    int m_indexId;
    // TODO store a config of what we actually want to show.  The hardcoded pagenumber is not enough.
    // we want 'section' / chapter name/number and maybe word.  All in a nice formatted text.
    // see also the ODF spec.
};

#endif
