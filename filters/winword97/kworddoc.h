#ifndef KWORDDOC_H
#define KWORDDOC_H

#include <qstring.h>

class KWordDoc {

public:
    KWordDoc();
    ~KWordDoc();

    void part(const QString &part);

    const QString kwdFile();
    const bool isOk() const { return success; }

private:
    KWordDoc(const KWordDoc &);
    const KWordDoc &operator=(const KWordDoc &);

    QString kwd;
    bool success;
    bool ready;
};
#endif // KWORDDOC_H
