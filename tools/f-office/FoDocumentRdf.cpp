/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Sugnan Prabhu S <sugnan.prabhu@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "FoDocumentRdf.h"
#include <KoStoreDevice.h>
#include <KoTextInlineRdf.h>
#include <KoInlineTextObjectManager.h>
#include <Soprano/Statement>
#include <Soprano/Soprano>
#include <KoInlineObject.h>
#include <KoBookmark.h>
#include <KoTextMeta.h>
#include <KoTextEditor.h>
#include <KoShape.h>

#include <QTextCursor>
#include <KWDocument.h>
#include <KoTextRdfCore.h>
#include "kotext_export.h"
#include "RdfInfoDialog.h"
#include <QFile>
#include <QTextDocument>
#include <QImage>
#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif

using namespace Soprano;
using namespace KoTextRdfCore;

FoDocumentRdf::FoDocumentRdf(KoDocument *parent,KoTextEditor *m_editor)
    : KoDocumentRdfBase(parent),
    m_model(Soprano::createModel()),
    rdfInfoDialog(0)
{
    m_doc=(KWDocument *)parent;
    this->m_editor=m_editor;
    connect(m_doc,SIGNAL(cursorPositionChanged()),this,SLOT(cursorPositionChanged()));
    inRdfMode=false;
}

FoDocumentRdf::~FoDocumentRdf()
{
}

Soprano::Model *FoDocumentRdf::model() const
{
    return m_model;
}

bool FoDocumentRdf::loadRdf(KoStore *store, const Soprano::Parser *parser, const QString &fileName)
{
    bool ok = true;
    if (!store->open(fileName)) {
        return false;
    }
    Soprano::Node context(QUrl("http://www.koffice.org/Rdf/path/" + fileName));
    QUrl BaseURI = QUrl(QString());
    QString rdfxmlData(store->device()->readAll());
    Soprano::StatementIterator it = parser->parseString(rdfxmlData, BaseURI,
                                                        Soprano::SerializationRdfXml);
    QList<Statement> allStatements = it.allElements();
    Soprano::Model *tmpmodel(Soprano::createModel());
    foreach (Soprano::Statement s, allStatements) {
        Soprano::Node subj = s.subject();
        Soprano::Node pred = s.predicate();
        Soprano::Node obj  = s.object();
        tmpmodel->addStatement(subj, pred, obj, context);
    }

    freshenBNodes(tmpmodel);

    m_model->addStatements(tmpmodel->listStatements().allElements());
    delete tmpmodel;
    store->close();
    return ok;
}

bool FoDocumentRdf::loadOasis(KoStore *store)
{
    if (!store)
        return false;

    const Soprano::Parser *parser =
            Soprano::PluginManager::instance()->discoverParserForSerialization(
                    Soprano::SerializationRdfXml);
    bool ok = loadRdf(store, parser, "manifest.rdf");
    ok = ok & (loadRdf(store, parser, "data.rdf"));
    ok = ok & (loadRdf(store, parser, "geo1.rdf"));
    ok = ok & (loadRdf(store, parser, "openingHours.rdf"));

    if (!ok){
        return ok;
    }

    QList<Soprano::Statement> allStatements = m_model->listStatements().allElements();
    foreach (Soprano::Statement s, allStatements) {
        subject.append(s.subject().toString());
        predicate.append(s.predicate().toString());
        object.append(s.object().toString());
    }

    return ok;
}

void FoDocumentRdf::freshenBNodes(Soprano::Model *m)
{
    QList<Soprano::Statement> removeList;
    QList<Soprano::Statement> addList;
    QMap<QString, Soprano::Node> bnodeMap;
    StatementIterator it = m->listStatements();
    QList<Statement> allStatements = it.allElements();
    foreach (Soprano::Statement s, allStatements) {
        Soprano::Node subj = s.subject();
        Soprano::Node obj = s.object();
        Soprano::Statement news;
        if (subj.type() == Soprano::Node::BlankNode) {
            QString nodeStr = subj.toString();
            Soprano::Node n = bnodeMap[ nodeStr ];
            if (!n.isValid()) {
                n = this->model()->createBlankNode();
                bnodeMap[ nodeStr ] = n;
            }
            removeList << s;
            subj = n;
            news = Statement(subj, s.predicate(), obj, s.context());
        }
        if (obj.type() == Soprano::Node::BlankNode) {
            QString nodeStr = obj.toString();
            Soprano::Node n = bnodeMap[ nodeStr ];
            if (!n.isValid()) {
                n = this->model()->createBlankNode();
                bnodeMap[ nodeStr ] = n;
            }
            removeList << s;
            obj = n;
            news = Statement(subj, s.predicate(), obj, s.context());
        }
        if (news.isValid()) {
            addList << news;
        }
    }

    removeStatementsIfTheyExist(m, removeList);
    m->addStatements(addList);
}

