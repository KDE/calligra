/* This file is part of the KDE project
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
             (C) 1999 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef _KPART_H
#define _KPART_H

#include <QtCore/QPointer>
#include <QtCore/QEvent>
#include <QtCore/QSharedDataPointer>
#include <QtXml/QDomElement> // KDE5: remove

#include <kurl.h>
#include <kxmlguiclient.h>

#include "komain_export.h"

#define KPARTS_DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(Part::d_ptr); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(Part::d_ptr); } \
    friend class Class##Private;

class KIconLoader;
class KComponentData;
class QEvent;
class QPoint;
struct QUnknownInterface;

class KJob;
namespace KIO {
  class Job;
}

class KoMainWindow;

namespace KoParts
{

class PartPrivate;
class PartBasePrivate;

/**
 * Base class for parts.
 *
 * A "part" is a GUI component, featuring:
 * @li A widget embeddedable in any application.
 * @li GUI elements that will be merged in the "host" user interface
 * (menubars, toolbars... ).
 *
 * <b>About the widget:</b>\n
 *
 * Note that KoParts::Part does not inherit QWidget.
 * This is due to the fact that the "visual representation"
 * will probably not be a mere QWidget, but an elaborate one.
 * That's why when implementing your KoParts::Part (or derived)
 * you should call KoParts::Part::setWidget() in your constructor.
 *
 * <b>About the GUI elements:</b>\n
 *
 * Those elements trigger actions, defined by the part ( action()).
 * The layout of the actions in the GUI is defined by an XML file ( setXMLFile()).
 *
 * See also ReadOnlyPart and ReadWritePart, which define the
 * framework for a "viewer" part and for an "editor"-like part.
 * Use Part directly only if your part doesn't fit into those.
 */
class KOMAIN_EXPORT Part : public QObject, public KXMLGUIClient
{
    Q_OBJECT

    KPARTS_DECLARE_PRIVATE(Part)

public:

    /**
     *  Constructor.
     *
     *  @param parent Parent object of the part.
     */
    explicit Part( QObject *parent = 0 );

    /**
     *  Destructor.
     */
    virtual ~Part();

    /**
     * @internal
     * Used by the part manager.
     */
    virtual void setManager( KoMainWindow * manager );

protected:
    /**
     * Set the componentData(KComponentData) for this part.
     *
     * Call this *first* in the inherited class constructor,
     * because it loads the i18n catalogs.
     */
    virtual void setComponentData(const KComponentData &componentData);


    Part(PartPrivate &dd, QObject *parent);
    PartPrivate *d_ptr;

private:
    Q_DISABLE_COPY(Part)
};

class ReadWritePart;
class ReadOnlyPartPrivate;
class OpenUrlArgumentsPrivate;


/**
 * Base class for any "viewer" part.
 *
 * This class takes care of network transparency for you,
 * in the simplest way (downloading to a temporary file, then letting the part
 * load from the temporary file).
 * To use the built-in network transparency, you only need to implement
 * openFile(), not openUrl().
 *
 * To implement network transparency differently (e.g. for progressive loading,
 * like a web browser does for instance), or to prevent network transparency
 * (but why would you do that?), you can override openUrl().
 *
 * KoParts Application can use the signals to show feedback while the URL is being loaded.
 *
 * ReadOnlyPart handles the window caption by setting it to the current URL
 * (set in openUrl(), and each time the part is activated).
 * If you want another caption, set it in openFile()
 */
class KOMAIN_EXPORT ReadOnlyPart : public Part
{
    Q_OBJECT

    Q_PROPERTY( KUrl url READ url )

    KPARTS_DECLARE_PRIVATE(ReadOnlyPart)

public:
    /**
     * Constructor
     * See also Part for the setXXX methods to call.
     */
    explicit ReadOnlyPart( QObject *parent = 0 );

