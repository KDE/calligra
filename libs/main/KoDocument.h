/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2000-2005 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KODOCUMENT_H
#define KODOCUMENT_H

#include <QDateTime>
#include <QList>

#include "komain_export.h"
#include <KoDocumentBase.h>
#include <KoXmlReaderForward.h>
#include <kundo2stack.h>

class KUndo2Command;
class KoPart;
class KoStore;
class KoDocumentInfo;
class KoDocumentRdf;
class KoDocumentRdfBase;
class KoProgressUpdater;
class KoProgressProxy;
class KoDocumentInfoDlg;
class KoUnit;
class KoGridData;
class KoGuidesData;
class KoXmlWriter;

class QDomDocument;

// MSVC seems to need to know the declaration of the classes
// we pass references of in, when used by external modules
// e.g.
//     when building chartshapecore.lib, the forward-declaration
//     approach lead to unresolved externals warnings when it used
//     the pagelayout functions.
//     Also when building calligra_shape_formular.dll - FormulaDocument
//     referenced the same two pagelayout functions incorrectly.
#if defined(_WIN32) || defined(_WIN64)
#include <KoPageLayout.h>
#else
struct KoPageLayout;
#endif

class KoVersionInfo
{
public:
    QDateTime date;
    QString saved_by;
    QString comment;
    QString title;

    QByteArray data; // the content of the compressed version
};

/**
 *  The %Calligra document class
 *
 *  This class provides some functionality each %Calligra document should have.
 *
 *  @short The %Calligra document class
 */
class KOMAIN_EXPORT KoDocument : public QObject, public KoDocumentBase
{
    Q_OBJECT
    Q_PROPERTY(bool backupFile READ backupFile WRITE setBackupFile)
    Q_PROPERTY(int pageCount READ pageCount)

public:
    /**
     * Constructor.
     *
     * @param parent The KoPart that owns the document. XXX: should be removed!
     * @param undoStack accepts the stack for the document. You can create any type of stack if you need.
     *        The stack objects will become owned by the document. This is used by Krita's KisDoc2. The default value for this
     *        parameter is a usual Qt's stack.
     */
    explicit KoDocument(KoPart *parent, KUndo2Stack *undoStack = new KUndo2Stack());

    /**
     *  Destructor.
     *
     * The destructor does not delete any attached KoView objects and it does not
     * delete the attached widget as returned by widget().
     */
    ~KoDocument() override;

    /// XXX: Temporary!
    KoPart *documentPart() const;

    /**
     * Reimplemented from KoParts::ReadWritePart for internal reasons
     * (for the autosave functionality)
     */
    virtual bool openUrl(const QUrl &url);

    /**
     * Opens the document given by @p url, without storing the URL
     * in the KoDocument.
     * Call this instead of openUrl() to implement KoMainWindow's
     * File --> Import feature.
     *
     * @note This will call openUrl(). To differentiate this from an ordinary
     *       Open operation (in any reimplementation of openUrl() or openFile())
     *       call isImporting().
     */
    bool importDocument(const QUrl &url);

    /**
     * Saves the document as @p url without changing the state of the
     * KoDocument (URL, modified flag etc.). Call this instead of
     * KoParts::ReadWritePart::saveAs() to implement KoMainWindow's
     * File --> Export feature.
     *
     * @note This will call KoDocument::saveAs(). To differentiate this
     *       from an ordinary Save operation (in any reimplementation of
     *       saveFile()) call isExporting().
     */
    bool exportDocument(const QUrl &url);

    /**
     * @brief Sets whether the document can be edited or is read only.
     *
     * This recursively applied to all child documents and
     * KoView::updateReadWrite is called for every attached
     * view.
     */
    virtual void setReadWrite(bool readwrite = true);

    /**
     * To be preferred when a document exists. It is fast when calling
     * it multiple times since it caches the result that readNativeFormatMimeType()
     * delivers.
     * This comes from the X-KDE-NativeMimeType key in the .desktop file.
     */
    virtual QByteArray nativeFormatMimeType() const = 0;

    /**
     * Returns the OASIS OpenDocument mimetype of the document, if supported
     * This comes from the X-KDE-NativeOasisMimeType key in the
     * desktop file
     *
     * @return the oasis mimetype or, if it hasn't one, the nativeformatmimetype.
     */
    QByteArray nativeOasisMimeType() const override = 0;

    /// Checks whether a given mimetype can be handled natively.
    bool isNativeFormat(const QByteArray &mimetype) const;

