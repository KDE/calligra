#ifndef KWORDDOC_H
#define KWORDDOC_H

#include <qstring.h>

class KWordDoc {

public:
    KWordDoc();
    ~KWordDoc();

    const QString kwdFile() const { return kwd; }
    const bool isOk() const { return success; }

private:
    QString kwd;
    bool success;
};
#endif // KWORDDOC_H