void FoDocumentRdf::removeStatementsIfTheyExist(Soprano::Model *m, const QList<Soprano::Statement> &removeList)
{
    foreach (Soprano::Statement s, removeList) {
        StatementIterator it = m->listStatements(s.subject(), s.predicate(), s.object(), s.context());
        QList<Statement> allStatements = it.allElements();
        Q_FOREACH(Soprano::Statement z, allStatements) {
            m->removeStatement(z);
        }
    }
}

void FoDocumentRdf::highlightRdf()
{
    bool rdfPresent=false;
    if(inRdfMode==false){
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(0, i18n("RDF mode started"),
                                           QMaemo5InformationBox::DefaultTimeout);
#endif

        KoInlineTextObjectManager *textObjectManager= ((KWDocument *)m_doc)->inlineTextObjectManager();
        QList<KoInlineObject *>inlineObjects=textObjectManager->inlineTextObjects();
        for(int i=0;i<inlineObjects.length();i++){
            KoTextMeta *inob = dynamic_cast<KoTextMeta*>(inlineObjects.at(i));
            if(inob){
                m_editor->setPosition(inob->position(),QTextCursor::MoveAnchor);
                m_editor->select(QTextCursor::WordUnderCursor);
                m_editor->setTextColor(QColor("blue"));
                m_editor->setTextBackgroundColor(QColor("yellow"));
                rdfPresent=true;
            }
        }
        if(rdfPresent==true){
            inRdfMode=true;
            m_doc->setReadWrite(false);
        }
        else{
#ifdef Q_WS_MAEMO_5
            QMaemo5InformationBox::information(0, i18n("RDF data is absent"),
                                               QMaemo5InformationBox::DefaultTimeout);
#endif
        }
    }
    else{
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(0, i18n("RDF mode exited"),
                                           QMaemo5InformationBox::DefaultTimeout);
#endif

        KoInlineTextObjectManager *textObjectManager= ((KWDocument *)m_doc)->inlineTextObjectManager();
        QList<KoInlineObject *>inlineObjects=textObjectManager->inlineTextObjects();
        for(int i=0;i<inlineObjects.length();i++){
            KoTextMeta *inob = dynamic_cast<KoTextMeta*>(inlineObjects.at(i));
            if(inob){
                m_editor->setPosition(inob->position(),QTextCursor::MoveAnchor);
                m_editor->select(QTextCursor::WordUnderCursor);
                m_editor->setTextColor(QColor("black"));
                m_editor->setTextBackgroundColor(QColor("white"));
            }
        }
        inRdfMode=false;
        m_doc->setReadWrite(true);
    }
}

bool FoDocumentRdf::completeLoading(KoStore *)
{
    return true;
}

bool FoDocumentRdf::completeSaving(KoStore *, KoXmlWriter *, KoShapeSavingContext *)
{
    return true;
}

void FoDocumentRdf::findStatements(QTextCursor &cursor, int depth)
{
    if(inRdfMode==false)
        return;

    Soprano::Model *ret(Soprano::createModel());
    KoTextInlineRdf *inlineRdf(0);

    if (KoInlineTextObjectManager *textObjectManager
        = ((KWDocument *)m_doc)->inlineTextObjectManager()) {

        long searchStartPosition = cursor.position();
        int limit = 500;
        for (QTextCursor tc = cursor; !tc.atStart() && limit;
        tc.movePosition(QTextCursor::Left), --limit) {
            KoInlineObject *inlineObject = textObjectManager->inlineTextObject(tc);
            if (inlineObject) {
                if (KoBookmark *bm = dynamic_cast<KoBookmark*>(inlineObject)) {
                    if (bm->type() == KoBookmark::EndBookmark) {
                        continue;
                    }
                    if (bm->type() == KoBookmark::StartBookmark) {
                        KoBookmark *e = bm->endBookmark();
                        if (e && e->position() < searchStartPosition)
                            continue;
                    }
                }
                if (KoTextMeta *bm = dynamic_cast<KoTextMeta*>(inlineObject)) {
                    if (bm->type() == KoTextMeta::EndBookmark) {
                        continue;
                    }
                    if (bm->type() == KoTextMeta::StartBookmark) {
                        KoTextMeta *e = bm->endBookmark();

                        if (e && e->position() < searchStartPosition)
                            continue;
                    }
                }
                if (KoInlineObject *shape = dynamic_cast<KoInlineObject*>(inlineObject)) {
                    inlineRdf = shape->inlineRdf();
                    if (inlineRdf) {
                        break;
                    }
                }
            }
        }
    }

    if (inlineRdf) {
        ret->addStatement(toStatement(inlineRdf));
        QString xmlid = inlineRdf->xmlId();
        addStatements(ret, xmlid);
    }

    inlineRdf = KoTextInlineRdf::tryToGetInlineRdf(cursor);
    if (inlineRdf) {

        ret->addStatement(toStatement(inlineRdf));
        QString xmlid = inlineRdf->xmlId();
        addStatements(ret, xmlid);
    }

    for (int i = 1; i < depth; ++i) {
        expandStatements(ret);
    }
    return;
}


