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

    bool filterIt();

private:
    WordFilter *wordFilter;
};
#endif // OLEFILTER_H
