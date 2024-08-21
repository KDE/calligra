/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "InsertTextReferenceAction_p.h"
#include "KoInlineTextObjectManager.h"
#include "KoTextLocator.h"
#include "KoTextReference.h"

#include <KoCanvasBase.h>

#include <KLocalizedString>
#include <KMessageBox>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include <kpagedialog.h>

InsertTextReferenceAction::InsertTextReferenceAction(KoCanvasBase *canvas, const KoInlineTextObjectManager *manager)
    : InsertInlineObjectActionBase(canvas, i18n("Text Reference"))
    , m_manager(manager)
{
}

KoInlineObject *InsertTextReferenceAction::createInlineObject()
{
    const QList<KoTextLocator *> textLocators = m_manager->textLocators();
    if (textLocators.isEmpty()) {
        KMessageBox::information(m_canvas->canvasWidget(), i18n("Please create an index to reference first."));
        return nullptr;
    }

    QWidget *widget = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(widget);
    widget->setLayout(lay);
    lay->setContentsMargins({});

    QLabel *label = new QLabel(i18n("Select the index you want to reference"), widget);
    lay->addWidget(label);
    QStringList selectionList;
    foreach (KoTextLocator *locator, textLocators)
        selectionList << locator->word() + '(' + QString::number(locator->pageNumber()) + ')';
    QListWidget *list = new QListWidget(widget);
    lay->addWidget(list);
    list->addItems(selectionList);

    KPageDialog dialog(m_canvas->canvasWidget());
    dialog.setWindowTitle(i18n("%1 Options", i18n("Text Reference"))); // reuse the text passed in the constructor
    dialog.addPage(widget, QString());

    KoVariable *variable = nullptr;
    if (dialog.exec() == KPageDialog::Accepted && list->currentRow() >= 0) {
        KoTextLocator *locator = textLocators[list->currentRow()];
        Q_ASSERT(locator);
        variable = new KoTextReference(locator->id());
    }
    return variable;
}
