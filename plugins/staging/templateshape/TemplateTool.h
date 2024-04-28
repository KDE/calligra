/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2007 Montel Laurent <montel@kde.org>
   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEMPLATE_TOOL
#define TEMPLATE_TOOL

#include <KoToolBase.h>

class TemplateShape;

class TemplateTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit TemplateTool(KoCanvasBase *canvas);

    /// reimplemented from KoToolBase
    virtual void paint(QPainter &, const KoViewConverter &)
    {
    }
    /// reimplemented from KoToolBase
    virtual void mousePressEvent(KoPointerEvent *);
    /// reimplemented from superclass
    virtual void mouseDoubleClickEvent(KoPointerEvent *event);
    /// reimplemented from KoToolBase
    virtual void mouseMoveEvent(KoPointerEvent *)
    {
    }
    /// reimplemented from KoToolBase
    virtual void mouseReleaseEvent(KoPointerEvent *)
    {
    }

    /// reimplemented from KoToolBase
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes);
    /// reimplemented from KoToolBase
    virtual void deactivate();

protected:
    /// reimplemented from KoToolBase
    virtual QWidget *createOptionWidget();

private Q_SLOTS:
    void openPressed(); // Template: example code

private:
    // Pointer to the shape that we manipulate.
    TemplateShape *m_templateshape;
};

#endif
