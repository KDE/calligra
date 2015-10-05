/* This file is part of the KDE project
 * Copyright (C) 2010 KO GmbH <ben.martin@kogmbh.com>
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

/**
 * A few notes. As exportToMime() is based on exportToFile() in many
 * cases, we are testing the core of drag part of D&D as well by
 * testing the export.
 *
 *
 * FIXME: TODO:
 * These need KoDocument
 * findXmlId()
 * insertReflow(), applyReflow()
 * toStatement(), findExtent(),
 * copy and paste of a region of text containing/overlapping RDF
 *
 */
#include "TestRdf.h"

#include <QDebug>
#include <QtGui>
#include <QTemporaryFile>

#include <KWDocument.h>
#include "KWAboutData.h"
#include <frames/KWTextFrameSet.h>

#include <rdf/KoDocumentRdf.h>
#include <rdf/KoRdfLocation.h>
#include <rdf/KoRdfFoaF.h>
#include <rdf/KoRdfPrefixMapping.h>
#include <rdf/KoSopranoTableModel.h>

#include <KoPart.h>
#include <KoBookmark.h>
#include <KoTextInlineRdf.h>
#include <KoStore.h>
#include <KoTextDocument.h>
#include <KoApplication.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoStyleManager.h>
#include <KoXmlNS.h>
#include <KoTextRdfCore.h>
#include <KoDocument.h>
#include <KoInlineTextObjectManager.h>
#include <KoTextRangeManager.h>

#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kurl.h>

#include "MockPart.h"

using namespace Soprano;
#define RDEBUG if (0) qDebug()

#include <iostream>
using namespace std;



static KoDocumentRdf *loadDocument(const QString &odt)
{
    if (!QFile(odt).exists()) {
        qFatal("%s does not exist", qPrintable(odt));
        return 0;
    }

    KoStore *store = KoStore::createStore(odt, KoStore::Read, "", KoStore::Zip);
    KoOdfReadStore odfReadStore(store);
    KoXmlDocument metaDoc;
    KoDocumentRdf *rdf = new KoDocumentRdf;
    QString error;
    if (!odfReadStore.loadAndParse(error)) {
        delete store;
        return 0;
    }
    if (!rdf->loadOasis(store)) {
        delete store;
        return 0;
    }
    delete store;
    return rdf;
}

void TestRdf::basicload()
{
    RDEBUG;

    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    QSharedPointer<Soprano::Model> m = rdf->model();
    QVERIFY(m);
    QCOMPARE (234, m->statementCount());

    StatementIterator it;
    QList<Statement> allStatements;

    // check a triple
    it = m->listStatements(
                Node::createResourceNode(QUrl("uri:james")),
                Node::createResourceNode(QUrl("http://xmlns.com/foaf/0.1/name")),
                Node());
    allStatements = it.allElements();
    QCOMPARE (allStatements.size(), 1);
    foreach (Soprano::Statement s, allStatements) {
        // RDEBUG << "HAVE:" << s;
        QVERIFY (s.object().toString() == "James Smith");
    }

    // check that context was tracked
    // object and context value are checked for a single found triple
    // using an ical lookup
    it = m->listStatements(
                Node::createResourceNode(QUrl("http://www.w3.org/2002/12/cal/test/geo1#CDC474D4-1393-11D7-9A2C-000393914268")),
                Node::createResourceNode(QUrl("http://www.w3.org/2002/12/cal/icaltzd#uid")),
                Node());
    allStatements = it.allElements();
    QCOMPARE (allStatements.size(), 1);
    foreach (Soprano::Statement s, allStatements) {
        // RDEBUG << "HAVE:" << s;
        QVERIFY (s.object().toString() == "CDC474D4-1393-11D7-9A2C-000393914268");
        QVERIFY (s.context().toString() == rdf->RDF_PATH_CONTEXT_PREFIX + "geo1.rdf");
    }

}

void TestRdf::findStatements()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    QSharedPointer<Soprano::Model> m = rdf->model();
    QVERIFY(m);

    StatementIterator it;
    QList<Statement> allStatements;

    QSharedPointer<Soprano::Model> submodel = rdf->findStatements("james2");
    QVERIFY (submodel);
    QCOMPARE (9, submodel->statementCount());
    it = submodel->listStatements(
                Node::createResourceNode(QUrl("uri:james")),
                Node::createResourceNode(QUrl("http://xmlns.com/foaf/0.1/phone")),
                Node());
    allStatements = it.allElements();
    QCOMPARE (allStatements.size(), 1);
    foreach (Soprano::Statement s, allStatements) {
        QVERIFY (s.object().toString() == "tel:44 2324543");
    }
}

#ifdef KDEPIMLIBS_FOUND

