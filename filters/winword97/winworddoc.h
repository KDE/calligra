#ifndef WINWORDDOC_H
#define WINWORDDOC_H

#include <string.h>
#include <qobject.h>
#include <qstring.h>
#include <kdebug.h>
#include "klaola.h"
#include "myfile.h"
#include "fib.h"


class WinWordDoc : public QObject {

    Q_OBJECT

public:
    WinWordDoc(myFile in);
    ~WinWordDoc();

signals:
    void signalFilterError();

public slots:
    void slotFilterError();
    
private:
    bool getMainStream(KLaola *laola);

    myFile doc;     // raw data
    bool success;
    FIB *fib;
    unsigned char *mainStream;
};
#endif // WINWORDDOC_H
