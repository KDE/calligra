#include "KPrAnimTransitionFilterFactory.h"

#define BOOST_MULTI_INDEX_DISABLE_SERIALIZATION
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <KoXmlReader.h>
#include <KoXmlNS.h>

#include "KPrDurationParser.h"

#include <kdebug.h>

struct SmilData : boost::multi_index::composite_key<
    KPrAnimTransitionFilterStrategy,
    boost::multi_index::const_mem_fun<KPrAnimTransitionFilterStrategy, const QString &, &KPrAnimTransitionFilterStrategy::smilSubType>,
    boost::multi_index::const_mem_fun<KPrAnimTransitionFilterStrategy, bool, &KPrAnimTransitionFilterStrategy::reverse>
>
{
};


typedef boost::multi_index_container<
    KPrAnimTransitionFilterStrategy *,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
            boost::multi_index::const_mem_fun<KPrAnimTransitionFilterStrategy, int, &KPrAnimTransitionFilterStrategy::subType>
        >,
        boost::multi_index::ordered_unique<
            SmilData
        >
    >
> EffectStrategies;

struct KPrAnimTransitionFilterFactory::Private
{
    Private( const QString & id, const QString & name )
    : id( id )
    , name( name )
    {}

    ~Private()
    {
        EffectStrategies::const_iterator it = strategies.begin();
        for ( ; it != strategies.end(); ++it ) {
            delete *it;
        }
    }

    QString id;
    QString name;
    QList<int> subTypes;
    EffectStrategies strategies;
    // this defines for which smil:type and smil:direction this factory
    // is responsible. If the bool is false the smil:direction is forward if
    // it is true the smil:direction is reverse.
    QList<QPair<QString, bool> > tags;
};

KPrAnimTransitionFilterFactory::KPrAnimTransitionFilterFactory( const QString & id, const QString & name )
: d( new Private( id, name ) )
{
}

KPrAnimTransitionFilterFactory::~KPrAnimTransitionFilterFactory()
{
    delete d;
}

KPrAnimTransitionFilterEffect *KPrAnimTransitionFilterFactory::createShapeEffect( const Properties & properties ) const
{
    KPrAnimTransitionFilterStrategy * strategy = 0;

    EffectStrategies::iterator it( d->strategies.find( properties.subType ) );

    if ( it != d->strategies.end() ) {
        strategy = *it;
    }
    Q_ASSERT( strategy );

    return new KPrAnimTransitionFilterEffect( properties.duration, d->id, strategy );
}

KPrAnimTransitionFilterEffect * KPrAnimTransitionFilterFactory::createShapeEffect( const KoXmlElement & element ) const
{
    KPrAnimTransitionFilterStrategy * strategy = 0;
    KPrAnimTransitionFilterEffect * shapeEffect = 0;

    if ( element.hasAttributeNS( KoXmlNS::smil, "type" ) ) {
        QString smilSubType( element.attributeNS( KoXmlNS::smil, "subtype" ) );
        bool reverse = false;
        if ( element.attributeNS( KoXmlNS::smil, "direction" ) == "reverse" ) {
            reverse = true;
        }

        int duration = 5000;
        if (element.hasAttributeNS(KoXmlNS::smil, "dur")) {
            duration = KPrDurationParser::durationMs(element.attributeNS(KoXmlNS::smil, "dur"));
            // TODO what if duration is -1
        }
        else if (element.hasAttributeNS(KoXmlNS::presentation, "transition-speed")) {
            QString transitionSpeed(element.attributeNS(KoXmlNS::presentation, "transition-speed"));
            if (transitionSpeed == "fast") {
                duration = 2000;
            }
            else if (transitionSpeed == "slow") {
                duration = 10000;
            }
            // wrong values and medium ar treated as default which is 5s
        }

        EffectStrategies::nth_index<1>::type::iterator it( d->strategies.get<1>().find( boost::make_tuple( smilSubType, reverse ) ) );

        if ( it != d->strategies.get<1>().end() ) {
            qDebug() << "Strategy found in factory";
            strategy = *it;
            strategy->loadOdfSmilAttributes( element );
            shapeEffect = new KPrAnimTransitionFilterEffect( duration, d->id, strategy );
        }
        else {
            kWarning(33002) << "effect for " << d->id << smilSubType << reverse << "not supported";
        }
    }

    return shapeEffect;
}

QString KPrAnimTransitionFilterFactory::id() const
{
    return d->id;
}

QString KPrAnimTransitionFilterFactory::name() const
{
    return d->name;
}

QList<int> KPrAnimTransitionFilterFactory::subTypes() const
{
    return d->subTypes;
}

QList<QPair<QString, bool> > KPrAnimTransitionFilterFactory::tags() const
{
    return d->tags;
}

QMap<QString, int> KPrAnimTransitionFilterFactory::subTypesByName() const
{
    QMap<QString, int> nameToType;
    foreach( const int subType, d->subTypes ) {
        nameToType.insertMulti( subTypeName( subType ), subType );
    }
    return nameToType;
}

void KPrAnimTransitionFilterFactory::addStrategy( KPrAnimTransitionFilterStrategy * strategy )
{
    bool inserted = d->strategies.insert( strategy ).second;
    Q_ASSERT( inserted == true );
#ifdef NDEBUG
    Q_UNUSED(inserted);
#endif
    d->subTypes.append( strategy->subType() );
    QPair<QString, bool> tag( strategy->smilType(), strategy->reverse() );
    if ( !d->tags.contains( tag ) ) {
        qDebug() << "append: " << strategy->smilType();
        d->tags.append( tag );
    }
}
