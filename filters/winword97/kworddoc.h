#ifndef KWORDDOC_H
#define KWORDDOC_H

#include <qstring.h>

class KWordDoc {

public:
    KWordDoc();
    ~KWordDoc();
    const QString kwdFile();

private:
    QString kwd;
    bool success;
};
#endif // KWORDDOC_H
