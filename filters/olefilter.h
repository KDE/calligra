#ifndef OLEFILTER_H
#define OLEFILTER_H

#include <qobject.h>
#include <wordfilter.h>
#include <myfile.h>

class OLEFilter : public QObject {

    Q_OBJECT

public:
    OLEFilter(myFile in);
    ~OLEFilter();

    bool filter();            // manages the filtering process
    bool store();             // will soon use KTar to store the
                              // decoded file(s)

private:
    //WordFilter *wordFilter;
};
#endif // OLEFILTER_H
