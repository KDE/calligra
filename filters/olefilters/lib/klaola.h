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
*/

// KLaola is the class which is used to decode the OLE 2 streams.

#ifndef KLAOLA_H
#define KLAOLA_H

#include <string.h>
#include <qstring.h>
#include <qarray.h>
#include <qlist.h>
#include <kdebug.h>
#include <myfile.h>

class KLaola {

public:
    KLaola(const myFile &file);               // see myfile.h!
    ~KLaola();

    bool isOk() {return ok;}

    // A little bit of OLE-Information. If you want to navigate through
    // the "filesystem" you have to use this struct.

    class OLENode {
    public:
        virtual ~OLENode() {};
        virtual unsigned handle() const = 0;
        virtual QString name() const = 0;
        virtual bool isDirectory() const = 0;
        // Return a human-readable description of a stream.
        virtual QString describe() const = 0;
    protected:
        OLENode() {}
    };

    typedef QList<OLENode> NodeList;
    NodeList parseRootDir();
    NodeList parseCurrentDir();

    // Wade through the "file system"
    bool enterDir(const OLENode *node);
    bool leaveDir();
    const NodeList currentPath() const;

    // Return the stream for a given node.
    //
    // Note: data - 512 byte blocks, but length is set correctly :)
    // Make sure that you delete [] the data!
    myFile stream(const OLENode *node);
    myFile stream(int handle);
    const NodeList find(const QString &name, const bool onlyCurrentDir=false);

private:
    KLaola(const KLaola &);
    const KLaola &operator=(const KLaola &);
    static const int s_area = 30510;

    unsigned char read8(int i) const;
    unsigned short read16(int i) const;
    unsigned int read32(int i) const;

    bool parseHeader();
    void readBigBlockDepot();
    void readSmallBlockDepot();
    void readSmallBlockFile();
    void readRootList();
    void readPPSEntry(int pos, const int handle);
    void createTree(const int handle, const short index);
    const unsigned char *readBBStream(int start, const bool setmaxSblock=false);
    const unsigned char *readSBStream(int start) const;
    int nextBigBlock(int pos) const;
    int nextSmallBlock(int pos) const;

    // dump some info (similar to "lls"
    // of the LAOLA-project)
    void testIt(QString prefix = "");

    typedef enum
    {
        DIRECTORY = 1,
        FILE = 2,
        ROOT_ENTRY = 5
    } NodeType;

    class Node: public OLENode {
    public:
        ~Node() {}
        unsigned handle() const { return  m_handle; }
        QString name() const { return m_name; }
        bool isDirectory() const { return (type == DIRECTORY) || (type == ROOT_ENTRY); }
        QString describe() const;

        unsigned m_handle;       // PPS entry number
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

    // The OLE storage is represented as a tree. Each node in the tree may
    // refer to a subtree. Each subtree is stored asa list of nodes.

    struct TreeNode
    {
        Node *node;
        short subtree;
    };
    typedef QList<TreeNode> SubTree;
    QList<SubTree> m_tree;

    NodeList ppsList;
    NodeList m_currentPath;

    bool ok;        // is the file OK?

    unsigned char *data;
    unsigned char *bigBlockDepot;
    unsigned char *smallBlockDepot;
    unsigned char *smallBlockFile;

    unsigned int maxblock;          // maximum number of big-blocks
    unsigned int maxSblock;         //                   small-blocks
    unsigned int num_of_bbd_blocks; // number of big block depot blocks
    unsigned int root_startblock;   // Root chain's first big block
    unsigned int sbd_startblock;    // small block depot's first big block
    unsigned int *bbd_list;  //array of num_of_bbd_blocks big block numbers
};
#endif // KLAOLA_H