    /// Returns a list of the mimetypes considered "native", i.e. which can
    /// be saved by KoDocument without a filter, in *addition* to the main one
    virtual QStringList extraNativeMimeTypes() const = 0;

    /**
     * Return the set of SupportedSpecialFormats that the application wants to
     * offer in the "Save" file dialog.
     */
    int supportedSpecialFormats() const override;

    /**
     * Returns the actual mimetype of the document
     */
    QByteArray mimeType() const override;

    /**
     * @brief Sets the mime type for the document.
     *
     * When choosing "save as" this is also the mime type
     * selected by default.
     */
    void setMimeType(const QByteArray &mimeType) override;

    /**
     * @brief Set the format in which the document should be saved.
     *
     * This is called on loading, and in "save as", so you shouldn't
     * have to call it.
     *
     * @param mimeType the mime type (format) to use.
     * @param specialOutputFlag is for "save as older version" etc.
     */
    void setOutputMimeType(const QByteArray &mimeType, int specialOutputFlag = 0) override;
    QByteArray outputMimeType() const override;
    int specialOutputFlag() const override;

    /**
     * Returns true if this document was the result of opening a foreign
     * file format and if the user hasn't yet saved the document (in any
     * format).
     *
     * Used by KoMainWindow to warn the user when s/he lazily presses
     * CTRL+S to save in the same foreign format, putting all his/her
     * formatting at risk (normally an export confirmation only comes up
     * with Save As).
     *
     * @param exporting specifies whether this is the setting for a
     * File --> Export or File --> Save/Save As operation.
     */
    bool confirmNonNativeSave(const bool exporting) const;
    void setConfirmNonNativeSave(const bool exporting, const bool on);

    /**
     * @return true if saving/exporting should inhibit the option dialog
     */
    bool saveInBatchMode() const;

    /**
     * @param batchMode if true, do not show the option dialog when saving or exporting.
     */
    void setSaveInBatchMode(const bool batchMode);

    /**
     * Sets the error message to be shown to the user (use i18n()!)
     * when loading or saving fails.
     * If you asked the user about something and they chose "Cancel",
     * set the message to the magic string "USER_CANCELED", to skip the error dialog.
     */
    void setErrorMessage(const QString &errMsg);

    /**
     * Return the last error message. Usually KoDocument takes care of
     * showing it; this method is mostly provided for non-interactive use.
     */
    QString errorMessage() const;

    /**
     * Show the last error message in a message box.
     * The dialog box will mention a loading problem.
     * openUrl/openFile takes care of doing it, but not loadNativeFormat itself,
     * so this is often called after loadNativeFormat returned false.
     */
    void showLoadingErrorDialog();

    /**
     * @brief Generates a preview picture of the document
     * @note The preview is used in the File Dialog and also to create the Thumbnail
     */
    virtual QPixmap generatePreview(const QSize &size);

    /**
     *  Paints the data itself.
     *  It's this method that %Calligra Parts have to implement.
     *
     *  @param painter     The painter object onto which will be drawn.
     *  @param rect        The rect that should be used in the painter object.
     */
    virtual void paintContent(QPainter &painter, const QRect &rect) = 0;

    /**
     *  Tells the document that its title has been modified, either because
     *  the modified status changes (this is done by setModified() ) or
     *  because the URL or the document-info's title changed.
     */
    void setTitleModified();

    /**
     *  @return true if the document is empty.
     */
    bool isEmpty() const override;

    /**
     *  @brief Sets the document to empty.
     *
     *  Used after loading a template
     *  (which is not empty, but not the user's input).
     *
     *  @see isEmpty()
     */
    virtual void setEmpty();

    /**
     *  @brief Loads a document from a store.
     *
     *  You should never have to reimplement.
     *
     *  @param store The store to load from
     *  @param url An internal url, like tar:/1/2
     */
    virtual bool loadFromStore(KoStore *store, const QString &url);

    /**
     *  @brief Loads an OASIS document from a store.
     *  This is used for both the main document and embedded objects.
     */
    bool loadOasisFromStore(KoStore *store) override;

    /**
     *  @brief Saves a sub-document to a store.
     *
     *  You should not have to reimplement this.
     */
    bool saveToStore(KoStore *store, const QString &path) override;

    /**
     *  Reimplement this method to load the contents of your Calligra document,
     *  from the XML document. This is for the pre-Oasis file format (maindoc.xml).
     */
    virtual bool loadXML(const KoXmlDocument &doc, KoStore *store) = 0;

    /**
     *  Reimplement this to save the contents of the %Calligra document into
     *  a QDomDocument. The framework takes care of saving it to the store.
     */
    virtual QDomDocument saveXML();

