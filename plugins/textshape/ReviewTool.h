/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef REVIEWTOOL_H
#define REVIEWTOOL_H

class KoCanvasBase;
class KoPointerEvent;
class KoTextEditor;
class KoTextShapeData;
class KoViewConverter;
class TextShape;

class QAction;

class QPainter;
class QKeyEvent;
/// This tool allows to manipulate the tracked changes of a document. You can accept or reject changes.

#include <TextTool.h>

class ReviewTool : public TextTool
{
    Q_OBJECT
public:
    explicit ReviewTool(KoCanvasBase *canvas);
    ~ReviewTool() override;

    void mouseReleaseEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mousePressEvent(KoPointerEvent *event) override;
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void keyPressEvent(QKeyEvent *event) override;
    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    void deactivate() override;
    void createActions() override;

    QList<QPointer<QWidget>> createOptionWidgets() override;

public Q_SLOTS:
    void removeAnnotation();

private:
    KoTextEditor *m_textEditor;
    KoTextShapeData *m_textShapeData;
    KoCanvasBase *m_canvas;
    TextShape *m_textShape;
    QAction *m_removeAnnotationAction;
    KoShape *m_currentAnnotationShape;
};

#endif // REVIEWTOOL_H
