/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <trobin@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

DESCRIPTION

    This class is used to decode OLE 2 streams. When instantiated, it
    constructs an internal "filesystem" that corresponds to the OLE storage
    tree. This tree can be navigated, and the individual OLE streams
    returned as a linear memory buffer.
*/

#ifndef KLAOLA_H
#define KLAOLA_H

#include <myfile.h>
#include <qstring.h>
#include <qptrlist.h>

class KLaola {

public:
    KLaola(const myFile &file);               // see myfile.h!
    ~KLaola();

    bool isOk() {return ok;}

    // A class representing an abstracted node in the OLE filesystem.

    class OLENode {
    public:
        virtual ~OLENode() {};
        virtual unsigned handle() const = 0;
        virtual QString name() const = 0;

        // Does the node represent a stream datum, or a storage container
        // of data?
        virtual bool isDirectory() const = 0;

        // If isDirectory() is true, return the CLSID associated with
        // any child CompObj node. If the Node is a CompObj, return
        // its CLSID. Otherwise return QString::null.
        //
        // The CLSID is returned in the form:
        //
        //  00020900-0000-0000-C000-000000000046
        //
        virtual QString readClassStream() const = 0;

        // Return a human-readable description of a stream.
        virtual QString describe() const = 0;
    protected:
        OLENode() {}
    };

    // Wade through the "file system"

    typedef QPtrList<OLENode> NodeList;
    NodeList parseRootDir();
    NodeList parseCurrentDir();
    const NodeList currentPath() const;
    const NodeList find(const QString &name, bool onlyCurrentDir=false);
    bool enterDir(const OLENode *node);
    bool leaveDir();

    // Return the stream for a given node.
    //
    // Note: data - 512 byte blocks, but length is set correctly :)
    myFile stream(const OLENode *node);
    myFile stream(unsigned handle);

private:
    KLaola(const KLaola &);
    const KLaola &operator=(const KLaola &);
    static const int s_area;

    unsigned char read8(int i) const;
    unsigned short read16(int i) const;
    unsigned int read32(int i) const;

    // Parsing functions.
    bool parseHeader();
    void readBigBlockDepot();
    void readSmallBlockDepot();
    void readSmallBlockFile();
    void readRootList();
    void readPPSEntry(int pos, const int handle);
    void createTree(const int handle, const short index);
    const unsigned char *readBBStream(int start, bool setmaxSblock=false);
    const unsigned char *readSBStream(int start) const;
    int nextBigBlock(int pos) const;
    int nextSmallBlock(int pos) const;

    // Dump the parsed structure info (similar to "lls"
    // of the LAOLA-project).
    void testIt(QString prefix = "");

public:
    typedef enum
    {
        DIRECTORY = 1,
        FILE = 2,
        ROOT_ENTRY = 5
    } NodeType;

    // If the first part of an on-disk name is less than 32, it is a prefix.
    typedef enum
    {
        OLE_MANAGED_0,
        CLSID,
        OLE_MANAGED_2,
        PARENT_MANAGED,         // Marks an element as owned by the code that
                                // manages the parent storage of that element.
        STRUCTURED_STORAGE,     // For the exclusive use of the Structured Storage
                                // implementation.
        RESERVED_FIRST,
        RESERVED_LAST = 31,
        NONE = 32
    } Prefix;

    class Node: public OLENode {
    public:
        Node(KLaola *laola) { m_laola = laola; }
        ~Node() {}
        unsigned handle() const { return  m_handle; }
        QString name() const;
        bool isDirectory() const { return (type == DIRECTORY) || (type == ROOT_ENTRY); }
        QString readClassStream() const;
        QString describe() const;

        KLaola *m_laola;
        unsigned m_handle;       // PPS entry number
        Prefix m_prefix;
        QString m_name;
        NodeType type;
        int prevHandle;   // Last pps
        int nextHandle;   // Next pps
        int dirHandle;    // Dir pps
        int ts1s;         // Timestamp 1, seconds
        int ts1d;         // Timestamp 1, days
        int ts2s;         // Timestamp 2, seconds
        int ts2d;         // Timestamp 2, days
        unsigned sb;      // Starting block
        unsigned size;    // Size of property
        bool deadDir;     // true, if the dir is a "dead end"
    };

private:
    // Lists of nodes.

    NodeList m_nodeList;
    NodeList m_currentPath;

    // The OLE storage is represented as a tree. Each node in the tree may
    // refer to a subtree. Each subtree is stored as a list of nodes.

    struct TreeNode
    {
        Node *node;
        short subtree;
    };
    typedef QPtrList<TreeNode> SubTree;
    QPtrList<SubTree> m_nodeTree;

    bool ok;        // is the file OK?

    myFile m_file;
    unsigned char *bigBlockDepot;
    unsigned char *smallBlockDepot;
    unsigned char *smallBlockFile;

    unsigned int maxblock;          // maximum number of big-blocks
    unsigned int maxSblock;         //                   small-blocks
    unsigned int num_of_bbd_blocks; // number of big block depot blocks
    unsigned int root_startblock;   // Root chain's first big block
    unsigned int sbd_startblock;    // small block depot's first big block
    unsigned int *bbd_list;         //array of num_of_bbd_blocks big block numbers
};
#endif // KLAOLA_H