    /**
     * Destructor
     */
    virtual ~ReadOnlyPart();

public Q_SLOTS:
    /**
     * Only reimplement openUrl if you don't want the network transparency support
     * to download from the url into a temporary file (when the url isn't local).
     * Otherwise, reimplement openFile() only .
     *
     * If you reimplement it, don't forget to set the caption, usually with
     * emit setWindowCaption( url.prettyUrl() );
     */
    virtual bool openUrl( const KUrl &url );

public:
    /**
     *  Returns the URL currently opened in this part.
     *
     *  @return The current URL.
     */
    KUrl url() const;

    /**
     * Called when closing the current url (e.g. document), for instance
     * when switching to another url (note that openUrl() calls it
     * automatically in this case).
     * If the current URL is not fully loaded yet, aborts loading.
     * Deletes the temporary file used when the url is remote.
     * @return always true, but the return value exists for reimplementations
     */
    virtual bool closeUrl();

    /**
     * @return the mimetype of the arguments that were used to open this URL.
     */
    QString mimeType() const;


Q_SIGNALS:
    /**
     * The part emits this when starting data.
     * If using a KIO::Job, it sets the job in the signal, so that
     * progress information can be shown. Otherwise, job is 0.
     **/
    void started( KIO::Job * );

    /**
     * Emit this when you have completed loading data.
     * Hosting apps will want to know when the process of loading the data
     * is finished, so that they can access the data when everything is loaded.
     **/
    void completed();

    /**
     * Same as the above signal except it indicates whether there is
     * a pending action to be executed on a delay timer. An example of
     * this is the meta-refresh tags on web pages used to reload/redirect
     * after a certain period of time. This signal is useful if you want
     * to give the user the ability to cancel such pending actions.
     *
     * @p pendingAction true if a pending action exists, false otherwise.
     */
    void completed( bool pendingAction );

    /**
     * Emit this if loading is canceled by the user or by an error.
     * @param errMsg the error message, empty if the user canceled the loading voluntarily.
     */
    void canceled( const QString &errMsg );


protected:
    /**
     * If the part uses the standard implementation of openUrl(),
     * it must reimplement this, to open the local file.
     * The default implementation is simply { return false; }
     */
    virtual bool openFile();

    /**
     * @internal
     */
    void abortLoad();

    /**
     * @internal
     */
#ifndef KDE_NO_DEPRECATED
    KDE_DEPRECATED bool isLocalFileTemporary() const;
#endif

    /**
     * @internal
     */
#ifndef KDE_NO_DEPRECATED
    KDE_DEPRECATED void setLocalFileTemporary( bool temp );
#endif

    /**
     * Sets the url associated with this part.
     */
    void setUrl(const KUrl &url);

    /**
     * Returns the local file path associated with this part.
     */
    QString localFilePath() const;

    /**
     * Sets the local file path associated with this part.
     */
    void setLocalFilePath( const QString &localFilePath );

protected:
    ReadOnlyPart(ReadOnlyPartPrivate &dd, QObject *parent);

private:
    Q_PRIVATE_SLOT(d_func(), void _k_slotJobFinished( KJob * job ))
    Q_PRIVATE_SLOT(d_func(), void _k_slotStatJobFinished(KJob*))
    Q_PRIVATE_SLOT(d_func(), void _k_slotGotMimeType(KIO::Job *job, const QString &mime))

    Q_DISABLE_COPY(ReadOnlyPart)
};
class ReadWritePartPrivate;

/**
 * Base class for an "editor" part.
 *
 * This class handles network transparency for you.
 * Anything that can open a URL, allow modifications, and save
 * (to the same URL or a different one).
 *
 * A read-write part can be set to read-only mode, using setReadWrite().
 *
 * Part writers :
 * Any part inheriting ReadWritePart should check isReadWrite
 * before allowing any action that modifies the part.
 * The part probably wants to reimplement setReadWrite, disable those
 * actions. Don't forget to call the parent setReadWrite.
 */
