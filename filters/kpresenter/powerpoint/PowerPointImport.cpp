/* This file is part of the KDE project
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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
 * Boston, MA 02110-1301, USA.
*/

#include <PowerPointImport.h>

#include "PptToOdp.h"

#include <QBuffer>
#include <QColor>
#include <QString>
#include <QMatrix>
#include <QPair>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <kgenericfactory.h>
#include <KoXmlNS.h>
#include <KoOdf.h>

typedef KGenericFactory<PowerPointImport> PowerPointImportFactory;
K_EXPORT_COMPONENT_FACTORY(libpowerpointimport,
                           PowerPointImportFactory("kofficefilters"))

KoFilter::ConversionStatus PowerPointImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "application/vnd.ms-powerpoint")
        return KoFilter::NotImplemented;

    if (to != KoOdf::mimeType(KoOdf::Presentation))
        return KoFilter::NotImplemented;

    PptToOdp ppttoodp;
    return ppttoodp.convert(m_chain->inputFile(), m_chain->outputFile(),
                            KoStore::Zip);
}
#include "PowerPointImport.moc"
