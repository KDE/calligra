#include "KPrTransitionFilterRegistry.h"

#include <QString>

#include <kglobal.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoPluginLoader.h>
#include "KPrAnimTransitionFilterFactory.h"
#include <kdebug.h>

class KPrTransitionFilterRegistry::Singleton
{
public:
    Singleton()
    : initDone( false )
    {
    }

    KPrTransitionFilterRegistry q;
    bool initDone;
};

struct KPrTransitionFilterRegistry::Private
{
    QHash<QPair<QString, bool>, KPrAnimTransitionFilterFactory *> tagToFactory;
};

K_GLOBAL_STATIC( KPrTransitionFilterRegistry::Singleton, singleton )

KPrTransitionFilterRegistry * KPrTransitionFilterRegistry::instance()
{
    KPrTransitionFilterRegistry * registry = &( singleton->q );
    if ( ! singleton->initDone ) {
        singleton->initDone = true;
        registry->init();
    }
    return registry;
}

KPrAnimTransitionFilterEffect * KPrTransitionFilterRegistry::createTransitionFilterEffect( const KoXmlElement & element )
{
    Q_UNUSED(element);

    KPrAnimTransitionFilterEffect * shapeEffect = 0;
    if ( element.hasAttributeNS( KoXmlNS::smil, "type" ) ) {
        QString smilType( element.attributeNS( KoXmlNS::smil, "type" ) );
        qDebug() << "Registry smil type: " << smilType;
        bool reverse = false;
        if ( element.hasAttributeNS( KoXmlNS::smil, "direction" ) && element.attributeNS( KoXmlNS::smil, "direction" ) == "reverse" ) {
            reverse = true;
        }

        QHash<QPair<QString, bool>, KPrAnimTransitionFilterFactory *>::iterator it( d->tagToFactory.find( QPair<QString, bool>( smilType, reverse ) ) );

        // call the factory to create the page effect
        if ( it != d->tagToFactory.end() ) {
            qDebug() << "Creating shape effect in registry";
            shapeEffect = it.value()->createShapeEffect( element );
        }
        else {
            kWarning(33002) << "page effect of smil:type" << smilType << "not supported";
            qDebug() << "Transition effect of smil:type" << smilType << "not supported";
        }
    }
    // return it
    return shapeEffect;
}

KPrTransitionFilterRegistry::KPrTransitionFilterRegistry()
: d( new Private() )
{
}

KPrTransitionFilterRegistry::~KPrTransitionFilterRegistry()
{
    qDeleteAll(doubleEntries());
    qDeleteAll(values());
    delete d;
}

void KPrTransitionFilterRegistry::init()
{
    qDebug() << "Init Shape Effect Registry";
    KoPluginLoader::PluginsConfig config;
    config.whiteList = "ShapeEffectPlugins";
    config.blacklist = "ShapeEffectPluginsDisabled";
    config.group = "stage";

    // XXX: Use minversion here?
    // The plugins are responsible for adding a factory to the registry
    KoPluginLoader::instance()->load( QString::fromLatin1("CalligraStage/ShapeEffect"),
            QString::fromLatin1("[X-KPresenter-Version] <= 0"),
            config);

    QList<KPrAnimTransitionFilterFactory*> factories = values();

    foreach ( KPrAnimTransitionFilterFactory * factory, factories ) {
        qDebug() << "Shape Effect Factory added in Init Registry";
        QList<QPair<QString, bool> > tags( factory->tags() );
        QList<QPair<QString, bool> >::iterator it( tags.begin() );
        for ( ; it != tags.end(); ++it ) {
            d->tagToFactory.insert( *it, factory );
        }
    }
}