void TestRdf::foaf()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    QSharedPointer<Soprano::Model> m = rdf->model();
    QVERIFY(m);

    QString nameThatRemains ("This one will remain, as it's not in the RDF");
    QSet<QString> expectedNames;
    expectedNames << "Dan Brickley";
    expectedNames << "Gollum";
    expectedNames << "O'Brien";
    expectedNames << nameThatRemains;

    const QList<hKoRdfSemanticItem> semanticItems = rdf->semanticItems("Contact");
    QCOMPARE (semanticItems.size(),10);
    foreach (hKoRdfSemanticItem semanticItem, semanticItems) {
        KoRdfFoaF* f = static_cast<KoRdfFoaF*>(semanticItem);
        expectedNames.remove (f->name());
        if (f->name()=="Dan Brickley") {
            QTemporaryFile file;
            if (file.open()) {
                f->exportToFile(file.fileName());
                QByteArray ba = KoTextRdfCore::fileToByteArray(file.fileName());
                QVERIFY (ba.contains ("BEGIN:VCARD"));
                QVERIFY (ba.contains ("END:VCARD"));
                QVERIFY (ba.contains ("NAME:Dan Brickley"));
                QVERIFY (ba.contains ("tel:123456789"));
            }

            QList<hKoSemanticStylesheet> systemStylesheets = f->stylesheets();
            QVERIFY (systemStylesheets.size()>=4);
            bool foundTextSystemStylesheet = false;
            foreach (hKoSemanticStylesheet ss, systemStylesheets)  {
                if (ss->uuid() == "0dd5878d-95c5-47e5-a777-63ec36da3b9a") {
                    foundTextSystemStylesheet = true;
                    QVERIFY (ss->name()=="name, phone");
                    QVERIFY (ss->templateString()=="%NAME%, %PHONE%");
                    QVERIFY (ss->isMutable()==false);
                    QVERIFY (ss->type()==KoSemanticStylesheet::stylesheetTypeSystem());
                }
            }
            QVERIFY (foundTextSystemStylesheet);

            // stylesheet template mappings
            {
                QMap<QString, QString> m;
                f->setupStylesheetReplacementMapping (m);
                QVERIFY (m["%PHONE%"] == "tel:123456789");
            }
        }
    }
    QCOMPARE (expectedNames.size(),1);
    QVERIFY (expectedNames.contains (nameThatRemains));

}

void TestRdf::calendarEvents()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    QSharedPointer<Soprano::Model> m = rdf->model();
    QVERIFY(m);

    QString nameThatRemains ("This one will remain, as it's not in the RDF");
    QSet<QString> expectedNames;
    expectedNames << "Lets party like its 1999";
    expectedNames << "Add Test";
    expectedNames << nameThatRemains;

    const QList<hKoRdfSemanticItem> semanticItems = rdf->semanticItems("Event");
    QCOMPARE (semanticItems.size(),4);
    foreach (hKoRdfSemanticItem semanticItem, semanticItems) {
        KoRdfCalendarEvent* f = static_cast<KoRdfCalendarEvent*>(semanticItem);
        expectedNames.remove (f->name());

        if (f->name()=="Lets party like its 1999") {
            QTemporaryFile file;
            if (file.open()) {
                f->exportToFile(file.fileName());
                QByteArray ba = KoTextRdfCore::fileToByteArray(file.fileName());
                //                RDEBUG << ba;
                QVERIFY (ba.contains ("END:VEVENT"));
                QVERIFY (ba.contains ("UID:dec09#8c3349c4-b240-4d50-997e-bd0d28044010"));
                QVERIFY (ba.contains ("DTSTART:20091216T130000Z"));
                QVERIFY (ba.contains ("DTEND:20091216T160000Z"));
                QVERIFY (ba.contains ("SUMMARY:Lets party like its 1999"));
            }

            QList<hKoSemanticStylesheet> systemStylesheets = f->stylesheets();
            QVERIFY (systemStylesheets.size()>=4);
            bool foundTextSystemStylesheet = false;
            foreach (hKoSemanticStylesheet ss, systemStylesheets)  {
                if (ss->uuid() == "853242eb-031c-4a36-abb2-7ef1881c777e") {
                    foundTextSystemStylesheet = true;
                    QVERIFY (ss->name()=="summary, location");
                    QVERIFY (ss->templateString()=="%SUMMARY%, %LOCATION%");
                    QVERIFY (ss->isMutable()==false);
                    QVERIFY (ss->type()==KoSemanticStylesheet::stylesheetTypeSystem());
                }
            }
            QVERIFY (foundTextSystemStylesheet);

            // stylesheet template mappings
            {
                QMap<QString, QString> m;
                f->setupStylesheetReplacementMapping (m);
                QVERIFY (m["%SUMMARY%"] == "Lets party like its 1999");
                QVERIFY (m["%UID%"] == "dec09#8c3349c4-b240-4d50-997e-bd0d28044010");
            }
        }
    }
    QCOMPARE (expectedNames.size(),1);
    QVERIFY (expectedNames.contains (nameThatRemains));
}

