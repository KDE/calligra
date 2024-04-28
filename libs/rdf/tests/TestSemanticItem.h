/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TEST_SEMANTIC_ITEM
#define TEST_SEMANTIC_ITEM

#include <QString>
#include <QTest>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextTable>
#include <QUuid>

#include <KoDocumentRdf.h>
#include <KoInlineTextObjectManager.h>
#include <KoRdfSemanticItem.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoTextInlineRdf.h>

class TestSemanticItem;
typedef QExplicitlySharedDataPointer<TestSemanticItem> hTestSemanticItem;

class TestSemanticItem : public KoRdfSemanticItem
{
public:
    const QString PREDBASE;

    TestSemanticItem(QObject *parent, const KoDocumentRdf *rdf = 0)
        : KoRdfSemanticItem(parent, const_cast<KoDocumentRdf *>(rdf))
        , PREDBASE("http://calligra.org/testrdf/")
        , m_uri(QUuid::createUuid().toString())
    {
        Q_ASSERT(!m_uri.isEmpty());

        m_linkingSubject = Soprano::Node::createResourceNode(m_uri);

        Q_ASSERT(context().isValid());
        Q_ASSERT(m_linkingSubject.isResource());
        setRdfType(PREDBASE + "testitem");
        Q_ASSERT(documentRdf()->model()->statementCount() > 0);

        updateTriple(m_payload, "payload, payload, payload", PREDBASE + "payload");
    }

    TestSemanticItem(QObject *parent, const KoDocumentRdf *rdf, Soprano::QueryResultIterator &it)
        : KoRdfSemanticItem(parent, const_cast<KoDocumentRdf *>(rdf), it)
    {
        m_uri = it.binding("object").toString();
        Q_ASSERT(!m_uri.isNull());
        m_linkingSubject = Soprano::Node::createResourceNode(m_uri);
        Q_ASSERT(m_linkingSubject.isResource());
        m_name = it.binding("name").toString();
        Q_ASSERT(!m_name.isNull());
        m_payload = it.binding("payload").toString();
        Q_ASSERT(!m_payload.isNull());
    }

    virtual ~TestSemanticItem()
    {
    }

    virtual QWidget *createEditor(QWidget *parent)
    {
        Q_UNUSED(parent)
        return 0;
    }

    virtual void updateFromEditorData()
    {
    }

    virtual void exportToFile(const QString & /*fileName*/ = QString()) const
    {
    }

    virtual void importFromData(const QByteArray &ba, const KoDocumentRdf *rdf = 0, KoCanvasBase *host = 0)
    {
        Q_UNUSED(ba)
        Q_UNUSED(rdf)
        Q_UNUSED(host)
    }

    void setName(const QString &name)
    {
        updateTriple(m_name, name, PREDBASE + "name");
        if (documentRdf()) {
            const_cast<KoDocumentRdf *>(documentRdf())->emitSemanticObjectUpdated(hKoRdfSemanticItem(this));
        }
    }

    virtual QString name() const
    {
        return m_name;
    }

    virtual QString className() const
    {
        return "TestSemanticItem";
    }

    virtual QList<hKoSemanticStylesheet> stylesheets() const
    {
        QList<hKoSemanticStylesheet> sheets;
        return sheets;
    }

    Soprano::Node linkingSubject() const
    {
        return m_linkingSubject;
    }

    static QList<hTestSemanticItem> allObjects(KoDocumentRdf *rdf, QSharedPointer<Soprano::Model> model = QSharedPointer<Soprano::Model>(0))
    {
        QList<hTestSemanticItem> result;
        QSharedPointer<Soprano::Model> m = model ? model : rdf->model();

        QString query =
            "prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> \n"
            "prefix testrdf: <http://calligra.org/testrdf/> \n"
            "select distinct ?name ?object ?payload \n"
            "where { \n"
            "    ?object rdf:type testrdf:testitem . \n"
            "    ?object testrdf:name ?name . \n"
            "    ?object testrdf:payload ?payload \n"
            "}\n"
            "    order by  DESC(?name) \n ";

        Soprano::QueryResultIterator it = m->executeQuery(query, Soprano::Query::QueryLanguageSparql);

        while (it.next()) {
            hTestSemanticItem item(new TestSemanticItem(0, rdf, it));
            result << item;
        }
        return result;
    }

    static QList<hTestSemanticItem>
    findItemsByName(const QString name, KoDocumentRdf *rdf, QSharedPointer<Soprano::Model> model = QSharedPointer<Soprano::Model>(0))
    {
        QList<hTestSemanticItem> result;
        QSharedPointer<Soprano::Model> m = model ? model : rdf->model();

        QString query(
            "prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> \n"
            "prefix testrdf: <http://calligra.org/testrdf/> \n"
            "select distinct ?name ?object ?payload \n"
            "where { \n"
            "    ?object rdf:type testrdf:testitem . \n"
            "    ?object testrdf:name ?name . \n"
            "    ?object testrdf:payload ?payload . \n"
            "    filter (?name = %1) "
            "}\n"
            "    order by  DESC(?name) \n ");

        Soprano::QueryResultIterator it = m->executeQuery(query.arg(Soprano::Node::literalToN3(name)), Soprano::Query::QueryLanguageSparql);

        while (it.next()) {
            hTestSemanticItem item(new TestSemanticItem(0, rdf, it));
            result << item;
        }
        return result;
    }

private:
    Soprano::Node m_linkingSubject;
    QString m_name;
    QString m_uri;
    QString m_payload;
};

#endif
