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

#include <klaola.h>
#include <kdebug.h>

const int KLaola::s_area = 30510;

KLaola::KLaola(const myFile &file) {

    smallBlockDepot=0L;
    bigBlockDepot=0L;
    smallBlockFile=0L;
    bbd_list=0L;
    ok=true;
    m_nodeList.setAutoDelete(true);
    //m_currentPath.setAutoDelete(true);

    if( (file.length % 0x200) != 0 ) {
        kdError(s_area) << "KLaola::KLaola(): Invalid file size!" << endl;
        ok=false;
    }
    if(ok) {
        m_file=file;
        maxblock = file.length / 0x200 - 2;
        maxSblock=0;  // will be set in readSmallBlockDepot

        if(!parseHeader())
            ok=false;
        if(ok) {
            readBigBlockDepot();
            readSmallBlockDepot();
            readSmallBlockFile();
            readRootList();
        }
    }
    m_currentPath.clear();
    testIt();

    // current path=root dirHandle
    m_currentPath.clear();
    if ( !m_nodeTree.isEmpty() )
        m_currentPath.append(m_nodeTree.getFirst()->getFirst()->node);
}

KLaola::~KLaola() {

    delete [] bigBlockDepot;
    bigBlockDepot=0L;
    delete [] smallBlockDepot;
    smallBlockDepot=0L;
    delete [] smallBlockFile;
    smallBlockFile=0L;
    delete [] bbd_list;
    bbd_list=0L;
}

// Comvert the given list of nodes into a tree.
void KLaola::createTree(const int handle, const short index) {

    Node *node = dynamic_cast<Node *>(m_nodeList.at(handle));
    SubTree *subtree;

    TreeNode *tree=new TreeNode;
    tree->node=node;
    tree->subtree=-1;

    //QString nix="### entering create tree: handle=";
    //nix+=QString::number(handle);
    //nix+=" index=";
    //nix+=QString::number(index);
    //kdDebug(s_area) << nix << endl;

    if(node->prevHandle!=-1) {
        //kdDebug(s_area) << "create tree: prevHandle" << endl;
        createTree(node->prevHandle, index);
    }
    if(node->dirHandle!=-1) {
        subtree=new SubTree;
        subtree->setAutoDelete(true);
        m_nodeTree.append(subtree);
        tree->subtree=m_nodeTree.at();
        //kdDebug(s_area) << "create tree: dirHandle" << endl;
        createTree(node->dirHandle, tree->subtree);
    }
    subtree=m_nodeTree.at(index);
    //kdDebug(s_area) << "create tree: APPEND " << handle << " tree node " << tree << endl;
    subtree->append(tree);
    if(node->nextHandle!=-1) {
        //kdDebug(s_area) << "create tree: nextHandle" << endl;
        createTree(node->nextHandle, index);
    }
}

const KLaola::NodeList KLaola::currentPath() const {
    return m_currentPath;
}

bool KLaola::enterDir(const OLENode *dirHandle) {

    NodeList nodes;
    Node *node;

    if(ok) {
        nodes = parseCurrentDir();
        for (node = dynamic_cast<Node *>(nodes.first()); node; node = dynamic_cast<Node *>(nodes.next()))
        {
            if(node->m_handle==dirHandle->handle() && node->isDirectory() && !node->deadDir) {
                m_currentPath.append(node);
                return true;
            }
        }
    }
    return false;
}

const KLaola::NodeList KLaola::find(const QString &name, bool onlyCurrentDir) {

    OLENode *node;
    NodeList ret;
    int i=0;

    if(ok) {
        if(!onlyCurrentDir) {
            for(node=m_nodeList.first(); node; node=m_nodeList.next()) {
                if(node->name()==name) {
                    ret.append(node);
                    ++i;
                }
            }
        }
        else {
            NodeList list=parseCurrentDir();

            for(node=list.first(); node; node=list.next()) {
                if(node->name()==name) {
                    ret.append(node);
                    ++i;
                }
            }
        }
    }
    return ret;
}