#endif // KDEPIMLIBS_FOUND

void TestRdf::locations()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    QSharedPointer<Soprano::Model> m = rdf->model();
    QVERIFY(m);

    QString nameThatRemains ("This one will remain, as it's not in the RDF");
    QSet<QString> expectedNames;
    expectedNames << "-79.9458,40.4427";
    expectedNames << nameThatRemains;

    const QList<hKoRdfSemanticItem> semanticItems = rdf->semanticItems("Location");
    QCOMPARE (semanticItems.size(),2);
    foreach (hKoRdfSemanticItem semanticItem, semanticItems) {
        hKoRdfLocation* f = static_cast<hKoRdfLocation*>(semanticItem);
        expectedNames.remove (f->name());

        if (f->name()=="-79.9458,40.4427") {
            QTemporaryFile file;
            if (file.open()) {
                f->exportToFile(file.fileName());
                QByteArray ba = KoTextRdfCore::fileToByteArray(file.fileName());
                //                RDEBUG << ba;
                QVERIFY (ba.contains ("<name>-79.9458,40.4427</name>"));
                QVERIFY (ba.contains ("<longitude>-79.9458</longitude>"));
                QVERIFY (ba.contains ("<latitude>40.4427</latitude>"));
            }

            QList<hKoSemanticStylesheet> systemStylesheets = f->stylesheets();
            QVERIFY (systemStylesheets.size()>=2);
            bool foundTextSystemStylesheet = false;
            foreach (hKoSemanticStylesheet ss, systemStylesheets)  {
                if (ss->uuid() == "34584133-52b0-449f-8b7b-7f1ef5097b9a") {
                    foundTextSystemStylesheet = true;
                    QVERIFY (ss->name()=="name, digital latitude, digital longitude");
                    QVERIFY (ss->templateString()=="%NAME%, %DLAT%, %DLONG%");
                }
            }
            QVERIFY (foundTextSystemStylesheet);

            // stylesheet template mappings
            {
                QMap<QString, QString> m;
                f->setupStylesheetReplacementMapping (m);
                //                RDEBUG << m;
                QVERIFY (m["%DLAT%"] == "40.4427");
                QVERIFY (m["%DLONG%"] == "-79.9458");
                QVERIFY (m["%ISGEO84%"] == "0");
            }
        }
    }
    QCOMPARE (expectedNames.size(),1);
    QVERIFY (expectedNames.contains (nameThatRemains));
}

void TestRdf::prefixMapping()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);

    KoRdfPrefixMapping* pm = rdf->prefixMapping();
    QVERIFY(pm);

    QVERIFY(pm->URItoPrefexedLocalname ("http://www.w3.org/2001/XMLSchema#bar")=="xsd:#bar");
    QVERIFY(pm->PrefexedLocalnameToURI ("xsd:#bar") == "http://www.w3.org/2001/XMLSchema#bar");
    QVERIFY(pm->prefexToURI ("xsd")=="http://www.w3.org/2001/XMLSchema");
}

static QSharedPointer<Soprano::Model> loadRDFXMLFromODT(const QString &odt)
{
    Soprano::Node context;
    QUrl BaseURI = QUrl(QString());
    const Soprano::Parser *parser =
            Soprano::PluginManager::instance()->discoverParserForSerialization(
                Soprano::SerializationRdfXml);

    KIO::StoredTransferJob *job = KIO::storedGet(KUrl(odt), KIO::NoReload, KIO::HideProgressInfo);
    const bool success = job->exec();
    if (!success) {
        qWarning() << "KIO failed; " << job->errorString();
        return QSharedPointer<Soprano::Model>(0);
    }
    QByteArray ba = job->data();
    RDEBUG << "rdfxml.sz:" << ba.size();
    QSharedPointer<Soprano::Model> ret(Soprano::createModel());
    QString rdfxmlData = ba;

    Soprano::StatementIterator it = parser->parseString(rdfxmlData, BaseURI,
                                                        Soprano::SerializationRdfXml);
    QList<Statement> allStatements = it.allElements();
    foreach (Soprano::Statement s, allStatements) {
        ret->addStatement(s.subject(),s.predicate(),s.object(),context);
    }
    RDEBUG << "ret.sz:" << ret->statementCount();
    return ret;
}

