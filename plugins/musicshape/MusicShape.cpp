/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 * SPDX-FileCopyrightText: 2011 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "MusicShape.h"

#include <limits.h>

#include <QBuffer>
#include <QByteArray>
#include <QPainter>
#include <QSvgGenerator>

#include "MusicDebug.h"
#include <KLocalizedString>

#include <KoEmbeddedDocumentSaver.h>
#include <KoShapeSavingContext.h>
#include <KoViewConverter.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include "core/Bar.h"
#include "core/Chord.h"
#include "core/Clef.h"
#include "core/KeySignature.h"
#include "core/MusicXmlReader.h"
#include "core/MusicXmlWriter.h"
#include "core/Note.h"
#include "core/Part.h"
#include "core/Sheet.h"
#include "core/Staff.h"
#include "core/StaffSystem.h"
#include "core/TimeSignature.h"
#include "core/Voice.h"
#include "core/VoiceBar.h"

#include "Engraver.h"
#include "MusicStyle.h"
#include "Renderer.h"

using namespace MusicCore;

// static MusicShape* firstShape = 0;

MusicShape::MusicShape()
    : KoFrameShape("http://www.calligra.org/music", "shape")
    , m_firstSystem(0)
    , m_style(new MusicStyle)
    , m_engraver(new Engraver())
    , m_renderer(new MusicRenderer(m_style))
    , m_successor(nullptr)
    , m_predecessor(nullptr)
{
    /*    debugMusic << "firstShape:" << firstShape << "this:" << this;

        if (firstShape) {
            firstShape->m_successor = this;
            m_predecessor = firstShape;
            m_sheet = firstShape->m_sheet;
            m_firstSystem = firstShape->m_lastSystem+1;
            m_engraver->engraveSheet(m_sheet, m_firstSystem, QSizeF(1e9, 1e9), true, &m_lastSystem);
            firstShape = this;
        } else {
            firstShape = this;*/
    m_sheet = new Sheet();
    Bar *bar = m_sheet->addBar();

    Part *part = m_sheet->addPart(i18n("Part 1"));
    Staff *staff = part->addStaff();
    part->addVoice();
    bar->addStaffElement(new Clef(staff, 0, Clef::Trebble, 2, 0));
    bar->addStaffElement(new TimeSignature(staff, 0, 4, 4));
    // add some more default bars
    for (int i = 0; i < 9; i++) {
        m_sheet->addBar();
    }

    m_engraver->engraveSheet(m_sheet, 0, QSizeF(1e9, 1e9), true, &m_lastSystem);
    //    }
}

MusicShape::~MusicShape()
{
    // debugMusic << "destroying" << this;
    if (!m_predecessor && !m_successor) {
        delete m_sheet;
    }
    delete m_style;
    delete m_engraver;
    delete m_renderer;
    //    if (this == firstShape) firstShape = this->m_predecessor;
}

void MusicShape::setSize(const QSizeF &newSize)
{
    KoShape::setSize(newSize);

    engrave(false);
}

void MusicShape::paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &)
{
    constPaint(painter, converter);
}

void MusicShape::constPaint(QPainter &painter, const KoViewConverter &converter) const
{
    applyConversion(painter, converter);

    painter.setClipping(true);
    painter.setClipRect(QRectF(0, 0, size().width(), size().height()), Qt::IntersectClip);

    m_renderer->renderSheet(painter, m_sheet, m_firstSystem, m_lastSystem);
}

