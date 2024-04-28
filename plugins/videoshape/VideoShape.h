/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VIDEOSHAPE_H
#define VIDEOSHAPE_H

#include <KoFrameShape.h>
#include <KoShape.h>

#include <QIcon>

#include <QPixmap>

#define VIDEOSHAPEID "VideoShape"

class VideoCollection;
class VideoEventAction;
class VideoData;
#ifdef SHOULD_BUILD_THUMBNAIL
class VideoThumbnailer;
#endif

class VideoShape : public KoShape, public KoFrameShape
{
public:
    VideoShape();
    virtual ~VideoShape();

    // reimplemented
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;
    // reimplemented
    void saveOdf(KoShapeSavingContext &context) const override;
    // reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    /**
     * Get the collection used in the shape.
     */
    VideoCollection *videoCollection() const;
    /**
     * Set the collection used in the shape.
     */
    void setVideoCollection(VideoCollection *collection);

    /**
     * return the video data used in this shape. Returns 0 if not set
     */
    VideoData *videoData() const;

protected:
    bool loadOdfFrameElement(const KoXmlElement &element, KoShapeLoadingContext &context) override;

private Q_SLOTS:
    void updateThumbnail();

Q_SIGNALS:
    void createThumbnail(VideoData *videoData, const QSize &size);

private:
    VideoCollection *m_videoCollection;
    VideoEventAction *m_videoEventAction;
#ifdef SHOULD_BUILD_THUMBNAIL
    VideoThumbnailer *m_thumbnailer;
#endif
    VideoData *m_oldVideoData;
    QIcon m_icon;
    QRectF m_playIconArea;
};

#endif
