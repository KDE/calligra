#ifndef KDCHARTWRAPPERFACTORY_H
#define KDCHARTWRAPPERFACTORY_H
#include <qswrapperfactory.h>

class KDChartWrapperFactory :public QSWrapperFactory
{
public:
    KDChartWrapperFactory();
    virtual QObject* create ( const QString& className, void* ptr );
};

#endif /* KDCHARTWRAPPERFACTORY_H */

