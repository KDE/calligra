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

#include <KoDocumentRdf.h>
#include <KoTextRdfCore.h>

#include <QUuid>
#include <QTextEdit>
#include <KActionCollection>

using namespace Soprano;

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
    : KoRdfBasicSemanticItem(parent, rdf)
{
    m_magicId = QUuid::createUuid().toString();
}

CAuSectionRdf::CAuSectionRdf(QObject *parent, const KoDocumentRdf *rdf, Soprano::QueryResultIterator &it)
    : KoRdfBasicSemanticItem(parent, rdf, it)
{
    m_uri = it.binding("section").toString();
    m_status = it.binding("status").toString();
    m_descr = it.binding("descr").toString();
    m_magicId = it.binding("magicid").toString();
}

QString CAuSectionRdf::name() const
{
    return i18n("AuthorSection");
}

QWidget* CAuSectionRdf::createEditor(QWidget *parent)
{
    QWidget *ret = new QWidget(parent);
    m_editWidgetUI.setupUi(ret);
    if (m_descr.size()) {
        m_editWidgetUI.descrRichTextWidget->setText(m_descr);
    }

    for (int i = 0; i < STATUS_COUNT; i++) {
        m_editWidgetUI.statusComboBox->addItem(STATUS[i]);
    }
    m_editWidgetUI.statusComboBox->setCurrentIndex(m_status.toInt());

    KActionCollection *actions = new KActionCollection(ret);

    m_editWidgetUI.descrRichTextWidget->createActions(actions);
    m_editWidgetUI.boldToolButton->setDefaultAction(actions->action("format_text_bold"));
    m_editWidgetUI.italicToolButton->setDefaultAction(actions->action("format_text_italic"));
    m_editWidgetUI.underlineToolButton->setDefaultAction(actions->action("format_text_underline"));

    actions->addAction("text_undo", m_editWidgetUI.descrRichTextWidget, SLOT(undo()));
    actions->addAction("text_redo", m_editWidgetUI.descrRichTextWidget, SLOT(redo()));
    actions->action("text_undo")->setIcon(KIcon("edit-undo"));
    actions->action("text_redo")->setIcon(KIcon("edit-redo"));
    m_editWidgetUI.undoToolButton->setDefaultAction(actions->action("text_undo"));
    m_editWidgetUI.redoToolButton->setDefaultAction(actions->action("text_redo"));

    return ret;
}

void CAuSectionRdf::updateFromEditorData()
{
    if (m_uri.size() <= 0) {
        QUuid u = QUuid::createUuid();
        m_uri = u.toString();
    }
    QString predBase = "http://www.calligra.org/author/"; //FIXME: this thing should be global
    setRdfType(predBase + "Section");
    updateTriple(m_descr, m_editWidgetUI.descrRichTextWidget->toHtml(), predBase + "Section#descr");
    updateTriple(m_magicId, m_magicId, predBase + "Section#magicid");
    updateTriple(m_status, QString::number(m_editWidgetUI.statusComboBox->currentIndex()), predBase + "Section#status");
    if (documentRdf()) {
        const_cast<KoDocumentRdf*>(documentRdf())->emitSemanticObjectUpdated(hKoRdfBasicSemanticItem(this));
    }
}

Soprano::Node CAuSectionRdf::linkingSubject() const
{
    return Node::createResourceNode(m_uri);
}

QString CAuSectionRdf::className() const
{
    return "AuthorSection";
}

Node CAuSectionRdf::context() const
{
    if (m_context.isValid()) {
        return m_context;
    }

    return Node::createResourceNode(QUrl(documentRdf()->rdfPathContextPrefix() + "author.rdf")); //FIXME: this should be global
}
