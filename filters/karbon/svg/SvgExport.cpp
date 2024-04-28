/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Lars Siebold <khandha5@gmx.net>
   SPDX-FileCopyrightText: 2002-2003, 2005 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2002, 2005-2006 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2002 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 2002 Lennart Kudling <kudling@kde.org>
   SPDX-FileCopyrightText: 2004 Nicolas Goutte <nicolasg@snafu.de>
   SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
   SPDX-FileCopyrightText: 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2005, 2007-2008 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006 Gábor Lehel <illissius@gmail.com>
   SPDX-FileCopyrightText: 2006 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2006 Christian Mueller <cmueller@gmx.de>
   SPDX-FileCopyrightText: 2006 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SvgExport.h"
#include <SvgWriter.h>

#include <KarbonDocument.h>
#include <KarbonPart.h>

#include <KoDocument.h>
#include <KoFilterChain.h>
#include <KoPAPageBase.h>
#include <KoPageLayout.h>

#include <KPluginFactory>

#include <QString>

K_PLUGIN_FACTORY_WITH_JSON(SvgExportFactory, "calligra_filter_karbon2svg.json", registerPlugin<SvgExport>();)

SvgExport::SvgExport(QObject *parent, const QVariantList &)
    : KoFilter(parent)
{
}

KoFilter::ConversionStatus SvgExport::convert(const QByteArray &from, const QByteArray &to)
{
    if (to != "image/svg+xml" || from != "application/vnd.oasis.opendocument.graphics")
        return KoFilter::NotImplemented;

    KoDocument *document = m_chain->inputDocument();
    if (!document)
        return KoFilter::ParsingError;

    KarbonDocument *karbonPart = dynamic_cast<KarbonDocument *>(document);
    if (!karbonPart)
        return KoFilter::WrongFormat;

    KoPAPageBase *page = karbonPart->pages().value(0);
    if (!page) {
        return KoFilter::WrongFormat;
    }
    const KoPageLayout &layout = page->pageLayout();
    const QSizeF size(layout.width, layout.height);
    SvgWriter writer(page->shapes(), size);
    if (!writer.save(m_chain->outputFile(), true))
        return KoFilter::CreationError;

    return KoFilter::OK;
}

#include "SvgExport.moc"
