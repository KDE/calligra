#ifndef KIMAGE_FACTORY_H
#define KIMAGE_FACTORY_H

#include <klibloader.h>

class KInstance;

class KImageFactory : public KLibFactory
{
    Q_OBJECT
public:
    KImageFactory( QObject* parent = 0, const char* name = 0 );
    ~KImageFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject", const QStringList &args = QStringList() );

    static KInstance* global();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();

private:
    static KInstance* s_global;
};

#endif
