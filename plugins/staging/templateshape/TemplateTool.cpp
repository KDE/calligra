/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2007 Montel Laurent <montel@kde.org>
   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "TemplateTool.h"

// Qt
#include <QFileDialog>
#include <QGridLayout>
#include <QToolButton>
#include <QUrl>

// KF5
#include <KLocalizedString>

// Calligra
#include <KoCanvasBase.h>
#include <KoIcon.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

// This shape
#include "TemplateShape.h"
// #include "ChangeSomethingCommand.h"

TemplateTool::TemplateTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
    , m_templateshape(0)
{
}

void TemplateTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    Q_UNUSED(toolActivation);

    foreach (KoShape *shape, shapes) {
        m_templateshape = dynamic_cast<TemplateShape *>(shape);
        if (m_templateshape)
            break;
    }

    if (!m_templateshape) {
        Q_EMIT done();
        return;
    }

    useCursor(Qt::ArrowCursor);
}

void TemplateTool::deactivate()
{
    m_templateshape = 0;
}

QWidget *TemplateTool::createOptionWidget()
{
    // Template: This function should not be implemented if the tool
    // does not have an option widget.

    // Template: Example code: a single button that opens a file.
    // Another alternative is to have the option widget in a separate class.

    QWidget *optionWidget = new QWidget();
    QGridLayout *layout = new QGridLayout(optionWidget);

    QToolButton *button = 0;

    button = new QToolButton(optionWidget);
    button->setIcon(koIcon("document-open"));
    button->setToolTip(i18n("Open"));
    layout->addWidget(button, 0, 0);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(openPressed()));

    return optionWidget;
}

// ----------------------------------------------------------------
//                         Private slots

void TemplateTool::openPressed()
{
    if (m_templateshape == 0)
        return;

    QUrl url = QFileDialog::getOpenFileUrl();
    if (!url.isEmpty()) {
        // Template: do something with the file here.
    }
}

// ----------------------------------------------------------------
//                         Event handling

void TemplateTool::mousePressEvent(KoPointerEvent *event)
{
    if (event->button() == Qt::RightButton) {
        event->ignore();
    }
}

void TemplateTool::mouseDoubleClickEvent(KoPointerEvent *event)
{
    // Template: Example code: if the shape is double clicked, do the
    //           same as if the button in the option widget is pressed.

    if (canvas()->shapeManager()->shapeAt(event->point) != m_templateshape) {
        event->ignore(); // allow the event to be used by another
        return;
    }

    openPressed();
}
