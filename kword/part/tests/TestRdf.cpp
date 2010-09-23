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

#include <KWDocument.h>
#include <frames/KWTextFrameSet.h>
#include <rdf/KoDocumentRdf.h>
#include <rdf/KoRdfPrefixMapping.h>
#include <rdf/KoSopranoTableModel.h>
#include <KoStore.h>
#include <KoTextDocument.h>
#include "KWAboutData.h"
#include <KoApplication.h>
#include <KoXmlWriter.h>
#include <KoShapeRegistry.h>
#include <KoXmlReader.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoStyleManager.h>
#include <KoXmlNS.h>
#include <kcomponentdata.h>
#include <QTemporaryFile>
#include <KoTextRdfCore.h>

#include <KFileItem>
#include <kio/job.h>
#include <kio/jobclasses.h>

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
        return 0;
    }
    if (!rdf->loadOasis(store)) {
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
    Soprano::Model *m = rdf->model();
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
        QVERIFY (s.context().toString() == rdf->rdfPathContextPrefix() + "geo1.rdf");
    }

}

void TestRdf::findStatements()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    Soprano::Model *m = rdf->model();
    QVERIFY(m);

    StatementIterator it;
    QList<Statement> allStatements;

    Soprano::Model * submodel = rdf->findStatements("james2");
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
    Soprano::Model *m = rdf->model();
    QVERIFY(m);

    QString nameThatRemains ("This one will remain, as it's not in the RDF");
    QSet<QString> expectedNames;
    expectedNames << "Dan Brickley";
    expectedNames << "Gollum";
    expectedNames << "O'Brien";
    expectedNames << nameThatRemains;

    QList<KoRdfFoaF*> col = rdf->foaf();
    QCOMPARE (col.size(),10);
    foreach (KoRdfFoaF* f, col) {
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

            QList<KoSemanticStylesheet*> systemStylesheets = f->stylesheets();
            QVERIFY (systemStylesheets.size()>=4);
            bool foundTextSystemStylesheet = false;
            foreach (KoSemanticStylesheet* ss, systemStylesheets)  {
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
    Soprano::Model *m = rdf->model();
    QVERIFY(m);

    QString nameThatRemains ("This one will remain, as it's not in the RDF");
    QSet<QString> expectedNames;
    expectedNames << "Lets party like its 1999";
    expectedNames << "Add Test";
    expectedNames << nameThatRemains;

    QList<KoRdfCalendarEvent*> col = rdf->calendarEvents();
    QCOMPARE (col.size(),4);
    foreach (KoRdfCalendarEvent* f, col) {
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

            QList<KoSemanticStylesheet*> systemStylesheets = f->stylesheets();
            QVERIFY (systemStylesheets.size()>=4);
            bool foundTextSystemStylesheet = false;
            foreach (KoSemanticStylesheet* ss, systemStylesheets)  {
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
    Soprano::Model *m = rdf->model();
    QVERIFY(m);

    QString nameThatRemains ("This one will remain, as it's not in the RDF");
    QSet<QString> expectedNames;
    expectedNames << "-79.9458,40.4427";
    expectedNames << nameThatRemains;

    QList<KoRdfLocation*> col = rdf->locations();
    QCOMPARE (col.size(),2);
    foreach (KoRdfLocation* f, col) {
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

            QList<KoSemanticStylesheet*> systemStylesheets = f->stylesheets();
            QVERIFY (systemStylesheets.size()>=2);
            bool foundTextSystemStylesheet = false;
            foreach (KoSemanticStylesheet* ss, systemStylesheets)  {
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

static Soprano::Model *loadRDFXMLFromODT(const QString &odt)
{
    Soprano::Node context;
    QUrl BaseURI = QUrl(QString());
    const Soprano::Parser *parser =
        Soprano::PluginManager::instance()->discoverParserForSerialization(
            Soprano::SerializationRdfXml);

    KIO::StoredTransferJob *job = KIO::storedGet(KUrl (odt));
    job->exec();
    QByteArray ba = job->data();
    RDEBUG << "rdfxml.sz:" << ba.size();
    Soprano::Model * ret = Soprano::createModel();
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

void TestRdf::addAndSage()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    Soprano::Model *m = rdf->model();
    QVERIFY(m);
    QCOMPARE (234, m->statementCount());

    Node explicitContext = Node(QUrl(rdf->rdfPathContextPrefix() + "explicit.rdf"));
    m->addStatement(Node::createResourceNode(QUrl("uri:test1")),
                     Node::createResourceNode(QUrl("uri:test2")),
                     Node::createResourceNode(QUrl("uri:test3")),
                     rdf->manifestRdfNode());
    m->addStatement(Node::createResourceNode(QUrl("uri:test4")),
                     Node::createResourceNode(QUrl("uri:test5")),
                     Node::createResourceNode(QUrl("uri:test6")),
                     explicitContext);
    QCOMPARE (236, m->statementCount());

    const char* mimeType = "application/zip";
    QTemporaryFile file;
    if (file.open()) {
//        QString todt = "/tmp/testing.odt";
        QString todt = file.fileName();
        // QByteArray manifestBA;
        // QBuffer manifestBuffer(&manifestBA);
        // manifestBuffer.open(QBuffer::ReadWrite);
        // KoXmlWriter* manifestWriter = new KoXmlWriter (&manifestBuffer);
        KoStore *store = KoStore::createStore(todt, KoStore::Write, mimeType, KoStore::Zip);
        store->disallowNameExpansion();
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

        if (Soprano::Model* m = loadRDFXMLFromODT("zip:" + todt + "/manifest.rdf"))  {
            it = m->listStatements(
                Node::createResourceNode(QUrl("uri:test1")),
                Node::createResourceNode(QUrl("uri:test2")),
                Node());
            allStatements = it.allElements();
            QCOMPARE (allStatements.size(), 1);
            foreach (Soprano::Statement s, allStatements) {
                RDEBUG << "HAVE:" << s;
                QVERIFY (s.object().toString() == "uri:test3");
            }
        }

        if (Soprano::Model* m = loadRDFXMLFromODT("zip:" + todt + "/explicit.rdf"))  {
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
    }
}

void TestRdf::semanticItemViewSite()
{
    RDEBUG;
    QString odt = QString(FILES_DATA_DIR) + "/weekend-hike.odt";
    KoDocumentRdf *rdf = loadDocument(odt);
    QVERIFY(rdf);
    Soprano::Model *m = rdf->model();
    QVERIFY(m);
    QCOMPARE (234, m->statementCount());

    QList<KoRdfFoaF*> col = rdf->foaf();
    foreach (KoRdfFoaF* f, col) {
        if (f->name() == "James Smith") {

            KoSemanticStylesheet *ss = f->findStylesheetByUuid("0dd5878d-95c5-47e5-a777-63ec36da3b9a");
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
    Soprano::Model *m = rdf->model();
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
    Soprano::Model *m = rdf->model();
    QVERIFY(m);
    Soprano::Model *localModel = Soprano::createModel();

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
    Soprano::Model *m = rdf->model();
    QVERIFY(m);
    Soprano::Model *localModel = Soprano::createModel();

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
    Soprano::Model *m = rdf->model();
    QVERIFY(m);
    Soprano::Model *localModel = Soprano::createModel();

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

void TestRdf::serailizeRDFLists()
{
    Soprano::Model *m = Soprano::createModel();
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
    Soprano::Model *m = Soprano::createModel();
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
    Soprano::Model *m = Soprano::createModel();
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
    m->addStatement(Node::createResourceNode(QUrl("http://www.koffice.org/testB1")),
                     Node::createResourceNode(QUrl("http://www.koffice.org/testB2")),
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
        "prefix ko: <http://www.koffice.org/> \n"
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
    Soprano::Model *m = rdf->model();
    QVERIFY(m);

    KoRdfSemanticItem* f = KoRdfSemanticItem::createSemanticItem(rdf, rdf, "Contact");
    int originalUserStylesheetsSize = f->userStylesheets().size();
    KoSemanticStylesheet* ss = f->createUserStylesheet("test sheet A",
                                                       "%NAME% and again %NAME%");
    QCOMPARE (f->userStylesheets().size(), originalUserStylesheetsSize+1);
    QVERIFY (ss);
    QVERIFY (ss->uuid() != "");
    QVERIFY (ss->name() == "test sheet A");
    QVERIFY (ss->templateString() == "%NAME% and again %NAME%");

    bool found = false;
    QList<KoSemanticStylesheet*> slist = f->userStylesheets();
    foreach(KoSemanticStylesheet* z, slist) {
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
    foreach(KoSemanticStylesheet* z, slist) {
        if(z->uuid()==ss->uuid()) {
            QVERIFY (z->name() == "new name");
            QVERIFY (z->templateString() == "%NAME% and then %NICK%");
            found = true;
        }
    }
    QVERIFY (found);

    KoSemanticStylesheet *z = 0;
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

QTEST_KDEMAIN(TestRdf, GUI)
#include <TestRdf.moc>