void TestRdf::addAndSave()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    QSharedPointer<Soprano::Model> m = rdf->model();
    QVERIFY(m);
    QCOMPARE (234, m->statementCount());

    Node explicitContext = Node(QUrl(rdf->RDF_PATH_CONTEXT_PREFIX + "explicit.rdf"));
    m->addStatement(Node::createResourceNode(QUrl("uri:test1")),
                    Node::createResourceNode(QUrl("uri:test2")),
                    Node::createResourceNode(QUrl("uri:test3")),
                    rdf->manifestRdfNode());
    m->addStatement(Node::createResourceNode(QUrl("uri:test4")),
                    Node::createResourceNode(QUrl("uri:test5")),
                    Node::createResourceNode(QUrl("uri:test6")),
                    explicitContext);
    QCOMPARE (236, m->statementCount());

    const char mimeType[] = "application/zip";
    QTemporaryFile file;
    QVERIFY(file.open());
    QString todt = file.fileName();
    KoStore *store = KoStore::createStore(todt, KoStore::Write, mimeType, KoStore::Zip);
    KoOdfWriteStore odfStore (store);
    KoXmlWriter *manifestWriter = odfStore.manifestWriter(mimeType);
    QVERIFY (manifestWriter);
    QVERIFY (rdf->saveOasis(store, manifestWriter));
    odfStore.closeManifestWriter();
    delete store;

    QByteArray ba = KoTextRdfCore::fileToByteArray(todt);
    RDEBUG << "ba.sz:" << ba.size();

    Soprano::StatementIterator it;
    QList<Statement> allStatements;

    m = loadRDFXMLFromODT("zip:" + todt + "/manifest.rdf");
    QVERIFY(m);
    it = m->listStatements(Node::createResourceNode(QUrl("uri:test1")),
                           Node::createResourceNode(QUrl("uri:test2")), Node());
    allStatements = it.allElements();
    QCOMPARE (allStatements.size(), 1);
    foreach (Soprano::Statement s, allStatements) {
        RDEBUG << "HAVE:" << s;
        QVERIFY (s.object().toString() == "uri:test3");
    }

    m = loadRDFXMLFromODT("zip:" + todt + "/explicit.rdf");
    QCOMPARE (1, m->statementCount());
    it = m->listStatements(
                Node::createResourceNode(QUrl("uri:test4")),
                Node::createResourceNode(QUrl("uri:test5")),
                Node());
    allStatements = it.allElements();
    QCOMPARE (allStatements.size(), 1);
    foreach (Soprano::Statement s, allStatements) {
        RDEBUG << "HAVE:" << s;
        QVERIFY (s.object().toString() == "uri:test6");
    }
}

void TestRdf::semanticItemViewSite()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    QSharedPointer<Soprano::Model> m = rdf->model();
    QVERIFY(m);
    QCOMPARE (234, m->statementCount());

    const QList<hKoRdfSemanticItem> semanticItems = rdf->semanticItems("Contact");
    foreach (hKoRdfSemanticItem semanticItem, semanticItems) {
        hKoRdfFoaF f = static_cast<hKoRdfFoaF>(semanticItem);
        if (f->name() == "James Smith") {

            hKoSemanticStylesheet ss = f->findStylesheetByUuid("0dd5878d-95c5-47e5-a777-63ec36da3b9a");
            KoRdfSemanticItemViewSite vs(f, "james2");
            vs.setStylesheetWithoutReflow(ss);
            RDEBUG << "ss.uuid:" << ss->uuid();
            RDEBUG << "vs.uuid:" << vs.stylesheet()->uuid();
            QVERIFY (ss->uuid() == vs.stylesheet()->uuid());
        }
    }

    // FIXME: to save the document and verify that the
    // stylesheet sticks, we need to have the KoDocument too.
}

void TestRdf::sopranoTableModel()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    QSharedPointer<Soprano::Model> m = rdf->model();
    QVERIFY(m);

    // FIXME: hmm, tablemodel would need to be exported for this
    // KoSopranoTableModel* tm = new KoSopranoTableModel(rdf);
    // QVERIFY (tm->model());
    // QCOMPARE (234, tm->model()->statementCount());

}

void TestRdf::expandStatementsToIncludeRdfLists()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    QSharedPointer<Soprano::Model> m = rdf->model();
    QVERIFY(m);
    QSharedPointer<Soprano::Model> localModel(Soprano::createModel());

    Node uuid = Node::createResourceNode(QUrl("http://www.w3.org/2002/12/cal/test/geo1#CDC474D4-1393-11D7-9A2C-000393914268"));
    Node icaltz = Node::createResourceNode(QUrl("http://www.w3.org/2002/12/cal/icaltzd#geo"));

    StatementIterator it;
    QList<Statement> allStatements;
    it = m->listStatements(uuid, icaltz, Node());
    allStatements = it.allElements();
    QCOMPARE (allStatements.size(), 1);
    foreach (Soprano::Statement s, allStatements) {
        RDEBUG << "HAVE:" << s;
        localModel->addStatement (s);
    }
    QCOMPARE (localModel->statementCount(),1);
    rdf->expandStatementsSubjectPointsTo(localModel);
    rdf->expandStatementsToIncludeRdfLists(localModel);
    QCOMPARE (localModel->statementCount(),5);

    Node rdfFirst = Node::createResourceNode(QUrl("http://www.w3.org/1999/02/22-rdf-syntax-ns#first"));
    Node rdfRest  = Node::createResourceNode(QUrl("http://www.w3.org/1999/02/22-rdf-syntax-ns#rest"));
    it = localModel->listStatements(Node(), rdfFirst, Node());
    allStatements = it.allElements();
    QVERIFY (allStatements.size() >= 2);
    foreach (Soprano::Statement s, allStatements) {
        //        RDEBUG << "HAVE:" << s;
        double v = s.object().toString().toDouble();
        QVERIFY (v == 40.442673 || v == -79.945815);
    }
}

