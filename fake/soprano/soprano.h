#ifndef FAKE_SOPRANO_H
#define FAKE_SOPRANO_H

#include <QTextStream>

#include "kofake_export.h"


namespace Soprano {

    class Node
    {
    public:
    };

    class Model
    {
    public:
        //int statementCount() { return 0; }
    };
    //Model* createModel() { return 0; }

    class Statement
    {
    public:
    };

    class StatementIterator
    {
    public:
    };

    class QueryResultIterator
    {
    public:
    };
    
    //enum SerializationRdfType { SerializationRdfXml };
    class Serializer
    {
    public:
        //void serialize(Soprano::StatementIterator, QTextStream, SerializationRdfType) {}
    };

    class PluginManager
    {
    public:
        //static PluginManager* instance();
        //const Serializer* discoverSerializerForSerialization(SerializationRdfType) { return 0; }
    };
    //Q_GLOBAL_STATIC(PluginManager, globalPluginManager)
    //PluginManager* PluginManager::instance() { return globalPluginManager(); }

    class Query
    {
    public:
        //enum { QueryLanguageSparql };
    };

}

#endif
