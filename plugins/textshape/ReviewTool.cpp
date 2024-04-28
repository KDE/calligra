/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009-2010 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ReviewTool.h"
#include "AnnotationTextShape.h"

#include <KoToolBase.h>
#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoAnnotation.h>
#include <KoShapeController.h>
#include "KoShapeBasedDocumentBase.h"
#include <KoCanvasResourceManager.h>
#include <KoTextRangeManager.h>
#include <KoAnnotationManager.h>
#include <KoShapeUserData.h>
#include <KoTextShapeData.h>

#include <dialogs/SimpleSpellCheckingWidget.h>
#include <dialogs/SimpleAnnotationWidget.h>

#include <QDebug>
#include <KLocalizedString>
#include <QAction>
#include <KUser>

#include <QDate>


//#include "TextShape.h"
#define AnnotationShape_SHAPEID "AnnotationTextShapeID"

ReviewTool::ReviewTool(KoCanvasBase* canvas): TextTool(canvas),
    m_textEditor(0),
    m_textShapeData(0),
    m_canvas(canvas),
    m_textShape(0),
    m_currentAnnotationShape(0)
{
    createActions();
}

ReviewTool::~ReviewTool()
{
}

void ReviewTool::createActions()
{
    m_removeAnnotationAction = new QAction(i18n("Remove Comment"), this);
    m_removeAnnotationAction->setToolTip(i18n("Remove Comment"));
    addAction("remove_annotation", m_removeAnnotationAction);
    connect(m_removeAnnotationAction, &QAction::triggered, this, &ReviewTool::removeAnnotation);
}

void ReviewTool::mouseReleaseEvent(KoPointerEvent* event)
{
    TextTool::mouseReleaseEvent(event);
    event->accept();
}
void ReviewTool::activate(KoToolBase::ToolActivation toolActivation, const QSet< KoShape* >& shapes)
{
    TextTool::activate(toolActivation, shapes);
}
void ReviewTool::deactivate()
{
    TextTool::deactivate();
}
void ReviewTool::mouseMoveEvent(KoPointerEvent* event)
{
    TextTool::mouseMoveEvent(event);
}
void ReviewTool::mousePressEvent(KoPointerEvent *event)
{
    TextTool::mousePressEvent(event);
    m_currentAnnotationShape = dynamic_cast<AnnotationTextShape *>(textShape());
}
void ReviewTool::keyPressEvent(QKeyEvent* event)
{
    TextTool::keyPressEvent(event);
}
void ReviewTool::paint(QPainter& painter, const KoViewConverter& converter)
{
    TextTool::paint(painter,converter);
}

QList<QPointer<QWidget> > ReviewTool::createOptionWidgets()
{
    QList<QPointer<QWidget> > widgets;
    SimpleSpellCheckingWidget* sscw = new SimpleSpellCheckingWidget(this, 0);
    SimpleAnnotationWidget *saw = new SimpleAnnotationWidget(this, 0);

    connect(saw, &SimpleAnnotationWidget::doneWithFocus, this, &ReviewTool::returnFocusToCanvas);

    sscw->setWindowTitle(i18n("Spell check"));
    widgets.append(sscw);

    saw->setWindowTitle(i18n("Comments"));
    widgets.append(saw);

    return widgets;
}

void ReviewTool::removeAnnotation()
{
    if (m_currentAnnotationShape) {
        QList<KoShape *> shapes;
        shapes << m_currentAnnotationShape;
        canvas()->addCommand(canvas()->shapeController()->removeShapes(shapes));
        m_currentAnnotationShape = 0;
    }
}