void TestRdf::expandStatementsToIncludeOtherPredicates()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    QSharedPointer<Soprano::Model> m = rdf->model();
    QVERIFY(m);
    QSharedPointer<Soprano::Model> localModel(Soprano::createModel());

    StatementIterator it;
    QList<Statement> allStatements;
    it = m->listStatements(Node::createResourceNode(QUrl("uri:1984-winston")),
                           Node::createResourceNode(QUrl("http://xmlns.com/foaf/0.1/name")),
                           Node());
    allStatements = it.allElements();
    QCOMPARE (allStatements.size(), 1);
    foreach (Soprano::Statement s, allStatements) {
        RDEBUG << "HAVE:" << s;
        localModel->addStatement (s);
    }
    QCOMPARE (localModel->statementCount(),1);

    rdf->expandStatementsToIncludeOtherPredicates(localModel);
    QCOMPARE (localModel->statementCount(),6);

    it = m->listStatements(Node::createResourceNode(QUrl("uri:1984-winston")),
                           Node::createResourceNode(QUrl("http://xmlns.com/foaf/0.1/nick")),
                           Node());
    allStatements = it.allElements();
    QCOMPARE (allStatements.size(), 1);
    foreach (Soprano::Statement s, allStatements) {
        RDEBUG << "HAVE:" << s;
        QVERIFY (s.object().toString() == "Winston");
    }
}

void TestRdf::expandStatementsReferencingSubject()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    QSharedPointer<Soprano::Model> m = rdf->model();
    QVERIFY(m);
    QSharedPointer<Soprano::Model> localModel(Soprano::createModel());

    Node rdfFirst = Node::createResourceNode(QUrl("http://www.w3.org/1999/02/22-rdf-syntax-ns#first"));
    Node rdfRest  = Node::createResourceNode(QUrl("http://www.w3.org/1999/02/22-rdf-syntax-ns#rest"));
    StatementIterator it;
    QList<Statement> allStatements;
    it = m->listStatements(Node(), rdfFirst, Node());
    allStatements = it.allElements();
    QVERIFY (allStatements.size() >= 2);
    foreach (Soprano::Statement s, allStatements) {
        RDEBUG << "HAVE:" << s;
        localModel->addStatement (s);
    }
    QVERIFY (localModel->statementCount() >= 2);

    rdf->expandStatementsReferencingSubject(localModel);
    RDEBUG << localModel->statementCount();
    QVERIFY (localModel->statementCount() >= 4);

    /// FIXME: check that the proper new triples are in there.
    it = localModel->listStatements(Node::createResourceNode(QUrl("uri:campingtime")),
                                    Node::createResourceNode(QUrl("http://www.w3.org/2002/12/cal/icaltzd#geo")),
                                    Node());
    allStatements = it.allElements();
    QCOMPARE (allStatements.size(), 1);
    foreach (Soprano::Statement s, allStatements) {
        //RDEBUG << "HAVE:" << s;
        QVERIFY (s.object().isBlank());
    }
}

void TestRdf::serializeRDFLists()
{
    QSharedPointer<Soprano::Model> m(Soprano::createModel());
    Soprano::Node context = Node();

    QList<Soprano::Node> dataBNodeList;
    dataBNodeList << Node(LiteralValue::createPlainLiteral("tango"));
    dataBNodeList << Node(LiteralValue::createPlainLiteral("bbq"));
    dataBNodeList << Node(LiteralValue::createPlainLiteral("bob"));

    Soprano::Node ListHeadSubject = Node::createResourceNode(QUrl("uri:majou"));
    KoTextRdfCore::saveList(m, ListHeadSubject, dataBNodeList, context);
    QCOMPARE (m->statementCount(), 7);

    // Is the reloaded data the same as what we sent out?
    QList<Soprano::Statement> slist = KoTextRdfCore::loadList(m,ListHeadSubject);
    QVERIFY (slist.size() ==  dataBNodeList.size());
    foreach (Soprano::Statement s, slist)  {
        QVERIFY (dataBNodeList.contains (s.object()));
    }

    // Add one more to the list, write/read again and make sure it is all OK still.
    dataBNodeList << Node(LiteralValue::createPlainLiteral("happy"));
    KoTextRdfCore::saveList(m, ListHeadSubject, dataBNodeList, context);
    QCOMPARE (m->statementCount(), 9);
    slist = KoTextRdfCore::loadList(m,ListHeadSubject);
    QVERIFY (slist.size() ==  dataBNodeList.size());
    foreach (Soprano::Statement s, slist)  {
        QVERIFY (dataBNodeList.contains (s.object()));
    }
}

