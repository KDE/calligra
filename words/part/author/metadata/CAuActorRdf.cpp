/* This file is part of the KDE project
 *   Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "CAuActorRdf.h"

#include <author/metadata/CAuMetaDataManager.h>

#include <QUuid>
#include <KRichTextWidget>
#include <KActionCollection>
#include <KoIcon.h>

using namespace Soprano;

QString CAuActorRdf::QUERY;

/// Possible types of the actor
const QString CAuActorRdf::TYPE[] = {
    i18n("None"),
    i18n("Main"),
    i18n("Secondary")
};

/// Possible roles of the actor
const QString CAuActorRdf::ROLE[] = {
    i18n("None"),
    i18n("Protagonist"),
    i18n("Supporting"),
    i18n("Neutral"),
    i18n("Antagonist")
};

CAuActorRdf::CAuActorRdf(QObject *parent, const KoDocumentRdf *rdf)
    : CAuSemanticItemBase(parent, rdf)
{
    setupProps();
    QUERY = formQuery();
}

CAuActorRdf::CAuActorRdf(QObject *parent, const KoDocumentRdf *rdf, Soprano::QueryResultIterator &it)
    : CAuSemanticItemBase(parent, rdf)
{
    setupProps();
    QUERY = formQuery();
    init(it);
}

QString CAuActorRdf::name() const
{
    return i18n("Author's actor");
}

QWidget* CAuActorRdf::createEditor(QWidget *parent)
{
    QWidget *ret = new QWidget(parent);
    m_editWidgetUI.setupUi(ret);

    // Descr
    m_editWidgetUI.descrRichTextWidget->setText(stringProp("descr"));

    KActionCollection *actions = new KActionCollection(ret);

    actions->addActions(m_editWidgetUI.descrRichTextWidget->createActions());
    m_editWidgetUI.boldToolButton->setDefaultAction(actions->action("format_text_bold"));
    m_editWidgetUI.italicToolButton->setDefaultAction(actions->action("format_text_italic"));
    m_editWidgetUI.underlineToolButton->setDefaultAction(actions->action("format_text_underline"));

    m_editWidgetUI.leftAlignToolButton->setDefaultAction(actions->action("format_align_left"));
    m_editWidgetUI.centerAlignToolButton->setDefaultAction(actions->action("format_align_center"));
    m_editWidgetUI.rightAlignToolButton->setDefaultAction(actions->action("format_align_right"));

    actions->addAction("text_undo", m_editWidgetUI.descrRichTextWidget, SLOT(undo()));
    actions->addAction("text_redo", m_editWidgetUI.descrRichTextWidget, SLOT(redo()));
    actions->action("text_undo")->setIcon(koIcon("edit-undo"));
    actions->action("text_redo")->setIcon(koIcon("edit-redo"));
    m_editWidgetUI.undoToolButton->setDefaultAction(actions->action("text_undo"));
    m_editWidgetUI.redoToolButton->setDefaultAction(actions->action("text_redo"));

    // Aliases
    m_editWidgetUI.aliasesLineEdit->setText(stringProp("aliases"));

    // Short descr
    m_editWidgetUI.shortDescrLineEdit->setText(stringProp("shortdescr"));

    // Type
    for (int i = 0; i < TYPE_COUNT; i++) {
        m_editWidgetUI.typeComboBox->addItem(TYPE[i]);
    }
    m_editWidgetUI.typeComboBox->setCurrentIndex(intProp("type"));

    // Role
    for (int i = 0; i < ROLE_COUNT; i++) {
        m_editWidgetUI.roleComboBox->addItem(ROLE[i]);
    }
    m_editWidgetUI.typeComboBox->setCurrentIndex(intProp("role"));

    // Age
    m_editWidgetUI.ageSpinBox->setValue(intProp("age"));

    return ret;
}

void CAuActorRdf::updateFromEditorData()
{
    CAuSemanticItemBase::updateFromEditorData();

    setStringProp("descr", m_editWidgetUI.descrRichTextWidget->toHtml());
    setStringProp("aliases", m_editWidgetUI.aliasesLineEdit->text());
    setStringProp("shortdescr", m_editWidgetUI.descrRichTextWidget->toHtml());

    setIntProp("type", m_editWidgetUI.typeComboBox->currentIndex());
    setIntProp("role", m_editWidgetUI.roleComboBox->currentIndex());
    setIntProp("age" , m_editWidgetUI.ageSpinBox->value());

    finishUpdateFromEditorData();
}

QString CAuActorRdf::className() const
{
    return "Actor";
}

QList< QString > CAuActorRdf::intProps()
{
    return QList<QString>() << "type" << "role" << "age";
}

QList< QString > CAuActorRdf::stringProps()
{
    return QList<QString>() << "descr" << "aliases" << "shortdescr";
}
