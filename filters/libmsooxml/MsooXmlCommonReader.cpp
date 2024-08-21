/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#include "MsooXmlCommonReader.h"
#include <KoGenStyles.h>
#include <KoXmlWriter.h>

#include <QMap>

using namespace MSOOXML;

MsooXmlCommonReader::MsooXmlCommonReader(KoOdfWriters *writers)
    : MsooXmlReader(writers)
{
    init();
}

MsooXmlCommonReader::MsooXmlCommonReader(QIODevice *io, KoOdfWriters *writers)
    : MsooXmlReader(io, writers)
{
    init();
}

MsooXmlCommonReader::~MsooXmlCommonReader()
{
    delete m_currentTextStyleProperties;
    m_currentTextStyleProperties = nullptr;
}

void MsooXmlCommonReader::init()
{
    m_currentTextStyleProperties = nullptr;
    m_read_p_args = {};
    m_pPr_lvl = 0;
    m_addManifestEntryForPicturesDirExecuted = false;
    m_moveToStylesXml = false;
}

//! CASE #420
bool MsooXmlCommonReader::isDefaultTocStyle(const QString &name) const
{
    if (name == QLatin1String("TOCHeading"))
        return true;
    if (name.startsWith(QLatin1String("TOC"))) {
        const QString num(name.mid(3));
        if (num.length() == 1 && num[0].isDigit() && num[0] != '0')
            return true;
    }
    return false;
}

void MsooXmlCommonReader::setupParagraphStyle()
{
    if (m_moveToStylesXml) {
        m_currentParagraphStyle.setAutoStyleInStylesDotXml(true);
    }

    QString currentParagraphStyleName(mainStyles->insert(m_currentParagraphStyle));
    if (currentParagraphStyleName.isEmpty()) {
        currentParagraphStyleName = QLatin1String("Standard");
    }
    body->addAttribute("text:style-name", currentParagraphStyleName);
    m_paragraphStyleNameWritten = true;
    // debugMsooXml << "currentParagraphStyleName:" << currentParagraphStyleName;
}

class MediaTypeMap : public QMap<QByteArray, QByteArray>
{
public:
    MediaTypeMap()
    {
        insert("bmp", "image/x-bmp");
        insert("gif", "image/gif");
        insert("jpg", "image/jpeg");
        insert("jpeg", "image/jpeg");
        insert("jpe", "image/jpeg");
        insert("jfif", "image/jpeg");
        insert("tif", "image/tiff");
        insert("tiff", "image/tiff");
        insert("png", "image/png");
        insert("emf", "application/x-openoffice-wmf;windows_formatname=\"Image EMF\"");
        insert("wmf", "application/x-openoffice-wmf;windows_formatname=\"Image WMF\"");
        insert("bin", "application/vnd.sun.star.oleobject");
        insert("xls", "application/vnd.sun.star.oleobject");
        insert("doc", "application/vnd.sun.star.oleobject");
        insert("ppt", "application/vnd.sun.star.oleobject");
        insert("", "application/vnd.sun.star.oleobject");
    }
};

void MsooXmlCommonReader::pushCurrentDrawStyle(KoGenStyle *newStyle)
{
    m_drawStyleStack.append(m_currentDrawStyle);

    // This step also takes ownership, so we have to delete it when popping.
    m_currentDrawStyle = newStyle;
}

void MsooXmlCommonReader::popCurrentDrawStyle()
{
    Q_ASSERT(!m_drawStyleStack.isEmpty());

    delete m_currentDrawStyle;
    m_currentDrawStyle = m_drawStyleStack.last();
    m_drawStyleStack.removeLast();
}

Q_GLOBAL_STATIC(MediaTypeMap, g_mediaTypes)

void MsooXmlCommonReader::addManifestEntryForFile(const QString &path)
{
    if (path.isEmpty())
        return;

    if (path.endsWith('/')) { // dir
        manifest->addManifestEntry(path, QString());
        return;
    }
    const int lastDot = path.lastIndexOf(QLatin1Char('.'));
    const QByteArray ext(path.mid(lastDot + 1).toLatin1().toLower());
    manifest->addManifestEntry(path, g_mediaTypes->value(ext));
}

//! Adds manifest entry for "Pictures/"
void MsooXmlCommonReader::addManifestEntryForPicturesDir()
{
    if (m_addManifestEntryForPicturesDirExecuted)
        return;
    m_addManifestEntryForPicturesDirExecuted = true;
    manifest->addManifestEntry("Pictures/", QString());
}
