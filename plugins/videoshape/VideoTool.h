/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VIDEOTOOL_H
#define VIDEOTOOL_H

#include <KoToolBase.h>

class VideoToolUI;
class VideoShape;
class KoCanvasBase;

class VideoTool : public KoToolBase
{
    Q_OBJECT

public:
    explicit VideoTool(KoCanvasBase *canvas);
    ~VideoTool();

    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void mousePressEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;
    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;

protected:
    QWidget *createOptionWidget() override;

private Q_SLOTS:
    void changeUrlPressed();
    void play();

private:
    VideoToolUI *m_videoToolUI;
    VideoShape *m_videoShape;
};

#endif // VIDEOTOOL_H
