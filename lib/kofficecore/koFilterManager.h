/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
                 2000 Werner Trobin <trobin@kde.org>

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

#ifndef __koffice_filter_manager_h__
#define __koffice_filter_manager_h__

#include <qmap.h>
#include <qwidgetstack.h>
#include <qobject.h>

#include <koQueryTrader.h>

class QDomDocument;

class KFileDialog;
class KoDocument;
class KoFilterDialog;
class KURL;
class KProcess;

class KoFilterManagerPrivate;
class PreviewStack;

/**
 *  This class manages all filters for a KOffice application. Normally
 *  you won't have to use it, since KoMainWindow takes care of loading
 *  and saving documents.
 *
 *  This class follows the singleton pattern. You don't create objects of this
 *  class directly, but rather use the static method @ref #self.
 *
 *  @short The class managing all the filters.
 *  @ref KoFilter
 *  @ref KoFilterDialog
 *
 *  @author Kalle Dalheimer <kalle@kde.org>
 *  @author Torben Weis <weis@kde.org>
 *  @author Werner Trobin <trobin@kde.org>
 *  @version $Id$
 */
class KoFilterManager : public QObject
{
    Q_OBJECT
public:
    KoFilterManager();
    virtual ~KoFilterManager();

    /**
     * Are we importing (loading) or exporting (saving) a file?
     */
    enum Direction { Import, Export };

    /**
     * Returns a string list (packed into a string) that is suitable for
     * passing to KFileDialog::setFilters().
     *
     * @param direction Whether the dialog is for opening or for saving.
     *                  It is either KoFilterManager::Import or KoFilterManager::Export.
     * @param _format is the mimetype that has to be exported/imported,
     *                for example "application/x-kspread".
     * @param _native_pattern is the filename pattern for the native format
     *                        of your application, for example "*.ksp".
     *                        This variable may be QString::null, then no native format
     *                        is added.
     * @param _native_name is the name for the native format
     *                        of your application, for example "KSpread".
     *                        This variable may be QString::null, then no native format
     *                        is added.
     * @param allfiles Whether a wildcard that matches all files should be added to the list.
     *
     * @deprecated. This obsolete method might be removed when kdelibs-2.2 is required.
     */
    QString fileSelectorList( const Direction &direction,
                              const char *_format,
                              const QString & _native_pattern=QString::null,
                              const QString & _native_name=QString::null,
                              bool allfiles=false ) const;

    /**
     * Prepares a KFileDialog instance. This means it adds the available
     * filters and dialogs.
     *
     * @param dialog The dialog you want to prepare
     * @param direction Whether the dialog is for opening or for saving.
     *                  It is either KoFilterManager::Import or KoFilterManager::Export.
     * @param _format is the mimetype that has to be exported/imported,
     *                for example "application/x-kspread".
     * @param _native_pattern is the filename pattern for the native format
     *                        of your application, for example "*.ksp".
     *                        This variable may be QString::null, then no native format
     *                        is added.
     * @param _native_name is the name for the native format
     *                        of your application, for example "KSpread".
     *                        This variable may be QString::null, then no native format
     *                        is added.
     * @param allfiles Whether a wildcard that matches all files should be added to the list.
     * @return Have we been sucessful?
     */
    bool prepareDialog( KFileDialog *dialog,
                        const Direction &direction,
                        const char *_format,
                        const QString & _native_pattern=QString::null,
                        const QString & _native_name=QString::null,
                        bool allfiles=false );

    /**
     * Cleans up the prepared KFileDialog (deletes all the stuff)
     * and saves the dialog config. Please don't forget to call it!
     */
    void cleanUp();

    /**
     * Import a file by applying a filter
     * @param url The URL of the file to load
     * @param native_format The Native Format supported by our application
     * @param document ...
     * @return the file name to load, either QString::null (error, QDomDocument, or KoDocument),
     * _url (no conversion) or a /tmp file (conversion)
     */
    QString import( const QString & url, const char * native_format,
                    KoDocument * document, const QString &storePrefix="tar:" );

    /**
     * Import a file by applying a filter. This is the same as above, but it
     * is more suited to being called without a GUI, for example, from within
     * another filter.
     *
     * @param url The URL of the file to load.
     * @param mimeType Output the Native Format supported by the application selected
     *                 to handle this file.
     * @param config Configuration information for the filter. The format of this
     *               is filter-specific.
     * @return the file name to load, either QString::null (error, QDomDocument, or KoDocument),
     * _url (no conversion) or a /tmp file (conversion)
     */
    QString import( const QString &_file, QString &mimeType,
                    const QString &config, const QString &storePrefix="tar:" );

    /**
     * Export a file using a filter - don't call this one it's automatically
     * called by KoMainWindow::saveDocument()
     * @return the filename (some /tmp/ name) to which the original file should
     * be saved. The export_() function fetches this file and saves it to the _url
     * @param _url the location where the converted file will be stored
     * @param _native_format the application's native file format
     * @param outputFormat the format in which to save
     */
    QString prepareExport( const QString &_url, const QCString & _native_format,
                           const QCString & outputFormat, KoDocument *document );
    /**
     * Performs the "real" exporting - don't call this function directly! It will
     * be called from KoMainWindow::saveDocument()
     */
    bool export_();

    /** @deprecated, will be removed later */
    static void incRef() {}
    /** @deprecated, will be removed later */
    static void decRef() {}
    /** @deprecated, will be removed later */
    static unsigned long refCnt() {return 1;}

    /**
     * Returns a pointer to the only instance of the KoFilterManager.
     * @return The pointer to the KoFilterManager instance
     * @deprecated Create an instance of KoFilterManager for each conversion to be done
     */
    static KoFilterManager* self();

protected:
    KoFilterManager &operator=(const KoFilterManager &);

private slots:
    void processExited(KProcess *p);
    void receivedStdout(KProcess *p, char *buffer, int buflen);

private:
    static const int s_area = 30003;

    /**
     *	Get the ID of the QWidget in the Stack which matches this
     *  extension. (internal)
     */
    const int findWidget(const QString &ext) const;

    friend class PreviewStack;

    static KoFilterManager* s_pSelf;
    KoFilterManagerPrivate *d;
    //static unsigned long s_refCnt;
};


/**
 * This class is used internally by KoFilterManager to stack all the
 * available dialogs. (@internal)
 */
class PreviewStack : public QWidgetStack {

    Q_OBJECT

public:
    PreviewStack(QWidget *parent, const char *name, KoFilterManager *m);
    virtual ~PreviewStack();

    // As the stack can be hidden/visible one has to know the status
    bool isHidden() const { return hidden; }

public slots:
    // The URL has changed -> show the correct dialog
    void showPreview(const KURL &url);
    // When exporting (saving) we can only check adapt
    // the dialog via the filter b/c there is no URL
    void filterChanged(const QString &filter);

private:
    void change(const QString &ext);

    const KoFilterManager * const mgr;
    bool hidden;
};
#endif  // __koffice_filter_manager_h__
