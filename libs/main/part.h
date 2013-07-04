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
class QWidget;
class QEvent;
class QPoint;
struct QUnknownInterface;

class KJob;
namespace KIO {
  class Job;
}

namespace KoParts
{

class PartManager;
class PartPrivate;
class PartActivateEvent;
class PartSelectEvent;
class GUIActivateEvent;
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
     * Embed this part into a host widget.
     *
     * You don't need to do this if you created the widget with the
     * correct parent widget - this is just a QWidget::reparent().
     * Note that the Part is still the holder
     * of the QWidget, meaning that if you delete the Part,
     * then the widget gets destroyed as well, and vice-versa.
     * This method is not recommended since creating the widget with the correct
     * parent is simpler anyway.
     */
    virtual void embed( QWidget * parentWidget );

    /**
     * @return The widget defined by this part, set by setWidget().
     */
    virtual QWidget *widget();

    /**
     * @internal
     * Used by the part manager.
     */
    virtual void setManager( PartManager * manager );

    /**
     * Returns the part manager handling this part, if any (0L otherwise).
     */
    PartManager * manager() const;

    /**
     * By default, the widget is deleted by the part when the part is deleted.
     * The hosting application can call setAutoDeleteWidget(false) to
     * disable this behavior, given that the widget is usually deleted by
     * its parent widget anyway.
     * This is a method for the hosting application only, Part subclasses
     * should never call this.
     */
    void setAutoDeleteWidget(bool autoDeleteWidget);

    /**
     * By default, the part deletes itself when its widget is deleted.
     * The hosting application can call setAutoDeletePart(false) to
     * disable this behavior, to be able to delete the widget and then the part,
     * independently.
     * This is a method for the hosting application only, Part subclasses
     * should never call this.
     */
    void setAutoDeletePart(bool autoDeletePart);

    /**
     * Returns the part (this, or a child part) at the given global position.
     * This is called by the part manager to ask whether a part should be activated
     * when clicking somewhere. In most cases the default implementation is enough.
     * Reimplement this if your part has child parts in some areas (like in khtml or koffice)
     * @param widget the part widget being clicked - usually the same as widget(), except in koffice.
     * @param globalPos the mouse coordinates in global coordinates
     */
    virtual Part *hitTest( QWidget *widget, const QPoint &globalPos );

    /**
     *  @param selectable Indicates whether the part is selectable or not.
     */
    virtual void setSelectable( bool selectable );

    /**
     *  Returns whether the part is selectable or not.
     */
    bool isSelectable() const;

    /**
     * Use this icon loader to load any icons that are specific to this part,
     * i.e. icons installed into this part's own directories as opposed to standard
     * kde icons. Use KIcon("myicon", iconLoader()).
     *
     * Make sure to call setComponentData before calling iconLoader.
     */
    KIconLoader* iconLoader();

Q_SIGNALS:
    /**
     * Emitted by the part, to set the caption of the window(s)
     * hosting this part
     */
    void setWindowCaption( const QString & caption );
    /**
     * Emitted by the part, to set a text in the statusbar of the window(s)
     * hosting this part
     */
    void setStatusBarText( const QString & text );

protected:
    /**
     * Set the main widget.
     *
     * Call this in the Part-inherited class constructor.
     */
    virtual void setWidget( QWidget * widget );

    /**
     * @internal
     */
    virtual void customEvent( QEvent *event );

    /**
     * Convenience method which is called when the Part received a PartActivateEvent .
     * Reimplement this if you don't want to reimplement event and test for the event yourself
     * or even install an event filter.
     */
    virtual void partActivateEvent( PartActivateEvent *event );

    /**
     * Convenience method which is called when the Part received a
     * PartSelectEvent .
     * Reimplement this if you don't want to reimplement event and
     * test for the event yourself or even install an event filter.
     */
    virtual void partSelectEvent( PartSelectEvent *event );

    /**
     * Convenience method which is called when the Part received a
     * GUIActivateEvent .
     * Reimplement this if you don't want to reimplement event and
     * test for the event yourself or even install an event filter.
     */
    virtual void guiActivateEvent( GUIActivateEvent *event );

    /**
     * Convenience method for KXMLGUIFactory::container.
     * @return a container widget owned by the Part's GUI.
     */
    QWidget *hostContainer( const QString &containerName );

protected Q_SLOTS:
    /**
     * @internal
     */
    void slotWidgetDestroyed();

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
 * OpenUrlArguments is the set of arguments that specify
 * how a URL should be opened by KoParts::ReadOnlyPart::openUrl().
 *
 * For instance reload() indicates that the url should be loaded
 * from the network even if it matches the current url of the part.
 *
 * All setter methods in this class are for the class that calls openUrl
 * (usually the hosting application), all the getter methods are for the part.
 */
class KOMAIN_EXPORT OpenUrlArguments
{
public:
    OpenUrlArguments();
    OpenUrlArguments(const OpenUrlArguments &other);
    OpenUrlArguments &operator=( const OpenUrlArguments &other);
    ~OpenUrlArguments();

