/* This file is part of the KOffice libraries
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __koffice_filter_chain_h__
#define __koffice_filter_chain_h__

#include <qcstring.h>
#include <qasciidict.h>
#include <qptrlist.h>
#include <qstringlist.h>

#include <koFilter.h>
#include <koQueryTrader.h>
#include <koStoreDevice.h>

class KTempFile;
class KoFilterManager;
class KoDocument;

// This class represents a chain of plain filters. Instances of
// it are created by the KOffice::Graph class -- and noone else ;)
// The links can be freely appended and prepended and we don't
// check whether the order of the links makes sense. Take care.
class KoFilterChain : public KShared
{

public:
    typedef KSharedPtr<KoFilterChain> Ptr;

    KoFilterChain( const KoFilterManager* manager );
    virtual ~KoFilterChain();

    const KoFilterManager* manager() const { return m_manager; }

    // Append a link to this filter chain
    void appendChainLink( KoFilterEntry::Ptr filterEntry, const QCString& from, const QCString& to );
    // Prepend a link to this filter chain
    void prependChainLink( KoFilterEntry::Ptr filterEntry, const QCString& from, const QCString& to );

    // Starts the filtering process
    KoFilter::ConversionStatus invokeChain();
    // Tells KoFilterManager the output file of the filter chain
    // in case of an import operation. If it's QString::null we
    // directly manipulated the document.
    QString chainOutput() const;


    // #### "API" for the filter. Here it get hold of files, storages,...
    // Note: Call that only once for your filter
    // Get the current file to read from
    QString inputFile();
    // Get the current file to write to
    QString outputFile();

    // Get a file from a storage. May return 0!
    KoStoreDevice* storageFile( const QString& name = "root", KoStore::Mode mode = KoStore::Read );

    // "Experimental" mode... may return 0!
    KoDocument* inputDocument();
    // "Experimental" mode... may return 0!
    KoDocument* outputDocument();


    // debugging
    void dump() const;

private:
    // These methods are friends of KoFilterManager and provide access
    // to a private part of its API. As I don't want to include
    // koFilterManager.h in this header the direction is "int" here.
    QString filterManagerImportFile() const;
    QString filterManagerExportFile() const;
    KoDocument* filterManagerKoDocument() const;
    int filterManagerDirection() const;

    // Helper method which keeps track of all the temp files, documents,
    // storages,... and properly deletes them as soon as they are not
    // needed anymore.
    void manageIO();
    void finalizeIO();

    bool createTempFile( KTempFile** tempFile, bool autoDelete = true );

    void inputFileHelper( KoDocument* document, const QString& alternativeFile );
    void outputFileHelper( bool autoDelete );
    KoStoreDevice* storageNewStreamHelper( KoStore** storage, KoStoreDevice** device, const QString& name );
    KoStoreDevice* storageHelper( const QString& file, const QString& streamName,
                                  KoStore::Mode mode, KoStore** storage, KoStoreDevice** device );
    KoStoreDevice* storageCleanupHelper( KoStore** storage );

    KoDocument* createDocument( const QString& file );
    KoDocument* createDocument( const QCString& mimeType );

    // A small private helper class with represents one single filter
    // (one link of the chain)
    class ChainLink
    {

    public:
        ChainLink( KoFilterChain* chain, KoFilterEntry::Ptr filterEntry,
                   const QCString& from, const QCString& to );

        KoFilter::ConversionStatus invokeFilter();

        QCString from() const { return m_from; }
        QCString to() const { return m_to; }

        // debugging
        void dump() const;

    private:
        KoFilterChain* m_chain;
        KoFilterEntry::Ptr m_filterEntry;
        QCString m_from, m_to;

        class Private;
        Private* d;
    };

    // "A whole is that which has beginning, middle, and end" - Aristotle
    // ...but we also need to signal "Done" state, Mr. Aristotle
    enum Whole { Beginning = 1, Middle = 2, End = 4, Done = 8 };

    // Don't copy or assign filter chains
    KoFilterChain( const KoFilterChain& rhs );
    KoFilterChain& operator=( const KoFilterChain& rhs );

    const KoFilterManager* m_manager;
    QPtrList<ChainLink> m_chainLinks;

    // stuff needed for bookkeeping
    int m_state;

    QString m_inputFile;              // Did we pass around plain files?
    QString m_outputFile;

    KoStore* m_inputStorage;          // ...or was it a storage+device?
    KoStoreDevice* m_inputStorageDevice;
    KoStore* m_outputStorage;
    KoStoreDevice* m_outputStorageDevice;

    KoDocument* m_inputDocument;      // ...or even documents?
    KoDocument* m_outputDocument;

    KTempFile* m_inputTempFile;
    KTempFile* m_outputTempFile;

    bool m_inputQueried, m_outputQueried;

    class Private;
    Private* d;
};


// As we use quite generic classnames...
namespace KOffice
{
    class Vertex;
    template<class T> class PriorityQueue;

    class Edge
    {

    public:
        // creates a new edge to "vertex" with the given weight.
        Edge( Vertex* vertex, KoFilterEntry::Ptr filterEntry );
        ~Edge() {}

        unsigned int weight() const { return m_filterEntry ? m_filterEntry->weight : 0; }
        KoFilterEntry::Ptr filterEntry() const { return m_filterEntry; }
        const Vertex* vertex() const { return m_vertex; }

        // Relaxes the "connected" vertex (i.e. the weight of the
        // connected vertex = "predec.->key()" (parameter) + weight of this edge
        // As this will only be called once we calculate the weight
        // of the edge "on the fly"
        // Note: We have to pass the queue as we have to call keyDecreased :}
        void relax( const Vertex* predecessor, PriorityQueue<Vertex>& queue );

        // debugging
        void dump( const QCString& indent ) const;

    private:
        Edge( const Edge& rhs );
        Edge& operator=( const Edge& rhs );

        Vertex* m_vertex;
        KoFilterEntry::Ptr m_filterEntry;

        class Private;
        Private* d;
    };


    class Vertex
    {

    public:
        Vertex( const QCString& mimeType );
        ~Vertex() {}

        QCString mimeType() const { return m_mimeType; }

        // current "weight" of the vertex - will be "relaxed" when
        // running the shortest path algorithm. Returns true if it
        // really was "relaxed"
        bool setKey( unsigned int key );
        unsigned int key() const { return m_weight; }
        // Can be used to set the key back to "Infinity" (UINT_MAX)
        // and reset the predecessor of this vertex
        void reset();

        // Position in the heap, needed for a fast keyDecreased operation
        void setIndex( int index ) { m_index=index; }
        int index() const { return m_index; }

        // predecessor on the way from the source to the destination,
        // needed for the shortest path algorithm
        void setPredecessor( const Vertex* predecessor ) { m_predecessor=predecessor; }
        const Vertex* predecessor() const { return m_predecessor; }

        // Adds an outgoing edge to the vertex, transfers ownership
        void addEdge( const Edge* edge );
        // Finds the lightest(!) edge pointing to the given vertex, if any (0 if not found)
        const Edge* findEdge( const Vertex* vertex ) const;

        // This method is called when we need to relax all "our" edges.
        // We need to pass the queue as we have to notify it about key changes - ugly :(
        void relaxVertices( PriorityQueue<Vertex>& queue );

        // debugging
        void dump( const QCString& indent ) const;

    private:
        Vertex( const Vertex& rhs );
        Vertex& operator=( const Vertex& rhs );

        QPtrList<Edge> m_edges;
        const Vertex* m_predecessor;
        QCString m_mimeType;
        unsigned int m_weight; // "key" inside the queue
        int m_index; // position inside the queue, needed for a fast keyDecreased()

        class Private;
        Private* d;
    };


    class Graph
    {

    public:
        Graph( const QCString& from );
        ~Graph() {}

        bool isValid() const { return m_graphValid; }

        QCString sourceMimeType() const { return m_from; }
        void setSourceMimeType( const QCString& from );

        // Creates a chain from "from" to the "to" mimetype
        // If the "to" mimetype isEmpty() then we try to find the
        // closest KOffice mimetype and use that as destination.
        // After such a search "to" will contain the dest. mimetype (return value)
        KoFilterChain::Ptr chain( const KoFilterManager* manager, QCString& to ) const;

        // debugging
        void dump() const;

    private:
        Graph( const Graph& rhs );
        Graph& operator=( const Graph& rhs );

        void buildGraph();
        void shortestPaths();
        QCString findKOfficePart() const;

        QAsciiDict<Vertex> m_vertices;
        QCString m_from;
        bool m_graphValid;

        class Private;
        Private* d;
    };

} // namespace KOffice

#endif // __koffice_filter_chain_h__