    /**
     *  Return a correctly created QDomDocument for this KoDocument,
     *  including processing instruction, complete DOCTYPE tag (with systemId and publicId), and root element.
     *  @param tagName the name of the tag for the root element
     *  @param version the DTD version (usually the application's version).
     */
    QDomDocument createDomDocument(const QString &tagName, const QString &version) const;

    /**
     *  Return a correctly created QDomDocument for an old (1.3-style) %Calligra document,
     *  including processing instruction, complete DOCTYPE tag (with systemId and publicId), and root element.
     *  This static method can be used e.g. by filters.
     *  @param appName the app's instance name, e.g. words, kspread, kpresenter etc.
     *  @param tagName the name of the tag for the root element, e.g. DOC for words/kpresenter.
     *  @param version the DTD version (usually the application's version).
     */
    static QDomDocument createDomDocument(const QString &appName, const QString &tagName, const QString &version);

    /**
     *  The first thing to do in loadOasis is get hold of the office:body tag, then its child.
     *  If the child isn't the expected one, the error message can indicate what it is instead.
     *  This method returns a translated name for the type of document,
     *  e.g. i18n("Word Processing") for office:text.
     */
    static QString tagNameToDocumentType(const QString &localName);

    /**
     *  Loads a document in the native format from a given URL.
     *  Reimplement if your native format isn't XML.
     *
     *  @param file the file to load - usually KReadOnlyPart::m_file or the result of a filter
     */
    virtual bool loadNativeFormat(const QString &file);

    /**
     *  Saves the document in native format, to a given file
     *  You should never have to reimplement.
     *  Made public for writing templates.
     */
    virtual bool saveNativeFormat(const QString &file);

    /**
     * Saves the document in native ODF format to the given store.
     */
    bool saveNativeFormatODF(KoStore *store, const QByteArray &mimeType);

    /**
     * Saves the document in the native format to the given store.
     */
    bool saveNativeFormatCalligra(KoStore *store);

    /**
     * Activate/deactivate/configure the autosave feature.
     * @param delay in seconds, 0 to disable
     */
    void setAutoSave(int delay);

    /**
     * Checks whether the document is currently in the process of autosaving
     */
    bool isAutosaving() const override;

    /**
     * Set whether the next openUrl call should check for an auto-saved file
     * and offer to open it. This is usually true, but can be turned off
     * (e.g. for the preview module). This only checks for names auto-saved
     * files, unnamed auto-saved files are only checked on KoApplication startup.
     */
    void setCheckAutoSaveFile(bool b);

    /**
     * Set whether the next openUrl call should show error message boxes in case
     * of errors. This is usually the case, but e.g. not when generating thumbnail
     * previews.
     */
    void setAutoErrorHandlingEnabled(bool b);

    /**
     * Checks whether error message boxes should be shown.
     */
    bool isAutoErrorHandlingEnabled() const;

    /**
     * Retrieve the default value for autosave in seconds.
     * Called by the applications to use the correct default in their config
     */
    static int defaultAutoSave();

    /**
     * @return the information concerning this document.
     * @see KoDocumentInfo
     */
    KoDocumentInfo *documentInfo() const;

    /**
     * @return the Rdf metadata for this document.
     * This method should only be used by code that links to
     * the RDF system and needs full access to the KoDocumentRdf object.
     * @see KoDocumentRdf
     */
    KoDocumentRdfBase *documentRdf() const;

    /**
     * Replace the current rdf document with the given rdf document. The existing RDF document
     * will be deleted, and if RDF support is compiled out, KoDocument does not take ownership.
     * Otherwise, KoDocument will own the rdf document.
     */
    void setDocumentRdf(KoDocumentRdfBase *rdfDocument);

    /**
     * @return the object to report progress to.
     * One can add more KoUpdaters to it to make the progress reporting more
     * accurate. If no active progress reporter is present, 0 is returned.
     **/
    KoProgressUpdater *progressUpdater() const;

    /**
     * Set a custom progress proxy to use to report loading
     * progress to.
     */
    void setProgressProxy(KoProgressProxy *progressProxy);
    KoProgressProxy *progressProxy() const;

    /**
     * Return true if url() is a real filename, false if url() is
     * an internal url in the store, like "tar:/..."
     */
    bool isStoredExtern() const override;

    /**
     * @return the page layout associated with this document (margins, pageSize, etc).
     * Override this if you want to provide different sized pages.
     *
     * @see KoPageLayout
     */
    virtual KoPageLayout pageLayout(int pageNumber = 0) const;
    virtual void setPageLayout(const KoPageLayout &pageLayout);

