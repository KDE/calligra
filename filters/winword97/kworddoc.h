#ifndef KWORDDOC_H
#define KWORDDOC_H

#include <qobject.h>
#include <qstring.h>

class KWordDoc : public QObject {

    Q_OBJECT

public:
    KWordDoc();
    ~KWordDoc();
    const QString kwdFile();

signals:
    void signalFilterError();

public slots:
    void slotFilterError();
    
private:
    QString kwd;
    bool success;
};
#endif // KWORDDOC_H
