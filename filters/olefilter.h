#ifndef OLEFILTER_H
#define OLEFILTER_H

#include <qobject.h>
#include <klaola.h>
#include <wordfilter.h>
#include <myfile.h>

class OLEFilter : public QObject {

    Q_OBJECT

public:

    enum Document { Word, Excel, Powerpoint };

    OLEFilter(const myFile &in, const Document d);
    ~OLEFilter();

    bool filter();            // manages the filtering process
    bool store();             // will soon use KTar to store the
                              // decoded file(s)

private:
    void parseFile();

    myFile file;
    Document document;
    KLaola *docfile;
    //WordFilter *wordFilter;
};
#endif // OLEFILTER_H
