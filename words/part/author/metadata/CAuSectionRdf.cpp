/* This file is part of the KDE project
   Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>

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
 * Boston, MA 02110-1301, USA.
*/

#include "CAuSectionRdf.h"

#include <author/metadata/CAuMetaDataManager.h>

#include <QTextEdit>
#include <KActionCollection>
#include <kicon.h>

using namespace Soprano;

QString CAuSectionRdf::QUERY;

/// Possible statuses of the review process for the section
const QString CAuSectionRdf::STATUS[CAuSectionRdf::STATUS_COUNT] = {
    i18n("1st Draft"),
    i18n("2nd Draft"),
    i18n("3rd Draft"),
    i18n("1st Edit"),
    i18n("2nd Edit"),
    i18n("3rd Edit"),
    i18n("Proofread"),
    i18n("Finished")
};

CAuSectionRdf::CAuSectionRdf(QObject *parent, const KoDocumentRdf *rdf)
    : CAuSemanticItemBase(parent, rdf)
{
    setupProps();
    QUERY = formQuery();
}

CAuSectionRdf::CAuSectionRdf(QObject *parent, const KoDocumentRdf *rdf, Soprano::QueryResultIterator &it)
    : CAuSemanticItemBase(parent, rdf)
{
    setupProps();
    QUERY = formQuery();
    init(it);
}

QString CAuSectionRdf::name() const
{
    return i18n("Author's section");
}

QWidget* CAuSectionRdf::createEditor(QWidget *parent)
{
    QWidget *ret = new QWidget(parent);
    m_editWidgetUI.setupUi(ret);

    // Synopsis
    m_editWidgetUI.synopRichTextWidget->setText(stringProp("synop"));

    KActionCollection *actions = new KActionCollection(ret);

    actions->addActions(m_editWidgetUI.synopRichTextWidget->createActions());
    m_editWidgetUI.boldToolButton->setDefaultAction(actions->action("format_text_bold"));
    m_editWidgetUI.italicToolButton->setDefaultAction(actions->action("format_text_italic"));
    m_editWidgetUI.underlineToolButton->setDefaultAction(actions->action("format_text_underline"));

    m_editWidgetUI.leftAlignToolButton->setDefaultAction(actions->action("format_align_left"));
    m_editWidgetUI.centerAlignToolButton->setDefaultAction(actions->action("format_align_center"));
    m_editWidgetUI.rightAlignToolButton->setDefaultAction(actions->action("format_align_right"));

    actions->addAction("text_undo", m_editWidgetUI.synopRichTextWidget, SLOT(undo()));
    actions->addAction("text_redo", m_editWidgetUI.synopRichTextWidget, SLOT(redo()));
    actions->action("text_undo")->setIcon(KIcon("edit-undo"));
    actions->action("text_redo")->setIcon(KIcon("edit-redo"));
    m_editWidgetUI.undoToolButton->setDefaultAction(actions->action("text_undo"));
    m_editWidgetUI.redoToolButton->setDefaultAction(actions->action("text_redo"));

    // Badge
    m_editWidgetUI.badgeLineEdit->setText(stringProp("badge"));

    // Status
    for (int i = 0; i < STATUS_COUNT; i++) {
        m_editWidgetUI.statusComboBox->addItem(STATUS[i]);
    }
    m_editWidgetUI.statusComboBox->setCurrentIndex(intProp("status"));

    return ret;
}

void CAuSectionRdf::updateFromEditorData()
{
    CAuSemanticItemBase::updateFromEditorData();

    setStringProp("synop", m_editWidgetUI.synopRichTextWidget->toHtml());
    setStringProp("badge", m_editWidgetUI.badgeLineEdit->text());
    setIntProp("status", m_editWidgetUI.statusComboBox->currentIndex());

    finishUpdateFromEditorData();
}

QString CAuSectionRdf::className() const
{
    return "Section";
}

QList< QString > CAuSectionRdf::intProps()
{
    return QList<QString>() << "status";
}

QList< QString > CAuSectionRdf::stringProps()
{
    return QList<QString>() << "synop" << "badge";
}
