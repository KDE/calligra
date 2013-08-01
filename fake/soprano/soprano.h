#ifndef FAKE_SOPRANO_H
#define FAKE_SOPRANO_H

#include <QTextStream>
#include <QList>
#include <QUrl>
#include <QDebug>

#include "kofake_export.h"


namespace Soprano {
    enum RdfSerialization {
        SerializationUnknown = 0x0,   /**< The serialization is unknown. */
        SerializationRdfXml = 0x1,    /**< Standard RDF/XML serialization */
        SerializationN3 = 0x2,        /**< Notation 3: http://www.w3.org/DesignIssues/Notation3 */
        SerializationNTriples = 0x4,  /**< N-Triples as defined by W3: http://www.w3.org/TR/rdf-testcases/#ntriples */
        SerializationTurtle = 0x8,    /**< Turtle - Terse RDF Triple Language: http://www.dajobe.org/2004/01/turtle/ */
        SerializationTrig = 0x10,     /**< TriG - Turtle + Named Graphs: http://sites.wiwiss.fu-berlin.de/suhl/bizer/TriG/ */
        SerializationNQuads = 0x20,   /**< N-Quads extends over N-Triples in that it adds an optional context node. */
        SerializationUser = 0x0       /**< The user type can be used to introduce unknown RDF serializations by name */
    };
    namespace Error {
        /**
         * %Soprano defines a number of error codes that are
         * used to provide a quick success status check in methods
         * such as Model::addStatement().
         *
         * \sa Error::Error::code(), Error::convertErrorCode
         */
        enum ErrorCode {
            ErrorNone = 0x0,             /**< No error occured, i.e. success. */
            ErrorInvalidArgument = 0x1,  /**< Error indicating that a method argument was invalid. For example an invalid Statement in Model::addStatement(). */
            ErrorInvalidStatement = ErrorInvalidArgument, /**< \deprecated: use ErrorInvalidArgument */
            ErrorNotSupported = 0x2,     /**< Error indicating that a certain functionality is not supported. */
            ErrorParsingFailed = 0x3,    /**< Parsing a query or an RDF serialization failed. */
            ErrorPermissionDenied = 0x4, /**< Permission is denied. \since 2.1 */
            ErrorTimeout = 0x5,          /**< The command timed out. \since 2.7.4 */
            ErrorUnknown = 0x1000        /**< An unknown error occured. */
        };
    }

    class Node
    {
    public:
        enum Type {
            EmptyNode    = 0, /**< An empty node, can be used as a wildcard in commands like Model::listStatements. */
            ResourceNode = 1, /**< A resource node has a URI which can be accessed via uri() */
            LiteralNode  = 2, /**< A literal node has a literal value and an optional language. */
            BlankNode    = 3  /**< A blank node has an identifier string */
        };
        Node() {}
        Node(QUrl) {}
        bool isValid() const {return false;}
        QString toString() const {return "stub node";}
        static Node createResourceNode( const QUrl& uri ) {return Node();}
        static Node createLiteralNode( const QString value ) {return Node();}
    };

    class Statement
    {
    public:
        Statement() {}

        Statement( const Node &subject, const Node &predicate, const Node &object, const Node &context = Node() ) {}

        /**
         * \return The subject.
         */
        Node subject() const {return Node();}

        /**
         * \return The predicate.
         */
        Node predicate() const {return Node();}

        /**
         * \return The object.
         */
        Node object() const {return Node();}

        /**
         * \return The Context node.
         */
        Node context() const {return Node();}
    };

    class StatementIterator
    {
    public:
        QList<Statement>  allElements() {return QList<Statement>();}
    };

    class QueryResultIterator
    {
    public:
        Node binding( const QString &name ) const {return Node();}
        bool next() {return false;}
    };

    class Query
    {
    public:
        enum QueryLanguage { QueryLanguageSparql };
    };

    class Model
    {
    public:
        int statementCount() { return 0; }
        Error::ErrorCode addStatement( const Statement &statement ) {return Error::ErrorNone;}
        Error::ErrorCode addStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() ) {return Error::ErrorNone;}
        Error::ErrorCode addStatements( const QList<Statement> &statements ) {return Error::ErrorNone;}
        StatementIterator listStatements() {return StatementIterator();}
        StatementIterator listStatements( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() ) const {return StatementIterator();}
        Error::ErrorCode removeStatement( const Statement &statement ) {return Error::ErrorNone;}
        Error::ErrorCode removeAllStatements( const Statement &statement ) {return Error::ErrorNone;}
        Error::ErrorCode removeAllStatements( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() ) {return Error::ErrorNone;}
        Node createBlankNode() {return Node();}
        QueryResultIterator executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage = QString() ) {return QueryResultIterator();}
    };
    static Model* createModel() { return 0; }

    class Serializer
    {
    public:
         bool serialize( StatementIterator it, QTextStream& stream, RdfSerialization serialization, const QString& userSerialization = QString() ) const {return true;}
    };

    class Parser
    {
    public:
        StatementIterator parseString( const QString& data, const QUrl& baseUri, RdfSerialization serialization, const QString& userSerialization = QString() ) const {return StatementIterator();}
    };

    class PluginManager
    {
    public:
        static PluginManager* instance() {return new PluginManager;}
        const Serializer* discoverSerializerForSerialization( RdfSerialization serialization, const QString& userSerialization = QString() ) { return 0; }
        const Parser* discoverParserForSerialization( RdfSerialization serialization, const QString& userSerialization = QString() ) { return 0; }
    };
}

static QDebug operator<<( QDebug s, const Soprano::Statement& ) {return s;}

static QTextStream& operator<<( QTextStream& s, const Soprano::Statement& ) {return s;}

static QDebug operator<<( QDebug s, const Soprano::Node& ) {return s;}

static QTextStream& operator<<( QTextStream& s, const Soprano::Node& ) {return s;}

#endif
