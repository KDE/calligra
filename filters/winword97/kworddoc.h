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
    KWordDoc(const KWordDoc &);
    const KWordDoc &operator=(const KWordDoc &);

    QString kwd;
    bool success;
};
#endif // KWORDDOC_H
