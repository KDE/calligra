/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "KPrPageEffectFactory.h"

#define BOOST_MULTI_INDEX_DISABLE_SERIALIZATION
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>

#include <KoXmlNS.h>
#include <KoXmlReader.h>

#include "KPrDurationParser.h"
#include "KPrPageEffectStrategy.h"
#include "StageDebug.h"

struct SmilData
    : boost::multi_index::composite_key<KPrPageEffectStrategy,
                                        boost::multi_index::const_mem_fun<KPrPageEffectStrategy, const QString &, &KPrPageEffectStrategy::smilSubType>,
                                        boost::multi_index::const_mem_fun<KPrPageEffectStrategy, bool, &KPrPageEffectStrategy::reverse>> {
};

typedef boost::multi_index_container<
    KPrPageEffectStrategy *,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<boost::multi_index::const_mem_fun<KPrPageEffectStrategy, int, &KPrPageEffectStrategy::subType>>,
        boost::multi_index::ordered_unique<SmilData>>>
    EffectStrategies;

struct Q_DECL_HIDDEN KPrPageEffectFactory::Private {
    Private(const QString &id, const QString &name)
        : id(id)
        , name(name)
    {
    }

    ~Private()
    {
        EffectStrategies::const_iterator it = strategies.begin();
        for (; it != strategies.end(); ++it) {
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
    QList<QPair<QString, bool>> tags;
};

KPrPageEffectFactory::KPrPageEffectFactory(const QString &id, const QString &name)
    : d(new Private(id, name))
{
}

KPrPageEffectFactory::~KPrPageEffectFactory()
{
    delete d;
}

KPrPageEffect *KPrPageEffectFactory::createPageEffect(const Properties &properties) const
{
    KPrPageEffectStrategy *strategy = nullptr;

    EffectStrategies::iterator it(d->strategies.find(properties.subType));

    if (it != d->strategies.end()) {
        strategy = *it;
    }
    Q_ASSERT(strategy);

    return new KPrPageEffect(properties.duration, d->id, strategy);
}

KPrPageEffect *KPrPageEffectFactory::createPageEffect(const KoXmlElement &element) const
{
    KPrPageEffectStrategy *strategy = nullptr;
    KPrPageEffect *pageEffect = nullptr;

    if (element.hasAttributeNS(KoXmlNS::smil, "subtype")) {
        QString smilSubType(element.attributeNS(KoXmlNS::smil, "subtype"));
        bool reverse = false;
        if (element.attributeNS(KoXmlNS::smil, "direction") == "reverse") {
            reverse = true;
        }

        int duration = 5000;
        if (element.hasAttributeNS(KoXmlNS::smil, "dur")) {
            duration = KPrDurationParser::durationMs(element.attributeNS(KoXmlNS::smil, "dur"));
            // TODO what if duration is -1
        } else if (element.hasAttributeNS(KoXmlNS::presentation, "transition-speed")) {
            QString transitionSpeed(element.attributeNS(KoXmlNS::presentation, "transition-speed"));
            if (transitionSpeed == "fast") {
                duration = 2000;
            } else if (transitionSpeed == "slow") {
                duration = 10000;
            }
            // wrong values and medium ar treated as default which is 5s
        }

        EffectStrategies::nth_index<1>::type::iterator it(d->strategies.get<1>().find(boost::make_tuple(smilSubType, reverse)));

        if (it != d->strategies.get<1>().end()) {
            strategy = *it;
            strategy->loadOdfSmilAttributes(element);
            pageEffect = new KPrPageEffect(duration, d->id, strategy);
        } else {
            warnStagePageEffect << "effect for " << d->id << smilSubType << reverse << "not supported";
        }
    }

    return pageEffect;
}

QString KPrPageEffectFactory::id() const
{
    return d->id;
}

QString KPrPageEffectFactory::name() const
{
    return d->name;
}

QList<int> KPrPageEffectFactory::subTypes() const
{
    return d->subTypes;
}

QList<QPair<QString, bool>> KPrPageEffectFactory::tags() const
{
    return d->tags;
}

QMultiMap<QString, int> KPrPageEffectFactory::subTypesByName() const
{
    QMultiMap<QString, int> nameToType;
    foreach (const int subType, d->subTypes) {
        nameToType.insert(subTypeName(subType), subType);
    }
    return nameToType;
}

void KPrPageEffectFactory::addStrategy(KPrPageEffectStrategy *strategy)
{
    bool inserted = d->strategies.insert(strategy).second;
    Q_ASSERT(inserted == true);
#ifdef NDEBUG
    Q_UNUSED(inserted);
#endif
    d->subTypes.append(strategy->subType());
    QPair<QString, bool> tag(strategy->smilType(), strategy->reverse());
    if (!d->tags.contains(tag)) {
        d->tags.append(tag);
    }
}
