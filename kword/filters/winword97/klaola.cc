/*
 * KLoala
 *
 * based on "LAOLA file system"
 *          "Structured Storage"
 *          (c) 1996, 1997 by martin schwartz@cs.tu-berlin.de
 */

#include "klaola.h"

KLaola::KLaola(myFile file) {
    
    smallBlockDepot=0L;
    bigBlockDepot=0L;
    smallBlockFile=0L;

    if( (file.length % 0x200) != 0 ) {
        kdebug(KDEBUG_ERROR, 31000, "Invalid file size!");
        exit(-1);
    }
    
    data=file.data;
    maxblock = file.length / 0x200 - 2;
    
    if(!parseHeader())
        exit(-1);
    readBigBlockDepot();
    readSmallBlockDepot();
    readSmallBlockFile();
    readRootList();
}

KLaola::~KLaola() {

    if(bigBlockDepot) {
        delete [] bigBlockDepot;
        bigBlockDepot=0L;
    }    
    if(smallBlockDepot) {
        delete [] smallBlockDepot;
        smallBlockDepot=0L;
    }
    if(smallBlockFile) {
        delete [] smallBlockFile;
        smallBlockFile=0L;
    }
}

//QList<OLENode> KLaola::parseRootDir() {

    /*OLENode *node;
    QList<OLENode> nodeList;
    OLETree *tmp;

    tmp=tree.first();
    //tmp=tmp.subtree.first();
    */
//}

//QList<OLENode> KLaola::parseCurrentDir() {
//}

//bool KLaola::enterDir(long handle) {
//}

//bool KLaola::leaveDir() {
//}

//QList<long> KLaola::currentPath() {
//}

//OLEInfo *KLaola::streamInfo(long handle) {

/*    OLEInfo *tmp, ret;
    bool found=false;

    tmp=ppsList.first();

    do {
        if(strcmp(tmp->name, (const char*)file)==0) {
            found=true;
            memcpy(&ret, tmp, sizeof(OLEInfo));
        }
        else
            tmp=ppsList.next();
    } while(tmp!=0 && !found);

    return ret;
*/
//}

//QString &KLaola::stream(long handle) {

/*    OLEInfo *info;

    info=streamInfo(dir, file);
    ret.size=info.size;

    if(info.size>=0x1000)
        ret.stream=(char*)readBBStream(info.sb);
    else
        ret.stream=(char*)readSBStream(info.sb);

    return ret;
*/
//}

void KLaola::testIt() {
}

bool KLaola::parseHeader() {

    if(qstrncmp((const char*)data,"\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1",8 )!=0) {
        kdebug(KDEBUG_ERROR, 31000, 
               "Invalid file format (unexpected id in header)!");
        return false;
    }

    num_of_bbd_blocks=read32(0x2c);
    root_startblock=read32(0x30);
    sbd_startblock=read32(0x3c);

    // QString foo;
    // foo.setNum(root_startblock);
    // kdebug(KDEBUG_INFO, 31000, (const char*)foo);

    for(unsigned int i=0;i<num_of_bbd_blocks;++i)
        bbd_list[i]=read32(0x4c+i);

    return true;
}

void KLaola::readBigBlockDepot() {

    bigBlockDepot=new unsigned char[0x200*num_of_bbd_blocks];
    
    for(unsigned int i=0;i<num_of_bbd_blocks;++i) 
        memcpy(&bigBlockDepot[i*0x200], &data[(bbd_list[i]+1)*0x200], 0x200);
}

void KLaola::readSmallBlockDepot() {
    smallBlockDepot=readBBStream(sbd_startblock);
}

void KLaola::readSmallBlockFile() {    
    smallBlockFile=readBBStream( read32( (root_startblock+1)*0x200 + 0x74) );
}

void KLaola::readRootList() {

    long pos=root_startblock;
    long handle=-1;

    while(pos!=-2 && pos>=0 && pos<=(long)maxblock) {
        for(int i=0;i<4;++i, ++handle)
            readPPSEntry((pos+1)*0x200+0x80*i, handle);

        pos=nextBigBlock(pos);
    }

    createTree(tree, 0);   // build the tree with a recursive method :)

    path.append(0);    // current dir == root dir

}

