#ifndef KPRANIMTRANSITIONFILTERFACTORY_H
#define KPRANIMTRANSITIONFILTERFACTORY_H

#include <QPair>
#include "stage_export.h"

#include "KPrAnimTransitionFilterEffect.h"

#include "KPrAnimTransitionFilterStrategy.h"
class KoXmlElement;


/**
 * Base class for shape transition filter effects
 */
class STAGE_EXPORT KPrAnimTransitionFilterFactory
{
public:
    KPrAnimTransitionFilterFactory();

    struct Properties {
        Properties( int duration, int subType )
        : duration( duration )
        , subType( subType )
        {}

        int duration;
        int subType;
    };

    /**
     * Constructor
     *
     * @param id The id of the page effect the factory is creating
     * @param name The name of the effect. This name is used in the UI
     * @param subTypes The possible subtypes of the page effect
     */
    KPrAnimTransitionFilterFactory( const QString & id, const QString & name );

    virtual ~KPrAnimTransitionFilterFactory();

    /**
     * Create a page effect
     *
     * @param properties The properties for creating a page effect
     */
    KPrAnimTransitionFilterEffect * createShapeEffect( const Properties & properties ) const;

    /**
     * Create a page effect
     *
     * This is used for loading a page effect from odf
     *
     * @param element The element containing the information for creating the page effect
     */
    KPrAnimTransitionFilterEffect * createShapeEffect( const KoXmlElement & element ) const;

    /**
     * Get the id of the page effect
     */
    QString id() const;

    /**
     * Get the name of the page effect
     */
    QString name() const;

    /**
     * Get the sub types of the page effect
     */
    QList<int> subTypes() const;

    /**
     * Get the tags the factory is responsible for
     *
     * This defines for which smil:type and smil:direction this factory is responsible.
     * If the bool is false the smil:direction is forward if it is true the smil:direction
     * is reverse.
     */
    QList<QPair<QString, bool> > tags() const;

    /**
     * Get the (i18n'ed) name of a given subtype.
     */
    virtual QString subTypeName(int subType) const = 0;

    /**
     * Get the subtypes sorted by name. This sorts by the i18n'ed name.
     */
    QMap<QString, int> subTypesByName() const;

protected:
    /**
     * Add a strategy to the factory
     *
     * @param strategy The strategy to add
     */
    void addStrategy( KPrAnimTransitionFilterStrategy * strategy );

private:
    struct Private;
    Private * const d;
};

#endif // KPRANIMTRANSITIONFILTERFACTORY_H