bool KLaola::leaveDir() {

    if (ok) {
        return m_currentPath.removeLast();
    }
    return false;
}

int KLaola::nextBigBlock(int pos) const
{

    int x=pos*4;
    return ( (bigBlockDepot[x+3] << 24) + (bigBlockDepot[x+2] << 16) +
             (bigBlockDepot[x+1] << 8) + bigBlockDepot[x] );
}

int KLaola::nextSmallBlock(int pos) const
{

    if(smallBlockDepot) {
        int x=pos*4;
        return ( (smallBlockDepot[x+3] << 24) + (smallBlockDepot[x+2] << 16) +
                 (smallBlockDepot[x+1] << 8) + smallBlockDepot[x] );
    }
    else
        return -2;   // Emergency Break :)
}

bool KLaola::parseHeader() {

    if(qstrncmp((const char*)m_file.data,"\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1",8 )!=0) {
        kdError(s_area) << "KLaola::parseHeader(): Invalid file format (unexpected id in header)!" << endl;
        return false;
    }

    num_of_bbd_blocks=read32(0x2c);
    root_startblock=read32(0x30);
    sbd_startblock=read32(0x3c);
    bbd_list=new unsigned int[num_of_bbd_blocks];

    unsigned int i, j;
    for(i=0, j=0; i<num_of_bbd_blocks; ++i, j=j+4)
        bbd_list[i]=read32(0x4c+j);
    return true;
}

KLaola::NodeList KLaola::parseCurrentDir() {

    Node *node;
    SubTree *subtree;
    NodeList nodeList;
    TreeNode *treeNode;
    unsigned int i;
    bool found;

    if(ok) {
        for(i=0, subtree=m_nodeTree.first(); i<m_currentPath.count(); ++i) {
            treeNode=subtree->first();
            found=false;
            do {
                if(treeNode==0) {
                    kdError(s_area) << "KLaola::parseCurrentDir(): path seems to be corrupted!" << endl;
                    ok=false;
                }
                else if(treeNode->node->handle()==m_currentPath.at(i)->handle() && treeNode->subtree!=-1) {
                    found=true;
                }
                else
                    treeNode=subtree->next();
            } while(!found && ok);
            subtree=m_nodeTree.at(treeNode->subtree);
        }
    }
    if(ok) {
        for(treeNode=subtree->first(); treeNode!=0; treeNode=subtree->next()) {
            node=new Node(*treeNode->node);
            node->deadDir = (node->dirHandle==-1 && node->isDirectory());
            // this is a strange situation :)
            if (node->deadDir)
                kdDebug(s_area) << "ignoring: " << node->describe() << " is empty" << endl;
            else
                nodeList.append(node);
        }
    }
    return nodeList;
}

KLaola::NodeList KLaola::parseRootDir() {

    NodeList tmpNodeList;
    NodeList tmp;

    if(ok) {
        tmp=m_currentPath;
        m_currentPath.clear();      // current path=root dirHandle
        m_currentPath.append(m_nodeTree.getFirst()->getFirst()->node);
        tmpNodeList=parseCurrentDir();
        m_currentPath=tmp;
    }
    return tmpNodeList;
}

unsigned char KLaola::read8(int i) const
{
    return m_file.data[i];
}

unsigned short KLaola::read16(int i) const
{
    return ( (m_file.data[i+1] << 8) + m_file.data[i] );
}

unsigned int KLaola::read32(int i) const
{
    return ( (read16(i+2) << 16) + read16(i) );
}

