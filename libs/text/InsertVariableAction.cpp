/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "InsertVariableAction_p.h"
#include "KoInlineObjectFactoryBase.h"
#include "KoText.h"
#include "KoVariable.h"

#include <KoCanvasBase.h>
#include <KoInlineTextObjectManager.h>
#include <KoShapeController.h>

#include <kpagedialog.h>

#include <KLocalizedString>
#include <QLayout>

InsertVariableAction::InsertVariableAction(KoCanvasBase *base, KoInlineObjectFactoryBase *factory, const KoInlineObjectTemplate &templ)
    : InsertInlineObjectActionBase(base, templ.name)
    , m_factory(factory)
    , m_templateId(templ.id)
    , m_properties(templ.properties)
    , m_templateName(templ.name)
{
}

KoInlineObject *InsertVariableAction::createInlineObject()
{
    KoInlineObject *io = m_factory->createInlineObject(m_properties);
    KoVariable *variable = dynamic_cast<KoVariable *>(io);
    Q_ASSERT(variable);
    KoInlineTextObjectManager *objManager =
        m_canvas->shapeController()->resourceManager()->resource(KoText::InlineTextObjectManager).value<KoInlineTextObjectManager *>();
    Q_ASSERT(objManager);
    variable->setManager(objManager);
    QWidget *widget = variable->createOptionsWidget();
    if (widget) {
        if (widget->layout()) {
            widget->layout()->setContentsMargins({});
        }
        KPageDialog *dialog = new KPageDialog(m_canvas->canvasWidget());
        dialog->setWindowTitle(i18n("%1 Options", m_templateName));
        dialog->addPage(widget, QString());
        if (dialog->exec() != KPageDialog::Accepted) {
            delete variable;
            variable = nullptr;
        }
        delete dialog;
    }
    return variable;
}