Soprano::Statement FoDocumentRdf::toStatement(KoTextInlineRdf *inlineRdf) const
{
    if (!inlineRdf) {
        return Soprano::Statement();
    }
    if (inlineRdf->predicate().isEmpty())  {
        return Soprano::Statement();
    }
    Soprano::Node subj = Soprano::Node::createResourceNode(QUrl(inlineRdf->subject()));
    Soprano::Node pred = Soprano::Node::createResourceNode(QUrl(inlineRdf->predicate()));
    Soprano::Node obj;
    switch (inlineRdf->sopranoObjectType()) {
    case Node::ResourceNode:
        obj = Soprano::Node::createResourceNode(inlineRdf->object());
        break;
    case Node::LiteralNode:
        obj = Soprano::Node::createLiteralNode(inlineRdf->object());
        break;
    case Node::BlankNode:
        obj = Soprano::Node::createBlankNode(inlineRdf->object());
        break;
    }
    if (!inlineRdf->subject().size()) {
        subj = inlineRdfContext();
    }

    Soprano::Statement ret(subj, pred, obj, inlineRdfContext());
    return ret;
}

void FoDocumentRdf::addStatements(Soprano::Model *model, const QString &xmlid)
{
    bool noDataFlag=true;
    int index=object.indexOf(xmlid);
    struct Details personDetails;

    if(index>0 && index<object.length()){

        int lastindex=subject.lastIndexOf(subject.at(index));

        for(int i=subject.indexOf(subject.at(index));i<subject.length() ;i=subject.indexOf(subject.at(index),i+1)){
            if(predicate.at(i).startsWith("http://xmlns.com/foaf/0.1/")){
                if(predicate.at(i).endsWith("name"))
                    personDetails.name=object.at(i);
                if(predicate.at(i).endsWith("nick"))
                    personDetails.nick=object.at(i);
                if(predicate.at(i).endsWith("phone"))
                    personDetails.phone=object.at(i);
                if(predicate.at(i).endsWith("homepage"))
                    personDetails.homePage=object.at(i);

                noDataFlag=false;
            }

            if(predicate.at(i).startsWith("http://www.w3.org/2002/12/cal/icaltzd#")){
                if(predicate.at(i).endsWith("summary"))
                    personDetails.summary=object.at(i);
                if(predicate.at(i).endsWith("dtstart"))
                    personDetails.taskStart=object.at(i);
                if(predicate.at(i).endsWith("dtend"))
                    personDetails.taskEnd=object.at(i);
                if(predicate.at(i).endsWith("location"))
                    personDetails.location=object.at(i);

                noDataFlag=false;
            }
            if(predicate.at(i).startsWith("http://www.w3.org/2003/01/geo/wgs84_pos#")){
                if(predicate.at(i).endsWith("homepage"))
                    personDetails.homePage=object.at(i);

                noDataFlag=false;
            }
            if(i==lastindex)
                break;
        }
        if(noDataFlag==false){
            if(rdfInfoDialog)
                delete rdfInfoDialog;
            rdfInfoDialog=new RdfInfoDialog();
            rdfInfoDialog->setData(personDetails);
            rdfInfoDialog->show();
        }
    }

}

void FoDocumentRdf::expandStatements(Soprano::Model *model)
{
    expandStatementsReferencingSubject(model);
    expandStatementsToIncludeOtherPredicates(model);
}

void FoDocumentRdf::expandStatementsReferencingSubject(Soprano::Model *model)
{
    QList<Statement> addList;
    QList<Statement> allStatements = model->listStatements().allElements();
    foreach (Soprano::Statement s, allStatements) {
        QList<Statement> all = m_model->listStatements(Node(), Node(), s.subject()).allElements();
        foreach (Soprano::Statement e, all) {
            addList << e;
        }
    }
    model->addStatements(addList);
}

void FoDocumentRdf::expandStatementsToIncludeOtherPredicates(Soprano::Model *model)
{
    QList<Statement> addList;
    QList<Statement> allStatements = model->listStatements().allElements();
    foreach (Soprano::Statement s, allStatements) {
        QList<Statement> all = m_model->listStatements(s.subject(), Node(), Node()).allElements();
        foreach (Soprano::Statement e, all) {
            addList << e;
        }
    }
    model->addStatements(addList);
}

Soprano::Node FoDocumentRdf::inlineRdfContext() const
{
    return Node(QUrl("http://www.koffice.org/Rdf/inline-rdf"));
}