void MusicShape::saveOdf(KoShapeSavingContext &context) const
{
    // The name of this object in the ODF file.
    KoEmbeddedDocumentSaver &fileSaver = context.embeddedSaver();
    QString objectName = fileSaver.getFilename("Object");

    KoXmlWriter &writer = context.xmlWriter();
    writer.startElement("draw:frame");
    saveOdfAttributes(context, OdfAllAttributes);

    writer.startElement("music:shape");
    writer.addAttribute("xmlns:music", "http://www.calligra.org/music");
    MusicXmlWriter().writeSheet(writer, m_sheet, false);
    writer.endElement(); // music:shape

    const qreal previewZoom = 150 / 72.; // 150 DPI
    QSizeF imgSize = size(); // in points
    imgSize *= previewZoom;
    KoViewConverter converter;

    // Save a preview SVG image.
    // -------------------------

    // 1. Set up the svg renderer.
    QByteArray svgContents; // The contents
    QBuffer svgBuffer(&svgContents); // The corresponding QIODevice
    QSvgGenerator svg;
    svg.setOutputDevice(&svgBuffer); // Write to the buffer
    svg.setSize(imgSize.toSize());
    svg.setViewBox(QRect(0, 0, boundingRect().width(), boundingRect().height()));

    // 2. Paint the svg preview image.
    //
    // We need to create all text as paths, because otherwise it
    // will be difficult for most people to preview the SVG
    // image. Not many people have music fonts installed.
    QPainter svgPainter;
    svgPainter.begin(&svg);
    svgPainter.setRenderHint(QPainter::Antialiasing);
    svgPainter.setRenderHint(QPainter::TextAntialiasing);
    m_style->setTextAsPath(true);
    constPaint(svgPainter, converter);
    m_style->setTextAsPath(false);
    svgPainter.end();

    // 3. Create the xml to embed the svg image and the contents to the file.
    writer.startElement("draw:image");
    QString name = QString("ObjectReplacements/") + objectName + ".svg";
    writer.addAttribute("xlink:type", "simple");
    writer.addAttribute("xlink:show", "embed");
    writer.addAttribute("xlink:actuate", "onLoad");
    writer.addAttribute("xlink:href", name);
    writer.endElement(); // draw:image
    fileSaver.saveFile(name, "image/svg+xml", svgContents);

    // Save a preview bitmap image.
    // ----------------------------

    // 1. Create the image.
    QImage img(imgSize.toSize(), QImage::Format_ARGB32);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    converter.setZoom(previewZoom);
    constPaint(painter, converter);

    // 2. Create the xml to embed the svg image and the contents to the file.
    writer.startElement("draw:image");
    name = context.imageHref(img);
    // FIXME: Find out how to save a picture using the embeddedSaver and saveFile()
    // name = QString("ObjectReplacements/") + objectName + ".png";
    writer.addAttribute("xlink:type", "simple");
    writer.addAttribute("xlink:show", "embed");
    writer.addAttribute("xlink:actuate", "onLoad");
    writer.addAttribute("xlink:href", name);
    writer.endElement(); // draw:image

    saveOdfCommonChildElements(context);
    writer.endElement(); // draw:frame
}

bool MusicShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    loadOdfAttributes(element, context, OdfAllAttributes);
    return loadOdfFrame(element, context);
}

bool MusicShape::loadOdfFrameElement(const KoXmlElement &element, KoShapeLoadingContext & /*context*/)
{
    KoXmlElement score = KoXml::namedItemNS(element, "http://www.calligra.org/music", "score-partwise");
    if (score.isNull()) {
        warnMusic << "no music:score-partwise element as first child";
        return false;
    }
    Sheet *sheet = MusicXmlReader().loadSheet(score);
    if (sheet) {
        if (!m_predecessor && !m_successor) {
            delete m_sheet;
        }
        m_sheet = sheet;
        m_engraver->engraveSheet(m_sheet, m_firstSystem, size(), true, &m_lastSystem);
        return true;
    }
    return false;
}

Sheet *MusicShape::sheet()
{
    return m_sheet;
}

void MusicShape::setSheet(Sheet *sheet, int firstSystem)
{
    if (!m_predecessor && !m_successor) {
        delete m_sheet;
    }
    m_sheet = sheet;
    m_firstSystem = firstSystem;
    m_engraver->engraveSheet(m_sheet, m_firstSystem, size(), true, &m_lastSystem);
}

int MusicShape::firstSystem() const
{
    return m_firstSystem;
}

void MusicShape::setFirstSystem(int system)
{
    m_firstSystem = system;
    engrave();
    update();
}

int MusicShape::lastSystem() const
{
    return m_lastSystem;
}

int MusicShape::firstBar() const
{
    return m_sheet->staffSystem(m_firstSystem)->firstBar();
}

int MusicShape::lastBar() const
{
    int lastBar = INT_MAX;
    if (m_lastSystem < m_sheet->staffSystemCount() - 1) {
        lastBar = m_sheet->staffSystem(m_lastSystem + 1)->firstBar() - 1;
    }
    return lastBar;
}

MusicRenderer *MusicShape::renderer()
{
    return m_renderer;
}

void MusicShape::engrave(bool engraveBars)
{
    m_engraver->engraveSheet(m_sheet, m_firstSystem, size(), engraveBars, &m_lastSystem);
    if (m_successor) {
        m_successor->setFirstSystem(m_lastSystem + 1);
    }
}

MusicStyle *MusicShape::style()
{
    return m_style;
}
