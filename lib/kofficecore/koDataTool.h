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

/**
 * A generic tool for a KOffice application.
 * 
 * A koffice-data-tool is a "plugin" for a KOffice application, that acts
 * on a portion of the data present in the document (e.g. a KSpread cell,
 * a KWord word or paragraph, etc.)
 * The application has some generic code for presenting the tools in a popupmenu,
 * and activating the selected one, passing it the data (and eventually getting
 * modified data from it).
 */
class KoDataTool : public QObject
{
    Q_OBJECT
public:
    KoDataTool( QObject* parent = 0, const char* name = 0 );
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
};

#endif
