/* This file is part of the KDE project
 * Copyright (C) 2009-2010 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ReviewTool.h"
#include <KoToolBase.h>
#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoShapeRegistry.h>
#include <KoAnnotation.h>
#include <KoShapeController.h>
#include "KoShapeBasedDocumentBase.h"
#include <KoCanvasResourceManager.h>
#include <KoTextRangeManager.h>
#include <KoAnnotationManager.h>

#include <dialogs/SimpleSpellCheckingWidget.h>
#include <dialogs/SimpleAnnotationWidget.h>

#include <kdebug.h>
#include <klocale.h>
#include <kaction.h>

//#include "TextShape.h"
#define AnnotationShape_SHAPEID "AnnotationTextShapeID"

ReviewTool::ReviewTool(KoCanvasBase* canvas): TextTool(canvas),
    m_textEditor(0),
    m_textShapeData(0),
    m_canvas(canvas),
    m_textShape(0)
{
    createActions();
}

ReviewTool::~ReviewTool()
{
}

void ReviewTool::createActions()
{
    m_insertAnnotationAction = new KAction(i18n("Insert Annotation"), this);
    m_insertAnnotationAction->setToolTip(i18n("Insert Annotation"));
    addAction("insert_annotation", m_insertAnnotationAction);

    m_removeAnnotationAction = new KAction(i18n("Remove Annotation"), this);
    m_removeAnnotationAction->setToolTip(i18n("Remove Annotation"));
    addAction("remove_annotation", m_removeAnnotationAction);
}

void ReviewTool::mouseReleaseEvent(KoPointerEvent* event)
{
    TextTool::mouseReleaseEvent(event);
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
void ReviewTool::mousePressEvent(KoPointerEvent* event)
{
    KoShape *annotationShape = canvas()->shapeManager()->shapeAt(event->point);
    if (annotationShape) {
        if (annotationShape->shapeId() == AnnotationShape_SHAPEID) {
            KoCanvasResourceManager *rm = canvas()->resourceManager();
            const KoAnnotationManager *annotationManager = textEditor()->textRangeManager()->annotationManager();
            m_currentAnnotationShape = annotationShape;
            foreach (QString name, annotationManager->annotationNameList()) {
                KoAnnotation *annotation = annotationManager->annotation(name);
                if(annotation->annotationShape() == m_currentAnnotationShape) {
                    if ((annotation->positionOnlyMode() == false) && annotation->hasRange()) {
                        rm->clearResource(KoText::SelectedTextPosition);
                        rm->clearResource(KoText::SelectedTextAnchor);
                    }
                    if (annotation->positionOnlyMode()) {
                        rm->setResource(KoText::CurrentTextPosition, annotation->rangeStart());
                        rm->setResource(KoText::CurrentTextAnchor, annotation->rangeStart());
                    } else {
                        rm->setResource(KoText::CurrentTextPosition, annotation->rangeStart());
                        rm->setResource(KoText::CurrentTextAnchor, annotation->rangeEnd());
                    }
                    break;
                }
            }
            return;
        }
    }
    TextTool::mousePressEvent(event);
}
void ReviewTool::keyPressEvent(QKeyEvent* event)
{
    TextTool::keyPressEvent(event);
}
void ReviewTool::paint(QPainter& painter, const KoViewConverter& converter)
{
    TextTool::paint(painter,converter);
}

QList<QWidget *> ReviewTool::createOptionWidgets()
{
    QList<QWidget *> widgets;
    SimpleSpellCheckingWidget* sscw = new SimpleSpellCheckingWidget(this, 0);
    SimpleAnnotationWidget *saw = new SimpleAnnotationWidget(this, 0);

    connect(saw, SIGNAL(doneWithFocus()), this, SLOT(returnFocusToCanvas()));

    sscw->setWindowTitle("SpellCheck");
    widgets.append(sscw);


    saw->setWindowTitle("Annotation");
    widgets.append(saw);

    return widgets;

}

void ReviewTool::insertAnnotation()
{
    KoShape *shape = KoShapeRegistry::instance()->value(AnnotationShape_SHAPEID)->createDefaultShape(m_canvas->shapeController()->resourceManager());
    m_canvas->shapeController()->documentBase()->addShape(shape);
    KoAnnotation *annotation = textEditor()->addAnnotation();
    annotation->setAnnotationShape(shape);
}

void ReviewTool::removeAnnotation()
{
    if (m_currentAnnotationShape) {
        m_canvas->shapeManager()->remove(m_currentAnnotationShape);
        m_currentAnnotationShape = 0;
    }
}
