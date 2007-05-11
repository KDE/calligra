#ifndef __KDCHART_SERIALIZE_COLLECTOR_H__
#define __KDCHART_SERIALIZE_COLLECTOR_H__

#include <KDChartGlobal>

#include <QObject>
#include <QDomDocument>
#include <QDomElement>
#include <QMap>

#include "kdchartserializer_export.h"

namespace KDChart {

    /**
     * Auxiliary class for information to be stored in the InitializedPointersMap
     * used at parse time.
     * 
     * For KD Chart's built-in classes that's taken care for automatically,
     * so you only need to use it in case you have added your own
     * objects to the InitializedPointersMap
     *
     * \sa initializedPointersMap, setWasParsed
     */
    struct InitializedPointersMapItem
    {
        InitializedPointersMapItem(QObject* p) :
                pointer(p), wasParsed(false) {}
        InitializedPointersMapItem() :
                pointer(0), wasParsed(false) {}
        QObject* pointer;
        bool wasParsed;
    };
    typedef QMap<QString, InitializedPointersMapItem> InitializedPointersMap;

    /**
     * Auxiliary class used by the KDChart::*Serializer classes.
     *
     * Normally there should be no need to call any of these methods yourself.
     *
     * It is all handled automatically, if you use the main KDChart::Serializer
     * class as entry point of your serialization operations.
     */
    class KDCHARTSERIALIZER_EXPORT SerializeCollector
    {
        //friend class Serializer; // is allowed to delete the mapper instance
        Q_DISABLE_COPY( SerializeCollector )

        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC( SerializeCollector )

    public:
        static SerializeCollector* instance();

        SerializeCollector();
        virtual ~SerializeCollector();

    public:

    // ************** parsing the data *******************************

    static QDomElement findStoredGlobalElement(
            const QDomNode& rootNode,
            const QString& globalPointerName,
            const QString& globalGroupName,
            const QString& globalListName = "kdchart:global-objects" );

    /**
     * Auxiliary method to be called BEFORE all of your
     * structures will be parsed.
     *
     * This method parses all of the global object declarations,
     * it instantiates them, and it stores their pointers in a
     * global list - together with their symbolic "global names"
     * retrieved from the QDomDocument's root node.
     *
     * The following elements will be parsed,
     * and their top-level children will be instantiated:
     *
     * <kdchart:attribute-models>
     * <kdchart:axes>
     * <kdchart:charts>
     * <kdchart:coordinate-planes>
     * <kdchart:diagrams>
     * <kdchart:headers-footers>
     * <kdchart:legends>
     *
     * \sa foundInitializedPointers
     */
    static bool initializeGlobalPointers(
            const QDomNode& rootNode,
            const QString& name="kdchart:global-objects" );

    /**
     * Returns a reference to the global list of pointers parsed
     * by initializeGlobalPointers().
     *
     * Normally you do not need to call this method, but you can do
     * so in case you want to add additional pointers to the list:
     *
     * By modifying the QDomDocument after KDChart::Serializer had
     * finished creating it so you might have stored additional
     * QObject* references in it.
     *
     * All entries which you add to the InitializedPointersMap will be taken
     * into account by foundInitializedPointers() trying to find a matching
     * QObject* for a given name.
     *
     * SerializeCollector is a singleton, so you could act like this:
\verbatim
KDChart::SerializeCollector::instance()->initializeGlobalPointers( rootNode );

// Add your own object to the list,
// thePointer pointing to an object derived from QObject,
// theName being a unique name for this object instance.

KDChart::InitializedPointersMap& mapRef
    = KDChart::SerializeCollector::instance()->initializedPointersMap();
mapRef.insert( theName, InitializedPointersMapItem( myPointer ) );

// after the data was parsed you set the flag:
KDChart::SerializeCollector::instance()->setWasParsed( thePointer, true );
\endverbatim
     */
    InitializedPointersMap& initializedPointersMap();

    /**
     * Auxiliary method to find the QObject* for a given name.
     *
     * p is set the the QObject* found, or to 0 if none is found.
     *
     * wasParsed is set to true, if the respective element
     * has been completely parsed (not just initialized),
     * that is, if setWasParsedCompletely() has been called accordingly.
     * This flag is set by KD Chart's serializer methods automatically.
     * 
     * \note This only works if initializeGlobalPointers()
     * was called before, otherwise the global list will be empty.
     *
     * \sa initializeGlobalPointers
     */
    static bool foundInitializedPointer(
            const QString& globalName,
            QObject*& p,
            bool& wasParsed );

    /**
     * This flag is set to true when the element has been parsed
     * completely.
     * For KD Chart's built-in classes that's done automatically,
     * so you only need to call it in case you have added your own
     * objects to the InitializedPointersMap
     *
     * \sa initializedPointersMap
     */
    static void setWasParsed( QObject* p, bool parsed );



    // ************** storing the data *******************************


