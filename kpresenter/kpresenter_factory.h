#ifndef KPRESENTER_FACTORY_H
#define KPRESENTER_FACTORY_H

#include <klibloader.h>

class KPresenterFactory : public KLibFactory
{
    Q_OBJECT
public:
    KPresenterFactory( QObject* parent = 0, const char* name = 0 );
    ~KPresenterFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject", const QStringList &args = QStringList() );

    static KInstance* global();

private:
    static KInstance* s_global;
};

#endif