void TestRdf::removeStatementsIfTheyExist()
{
    QSharedPointer<Soprano::Model> m(Soprano::createModel());
    Soprano::Node context = Node();
    m->addStatement(Node::createResourceNode(QUrl("uri:test1")),
                    Node::createResourceNode(QUrl("uri:test2")),
                    Node::createResourceNode(QUrl("uri:test3")),
                    context);
    m->addStatement(Node::createResourceNode(QUrl("uri:test4")),
                    Node::createResourceNode(QUrl("uri:test5")),
                    Node::createResourceNode(QUrl("uri:test6")),
                    context);
    m->addStatement(Node::createResourceNode(QUrl("uri:test7")),
                    Node::createResourceNode(QUrl("uri:test8")),
                    Node::createResourceNode(QUrl("uri:test9")),
                    context);

    QList<Soprano::Statement> removeList;
    removeList << Statement(Node::createResourceNode(QUrl("uri:test4")),
                            Node::createResourceNode(QUrl("uri:test5")),
                            Node::createResourceNode(QUrl("uri:test6")),
                            context);
    KoTextRdfCore::removeStatementsIfTheyExist(m, removeList);
    QCOMPARE (m->statementCount(), 2);
    removeList << Statement(Node::createResourceNode(QUrl("uri:testAA")),
                            Node::createResourceNode(QUrl("uri:testBB")),
                            Node::createResourceNode(QUrl("uri:testCC")),
                            context);
    removeList << Statement(Node::createResourceNode(QUrl("uri:test7")),
                            Node::createResourceNode(QUrl("uri:test8")),
                            Node::createResourceNode(QUrl("uri:test9")),
                            context);
    removeList << Statement(Node::createResourceNode(QUrl("uri:testAA")),
                            Node::createResourceNode(QUrl("uri:testBB")),
                            Node::createResourceNode(QUrl("uri:testCCDD")),
                            context);
    removeList << Statement(Node::createResourceNode(QUrl("uri:test7")),
                            Node::createResourceNode(QUrl("uri:test8")),
                            Node::createResourceNode(QUrl("uri:test9")),
                            context);
    removeList << Statement(Node::createResourceNode(QUrl("uri:test7")),
                            Node::createResourceNode(QUrl("uri:test8")),
                            Node::createResourceNode(QUrl("uri:test9")),
                            context);
    KoTextRdfCore::removeStatementsIfTheyExist(m, removeList);
    QCOMPARE (m->statementCount(), 1);

}

void TestRdf::KoTextRdfCoreTripleFunctions()
{
    QSharedPointer<Soprano::Model> m(Soprano::createModel());
    Soprano::Node context = Node();
    m->addStatement(Node::createResourceNode(QUrl("uri:test1")),
                    Node::createResourceNode(QUrl("uri:test2")),
                    Node::createResourceNode(QUrl("uri:test3")),
                    context);
    m->addStatement(Node::createResourceNode(QUrl("uri:test4")),
                    Node::createResourceNode(QUrl("uri:test5")),
                    Node::createResourceNode(QUrl("uri:test6")),
                    context);
    m->addStatement(Node::createResourceNode(QUrl("uri:test7")),
                    Node::createResourceNode(QUrl("uri:test8")),
                    Node(LiteralValue::createPlainLiteral("happy")),
                    context);
    m->addStatement(Node::createResourceNode(QUrl("http://www.calligra.org/testB1")),
                    Node::createResourceNode(QUrl("http://www.calligra.org/testB2")),
                    Node(LiteralValue::createPlainLiteral("zed")),
                    context);

    Soprano::Node n;
    QString s;

    n = KoTextRdfCore::getObject(m,
                                 Node::createResourceNode(QUrl("uri:test7")),
                                 Node::createResourceNode(QUrl("uri:test8")));
    QVERIFY (n.isValid());
    QVERIFY (n.toString() == "happy");

    QVERIFY (KoTextRdfCore::getProperty(m,
                                        Node::createResourceNode(QUrl("uri:test7")),
                                        Node::createResourceNode(QUrl("uri:test8")),
                                        "default-value") == "happy");
    QVERIFY (KoTextRdfCore::getProperty(m,
                                        Node::createResourceNode(QUrl("uri:test7")),
                                        Node::createResourceNode(QUrl("uri:AAAAAAAAAAAAAA")),
                                        "default-value") == "default-value");



    QString sparqlQuery = ""
            "prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> \n"
            "prefix ko: <http://www.calligra.org/> \n"
            "select ?s ?binding  \n"
            "where { \n"
            "    ?s ko:testB2 ?binding \n"
            "}\n";
    Soprano::QueryResultIterator it = m->executeQuery(sparqlQuery, Soprano::Query::QueryLanguageSparql);
    s =  KoTextRdfCore::optionalBindingAsString(it,"binding","default-value");
    QVERIFY (s == "zed");
    s =  KoTextRdfCore::optionalBindingAsString(it,"nothinghere","default-value");
    QVERIFY (s == "default-value");
}

