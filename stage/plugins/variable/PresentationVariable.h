/*
* This file is part of the KDE project
*
* Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*
* Contact: Amit Aggarwal <amit.5.aggarwal@nokia.com>
*
* Copyright (C) 2007-2009 Thorsten Zachmann <zachmann@kde.org>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
* 02110-1301 USA
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

    ///reimplemented
    void saveOdf(KoShapeSavingContext &context) override;

    ///reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

private:
    void resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd) override;

    KPrDeclarations::Type m_type;
};

#endif

