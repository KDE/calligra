#ifndef KILLUSTRATOR_FACTORY_H
#define KILLUSTRATOR_FACTORY_H

#include <koFactory.h>

class KInstance;

class KIllustratorFactory : public KoFactory
{
    Q_OBJECT
public:
    KIllustratorFactory( QObject* parent = 0, const char* name = 0 );
    ~KIllustratorFactory();

    virtual QObject* create( QObject* parent = 0, const char* oname = 0, const char* name = "QObject", const QStringList &args = QStringList() );

    static KInstance* global();

private:
    static KInstance* s_global;
};

#define KIBarIcon( x ) BarIcon( x, KIllustratorFactory::global() )

#endif
