/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <wtrobin@carinthia.com>

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

KLaola::KLaola(const myFile &file) {

    smallBlockDepot=0L;
    bigBlockDepot=0L;
    smallBlockFile=0L;
    bbd_list=0L;
    ok=true;

    if( (file.length % 0x200) != 0 ) {
        kDebugError(31000, "KLaola::KLaola(): Invalid file size!");
        ok=false;
    }
    if(ok) {
        data=file.data;
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

    QList<OLETree> *tmpList;
    OLETree *node;
    for(tmpList=treeList.first(); tmpList!=0; tmpList=treeList.next()) {
        for(node=tmpList->first(); node!=0; node=tmpList->next()) {
            delete node;
            node=0L;
        }
        delete tmpList;
        tmpList=0L;
    }
    OLEInfo *info;
    for(info=ppsList.first(); info!=0; info=ppsList.next()) {
        delete info;
        info=0L;
    }
}

const QList<OLENode> KLaola::parseRootDir() {

    QList<OLENode> tmpOLENodeList;
    QArray<int> tmp;

    if(ok) {
        tmp=path.copy();
        path.resize(1);
        path[0]=0;       // just to be sure...
        tmpOLENodeList=parseCurrentDir();
        path=tmp.copy();
    }
    return tmpOLENodeList;
}

const QList<OLENode> KLaola::parseCurrentDir() {

    OLENode *node;
    QList<OLETree> *tmpList;
    QList<OLENode> nodeList;
    OLETree *tree;
    OLEInfo *info;
    unsigned int i;
    bool found;

    if(ok) {
        for(i=0, tmpList=treeList.first(); i<path.size(); ++i) {
            tree=tmpList->first();
            found=false;
            do {
                if(tree==0) {
                    kDebugError(31000, "KLaola::parseCurrentDir(): path seems to be corrupted!");
                    ok=false;
                }
                else if(tree->handle==path[i] && tree->subtree!=-1)
                    found=true;
                else
                    tree=tmpList->next();
            } while(!found && ok);
            tmpList=treeList.at(tree->subtree);
        }
    }
    if(ok) {
        for(tree=tmpList->first(); tree!=0; tree=tmpList->next()) {
            node=new OLENode;
            info=ppsList.at(tree->handle);
            node->handle=info->handle;
            node->name=info->name;
            node->type=info->type;
            if(info->dir==-1 && info->type==1) {  // this is a strange situation :)
                node->deadDir=true;
                kDebugWarning(31000, "KLaola::parseCurrentDir(): ######## dead dir ahead ########");
            }
            else
                node->deadDir=false;
            nodeList.append(node);
        }
    }
    return nodeList;
}

const bool KLaola::enterDir(const int &handle) {

    QList<OLENode> dir;
    OLENode *node;

    if(ok) {
        dir=parseCurrentDir();
        node=dir.first();
        while(node!=0) {
            if(node->handle==handle && node->type==1 && !node->deadDir) {
                path.resize(path.size()+1);
                path[path.size()-1]=node->handle;
                return true;
            }
            node=dir.next();
        }
    }
    return false;
}

const bool KLaola::leaveDir() {

    if(ok) {
        if(path.size()>1) {
            path.resize(path.size()-1);
            return true;
        }
    }
    return false;
}

const QArray<int> KLaola::currentPath() const {
    return path;
}

const OLEInfo KLaola::streamInfo(const int &handle) {

    OLEInfo *tmp, ret;

    if(ok) {
        tmp=ppsList.at(handle);
        if(tmp) {
            ret.handle=tmp->handle;
            ret.name=tmp->name;
            ret.nameSize=tmp->nameSize;
            ret.type=tmp->type;
            ret.prev=tmp->prev;
            ret.next=tmp->next;
            ret.dir=tmp->dir;
            ret.ts1s=tmp->ts1s;
            ret.ts1d=tmp->ts1d;
            ret.ts2s=tmp->ts2s;
            ret.ts2d=tmp->ts2d;
            ret.sb=tmp->sb;
            ret.size=tmp->size;
        }
    }
    return ret;
}

const myFile KLaola::stream(const int &handle) {

    OLEInfo *info;
    myFile ret;

    if(ok) {
        info=ppsList.at(handle);
        if(info) {
            if(info->size>=0x1000)
                ret.data=const_cast<unsigned char*>(readBBStream(info->sb));
            else
                ret.data=const_cast<unsigned char*>(readSBStream(info->sb));
            ret.length=info->size;
        }
    }
    return ret;
}

const QArray<int> KLaola::find(const QString &name, const bool onlyCurrentDir) {

    QArray<int> ret(static_cast<int>(0));
    int i=0;

    if(ok) {
        if(!onlyCurrentDir) {
            for(OLEInfo *p=ppsList.first(); p!=0; p=ppsList.next()) {
                if(p->name==name) {
                    ret.resize(i+1);
                    ret[i]=p->handle;
                    ++i;
                }
            }
        }
        else {
            QList<OLENode> list=parseCurrentDir();
            OLENode *node;

            for(node=list.first(); node!=0; node=list.next()) {
                if(node->name==name) {
                    ret.resize(i+1);
                    ret[i]=node->handle;
                    ++i;
                }
            }
        }
    }
    return ret;
}

void KLaola::testIt() {

    QList<OLENode> dir;
    OLENode *node;
    OLEInfo info;
    QString foo;

    kDebugInfo(31000, "KLaola::testIt() - start -----------");

    if(ok) {
        dir=parseRootDir();
        for(node=dir.first(); node!=0; node=dir.next()) {
            info=streamInfo(node->handle);
            foo.setNum(info.handle);
            foo+="   ";
            foo+=info.name;
            foo+="   ";
            foo+=QString::number(info.sb);
            foo+="   ";
            foo+=QString::number(info.size);
            kDebugInfo(31000, static_cast<const char*>(foo));
        }
    }
    kDebugInfo(31000, "KLaola::testIt() - end -----------");
}

const bool KLaola::parseHeader() {

    if(qstrncmp((const char*)data,"\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1",8 )!=0) {
        kDebugError(31000, "KLaola::parseHeader(): Invalid file format (unexpected id in header)!");
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

void KLaola::readBigBlockDepot() {

    bigBlockDepot=new unsigned char[0x200*num_of_bbd_blocks];
    for(unsigned int i=0; i<num_of_bbd_blocks; ++i)
        memcpy(&bigBlockDepot[i*0x200], &data[(bbd_list[i]+1)*0x200], 0x200);
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
    QList<OLETree> *tmpList=new QList<OLETree>;
    treeList.append(tmpList);

    createTree(0, 0);   // build the tree with a recursive method :)

    path.resize(1);
    path[0]=0;   // current path=root dir
}

void KLaola::readPPSEntry(const int &pos, const int &handle) {

    OLEInfo *info=new OLEInfo;

    info->handle=handle;
    info->nameSize=read16(pos+0x40);
    if(info->nameSize!=0) {      // PPS Entry seems to be valid
        for(int i=0; i<(info->nameSize/2)-1; ++i)
            info->name+=data[pos+2*i];
        // kDebugInfo(31000, "PPS Entry %d #####################", pos);
        // kDebugInfo(31000, info->name);
        info->type=data[pos+0x42];
        // kDebugInfo(31000, QString::number((int)info->type));
        info->prev=static_cast<int>(read32(pos+0x44));
        // kDebugInfo(31000, QString::number((int)info->prev));
        info->next=static_cast<int>(read32(pos+0x48));
        // kDebugInfo(31000, QString::number((int)info->next));
        info->dir=static_cast<int>(read32(pos+0x4C));
        // kDebugInfo(31000, QString::number((int)info->dir));
        info->ts1s=static_cast<int>(read32(pos+0x64));
        // kDebugInfo(31000, QString::number((int)info->ts1s));
        info->ts1d=static_cast<int>(read32(pos+0x68));
        // kDebugInfo(31000, QString::number((int)info->ts1d));
        info->ts2s=static_cast<int>(read32(pos+0x6C));
        // kDebugInfo(31000, QString::number((int)info->ts2s));
        info->ts2d=static_cast<int>(read32(pos+0x70));
        // kDebugInfo(31000, QString::number((int)info->ts2d));
        info->sb=static_cast<int>(read32(pos+0x74));
        // kDebugInfo(31000, QString::number((int)info->sb));
        info->size=static_cast<int>(read32(pos+0x78));
        // kDebugInfo(31000, QString::number((int)info->size));
        // kDebugInfo(31000, "##################################");
        ppsList.append(info);
    }
}

void KLaola::createTree(const int &handle, const short &index) {

    OLEInfo *info=ppsList.at(handle);
    QList<OLETree> *tmpList;

    OLETree *node=new OLETree;
    node->handle=handle;
    node->subtree=-1;

    // QString nix="### entering create tree: handle=";
    // nix+=QString::number(handle);
    // nix+=" index=";
    // nix+=QString::number(index);
    // kDebugInfo(31000, nix);

    if(info->prev!=-1) {
        // kDebugInfo(31000, "create tree: prev");
        createTree(info->prev, index);
    }
    if(info->dir!=-1) {
        tmpList=new QList<OLETree>;
        treeList.append(tmpList);
        node->subtree=treeList.at();
        // kDebugInfo(31000, "create tree: dir");
        createTree(info->dir, node->subtree);
    }
    tmpList=treeList.at(index);
    // kDebugInfo(31000, "create tree: APPEND %d", handle);
    tmpList->append(node);
    if(info->next!=-1) {
        // kDebugInfo(31000, "create tree: next");
        createTree(info->next, index);
    }
}

const unsigned char *KLaola::readBBStream(const int &start, const bool setmaxSblock) {

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
            memcpy(&p[i*0x200], &data[(tmp+1)*0x200], 0x200);
            tmp=nextBigBlock(tmp);
            ++i;
        }
    }
    return p;
}

const unsigned char *KLaola::readSBStream(const int &start) {

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

inline const int KLaola::nextBigBlock(const int &pos) {

    int x=pos*4;
    return ( (bigBlockDepot[x+3] << 24) + (bigBlockDepot[x+2] << 16) +
             (bigBlockDepot[x+1] << 8) + bigBlockDepot[x] );
}

inline const int KLaola::nextSmallBlock(const int &pos) {

    if(smallBlockDepot) {
        int x=pos*4;
        return ( (smallBlockDepot[x+3] << 24) + (smallBlockDepot[x+2] << 16) +
             (smallBlockDepot[x+1] << 8) + smallBlockDepot[x] );
    }
    else
        return -2;   // Emergency Break :)
}

inline const unsigned short KLaola::read16(const int &i) {
    return ( (data[i+1] << 8) + data[i] );
}

inline const unsigned int KLaola::read32(const int &i) {
    return ( (read16(i+2) << 16) + read16(i) );
}