void TestRdf::createUserStylesheet()
{
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    QSharedPointer<Soprano::Model> m = rdf->model();
    QVERIFY(m);

    hKoRdfSemanticItem f = rdf->createSemanticItem("Contact", rdf);
    int originalUserStylesheetsSize = f->userStylesheets().size();
    hKoSemanticStylesheet ss = f->createUserStylesheet("test sheet A",
                                                       "%NAME% and again %NAME%");
    QCOMPARE (f->userStylesheets().size(), originalUserStylesheetsSize+1);
    QVERIFY (ss);
    QVERIFY (ss->uuid() != "");
    QVERIFY (ss->name() == "test sheet A");
    QVERIFY (ss->templateString() == "%NAME% and again %NAME%");

    bool found = false;
    QList<hKoSemanticStylesheet> slist = f->userStylesheets();
    foreach(hKoSemanticStylesheet z, slist) {
        if(z->uuid()==ss->uuid()) {
            found = true;
        }
    }
    QVERIFY (found);

    QVERIFY (ss->isMutable());
    ss->name ("new name");
    ss->templateString ("%NAME% and then %NICK%");
    QVERIFY (ss->name() == "new name");
    QVERIFY (ss->templateString() == "%NAME% and then %NICK%");
    found = false;
    slist = f->userStylesheets();
    foreach(hKoSemanticStylesheet z, slist) {
        if(z->uuid()==ss->uuid()) {
            QVERIFY (z->name() == "new name");
            QVERIFY (z->templateString() == "%NAME% and then %NICK%");
            found = true;
        }
    }
    QVERIFY (found);

    hKoSemanticStylesheet z = hKoSemanticStylesheet(0);
    z = f->findStylesheetByUuid(ss->uuid());
    QVERIFY (z);
    QVERIFY (z->uuid() == ss->uuid());

    z = f->findStylesheetByName(KoSemanticStylesheet::stylesheetTypeUser(),
                                ss->name());
    QVERIFY (z);
    QVERIFY (z->uuid() == ss->uuid());

    z = f->findStylesheetByName(f->userStylesheets(), ss->name());
    QVERIFY (z);
    QVERIFY (z->uuid() == ss->uuid());

}