const unsigned char *KLaola::readBBStream(int start, bool setmaxSblock)
{

    int i=0, tmp;
    unsigned char *p=0;

    tmp=start;
    while(tmp!=-2 && tmp>=0 && tmp<=static_cast<int>(maxblock)) {
        ++i;
        tmp=nextBigBlock(tmp);
    }
    if(i!=0) {
        p=new unsigned char[i*0x200];
        if(setmaxSblock)
            maxSblock=i*8-1;
        i=0;
        tmp=start;
        while(tmp!=-2 && tmp>=0 && tmp<=static_cast<int>(maxblock)) {
            memcpy(&p[i*0x200], &m_file.data[(tmp+1)*0x200], 0x200);
            tmp=nextBigBlock(tmp);
            ++i;
        }
    }
    return p;
}

const unsigned char *KLaola::readSBStream(int start) const {

    int i=0, tmp;
    unsigned char *p=0;

    tmp=start;
    while(tmp!=-2 && tmp>=0 && tmp<=static_cast<int>(maxSblock)) {
        ++i;
        tmp=nextSmallBlock(tmp);
    }
    if(i!=0) {
        p=new unsigned char[i*0x40];
        i=0;
        tmp=start;
        while(tmp!=-2 && tmp>=0 && tmp<=static_cast<int>(maxSblock)) {
            memcpy(&p[i*0x40], &smallBlockFile[tmp*0x40], 0x40);
            tmp=nextSmallBlock(tmp);
            ++i;
        }
    }
    return p;
}

void KLaola::readBigBlockDepot() {

    bigBlockDepot=new unsigned char[0x200*num_of_bbd_blocks];
    for(unsigned int i=0; i<num_of_bbd_blocks; ++i)
        memcpy(&bigBlockDepot[i*0x200], &m_file.data[(bbd_list[i]+1)*0x200], 0x200);
}

void KLaola::readSmallBlockDepot() {
    smallBlockDepot=const_cast<unsigned char*>(readBBStream(sbd_startblock));
}

void KLaola::readSmallBlockFile() {
    smallBlockFile=const_cast<unsigned char*>(readBBStream( read32( (root_startblock+1)*0x200 + 0x74), true));
}

void KLaola::readRootList() {

    int pos=root_startblock;
    int handle=0;

    while(pos!=-2 && pos>=0 && pos<=static_cast<int>(maxblock)) {
        for(int i=0; i<4; ++i, ++handle)
            readPPSEntry((pos+1)*0x200+0x80*i, handle);
        pos=nextBigBlock(pos);
    }
    SubTree *subtree=new SubTree;
    subtree->setAutoDelete(true);
    m_nodeTree.append(subtree);

    createTree(0, 0);           // build the tree with a recursive method :)
}

// Add the given OLE node to the list of nodes.
void KLaola::readPPSEntry(int pos, const int handle) {

    int nameSize = read16(pos + 0x40);

    // Does the PPS Entry seem to be valid?

    if (nameSize)
    {
        int i;
        Node *node = new Node(this);

        // The first character of the name can be a prefix.
        node->m_prefix = static_cast<Prefix>(read16(pos));
        if (node->m_prefix <= RESERVED_LAST)
        {
            i = 1;
        }
        else
        {
            node->m_prefix = NONE;
            i = 0;
        }

        // Get the rest of the name.
        for (; i < (nameSize / 2) - 1; ++i)
        {
            QChar tmp;

            tmp = read16(pos + 2 * i);
            node->m_name += tmp;
        }
        node->m_handle = handle;
        node->type = static_cast<NodeType>(read8(pos + 0x42));
        node->prevHandle = static_cast<int>(read32(pos + 0x44));
        node->nextHandle = static_cast<int>(read32(pos + 0x48));
        node->dirHandle = static_cast<int>(read32(pos + 0x4C));
        node->ts1s = static_cast<int>(read32(pos + 0x64));
        node->ts1d = static_cast<int>(read32(pos + 0x68));
        node->ts2s = static_cast<int>(read32(pos + 0x6C));
        node->ts2d = static_cast<int>(read32(pos + 0x70));
        node->sb = read32(pos + 0x74);
        node->size = read32(pos + 0x78);
        node->deadDir = false;
        m_nodeList.append(node);
    }
}

