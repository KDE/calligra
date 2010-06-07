/*
 * This file is part of Office 2007 Filters for KOffice
 * Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * Copyright (C) 2003 David Faure <faure@kde.org>
 * Copyright (C) 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef PPTXIMPORT_H
#define PPTXIMPORT_H

#include <MsooXmlImport.h>

//! PPTX to ODP import filter
class PptxImport : public MSOOXML::MsooXmlImport
{
    Q_OBJECT
public:
    PptxImport(QObject * parent, const QStringList &);
    virtual ~PptxImport();

protected:
    virtual bool acceptsSourceMimeType(const QByteArray& mime) const;

    virtual bool acceptsDestinationMimeType(const QByteArray& mime) const;

    virtual KoFilter::ConversionStatus parseParts(KoOdfWriters *writers, MSOOXML::MsooXmlRelationships *relationships,
            QString& errorMessage);
    class Private;
    Private * const d;
};

#endif
