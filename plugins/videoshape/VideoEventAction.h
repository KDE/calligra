/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef VIDEOEVENTACTION_H
#define VIDEOEVENTACTION_H

#include <KoEventAction.h>

class VideoShape;
class FullScreenPlayer;

/**
 * This class represents the click event action that starts the playing of the video.
 */
class VideoEventAction : public KoEventAction
{
public:
    explicit VideoEventAction(VideoShape *parent);

    /// destructor
    virtual ~VideoEventAction();

    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    void saveOdf(KoShapeSavingContext &context) const override;

    void start() override;
    void finish() override;

protected:
    VideoShape *m_shape;
    FullScreenPlayer *m_player;
};

#endif
