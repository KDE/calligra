/* This file is part of the KDE project
 * Copyright (C) 2002-2005,2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2002-2004 Nicolas Goutte <nicolasg@snafu.de>
 * Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2005-2009 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2005,2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2006-2007 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2007-2008 Thorsten Zachmann <t.zachmann@zagge.de>

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "SvgImport.h"
#include "SvgParser.h"

#include <KarbonPart.h>
#include <KarbonDocument.h>

#include <KoShape.h>
#include <KoShapeLayer.h>
#include <KoShapeGroup.h>
#include <KoFilterChain.h>
#include <commands/KoShapeUngroupCommand.h>
#include <KoXmlReader.h>
#include <KoPAPage.h>
#include <KoPAMasterPage.h>
#include <KoPageLayout.h>

#include <kpluginfactory.h>
#include <KCompressionDevice>

#include <QFileInfo>
#include <QDebug>
#include <QLoggingCategory>

K_PLUGIN_FACTORY_WITH_JSON(SvgImportFactory, "calligra_filter_svg2karbon.json",
                           registerPlugin<SvgImport>();)

const QLoggingCategory &SVG_LOG()
{
    static const QLoggingCategory category("calligra.filter.svg2karbon");
    return category;
}

#define debugSvg qCDebug(SVG_LOG)
#define warnSvg qCWarning(SVG_LOG)
#define errorSvg qCCritical(SVG_LOG)


SvgImport::SvgImport(QObject*parent, const QVariantList&)
        : KoFilter(parent), m_document(0)
{
}

SvgImport::~SvgImport()
{
}

KoFilter::ConversionStatus SvgImport::convert(const QByteArray& from, const QByteArray& to)
{
    // check for proper conversion
    if (to != "application/vnd.oasis.opendocument.graphics")
        return KoFilter::NotImplemented;
    if (from != "image/svg+xml" && from != "image/svg+xml-compressed")
        return KoFilter::NotImplemented;

    //Find the last extension
    QString strExt;
    QString fileIn(m_chain->inputFile());
    const int result = fileIn.lastIndexOf('.');
    if (result >= 0)
        strExt = fileIn.mid(result).toLower();

    const KCompressionDevice::CompressionType compressionType =
        (strExt == QLatin1String(".gz"))         //in case of .svg.gz (logical extension)
         || (strExt == QLatin1String(".svgz")) ? //in case of .svgz (extension used prioritary)
            KCompressionDevice::GZip :
        (strExt == QLatin1String(".bz2")) ?      //in case of .svg.bz2 (logical extension)
            KCompressionDevice::BZip2 :
            KCompressionDevice::None;

    /*debugSvg <<"File extension: -" << strExt <<"- Compression:" << strMime;*/

    QIODevice* in = new KCompressionDevice(fileIn, compressionType);
    if (!in->open(QIODevice::ReadOnly)) {
        errorSvg << "Cannot open file! Aborting!" << endl;
        delete in;
        return KoFilter::FileNotFound;
    }

    int line, col;
    QString errormessage;

    KoXmlDocument inputDoc;

    const bool parsed = inputDoc.setContent(in, &errormessage, &line, &col);

    in->close();
    delete in;

    if (! parsed) {
        errorSvg << "Error while parsing file: "
        << "at line " << line << " column: " << col
        << " message: " << errormessage << endl;
        // ### TODO: feedback to the user
        return KoFilter::ParsingError;
    }

    m_document = dynamic_cast<KarbonDocument*>(m_chain->outputDocument());
    if (!m_document) {
        return KoFilter::CreationError;
    }
    if (m_document->pages().isEmpty()) {
        KoPAMasterPage *mp = dynamic_cast<KoPAMasterPage*>(m_document->pages(true).value(0));
        if (!mp) {
            mp = new KoPAMasterPage();
            m_document->insertPage(mp, 0);
        }
        m_document->insertPage(new KoPAPage(mp), 0);
    }
    // Do the conversion!
    convert(inputDoc.documentElement());

    return KoFilter::OK;
}

void SvgImport::convert(const KoXmlElement &rootElement)
{
    if (! m_document)
        return;

    // set default page size to A4
    QSizeF pageSize(550.0, 841.0);

    SvgParser parser(m_document->resourceManager());

    parser.setXmlBaseDir(QFileInfo(m_chain->inputFile()).filePath());

    QList<KoShape*> toplevelShapes = parser.parseSvg(rootElement, &pageSize);
    // parse the root svg element
    buildDocument(toplevelShapes, parser.shapes());

    // set the page size
    KoPageLayout & layout = m_document->pages().at(0)->pageLayout();
    layout.width = pageSize.width();
    layout.height = pageSize.height();
}

void SvgImport::buildDocument(const QList<KoShape*> &toplevelShapes, const QList<KoShape*> &shapes)
{
    Q_UNUSED(shapes);
    KoPAPageBase *page = m_document->pages().first();
    // if we have only top level groups, make them layers
    bool onlyTopLevelGroups = true;
    foreach(KoShape * shape, toplevelShapes) {
        if (! dynamic_cast<KoShapeGroup*>(shape) || shape->filterEffectStack()) {
            onlyTopLevelGroups = false;
            break;
        }
    }
    KoShapeLayer *oldLayer = 0;
    if (page->shapeCount()) {
        oldLayer = dynamic_cast<KoShapeLayer*>(page->shapes().first());
    }
    if (onlyTopLevelGroups) {
        foreach(KoShape * shape, toplevelShapes) {
            // ungroup toplevel groups
            KoShapeGroup *group = dynamic_cast<KoShapeGroup*>(shape);
            QList<KoShape*> children = group->shapes();
            KoShapeUngroupCommand cmd(group, children, QList<KoShape*>() << group);
            cmd.redo();

            KoShapeLayer *layer = new KoShapeLayer();
            foreach(KoShape * child, children) {
                layer->addShape(child);
            }
            if (!group->name().isEmpty()) {
                layer->setName(group->name());
            }
            layer->setVisible(group->isVisible());
            layer->setZIndex(group->zIndex());
            page->addShape(layer);
            delete group;
        }
    } else {
        KoShapeLayer *layer = new KoShapeLayer();
        foreach(KoShape * shape, toplevelShapes) {
            layer->addShape(shape);
        }
        page->addShape(layer);
    }
    if (oldLayer) {
        page->removeShape(oldLayer);
        delete oldLayer;
    }
}

#include "SvgImport.moc"
