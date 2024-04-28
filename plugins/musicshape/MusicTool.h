/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_TOOL
#define MUSIC_TOOL

#include <KoToolBase.h>
class MusicShape;
class KUndo2Command;

class MusicTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit MusicTool(KoCanvasBase *canvas);
    ~MusicTool() override;

    void paint(QPainter &painter, const KoViewConverter &converter) override;

    void mousePressEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;

    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    void deactivate() override;

    void addCommand(KUndo2Command *command);
    MusicShape *shape();

protected:
    /*
     * Create default option widget
     */
    QWidget *createOptionWidget() override;

protected Q_SLOTS:
Q_SIGNALS:
    void shapeChanged(MusicShape *shape);

private:
    MusicShape *m_musicshape;
};

#endif
