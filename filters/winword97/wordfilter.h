/******************************************************************/
/* WordFilter - (c) by Werner Trobin 1999                         */
/* Version: 0.0.1                                                 */
/* Author: Werner Trobin                                          */
/* E-Mail: wtrobin@carinthia.com                                  */
/* Homepage: http://members.carinthia.com/wtrobin/                */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Filter (header)                                        */
/******************************************************************/

#ifndef WORDFILTER_H
#define WORDFILTER_H

#include <qobject.h>
#include <kdebug.h>
#include "winworddoc.h"
#include "kworddoc.h"
#include "myfile.h"

class QString;

class WordFilter : public QObject {

    Q_OBJECT

public:
    WordFilter(const QString &mainStream, const QString &tableStream);
    ~WordFilter();
    bool filter();
    QString kwdFile();

public slots:
    void slotFilterError();
    
private:
    WinWordDoc *myDoc;
    KWordDoc *myKwd;
    bool success;
};
#endif // WORDFILTER_H