    /**
     * @return true to indicate that the part should reload the URL,
     * i.e. the cache shouldn't be used (forced reload).
     */
    bool reload() const;
    /**
     * Indicates that the url should be loaded
     * from the network even if it matches the current url of the part.
     */
    void setReload(bool b);

    /**
     * xOffset is the horizontal scrolling of the part's widget
     * (in case it's a scrollview). This is saved into the history
     * and restored when going back in the history.
     */
    int xOffset() const;
    void setXOffset(int x);

    /**
     * yOffset is the horizontal scrolling of the part's widget
     * (in case it's a scrollview). This is saved into the history
     * and restored when going back in the history.
     */
    int yOffset() const;
    void setYOffset(int y);

    /**
     * The mimetype to use when opening the url, when known by the calling application.
     */
    QString mimeType() const;
    void setMimeType(const QString& mime);

    /**
     * True if the user requested that the URL be opened.
     * False if the URL should be opened due to an external event, like javascript popups
     * or automatic redirections.
     * This is true by default
     * @since 4.1
     */
    bool actionRequestedByUser() const;
    void setActionRequestedByUser(bool userRequested);

    /**
     * Meta-data to associate with the KIO operation that will be used to open the URL.
     * This method can be used to add or retrieve metadata.
     * @see KIO::TransferJob etc.
     */
    QMap<QString, QString> &metaData();
    const QMap<QString, QString> &metaData() const;

private:
    QSharedDataPointer<OpenUrlArgumentsPrivate> d;
};


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
 * If you want another caption, set it in openFile() and
 * (if the part might ever be used with a part manager) in guiActivateEvent()
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

    /**
     * Call this to turn off the progress info dialog used by
     * the internal KIO job. Use this if you provide another way
     * of displaying progress info (e.g. a statusbar), using the
     * signals emitted by this class, and/or those emitted by
     * the Job given by started.
     */
    void setProgressInfoEnabled( bool show );

    /**
     * Returns whether the part shows the progress info dialog used by internal
     * KIO job.
     */
    bool isProgressInfoEnabled() const;

#ifndef KDE_NO_COMPAT
    void showProgressInfo( bool show );
#endif

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
     * Sets the arguments to use for the next openUrl call.
     */
    void setArguments(const OpenUrlArguments& arguments);
    // TODO to avoid problems with the case where the loading fails, this could also be a openUrl() argument (heavy porting!).
    // However we need to have setArguments in any case for updated made by the part, see e.g. KHTMLPart::openUrl.
    // Well, maybe we should have setArguments (affects next openurl call) and updateArguments?


    /**
     * @return the arguments that were used to open this URL.
     */
    OpenUrlArguments arguments() const;

public:
    /**
     * Initiate sending data to this part.
     * This is an alternative to openUrl, which allows the user of the part
     * to load the data itself, and send it progressively to the part.
     *
     * @param mimeType the type of data that is going to be sent to this part.
     * @param url the URL representing this data. Although not directly used,
     * every ReadOnlyPart has a URL (see url()), so this simply sets it.
     * @return true if the part supports progressive loading and accepts data, false otherwise.
     */
    bool openStream( const QString& mimeType, const KUrl& url );

    /**
     * Send some data to the part. openStream must have been called previously,
     * and must have returned true.
     * @return true if the data was accepted by the part. If false is returned,
     * the application should stop sending data, and doesn't have to call closeStream.
     */
    bool writeStream( const QByteArray& data );

    /**
     * Terminate the sending of data to the part.
     * With some data types (text, html...) closeStream might never actually be called,
     * in the case of continuous streams, for instance plain text or HTML data.
     */
    bool closeStream();

private: // Makes no sense for inherited classes to call those. But make it protected there.

    /**
     * Called by openStream to initiate sending of data.
     * Parts which implement progress loading should check the @p mimeType
     * parameter, and return true if they can accept a data stream of that type.
     */
    virtual bool doOpenStream( const QString& /*mimeType*/ ) { return false; }
    /**
     * Receive some data from the hosting application.
     * In this method the part should attempt to display the data progressively.
     * With some data types (text, html...) closeStream might never actually be called,
     * in the case of continuous streams. This can't happen with e.g. images.
     */
    virtual bool doWriteStream( const QByteArray& /*data*/ ) { return false; }
    /**
     * This is called by closeStream(), to indicate that all the data has been sent.
     * Parts should ensure that all of the data is displayed at this point.
     * @return whether the data could be displayed correctly.
     */
    virtual bool doCloseStream() { return false; }

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

    /**
     * Emitted by the part when url() changes
     * @since 4.10
     */
    void urlChanged( const KUrl & url );

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
     * Reimplemented from Part, so that the window caption is set to
     * the current url (decoded) when the part is activated
     * This is the usual behavior in 99% of the apps
     * Reimplement if you don't like it - test for event->activated() !
     *
     * Technical note : this is done with GUIActivateEvent and not with
     * PartActivateEvent because it's handled by the mainwindow
     * (which gets the even after the PartActivateEvent events have been sent)
     */
    virtual void guiActivateEvent( GUIActivateEvent *event );

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