    /** Returns the QDomElement that was stored for this name.
     *
     * If none was stored before, a new element is created by the document
     * and the name is returned.
     *
     * This is used for saving global elements, like e.g. the
     * list of diagrams, or the list of attribute models, ...
     *
     * Note that the element is not added to any QDomNode,
     * since it is assumed that the calling process will do that
     * afterwards, e.g. KDChart::Serializer::saveChartElement() does that.
     *
     * The QDomElement is owned by the SerializeCollector,
     * so it will be deleted when the collector is deleted.
        */
    QDomElement* findOrMakeElement( QDomDocument& doc, const QString& name );

    /** Returns the QDomElement that was stored for this name.
     *
     * If none was stored before,zero is returned.
     */
    QDomElement* findElement( const QString& name )const;

    /** Delete all collected data.
     */
    void clear();

    /** Append all collected data to the element given.
     *
     * A new element of name \c name is created, and in this
     * all data are stored.
     *
     * \note This needs to be called after you have stored all of
     * your structures, but before you call resolvePointers().
     */
    void appendDataToElement( QDomDocument& doc,
                              QDomElement& element,
                              const QString& name="kdchart:global-objects" )const;

    /**
     * Auxiliary function:
     * create an element of the given name + ":pointers"
     * add the element to e
     * return the element
     */
    static QDomElement createPointersList(
            QDomDocument& doc, QDomElement& e, const QString& title );

    /**
     * Auxiliary function:
     * Adds an entry to the elementsList, if no element matching the pointer
     * was found in the list.
     * Also calls storePointerName() to create and store a string node with
     * the name "kdchart:pointer" and the value composed by the title
     * and the respective number.
     *
     * The returned element should be used to store the object that
     * the pointer is pointing to.
     * So if e.g. you have a (Some)Diagram pointer, you should call
     * DiagramsSerializer::saveDiagram passing the QDomElement to it that
     * you got as return value from findOrMakeChild.
     * 
     * \param wasFound returns if the node was in the list already
     */
    static QDomElement findOrMakeChild(
            QDomDocument& doc,
            QDomElement& elementsList,
            QDomElement& pointerContainer,
            const QString& title,
            const QString& classname,
            const void* p,
            bool& wasFound );

    /**
     * Auxiliary function for the global list of unresolved(!) pointers:
     *
     * That special list is decicated for temporarily storing pure QObject pointers.
     *
     * \note This list is for QObject pointers only. It is not to be used for
     * pointers to AttributeModel, Axis, Chart, CoordinatePlane, Diagram,
     * HeaderFooter, Legend. For such pointers use findOrMakeChild() instead!
     *
     * Working with the global list of unresolved pointers goes like this:
     *
     * Step 1: While storing your local structure
     *         you might want to store a QObject* p,
     *         typically this could point to some ReferenceObject.
     *         You look for it, it was not stored yet,
     *         so you store it temporarily as unresolved pointer:
\verbatim
    QDomElement localPointerElement =
            doc.createElement( "ReferenceObject" );
    someParentElement.appendChild( localPointerElement );

    const QString globalName( IdMapper::instance()->findName( p ) );
    const bool bOK = ! globalName.isEmpty();
    if( bOK ){
        // you store the global name:

        SerializeCollector::instance()->storePointerName(
                doc, localPointerElement, globalName );
    }else{
        // there was no global name found yet,
        // so you store it temporarily:

        SerializeCollector::instance()->storeUnresolvedPointer(
                doc, p, localPointerElement );
    }
\endverbatim
     * This does the following:
     * \li An entry is added to the global list of unresolved pointers,
     *     (or an existing entry pointing to the same object is reused, resp.)
     * \li Your local QDomElement receives a temporary string entry
     *     of this type: "kdchart:unresolved-pointer".
     *
     * 
     * Step 2: After you have stored ALL of your structures,
     *         and after you have called appendDataToElement()
     *         you run the resolve method on the root node
     *         of your QDomDocument:
\verbatim
    SerializeCollector::instance()->resolvePointers( doc, rootNode );
\endverbatim
     * This does the following:
     * \li All of the temporary "kdchart:unresolved-pointer"
     *     entries in the whole document are matched against the
     *     global ID list hold by IdMapper
     * \li When possible, an entry's value is replaced by the real
     *     pointer-name, that might have been stored in between.
     * \li All non-resolvable entries are marked as "U_N_R_E_S_O_L_V_E_D".
     */
    static void storeUnresolvedPointer(
            QDomDocument& doc,
            const QObject* p,
            QDomElement& pointerContainer );

    /**
     * Auxiliary method to be called AFTER all of your
     * structures have been stored.
     *
     * \sa storeUnresolvedPointer
     */
    static void resolvePointers(
            QDomDocument& doc,
            QDomElement& rootNode );

    /**
     * Auxiliary function:
     * Create and store a string node with the name "kdchart:pointer"
     * and the value given by pointerName.
     */
    static void storePointerName(
            QDomDocument& doc,
            QDomElement& pointerContainer,
            const QString& pointerName );
    };

} // end of namespace

#endif
