/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KODATATOOL_H
#define KODATATOOL_H

#include <qstring.h>
#include <qobject.h>
#include <qvaluelist.h>

#include <kaction.h>
#include <kservice.h>

class KoDataTool;
class QPixmap;
class QStringList;
class KoDocument;
class KInstance;

// If you're only looking at implementing a data-tool, skip directly to the last
// class definition, KoDataTool.

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
     * @return TRUE if the DataTool does not modify the data passed to it by @ref KoDataTool::run.
     */
    bool isReadOnly() const;

    /**
     * @return a large pixmap for the DataTool.
     * @deprecated, use iconName()
     */
    QPixmap icon() const;
    /**
     * @return a mini pixmap for the DataTool.
     * @deprecated, use iconName()
     */
    QPixmap miniIcon() const;
    /**
     * @return the name of the icon for the DataTool
     */
    QString iconName() const;
    /**
     * @return a list if strings that you can put in a QPopupMenu item, for example to
     *         offer the DataTools services to the user. The returned value
     *         is usually something like "Spell checking", "Shrink Image", "Rotate Image"
     *         or something like that.
     *
     * This list comes from the Comment field of the tool's desktop file
     * (so that it can be translated).
     *
     * Each of the strings returned corresponds to a string in the list returned by
     * @ref #commands.
     */
    QStringList userCommands() const;
    /**
     * @return the list of commands the DataTool can execute. The KOffice application
     * passes the command to the @ref KoDataTool::run method.
     *
     * This list comes from the Commands field of the tool's desktop file.
     *
     * Each of the strings returned corresponds to a string in the list returned by
     * @ref #userCommands.
     */
    QStringList commands() const;

    /**
     * Creates the data tool described by this KoDataToolInfo.
     * @param part the part (document) that creates this tool.
     * @return a pointer to the created data tool or 0 on error.
     */
    KoDataTool* createTool( KoDocument * part, QObject* parent = 0, const char* name = 0 ) const;

    KService::Ptr service() const;

    /**
     * A DataToolInfo may be invalid if the @ref KService passed to its constructor does
     * not feature the service type "KoDataTool".
     */
    bool isValid() const;

    /**
     * @deprecated, use the other query method
     * Queries the @ref KTrader about installed @ref KoDataTool implementations.
     */
    static QValueList<KoDataToolInfo> query( const QString& datatype = QString::null, const QString& mimetype = QString::null );
    /**
     * Queries the @ref KTrader about installed @ref KoDataTool implementations.
     * @param datatype a type that the application can 'export' to the tools (e.g. QString)
     * @param mimetype the mimetype of the data (e.g. text/plain)
     * @param part the application's part (to check if a tool is excluded from this part)
     */
    static QValueList<KoDataToolInfo> query( const QString& datatype, const QString& mimetype, KoDocument * part );

private:
    KService::Ptr m_service;
};


/**
 * This class helps the KOffice applications implement support for KoDataTool.
 * The steps to follow are simple:
 * 1) query for the available tools using KoDataToolInfo::query
 * 2) pass the result to KoDataToolAction::dataToolActionList (with a slot)
 * 3) plug the resulting actions, either using KXMLGUIClient::plugActionList, or by hand.
 *
 * The slot defined for step 2 is called when the action is activated, and
 * that's where the tool should be created and run.
 */
class KoDataToolAction : public KAction
{
    Q_OBJECT
public:
    KoDataToolAction( const QString & text, const KoDataToolInfo & info, const QString & command, QObject * parent = 0, const char * name = 0);

    /**
     * Create a list of actions from a list of information about data-tools.
     * The slot must have a signature corresponding to the @ref toolActivated signal.
     *
     * Note that it's the caller's responsibility to delete the actions when they're not needed anymore.
     */
    static QList<KAction> dataToolActionList( const QValueList<KoDataToolInfo> & tools, const QObject *receiver, const char* slot );

signals:
    void toolActivated( const KoDataToolInfo & info, const QString & command );

protected:
    virtual void slotActivated();

private:
    QString m_command;
    KoDataToolInfo m_info;
};

/**
 * A generic tool for a KOffice application.
 *
 * A koffice-data-tool is a "plugin" for a KOffice application, that acts (reads/modifies)
 * on a portion of the data present in the document (e.g. a KSpread cell,
 * a KWord word or paragraph, etc.)
 *
 * The application has some generic code for presenting the tools in a popupmenu
 * (@see KoDataToolAction), and for activating a tool, passing it the data
 * (and eventually getting modified data from it).
 */
class KoDataTool : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor
     * The data-tool is only created when a menu-item, that relates to it, is activated.
     */
    KoDataTool( QObject* parent = 0, const char* name = 0 );

    /**
     * @internal. Do not use under any circumstance (including bad weather).
     */
    void setPart( KoDocument* part ) { m_part = part; }

    /**
     * @return the instance of the part that created this tool.
     * (For compatibility reasons this can still be 0L, check result !).
     * Usually used if the tool wants to read its configuration in the app's config file.
     */
    KInstance* instance() const;

    /**
     * 'Run' this tool.
     * @param command is the command that was selected (see KoDataToolInfo::commands())
     * @param data the data provided by the application, on which to run the tool.
     *             The application is responsible for setting that data before running the tool,
     *             and for getting it back and updating itself with it, after the tool ran.
     * @param datatype defines the type of @p data.
     * @param mimetype defines the mimetype of the data (for instance datatype may be
     *                 QString, but the mimetype can be text/plain, text/html etc.)
     */
    virtual bool run( const QString& command, void* data, const QString& datatype, const QString& mimetype) = 0;

private:
    KoDocument * m_part;
    class KoDataToolPrivate;
    KoDataToolPrivate * d;
};

#endif
