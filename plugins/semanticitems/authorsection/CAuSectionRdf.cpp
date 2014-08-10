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

CAuSectionRdf::CAuSectionRdf(QObject *parent, const KoDocumentRdf *m_rdf)
    : KoRdfSemanticItem(m_rdf, parent)
{
    m_magicId = QUuid::createUuid().toString();
}

CAuSectionRdf::CAuSectionRdf(QObject *parent, const KoDocumentRdf *m_rdf, Soprano::QueryResultIterator &it)
    : KoRdfSemanticItem(m_rdf, it, parent)
{
    m_uri = it.binding("section").toString();
    m_descr = KoTextRdfCore::optionalBindingAsString(it, "descr");
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
    if (documentRdf()) {
        const_cast<KoDocumentRdf*>(documentRdf())->emitSemanticObjectUpdated(hKoRdfSemanticItem(this));
    }
}

Soprano::Node CAuSectionRdf::linkingSubject() const
{
    return Node::createResourceNode(m_uri);
}

void CAuSectionRdf::setupStylesheetReplacementMapping(QMap<QString, QString> &m)
{
}

QList<hKoSemanticStylesheet> CAuSectionRdf::stylesheets() const
{
    QList<hKoSemanticStylesheet> stylesheets;
    stylesheets.append(
        createSystemStylesheet("143c1aa3-d7bb-420b-8528-7f077264ff5f2",
                               "name", "REPORT A BUG PLEASE"));
    return stylesheets;
}

QString CAuSectionRdf::className() const
{
    return "AuthorSection";
}

void CAuSectionRdf::exportToFile(const QString &fileName) const
{
    Q_UNUSED(fileName);
    Q_ASSERT(false); // shouldn't be called
    return;
}

void CAuSectionRdf::importFromData(const QByteArray &ba, const KoDocumentRdf *rdf, KoCanvasBase *host)
{
    Q_UNUSED(ba);
    Q_UNUSED(rdf);
    Q_UNUSED(host);
    Q_ASSERT(false); // shouldn't be called
    return;
}

Node CAuSectionRdf::context() const
{
    if (m_context.isValid()) {
        return m_context;
    }

    return Node::createResourceNode(QUrl(documentRdf()->rdfPathContextPrefix() + "author.rdf")); //FIXME: this should be global
}