    /**
     * Performs a cleanup of unneeded backup files
     */
    void removeAutoSaveFiles();

    void setBackupFile(bool _b);

    bool backupFile() const;

    /**
     * Returns true if this document or any of its internal child documents are modified.
     */
    Q_INVOKABLE bool isModified() const override;

    /**
     * Returns true during loading (openUrl can be asynchronous)
     */
    bool isLoading() const;

    int queryCloseDia();

    /**
     * Sets the backup path of the document
     */
    void setBackupPath(const QString &_path);

    /**
     * @return path to the backup document
     */
    QString backupPath() const;

    /**
     * @return caption of the document
     *
     * Caption is of the form "[title] - [url]",
     * built out of the document info (title) and pretty-printed
     * document URL.
     * If the title is not present, only the URL it returned.
     */
    QString caption() const;

    /**
     * Sets the document URL to empty URL
     * KParts doesn't allow this, but %Calligra apps have e.g. templates
     * After using loadNativeFormat on a template, one wants
     * to set the url to QUrl()
     */
    void resetURL() override;

    bool hasExternURL() const;

    /**
     * @internal (public for KoMainWindow)
     */
    void setMimeTypeAfterLoading(const QString &mimeType);

    /**
     * @return returns the number of pages in the document.
     */
    virtual int pageCount() const;

    /**
     * Returns the unit used to display all measures/distances.
     */
    KoUnit unit() const;

    /**
     * Sets the unit used to display all measures/distances.
     */
    void setUnit(const KoUnit &unit);

    /**
     * Save the unit to the settings writer
     *
     * @param settingsWriter
     */
    void saveUnitOdf(KoXmlWriter *settingsWriter) const;

    QList<KoVersionInfo> &versionList();

    bool loadNativeFormatFromStore(QByteArray &data);

    /**
     * Adds a new version and then saves the whole document.
     * @param comment the comment for the version
     * @return true on success, otherwise false
     */
    bool addVersion(const QString &comment);

    /// return the grid data for this document.
    KoGridData &gridData();

    /// returns the guides data for this document.
    KoGuidesData &guidesData();

    void clearUndoHistory();

    /**
     *  Sets the modified flag on the document. This means that it has
     *  to be saved or not before deleting it.
     */
    Q_INVOKABLE virtual void setModified(bool _mod);

    /**
     * Initialize an empty document using default values
     */
    virtual void initEmpty();

    /**
     * Returns the global undo stack
     */
    KUndo2Stack *undoStack();

    /**
     * Set the output stream to report profile information to.
     */
    void setProfileStream(QTextStream *profilestream);

    /**
     * Set the output stream to report profile information to.
     */
    void setProfileReferenceTime(const QTime &referenceTime);

    /// If set, the document shall be saved even if it is not marked as modified.
    /// @see setAlwaysAllowSaving()
    bool alwaysAllowSaving() const;

    /// Set alwaysAllowSaving to @p allow.
    /// Enables applications to always allow saving even when document is not modified.
    /// This makes it possible to save settings/context info without marking
    /// the document as modified.
    /// @see alwaysAllowSaving()
    void setAlwaysAllowSaving(bool allow);

public Q_SLOTS:

    /**
     * Adds a command to the undo stack and executes it by calling the redo() function.
     * @param command command to add to the undo stack
     */
    virtual void addCommand(KUndo2Command *command);

    /**
     * Begins recording of a macro command. At the end endMacro needs to be called.
     * @param text command description
     */
    virtual void beginMacro(const KUndo2MagicString &text);

    /**
     * Ends the recording of a macro command.
     */
    virtual void endMacro();

Q_SIGNALS:

    /**
     * This signal is emitted when the unit is changed by setUnit().
     * It is common to connect views to it, in order to change the displayed units
     * (e.g. in the rulers)
     */
    void unitChanged(const KoUnit &unit);

    /**
     * Progress info while loading or saving. The value is in percents (i.e. a number between 0 and 100)
     * Your KoDocument-derived class should emit the signal now and then during load/save.
     * KoMainWindow will take care of displaying a progress bar automatically.
     */
    void sigProgress(int value);

    /**
     * Emitted e.g. at the beginning of a save operation
     * This is emitted by KoDocument and used by KoView to display a statusbar message
     */
    void statusBarMessage(const QString &text);

    /**
     * Emitted e.g. at the end of a save operation
     * This is emitted by KoDocument and used by KoView to clear the statusbar message
     */
    void clearStatusBarMessage();

