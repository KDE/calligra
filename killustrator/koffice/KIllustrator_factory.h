#ifndef KILLUSTRATOR_FACTORY_H
#define KILLUSTRATOR_FACTORY_H

#include <klibloader.h>

class KInstance;

class KIllustratorFactory : public KLibFactory
{
    Q_OBJECT
public:
    KIllustratorFactory( QObject* parent = 0, const char* name = 0 );
    ~KIllustratorFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* name = "QObject", const QStringList &args = QStringList() );

    static KInstance* global();

private:
    static KInstance* s_global;
};

#endif
