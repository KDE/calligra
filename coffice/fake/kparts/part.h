#ifndef FAKE_KPARTS_PART_H
#define FAKE_KPARTS_PART_H

#include <QDomElement>

#include <klocale.h>
#include <kurl.h>
#include <kio/netaccess.h>
#include <kconfiggroup.h>
#include <kcomponentdata.h>
#include <kactioncollection.h>
#include <kxmlguiclient.h>
#include <kiconloader.h>
#include <kparts/event.h>
//#include <kparts/partmanager.h>
#include <kparts/browserextension.h>

namespace KParts {

    class PartManager;

    class OpenUrlArguments
    {
    public:
#if 0
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
#endif

        QString mimeType() const { return m_mime; }
        void setMimeType(const QString& mime) { m_mime = mime; }

#if 0
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
#endif
    private:
        QString m_mime;
    };

    class PartBase : virtual public KXMLGUIClient
    {
    public:
        PartBase() : m_object(0) {}
        virtual ~PartBase() {}
        void setPartObject( QObject *object ) { m_object = object; }
        QObject *partObject() const { return m_object; }
    protected:
        virtual void setComponentData(const KComponentData &componentData) {}
        virtual void setComponentData(const KComponentData &componentData, bool loadPlugins) {}
        enum PluginLoadingMode { DoNotLoadPlugins = 0, LoadPlugins = 1, LoadPluginsIfEnabled = 2 };
        void loadPlugins(QObject *parent, KXMLGUIClient *parentGUIClient, const KComponentData &componentData) {}
        void setPluginLoadingMode( PluginLoadingMode loadingMode ) {}
        void setPluginInterfaceVersion( int version ) {}
    private:
        QObject *m_object;
    };

    class Part : public QObject, public PartBase
    {
        Q_OBJECT
    public:
        Part(QObject *parent = 0) : QObject(parent), PartBase(), m_manager(0) {}
        virtual void embed( QWidget * parentWidget ) {}
        virtual QWidget *widget() { return 0; }
        virtual void setManager( PartManager * manager ) { m_manager = manager; }
        PartManager * manager() const { return m_manager; }
        void setAutoDeleteWidget(bool autoDeleteWidget) {}
        void setAutoDeletePart(bool autoDeletePart) {}
        virtual Part *hitTest( QWidget *widget, const QPoint &globalPos ) { return 0; }
        virtual void setSelectable( bool selectable ) {}
        bool isSelectable() const { return false; }
        KIconLoader* iconLoader() { return 0; }
    protected:
        void loadPlugins() {}
    private:
        PartManager *m_manager;
    };

    class ReadOnlyPart : public Part
    {
        Q_OBJECT
    public:
        ReadOnlyPart( QObject *parent = 0 ) : Part(parent) {}
        void setProgressInfoEnabled( bool show ) {}
        bool isProgressInfoEnabled() const { return false; }
        void showProgressInfo( bool show ) {}
    public Q_SLOTS:
        virtual bool openUrl( const KUrl &url ) {
            qDebug() << Q_FUNC_INFO;
            if (!QFile(url.toLocalFile()).exists()) {
                qDebug() << Q_FUNC_INFO << "No such file" << url;
                return false;
            }
            m_url = url;
            m_args = OpenUrlArguments();
            return true;
        }
    public:
        KUrl url() const { return m_url; }
        virtual bool closeUrl() { return true; }
        BrowserExtension* browserExtension() const { return 0; }
        void setArguments(const OpenUrlArguments& arguments) { m_args = arguments; }
        OpenUrlArguments arguments() const { return m_args; }
    public:
        bool openStream( const QString& mimeType, const KUrl& url ) {
            qDebug() << Q_FUNC_INFO;
            return true;
        }
        bool writeStream( const QByteArray& data ) {
            qDebug() << Q_FUNC_INFO;
            return true;
        }
        bool closeStream() {
            qDebug() << Q_FUNC_INFO;
            return true;
        }
    private: // Makes no sense for inherited classes to call those. But make it protected there.
        virtual bool doOpenStream( const QString& /*mimeType*/ ) { return false; }
        virtual bool doWriteStream( const QByteArray& /*data*/ ) { return false; }
        virtual bool doCloseStream() { return false; }
    protected:
        virtual bool openFile() {
            qDebug() << Q_FUNC_INFO;
            return true;
        }
        void abortLoad() {}
        virtual void guiActivateEvent( GUIActivateEvent *event ) {}
        // #ifndef KDE_NO_DEPRECATED
        //     KDE_DEPRECATED bool isLocalFileTemporary() const;
        //     KDE_DEPRECATED void setLocalFileTemporary( bool temp );
        // #endif
        void setUrl(const KUrl &url) { m_url = url; }
        QString localFilePath() const { return m_url.toLocalFile(); }
        void setLocalFilePath( const QString &localFilePath ) { m_url.setUrl(localFilePath); }
    private:
        KUrl m_url;
        OpenUrlArguments m_args;
    };

    class ReadWritePart : public ReadOnlyPart
    {
        Q_OBJECT
    public:
        ReadWritePart(QObject *parent = 0) : ReadOnlyPart(parent), m_isReadWrite(true), m_modified(false) {}
        bool isReadWrite() const { return m_isReadWrite; }
        virtual void setReadWrite ( bool readwrite = true ) { m_isReadWrite = readwrite; }
        bool isModified() const { return m_modified; }
        virtual bool queryClose() {
            qDebug() << Q_FUNC_INFO;
            return true;
        }
        virtual bool closeUrl() {
            qDebug() << Q_FUNC_INFO;
            return true;
        }
        virtual bool closeUrl( bool promptToSave ) {
            qDebug() << Q_FUNC_INFO;
            return true;
        }
        virtual bool saveAs( const KUrl &url ) {
            qDebug() << Q_FUNC_INFO;
            return true;
        }
        virtual void setModified( bool modified ) { m_modified = modified; }
    public Q_SLOTS:
        void setModified() { m_modified = true; }
        virtual bool save() {
            qDebug() << Q_FUNC_INFO;
            return true;
        }
        bool waitSaveComplete() { return true; }
    protected:
        virtual bool saveFile() = 0;
        virtual bool saveToUrl() { return save(); }
    private:
        bool m_isReadWrite;
        bool m_modified;
    };

    class PartManager : public QObject
    {
    public:
        enum SelectionPolicy { Direct, TriState };
        enum Reason { ReasonLeftClick = 100, ReasonMidClick, ReasonRightClick, NoReason };
        PartManager(QObject *parent = 0) : QObject(parent), m_selectionPolicy(Direct), m_allowNestedParts(true), m_ignoreScrollBars(false) {}
        QList<Part*> parts() const { return m_parts; }
        void addPart(Part *part, bool = true) { m_parts.append(part); }
        void removePart(Part *part) { m_parts.removeAll(part); }
        void setSelectionPolicy( SelectionPolicy policy ) { m_selectionPolicy = policy; }
        SelectionPolicy selectionPolicy() const { return m_selectionPolicy; }
        void setAllowNestedParts( bool allow ) { m_allowNestedParts = allow; }
        bool allowNestedParts() const { return m_allowNestedParts; }
        void setIgnoreScrollBars( bool ignore ) { m_ignoreScrollBars = ignore; }
        bool ignoreScrollBars() const { return m_ignoreScrollBars; }
        void setActivePart( Part *part, QWidget *widget = 0 ) {}
        QWidget* activeWidget() const { return 0; }
    private:
        QList<Part*> m_parts;
        SelectionPolicy m_selectionPolicy;
        bool m_allowNestedParts;
        bool m_ignoreScrollBars;
    };
}

#endif
