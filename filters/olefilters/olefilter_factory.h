#ifndef OLEFILTER_FACTORY_H
#define OLEFILTER_FACTORY_H

#include <klibloader.h>

class KInstance;

class OLEFilterFactory : public KLibFactory
{
    Q_OBJECT
public:
    OLEFilterFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~OLEFilterFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject", const QStringList &args = QStringList() );

    static KInstance* global();

private:
    static KInstance* s_global;
};
#endif
