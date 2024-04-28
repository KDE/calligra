/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Montel Laurent <montel@kde.org>
   SPDX-FileCopyrightText: 2011 Silvio Heinrich <plassy@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PICTURE_TOOL
#define PICTURE_TOOL

#include <KoToolBase.h>
#include <kjob.h>

class PictureShape;
struct PictureToolUI;

class PictureTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit PictureTool(KoCanvasBase *canvas);

    /// reimplemented from KoToolBase
    void paint(QPainter &, const KoViewConverter &) override
    {
    }
    /// reimplemented from KoToolBase
    void mousePressEvent(KoPointerEvent *) override;
    /// reimplemented from superclass
    void mouseDoubleClickEvent(KoPointerEvent *event) override;
    /// reimplemented from KoToolBase
    void mouseMoveEvent(KoPointerEvent *) override
    {
    }
    /// reimplemented from KoToolBase
    void mouseReleaseEvent(KoPointerEvent *) override
    {
    }

    /// reimplemented from KoToolBase
    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    /// reimplemented from KoToolBase
    void deactivate() override;

protected:
    /// reimplemented from KoToolBase
    QWidget *createOptionWidget() override;

private Q_SLOTS:
    void colorModeChanged(int cmbBoxIndex);
    void changeUrlPressed();
    void cropRegionChanged(const QRectF &rect, bool undoPrev);
    void cropEditFieldsChanged();
    void aspectCheckBoxChanged(bool checked);
    void contourCheckBoxChanged(bool checked);
    void fillButtonPressed();
    void updateControlElements();
    void setImageData(KJob *job);

private:
    PictureShape *m_pictureshape;
    PictureToolUI *m_pictureToolUI;
};

#endif
