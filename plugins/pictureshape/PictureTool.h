/* This file is part of the KDE project
   Copyright 2007 Montel Laurent <montel@kde.org>
   Copyright 2011 Silvio Heinrich <plassy@web.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
    explicit PictureTool(KoCanvasBase* canvas);

    /// reimplemented from KoToolBase
    void paint(QPainter&, const KoViewConverter&) override {}
    /// reimplemented from KoToolBase
    void mousePressEvent(KoPointerEvent*) override;
    /// reimplemented from superclass
    void mouseDoubleClickEvent(KoPointerEvent *event) override;
    /// reimplemented from KoToolBase
    void mouseMoveEvent(KoPointerEvent*) override {}
    /// reimplemented from KoToolBase
    void mouseReleaseEvent(KoPointerEvent*) override {}

    /// reimplemented from KoToolBase
    void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes) override;
    /// reimplemented from KoToolBase
    void deactivate() override;

protected:
    /// reimplemented from KoToolBase
    QWidget *createOptionWidget() override;

private Q_SLOTS:
    void colorModeChanged(int cmbBoxIndex);
    void changeUrlPressed();
    void cropRegionChanged(const QRectF& rect, bool undoPrev);
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
