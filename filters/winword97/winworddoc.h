#ifndef WINWORDDOC_H
#define WINWORDDOC_H

#include <string.h>
#include <qobject.h>
#include <kdebug.h>
#include "klaola.h"
#include "myfile.h"
#include "fib.h"


class WinWordDoc : public QObject {

    Q_OBJECT

public:
    WinWordDoc(const myFile &mainStream, const myFile &tableStream,
               const myFile &dataStream);
    ~WinWordDoc();

signals:
    void signalFilterError();

public slots:
    void slotFilterError();
    
private:
    bool success;
    FIB *fib;
    unsigned char *main, *table;
};
#endif // WINWORDDOC_H
