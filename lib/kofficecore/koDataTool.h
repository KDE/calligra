#ifndef KODATATOOL_H
#define KODATATOOL_H

#include <qstring.h>
#include <qobject.h>
#include <qvaluelist.h>

#include <kservice.h>

class KoDataTool;
class QPixmap;
class QStringList;

/**
 * This is a convenience class for @ref KService. You can use it if you have
 * a KService describing a KoDataTool. In this case the KoDataToolInfo class
 * is more convenient to work with.
 *
 * Especially useful is the method @ref #createTool which creates the datatool
 * described by the service.
 */
class KoDataToolInfo
{
public:
    /**
     * Create an invalid KoDataToolInfo.
     */
    KoDataToolInfo();
    KoDataToolInfo( const KService::Ptr& service );
    /**
     * Copy constructor.
     */
    KoDataToolInfo( const KoDataToolInfo& info );
    /**
     * Assignment operator.
     */
    KoDataToolInfo& operator= ( const KoDataToolInfo& info );

    /**
     * @return the C++ data type that this DataTool accepts.
     *         For example "QString" or "QImage" or something more
     *         complicated.
     */
    QString dataType() const;
    /**
     * @return the mime types accepted by this DataTool. For example
     *         "image/gif" or "text/plain". In some cases the dataType
     *         determines the accepted type of data perfectly. In this cases
     *         this list may be empty.
     *
     * The mimetypes are only used if the @ref #dataType can be used to store
     * different mimetypes. For example in a "QString" you could save "text/plain"
     * or "text/html" or "text/xml".
     */
    QStringList mimeTypes() const;

    /**
     * @return TRUE if the DataTool does not modiy the data passed to it by @ref KoDataTool::run.
     */
    bool isReadOnly() const;

    /**
     * @return a large pixmap for the DataTool.
     */
    QPixmap icon() const;
    /**
     * @return a mini pixmap for the DataTool.
     */
    QPixmap miniIcon() const;
    /**
     * @return a list if strings that you can put in a QPopupMenu item, for example to
     *         offer the DataTools services to the user. The returned value
     *         is usually something like "Spell checking", "Shrink Image", "Rotate Image"
     *         or something like that.
     *
     * Each of the strings returned corresponds to a string in the list returned by
     * @ref #commands.
     */
    QStringList userCommands() const;
    /**
     * @return the list of commands the DataTool can execute. You have to pass the command
     *         to the @ref KoDataTool::run method.
     *
     * Each of the strings returned corresponds to a string in the list returned by
     * @ref #userCommands.
     */
    QStringList commands() const;

    /**
     * Creates the data tool described by this KoDataToolInfo.
     *
     * @return a pointer to the created data tool or 0 on error.
     */
    KoDataTool* createTool( QObject* parent = 0, const char* name = 0 );

    KService::Ptr service() const;

    /**
     * A DataToolInfo may be invalid if the @ref KService passed to its constructor does
     * not feature the service type "KoDataTool".
     */
    bool isValid() const;

    /**
     * Queries the @ref KTrader about installed @ref KoDataTool implementations.
     */
    static QValueList<KoDataToolInfo> query( const QString& datatype = QString::null, const QString& mimetype = QString::null );

private:
    KService::Ptr m_service;
};

class KoDataTool : public QObject
{
    Q_OBJECT
public:
    KoDataTool( QObject* parent = 0, const char* name = 0 );

    virtual bool run( const QString& command, void* data, const QString& datatype, const QString& mimetype ) = 0;
};

#endif