class KOMAIN_EXPORT ReadWritePart : public ReadOnlyPart
{
    Q_OBJECT

    KPARTS_DECLARE_PRIVATE(ReadWritePart)

public:
    /**
     * Constructor
     * See parent constructor for instructions.
     */
    explicit ReadWritePart( QObject *parent = 0 );
    /**
     * Destructor
     * Applications using a ReadWritePart should make sure, before
     * destroying it, to call closeUrl().
     * In KMainWindow::queryClose(), for instance, they should allow
     * closing only if the return value of closeUrl() was true.
     * This allows to cancel.
     */
    virtual ~ReadWritePart();

    /**
     * @return true if the part is in read-write mode
     */
    bool isReadWrite() const;

    /**
     * Changes the behavior of this part to readonly or readwrite.
     * @param readwrite set to true to enable readwrite mode
     */
    virtual void setReadWrite ( bool readwrite = true );

    /**
     * @return true if the document has been modified.
     */
    bool isModified() const;

    /**
     * If the document has been modified, ask the user to save changes.
     * This method is meant to be called from KMainWindow::queryClose().
     * It will also be called from closeUrl().
     *
     * @return true if closeUrl() can be called without the user losing
     * important data, false if the user chooses to cancel.
     */
    virtual bool queryClose();

    /**
     * Called when closing the current url (e.g. document), for instance
     * when switching to another url (note that openUrl() calls it
     * automatically in this case).
     *
     * If the current URL is not fully loaded yet, aborts loading.
     *
     * If isModified(), queryClose() will be called.
     *
     * @return false on cancel
     */
    virtual bool closeUrl();

    /**
     * Call this method instead of the above if you need control if
     * the save prompt is shown. For example, if you call queryClose()
     * from KMainWindow::queryClose(), you would not want to prompt
     * again when closing the url.
     *
     * Equivalent to promptToSave ? closeUrl() : ReadOnlyPart::closeUrl()
     */
    virtual bool closeUrl( bool promptToSave );

    /**
     * Save the file to a new location.
     *
     * Calls save(), no need to reimplement
     */
    virtual bool saveAs( const KUrl &url );

    /**
     *  Sets the modified flag of the part.
     */
    virtual void setModified( bool modified );

Q_SIGNALS:
    /**
     * set handled to true, if you don't want the default handling
     * set abortClosing to true, if you handled the request,
     * but for any reason don't  want to allow closing the document
     */
    void sigQueryClose(bool *handled, bool* abortClosing);

public Q_SLOTS:
    /**
     * Call setModified() whenever the contents get modified.
     * This is a slot for convenience, since it simply calls setModified(true),
     * so that you can connect it to a signal, like textChanged().
     */
    void setModified();

    /**
     * Save the file in the location from which it was opened.
     * You can connect this to the "save" action.
     * Calls saveFile() and saveToUrl(), no need to reimplement.
     */
    virtual bool save();

    /**
     * Waits for any pending upload job to finish and returns whether the
     * last save() action was successful.
     */
    bool waitSaveComplete();

protected:
    /**
     * Save to a local file.
     * You need to implement it, to save to the local file.
     * The framework takes care of re-uploading afterwards.
     *
     * @return true on success, false on failure.
     * On failure the function should inform the user about the
     * problem with an appropriate message box. Standard error
     * messages can be constructed using KIO::buildErrorString()
     * in combination with the error codes defined in kio/global.h
     */
    virtual bool saveFile() = 0;

    /**
     * Save the file.
     *
     * Uploads the file, if @p url is remote.
     * This will emit started(), and either completed() or canceled(),
     * in case you want to provide feedback.
     * @return true on success, false on failure.
     */
    virtual bool saveToUrl();

private:
    Q_PRIVATE_SLOT(d_func(), void _k_slotUploadFinished( KJob * job ))

    Q_DISABLE_COPY(ReadWritePart)
};

} // namespace


#undef KPARTS_DECLARE_PRIVATE

#endif
