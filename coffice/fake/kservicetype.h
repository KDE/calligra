#ifndef FAKE_KSERVICETYPE_H
#define FAKE_KSERVICETYPE_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QDataStream>
#include <QDebug>

#include <ksharedptr.h>
#include <kdesktopfile.h>

class KServiceType : public KShared
{
public:
    typedef KSharedPtr<KServiceType> Ptr;
    typedef QList<Ptr> List;

    KServiceType( KDesktopFile *config ) {}
    KServiceType( QDataStream& _str, int offset ) {}

#if 0

    /**
     * Returns the descriptive comment associated, if any.
     * @return the comment, or QString()
     */
    QString comment() const;

    /**
     * Returns the relative path to the desktop entry file responsible for
     *         this servicetype.
     * For instance inode/directory.desktop, or kpart.desktop
     * @return the path of the desktop file
     */
#ifndef KDE_NO_DEPRECATED
    KDE_DEPRECATED QString desktopEntryPath() const;
#endif

    /**
     * Checks whether this service type inherits another one.
     * @return true if this service type inherits another one
     * @see parentServiceType()
     */
    bool isDerived() const;

    /**
     * If this service type inherits from another service type,
     * return the name of the parent.
     * @return the parent service type, or QString:: null if not set
     * @see isDerived()
     */
    QString parentServiceType() const;

    /**
     * Checks whether this service type is or inherits from @p servTypeName.
     * @return true if this servicetype is or inherits from @p servTypeName
     */
    bool inherits( const QString& servTypeName ) const;

    /**
     * Returns the type of the property definition with the given @p _name.
     *
     * @param _name the name of the property
     * @return the property type, or null if not found
     * @see propertyDefNames
     */
    QVariant::Type propertyDef( const QString& _name ) const;

    /**
     * Returns the list of all property definitions for this servicetype.
     * Those are properties of the services implementing this servicetype.
     * For instance,
     * @code
     * [PropertyDef::X-KDevelop-Version]
     * Type=int
     * @endcode
     * means that all kdevelop plugins have in their .desktop file a line like
     * @code
     * X-KDevelop-Version=<some value>
     * @endcode
     */
    QStringList propertyDefNames() const;

    /// @internal (for KBuildServiceTypeFactory)
    QMap<QString,QVariant::Type> propertyDefs() const;

    /**
     * @internal
     * Pointer to parent service type
     */
    Ptr parentType();
    /**
     * @internal  only used by kbuildsycoca
     * Register offset into offers list
     */
    void setServiceOffersOffset( int offset );
    /**
     * @internal
     */
    int serviceOffersOffset() const;

    /**
     * Returns a pointer to the servicetype '_name' or 0L if the
     *         service type is unknown.
     * VERY IMPORTANT : don't store the result in a KServiceType * !
     * @param _name the name of the service type to search
     * @return the pointer to the service type, or 0
     */
#endif

    static Ptr serviceType( const QString& _name )
    {
        return Ptr();
    }

#if 0
    /**
     * Returns a list of all the supported servicetypes. Useful for
     *         showing the list of available servicetypes in a listbox,
     *         for example.
     * More memory consuming than the ones above, don't use unless
     * really necessary.
     * @return the list of all services
     */
    static List allServiceTypes();
#endif

#if 0
   /**
    * Safe demarshalling functions.
    */
   static void read( QDataStream &s, QString &str );
   static void read( QDataStream &s, QStringList &list );


   /**
    * @return the name of this entry
    */
   QString name() const;

   /**
    * @return the path of this entry
    * The path can be absolute or relative.
    * The corresponding factory should know relative to what.
    */
   QString entryPath() const;

   /**
    * @return the unique ID for this entry
    * In practice, this is storageId() for KService and name() for everything else.
    * \since 4.2.1
    */
   QString storageId() const;

    /**
    * @return true if valid
    */
   bool isValid() const;

   /**
    * @return true if deleted
    */
   bool isDeleted() const;

    /**
     * Returns the requested property. Some often used properties
     * have convenience access functions like exec(),
     * serviceTypes etc.
     *
     * @param name the name of the property
     * @return the property, or invalid if not found
     */
#endif

   QVariant property(const QString &name) const
   {
       return QVariant();
   }

#if 0
    /**
     * Returns the list of all properties that this service can have.
     * That means, that some of these properties may be empty.
     * @return the list of supported properties
     */
    QStringList propertyNames() const;

   /**
    * Sets whether or not this service is deleted
    */
   void setDeleted( bool deleted );


   /*
    * @returns true, if this is a separator
    */
   bool isSeparator() const;

   /**
    * @internal
    * @return the position of the entry in the sycoca file
    */
   int offset() const;

   /**
    * @internal
    * Save ourselves to the database.
    */
   void save(QDataStream &s);
#endif

};

#endif
