#ifndef FAKE_CALLIGRABIGS_KOTEXTRDFCORE_H
#define FAKE_CALLIGRABIGS_KOTEXTRDFCORE_H

#include <QSharedPointer>
#include <Soprano/Soprano>

namespace KoTextRdfCore
{

static bool saveRdf( QSharedPointer<Soprano::Model> model, Soprano::StatementIterator triples, KoStore *store, KoXmlWriter *manifestWriter, const QString &fileName) { return false; }
static bool createAndSaveManifest(QSharedPointer<Soprano::Model> model, const QMap<QString, QString> &idmap, KoStore *store, KoXmlWriter *manifestWriter) { return false; }
static bool loadManifest(KoStore *store, QSharedPointer<Soprano::Model> model) { return false; }
static void dumpModel(const QString &message, QSharedPointer<Soprano::Model> model) {}
static QList<Soprano::Statement> loadList(QSharedPointer<Soprano::Model> model, Soprano::Node ListHeadSubject) { return QList<Soprano::Statement>(); }
static void saveList(QSharedPointer<Soprano::Model> model, Soprano::Node ListHeadSubject, QList<Soprano::Node> &dataBNodeList, Soprano::Node context) {}
static void removeStatementsIfTheyExist( QSharedPointer<Soprano::Model> model, const QList<Soprano::Statement> &removeList) {}
static Soprano::Node getObject(QSharedPointer<Soprano::Model> model, Soprano::Node s, Soprano::Node p) { return Soprano::Node(); }
static QString getProperty(QSharedPointer<Soprano::Model> m, Soprano::Node subj, Soprano::Node pred, const QString &defval) { return QString(); }
static QString optionalBindingAsString(Soprano::QueryResultIterator& it, const QString &bindingName, const QString &def) { return QString(); }
static QByteArray fileToByteArray(const QString &fileName) { return QByteArray(); }

}

#endif