void TestRdf::testRoundtrip()
{
    KUrl url(QString(FILES_OUTPUT_DIR) + "/rdf_roundtrip.odt");
    const QString lorem(
                "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor"
                "incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud"
                "exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n"
                "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla"
                "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia"
                "deserunt mollit anim id est laborum.\n"
                );

    {

        // Get the words part and create a document
        KWDocument *doc = new KWDocument(new MockPart);
        Q_ASSERT(doc);
        doc->setAutoSave(0);
        doc->initEmpty();

        KoDocumentRdf *rdfDoc = new KoDocumentRdf(doc);
        doc->setDocumentRdf(rdfDoc);

        Q_ASSERT(rdfDoc);

        // get the main text frame
        KWTextFrameSet *mainFrameSet = doc->mainFrameSet();
        Q_ASSERT(mainFrameSet);

        QTextDocument *textDocument = mainFrameSet->document();
        Q_ASSERT(textDocument);

        // Insert some text and some rdf
        KoTextDocument koTextDocument(textDocument);
        KoTextEditor *editor = koTextDocument.textEditor();
        editor->insertText(lorem);

        editor->insertTable(5,10);
        const QTextTable *table = editor->currentTable();

        QTextCursor cursor(editor->document());
        cursor.setPosition(table->firstPosition());
        QCOMPARE(cursor.position(), lorem.length() + 1);
        KoBookmark *mark = new KoBookmark(cursor);
        mark->setPositionOnlyMode(false);

        KoTextInlineRdf *inlineRdf(new KoTextInlineRdf(editor->document(), mark));
        QString newId = inlineRdf->createXmlId();
        inlineRdf->setXmlId(newId);

        mark->setName("blablabla -- in any case, not the rdf xmlid...");
        mark->setInlineRdf(inlineRdf);
        KoTextRangeManager *rangeManager = koTextDocument.textRangeManager();
        rangeManager->insert(mark);

        hKoRdfLocation location(new KoRdfLocation(0, rdfDoc));
        location->setDlat(5.0);
        location->setDlong(10.0);

        Soprano::Statement st(
                    location->linkingSubject(), // subject
                    Soprano::Node::createResourceNode(QUrl("http://docs.oasis-open.org/ns/office/1.2/meta/pkg#idref")), // predicate
                    Soprano::Node::createLiteralNode(newId), // object
                    rdfDoc->manifestRdfNode()); // manifest datastore

        rdfDoc->model()->addStatement(st);
        rdfDoc->rememberNewInlineRdfObject(inlineRdf);

        Q_ASSERT(rdfDoc->model()->statementCount() > 0);
        QCOMPARE(mark->rangeStart(), lorem.length() + 1);

        editor->setPosition(table->lastPosition());
        editor->movePosition(QTextCursor::NextCharacter);
        mark->setRangeEnd(editor->position());

        // Check the position of the bookmark
        Q_ASSERT(location->xmlIdList().length() == 1);
        QString xmlid = location->xmlIdList()[0];
        QPair<int,int> position = rdfDoc->findExtent(location->xmlIdList()[0]);
        QCOMPARE(position.first, lorem.length());
        QCOMPARE(position.second, lorem.length() + 5*10 + 1);

        // search for locations and check the position
        // Find the location object
        QList<hKoRdfSemanticItem> locations = rdfDoc->semanticItems("Location");
        Q_ASSERT(locations.size() == 1);
        hKoRdfLocation location2 = static_cast<hKoRdfLocation>(locations[0]);
        QCOMPARE(location2->dlat(), location->dlat());
        QCOMPARE(location2->dlong(), location->dlong());

        // check the position for the location object we've just found,
        // and which should be the same as the other one...
        position = rdfDoc->findExtent(location2->xmlIdList()[0]);
        QCOMPARE(position.first, lorem.length());
        QCOMPARE(position.second, lorem.length() + 5*10 + 1);

        // Save the document -- this changes all xmlid's
        doc->documentPart()->saveAs(url);

        // Check the position again -- this xmlid doesn't exist anymore, so
        // should be 0,0
        position = rdfDoc->findExtent(xmlid);
        QCOMPARE(position.first, 0);
        QCOMPARE(position.second, 0);

        // Find the location object
        locations = rdfDoc->semanticItems("Location");
        Q_ASSERT(locations.size() == 1);
        hKoRdfLocation location3 = static_cast<hKoRdfLocation>(locations[0]);

        QCOMPARE(location3->dlat(), location->dlat());
        QCOMPARE(location3->dlong(), location->dlong());

        QPair<int,int> position3 = rdfDoc->findExtent(location3->xmlIdList()[0]);

        QCOMPARE(position3.first, 444);
        QCOMPARE(position3.second, 497);

        delete doc;
    }
    {
        // Load the document
        KWDocument *doc = new KWDocument(new MockPart);
        Q_ASSERT(doc);
        doc->setAutoSave(0);

        KoDocumentRdf *rdfDoc = new KoDocumentRdf(doc);
        doc->setDocumentRdf(rdfDoc);

        // this also creates a view...
        bool result = doc->openUrl(url);
        Q_ASSERT(result);

        // get the main text frame
        KWTextFrameSet *mainFrameSet = doc->mainFrameSet();
        Q_ASSERT(mainFrameSet);

        QTextDocument *textDocument = mainFrameSet->document();
        Q_ASSERT(textDocument);

        // Insert some text and some rdf
        KoTextDocument koTextDocument(textDocument);
        KoTextEditor *editor = koTextDocument.textEditor();
        editor->updateInlineObjectPosition();

        // Check for the rdf statements and spans
        const QList<hKoRdfSemanticItem> locations = rdfDoc->semanticItems("Location");
        Q_ASSERT(locations.size() == 1);
        hKoRdfLocation location = static_cast<hKoRdfLocation>(locations[0]);
        Q_ASSERT(location->name() == "10,5");
        Q_ASSERT(location->dlat() == 5.0);
        Q_ASSERT(location->dlong() == 10.0);
        Q_ASSERT(location->xmlIdList().length() == 1);
        QString xmlid = location->xmlIdList()[0];
        QPair<int,int> position = rdfDoc->findExtent(xmlid);
        // TODO: Why is this like the above but both - 1 ?
        Q_ASSERT(position.first == 443);
        Q_ASSERT(position.second == 496);

        // check whether the table between the bookmarks is in the right position
        // after loading
        editor->setPosition(position.first + 3);
        Q_ASSERT(editor->currentTable());
        editor->setPosition(position.second + 1);
        Q_ASSERT(!editor->currentTable());
        delete doc;
    }
}

QTEST_KDEMAIN(TestRdf, GUI)
