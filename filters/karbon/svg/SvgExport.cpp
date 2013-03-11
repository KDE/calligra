/* This file is part of the KDE project
   Copyright (C) 2002 Lars Siebold <khandha5@gmx.net>
   Copyright (C) 2002-2003,2005 Rob Buis <buis@kde.org>
   Copyright (C) 2002,2005-2006 David Faure <faure@kde.org>
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2004 Nicolas Goutte <nicolasg@snafu.de>
   Copyright (C) 2005 Boudewijn Rempt <boud@valdyas.org>
   Copyright (C) 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2005,2007-2008 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2006 Gábor Lehel <illissius@gmail.com>
   Copyright (C) 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2006 Christian Mueller <cmueller@gmx.de>
   Copyright (C) 2006 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>

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

#include "SvgExport.h"
#include <SvgWriter.h>

#include <KarbonDocument.h>
#include <KarbonPart.h>
#include <KarbonKoDocument.h>

#include <KoDocument.h>
#include <KoFilterChain.h>

#include <KPluginFactory>

#include <QString>

K_PLUGIN_FACTORY(SvgExportFactory, registerPlugin<SvgExport>();)
K_EXPORT_PLUGIN(SvgExportFactory("calligrafilters"))

SvgExport::SvgExport(QObject*parent, const QVariantList&)
    : KoFilter(parent)
{
}

KoFilter::ConversionStatus SvgExport::convert(const QByteArray& from, const QByteArray& to)
{
    if (to != "image/svg+xml" || from != "application/vnd.oasis.opendocument.graphics")
        return KoFilter::NotImplemented;

    KoDocument * document = m_chain->inputDocument();
    if (!document)
        return KoFilter::ParsingError;

    KarbonKoDocument * karbonPart = dynamic_cast<KarbonKoDocument*>(document);
    if (!karbonPart)
        return KoFilter::WrongFormat;

    const KarbonDocument &data = karbonPart->document();
    SvgWriter writer(data.layers(), data.pageSize());
    if (!writer.save(m_chain->outputFile(), true))
        return KoFilter::CreationError;

    return KoFilter::OK;
}

#include "SvgExport.moc"