myFile KLaola::stream(const OLENode *node) {

    const Node *realNode = dynamic_cast<const Node *>(node);
    const unsigned char *temp;
    myFile ret;

    if(ok) {
        if(realNode->size>=0x1000)
            temp = readBBStream(realNode->sb);
        else
            temp = readSBStream(realNode->sb);
        ret.setRawData(temp, realNode->size);
    }
    return ret;
}

myFile KLaola::stream(unsigned handle) {

    OLENode *node;

    node = m_nodeList.at(handle);
    return stream(node);
}

void KLaola::testIt(QString prefix)
{

    NodeList nodes;
    OLENode *node;

    nodes = parseCurrentDir();
    for (node = nodes.first(); node; node = nodes.next())
    {
        kdDebug(s_area) << prefix + node->describe() << endl;
        if (node->isDirectory())
        {
            enterDir(node);
            testIt(prefix + " ");
        }
    }
}

// Return a human-readable description of a stream.
QString KLaola::Node::describe() const
{
    QString description;
    myFile file;
    unsigned i;

    description = QString::number(m_handle) + " " +
                    m_name + "(" +
                    QString::number(sb) + " " +
                    QString::number(size) + " bytes)";
    if (isDirectory())
        description += ", directory";
    switch (m_prefix)
    {
    case OLE_MANAGED_0:
        description += ", OLE_0";
        break;
    case CLSID:
        description += ", CLSID=";
        description += readClassStream();
        file = m_laola->stream(this);
        description += ", ";
        for (i = 16; i < file.length; i++)
        {
            description += QString::number((file.data[i] >> 4) & 0xf, 16);
            description += QString::number(file.data[i] & 0xf, 16);
        }
        description += ", ";
        for (i = 16; i < file.length; i++)
        {
            QChar tmp = file.data[i];

            if (tmp.isPrint())
                description += tmp;
            else
                description += '.';
        }
        break;
    case OLE_MANAGED_2:
        description += ", OLE_2";
        break;
    case PARENT_MANAGED:
        description += ", parent managed";
        break;
    case STRUCTURED_STORAGE:
        description += ", reserved 0x" + QString::number(m_prefix, 16);
        break;
    case NONE:
        break;
    default:
        description += ", reserved 0x" + QString::number(m_prefix, 16);
        break;
    }
    return description;
}

QString KLaola::Node::name() const
{
    return m_name;
}

// See "Associating Code with Storage" in Inside OLE.
QString KLaola::Node::readClassStream() const
{
    if (isDirectory())
        return QString::null;
    if (m_prefix == CLSID)
    {
        myFile file;
        unsigned i;
        QString clsid;

        // CLSID format is: 00020900-0000-0000-C000-000000000046
        file = m_laola->stream(this);
        for (i = 0; i < 4; i++)
        {
            clsid += QString::number((file.data[i] >> 4) & 0xf, 16);
            clsid += QString::number(file.data[i] & 0xf, 16);
        }
        clsid += '-';
        for (; i < 6; i++)
        {
            clsid += QString::number((file.data[i] >> 4) & 0xf, 16);
            clsid += QString::number(file.data[i] & 0xf, 16);
        }
        clsid += '-';
        for (; i < 8; i++)
        {
            clsid += QString::number((file.data[i] >> 4) & 0xf, 16);
            clsid += QString::number(file.data[i] & 0xf, 16);
        }
        clsid += '-';
        for (; i < 10; i++)
        {
            clsid += QString::number((file.data[i] >> 4) & 0xf, 16);
            clsid += QString::number(file.data[i] & 0xf, 16);
        }
        clsid += '-';
        for (; i < 16; i++)
        {
            clsid += QString::number((file.data[i] >> 4) & 0xf, 16);
            clsid += QString::number(file.data[i] & 0xf, 16);
        }
        return clsid;
    }
    return QString::null;
}
