/*
* This file is part of the KDE project
*
* SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
*
* Contact: Amit Aggarwal <amit.5.aggarwal@nokia.com>
*
* SPDX-FileCopyrightText: 2007-2009 Thorsten Zachmann <zachmann@kde.org>
*
SPDX-License-Identifier: LGPL-2.1-or-later
*
*/

#ifndef PRESENTATION_VARIABLE_H
#define PRESENTATION_VARIABLE_H

#include <KoVariable.h>

#include <KPrDeclarations.h>

/**
 * This is a PresentationVariable for document declaration.
 * Ex: - Footer, Header, Date-time etc
 */
class PresentationVariable : public KoVariable
{
public:
    /**
     * Constructor.
     */
    PresentationVariable();

    void setProperties(const KoProperties *props) override;

    /// reimplemented
    void saveOdf(KoShapeSavingContext &context) override;

    /// reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

private:
    void resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd) override;

    KPrDeclarations::Type m_type;
};

#endif
