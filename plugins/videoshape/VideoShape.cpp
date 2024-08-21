/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2009 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "VideoShape.h"

#include <KoViewConverter.h>
#include <VideoCollection.h>
#include <VideoData.h>
#include <VideoEventAction.h>
#ifdef SHOULD_BUILD_THUMBNAIL
#include <VideoThumbnailer.h>
#endif
#include "VideoDebug.h"

#include <KoIcon.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoStore.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include <QPainter>
#include <QUrl>

VideoShape::VideoShape()
    : KoFrameShape(KoXmlNS::draw, "plugin")
    , m_videoCollection(nullptr)
    , m_videoEventAction(new VideoEventAction(this))
#ifdef SHOULD_BUILD_THUMBNAIL
    , m_thumbnailer(new VideoThumbnailer())
#endif
    , m_oldVideoData(nullptr)
    , m_icon(koIcon("video-x-generic"))
{
    setKeepAspectRatio(true);
    addEventAction(m_videoEventAction);
}

VideoShape::~VideoShape()
{
#ifdef SHOULD_BUILD_THUMBNAIL
    delete m_thumbnailer;
#endif
}

void VideoShape::paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &)
{
    QRectF pixelsF = converter.documentToView(QRectF(QPointF(0, 0), size()));

    VideoData *currentVideoData = videoData();
#ifdef SHOULD_BUILD_THUMBNAIL
    if (currentVideoData && currentVideoData != m_oldVideoData) {
        // generate thumbnails
        m_oldVideoData = currentVideoData;
        m_thumbnailer->createThumbnail(currentVideoData, pixelsF.size().toSize());
    }
    QImage thumnailImage = m_thumbnailer->thumbnail();
    if (thumnailImage.isNull()) {
        painter.fillRect(pixelsF, QColor(Qt::gray));
        painter.setPen(QPen(Qt::black, 0));
        painter.drawRect(pixelsF);

        m_icon.paint(&painter, pixelsF.toRect());
    } else {
        painter.drawImage(pixelsF, thumnailImage);
    }
#else
    painter.fillRect(pixelsF, QColor(Qt::gray));
    painter.setPen(QPen(Qt::black, 0));
    painter.drawRect(pixelsF);

    m_icon.paint(&painter, pixelsF.toRect());
#endif
}

void VideoShape::saveOdf(KoShapeSavingContext &context) const
{
    // make sure we have a valid image data pointer before saving
    VideoData *videoData = qobject_cast<VideoData *>(userData());
    if (videoData == nullptr)
        return;

    KoXmlWriter &writer = context.xmlWriter();

    writer.startElement("draw:frame");
    saveOdfAttributes(context, OdfAllAttributes);
    writer.startElement("draw:plugin");
    // In the spec, only the xlink:href attribute is marked as mandatory, cool :)
    QString name = videoData->tagForSaving(m_videoCollection->saveCounter);
    writer.addAttribute("xlink:type", "simple");
    writer.addAttribute("xlink:show", "embed");
    writer.addAttribute("xlink:actuate", "onLoad");
    writer.addAttribute("xlink:href", name);
    writer.addAttribute("draw:mime-type", "application/vnd.sun.star.media");
    writer.endElement(); // draw:plugin
    saveOdfCommonChildElements(context);
    writer.endElement(); // draw:frame

    context.addDataCenter(m_videoCollection);
}

bool VideoShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    loadOdfAttributes(element, context, OdfAllAttributes);
    return loadOdfFrame(element, context);
}

bool VideoShape::loadOdfFrameElement(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    /* the loading of the attributes might set the event actions which removes the m_videoEventAction
     * when there are other eventactions for the shape. Therefore we need to add it again. It is no
     * problem to add it again as internally a set is used and so it is not problematic when it is
     * already set. */
    addEventAction(m_videoEventAction);

    if (m_videoCollection) {
        const QString href = element.attribute("href");
        // this can happen in case it is a presentation:placeholder
        if (!href.isEmpty()) {
            QUrl url = QUrl::fromUserInput(href);
            VideoData *data = nullptr;

            if (href.startsWith("../")) {
                // file is outside store
                QUrl url = context.odfLoadingContext().store()->urlOfStore();
                QString path = url.path();
                if (!path.endsWith(QLatin1Char('/'))) {
                    path.append(QLatin1Char('/'));
                }
                path.append(href.mid(3));
                url.setPath(path);
                data = m_videoCollection->createExternalVideoData(url, false);
            } else if (!url.isRelative()) {
                // file is outside store and absolute
                data = m_videoCollection->createExternalVideoData(QUrl::fromUserInput(href), false);
            } else {
                // file is inside store
                KoStore *store = context.odfLoadingContext().store();
                data = m_videoCollection->createVideoData(href, store);
            }
            setUserData(data);
        }
    }

    return true;
}

VideoCollection *VideoShape::videoCollection() const
{
    return m_videoCollection;
}

void VideoShape::setVideoCollection(VideoCollection *collection)
{
    m_videoCollection = collection;
}

VideoData *VideoShape::videoData() const
{
    return qobject_cast<VideoData *>(userData());
}