void KLaola::readPPSEntry(long pos, long handle) {

    OLEInfo *info=new OLEInfo;

    info->handle=handle;
    info->nameSize=read16(pos+0x40);

    if(info->nameSize!=0) {      // PPS Entry seems to be OK

        for(int i=0;i<(info->nameSize/2);++i)
            info->name+=data[pos+2*i];

        info->type=data[pos+0x42];

        info->prev=(long)read32(pos+0x44);
        info->next=(long)read32(pos+0x48);
        info->dir=(long)read32(pos+0x4C);

        info->ts1s=(long)read32(pos+0x64);
        info->ts1d=(long)read32(pos+0x68);
        info->ts2s=(long)read32(pos+0x6C);
        info->ts2d=(long)read32(pos+0x70);

        info->sb=(long)read32(pos+0x74);
        info->size=(long)read32(pos+0x78);

        ppsList.append(info);
    }
}

void KLaola::createTree(QList<OLETree> &subtree, long handle) {

    OLEInfo *info=ppsList.take(handle);

    if(info->prev!=-1)
        createTree(subtree, info->prev);

    OLETree *node=new OLETree;
    node->handle=handle;
    subtree.append(node);

    if(info->dir!=-1)
        createTree(node->subtree, info->dir);

    if(info->next!=-1)
        createTree(subtree, info->next);
}

unsigned char *KLaola::readBBStream(long start) {

    long i=0, tmp;
    unsigned char *p=0;
    QString foo;

    tmp=start;

    kdebug(KDEBUG_INFO, 31000, "BB-in");
    foo.setNum(tmp);
    kdebug(KDEBUG_INFO, 31000, (const char*)foo);
    
    while(tmp!=-2 && tmp>=0 && tmp<=(long)maxblock) {
        ++i;
        tmp=nextBigBlock(tmp);
        foo.setNum(tmp);
        kdebug(KDEBUG_INFO, 31000, (const char*)foo);
    }

    if(i!=0) {
        p=new unsigned char[i*0x200];
        
        i=0;
        tmp=start;

        while(tmp!=-2 && tmp>=0 && tmp<=(long)maxblock) {
            memcpy(&p[i*0x200], &data[(tmp+1)*0x200], 0x200);
            tmp=nextBigBlock(tmp);
            ++i;
        }
    }
    kdebug(KDEBUG_INFO, 31000, "BB-out");
    return p;
}

unsigned char *KLaola::readSBStream(long start) {

    long i=0, tmp;
    unsigned char *p=0;
    QString foo;

    tmp=start;

    kdebug(KDEBUG_INFO, 31000, "SB-in");
    foo.setNum(tmp);
    kdebug(KDEBUG_INFO, 31000, (const char*)foo);

    while(tmp!=-2 && tmp>=0 && tmp<=(long)maxblock) {
        ++i;
        tmp=nextSmallBlock(tmp);
        foo.setNum(tmp);
        kdebug(KDEBUG_INFO, 31000, (const char*)foo);
    }

    if(i!=0) {
        p=new unsigned char[i*0x40];

        i=0;
        tmp=start;

        while(tmp!=-2 && tmp>=0 && tmp<=(long)maxblock) {
            memcpy(&p[i*0x40], &smallBlockFile[tmp*0x40], 0x40);
            tmp=nextSmallBlock(tmp);
            ++i;
        }
    }
    kdebug(KDEBUG_INFO, 31000, "SB-out");
    return p;
}

inline long KLaola::nextBigBlock(long pos) {

    long x=pos*4;
    return ( (bigBlockDepot[x+3] << 24) + (bigBlockDepot[x+2] << 16) +
             (bigBlockDepot[x+1] << 8) + bigBlockDepot[x] );
}

inline long KLaola::nextSmallBlock(long pos) {

    if(smallBlockDepot) {
        long x=pos*4;
        return ( (smallBlockDepot[x+3] << 24) + (smallBlockDepot[x+2] << 16) +
             (smallBlockDepot[x+1] << 8) + smallBlockDepot[x] );
    }
    else
        return -2;   // Emergency Break :)
}

inline unsigned short KLaola::read16(int i) {
    return ( (data[i+1] << 8) + data[i] );
}

inline unsigned long KLaola::read32(int i) {
    return ( (read16(i+2) << 16) + read16(i) );
}
