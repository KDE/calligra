/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef VECTOR_TOOL
#define VECTOR_TOOL

#include <KoToolBase.h>
#include <kjob.h>

class VectorShape;

class VectorTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit VectorTool(KoCanvasBase *canvas);

    /// reimplemented from KoToolBase
    void paint(QPainter &, const KoViewConverter &) override
    {
    }
    /// reimplemented from KoToolBase
    void mousePressEvent(KoPointerEvent *) override
    {
    }
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
    void changeUrlPressed();
    void setImageData(KJob *job);

private:
    VectorShape *m_shape;
};

#endif