    /**
     * Emitted when the document is modified
     */
    void modified(bool);

    void titleModified(const QString &caption, bool isModified);

protected:
    friend class KoPart;

    /**
     * Generate a name for the document.
     */
    QString newObjectName();

    QString autoSaveFile(const QString &path) const;
    void setDisregardAutosaveFailure(bool disregardFailure);

    /**
     *  Loads a document from KReadOnlyPart::m_file (KParts takes care of downloading
     *  remote documents).
     *  Applies a filter if necessary, and calls loadNativeFormat in any case
     *  You should not have to reimplement, except for very special cases.
     *
     * NOTE: this method also creates a new KoView instance!
     *
     * This method is called from the KReadOnlyPart::openUrl method.
     */
    virtual bool openFile();

    /**
     * This method is called by @a openFile() to allow applications to setup there
     * own KoProgressUpdater-subTasks which are then taken into account for the
     * displayed progressbar during loading.
     */
    virtual void setupOpenFileSubProgress();

    /**
     *  Saves a document to KReadOnlyPart::m_file (KParts takes care of uploading
     *  remote documents)
     *  Applies a filter if necessary, and calls saveNativeFormat in any case
     *  You should not have to reimplement, except for very special cases.
     */
    virtual bool saveFile();

    /**
     *  Overload this function if you have to load additional files
     *  from a store. This function is called after loadXML()
     *  and after loadChildren() have been called.
     */
    virtual bool completeLoading(KoStore *store);

    /**
     *  If you want to write additional files to a store,
     *  then you must do it here.
     *  In the implementation, you should prepend the document
     *  url (using url().url()) before the filename, so that everything is kept relative
     *  to this document. For instance it will produce urls such as
     *  tar:/1/pictures/picture0.png, if the doc url is tar:/1
     *  But do this ONLY if the document is not stored extern (see isStoredExtern() ).
     *  If it is, then the pictures should be saved to tar:/pictures.
     */
    virtual bool completeSaving(KoStore *store);

    /** @internal */
    virtual void setModified();

    /**
     *  Returns whether or not the current openUrl() or openFile() call is
     *  actually an import operation (like File --> Import).
     *  This is for informational purposes only.
     */
    bool isImporting() const;

    /**
     *  Returns whether or not the current saveFile() call is actually an export
     *  operation (like File --> Export).
     *  If this function returns true during saveFile() and you are changing
     *  some sort of state, you _must_ restore it before the end of saveFile();
     *  otherwise, File --> Export will not work properly.
     */
    bool isExporting() const;

public:
    /// Returns a valid default url
    QUrl defaultUrl() const;

    QString localFilePath() const override;
    void setLocalFilePath(const QString &localFilePath);

    virtual KoDocumentInfoDlg *createDocumentInfoDialog(QWidget *parent, KoDocumentInfo *docInfo) const;

    bool isReadWrite() const;

    QUrl url() const override;
    void setUrl(const QUrl &url) override;

    virtual bool closeUrl(bool promptToSave = true);

    virtual bool saveAs(const QUrl &url);

public Q_SLOTS:

    virtual bool save();
    bool waitSaveComplete();

Q_SIGNALS:

    void completed();
    void canceled(const QString &);

private Q_SLOTS:

    void slotAutoSave();

    /// Called by the undo stack when undo or redo is called
    void slotUndoStackIndexChanged(int idx);

protected:
    bool oldLoadAndParse(KoStore *store, const QString &filename, KoXmlDocument &doc);

private:
    bool saveToStream(QIODevice *dev);

    QString checkImageMimeTypes(const QString &mimeType, const QUrl &url) const;

    bool loadNativeFormatFromStore(const QString &file);
    bool loadNativeFormatFromStoreInternal(KoStore *store);

    bool savePreview(KoStore *store);
    bool saveOasisPreview(KoStore *store, KoXmlWriter *manifestWriter);

    QString prettyPathOrUrl() const;

    bool queryClose();
    bool saveToUrl();
    bool openUrlInternal(const QUrl &url);

    void abortLoad();

    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_slotJobFinished(KJob *job))
    Q_PRIVATE_SLOT(d, void _k_slotStatJobFinished(KJob *))
    Q_PRIVATE_SLOT(d, void _k_slotGotMimeType(KIO::Job *job, const QString &mime))
    Q_PRIVATE_SLOT(d, void _k_slotUploadFinished(KJob *job))
};

Q_DECLARE_METATYPE(KoDocument *)

#endif
