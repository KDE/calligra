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
    WordFilter(myFile in);
    ~WordFilter();
    bool filterIt();
    QString &kwdFile();

public slots:
    void slotFilterError();
    
private:
    WinWordDoc *myDoc;
    KWordDoc *myKwd;
    bool success;
};
#endif // WORDFILTER_H
