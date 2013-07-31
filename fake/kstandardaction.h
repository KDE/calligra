#ifndef FAKE_KSTANDARDACTION_H
#define FAKE_KSTANDARDACTION_H

#include <kaction.h>
#include <ktoggleaction.h>
#include <kstandardshortcut.h>
#include <krecentfilesaction.h>

#include <QObject>
#include <QMetaObject>
#include <QMetaEnum>
#include "kofake_export.h"
class KOFAKE_EXPORT KStandardAction : public QObject
{
    Q_OBJECT
    Q_ENUMS(StandardAction)
public:
    enum StandardAction {
        ActionNone,

        // File Menu
        New, Open, OpenRecent, Save, SaveAs, Revert, Close,
        Print, PrintPreview, Mail, Quit,

        // Edit Menu
        Undo, Redo, Cut, Copy, Paste, SelectAll, Deselect, Find, FindNext, FindPrev,
        Replace,

        // View Menu
        ActualSize, FitToPage, FitToWidth, FitToHeight, ZoomIn, ZoomOut,
        Zoom, Redisplay,

        // Go Menu
        Up, Back, Forward, Home /*Home page*/, Prior, Next, Goto, GotoPage, GotoLine,
        FirstPage, LastPage, DocumentBack, DocumentForward,

        // Bookmarks Menu
        AddBookmark, EditBookmarks,

        // Tools Menu
        Spelling,

        // Settings Menu
        ShowMenubar, ShowToolbar, ShowStatusbar,
        SaveOptions, KeyBindings,
        Preferences, ConfigureToolbars,

        // Help Menu
        Help, HelpContents, WhatsThis, ReportBug, AboutApp, AboutKDE,
        TipofDay,

        // Other standard actions
        ConfigureNotifications,
        FullScreen,
        Clear,
        PasteText,
        SwitchApplicationLanguage
    };

    KOFAKE_EXPORT static QString name(StandardAction a);

#if 0
    /**
    * Creates an action corresponding to one of the
    * KStandardAction::StandardAction actions, which is connected to the given
    * object and @p slot, and is owned by @p parent.
    *
    * The signal that is connected to @p slot is triggered(bool), except for the case of
    * OpenRecent standard action, which uses the urlSelected(const KUrl &) signal of
    * KRecentFilesAction.
    *
    * @param id The StandardAction identifier to create a KAction for.
    * @param recvr The QObject to receive the signal, or 0 if no notification
    *              is needed.
    * @param slot  The slot to connect the signal to (remember to use the SLOT() macro).
    * @param parent The QObject that should own the created KAction, or 0 if no parent will
    *               own the KAction returned (ensure you delete it manually in this case).
    */
    static KAction* create(StandardAction id, const QObject *recvr, const char *slot, QObject *parent);

    /**
    * This will return the internal name of a given standard action.
    */
    static const char* name( StandardAction id );

    /**
    * Returns a list of all standard names. Used by KAccelManager
    * to give those heigher weight.
    */
    static QStringList stdNames();

    /**
    * Returns a list of all actionIds.
    *
    * @since 4.2
    */
    static QList<StandardAction> actionIds();

    /**
    * Returns the standardshortcut associated with @a actionId.
    *
    * @param actionId    The actionId whose associated shortcut is wanted.
    *
    * @since 4.2
    */
    static KStandardShortcut::StandardShortcut shortcutForActionId(StandardAction id);
#endif

    KOFAKE_EXPORT static KAction *openNew(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *open(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KRecentFilesAction *openRecent(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *save(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *saveAs(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *revert(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *close(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *print(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *printPreview(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *mail(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *quit(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *undo(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *redo(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *cut(QObject *parent);
    KOFAKE_EXPORT static KAction *copy(QObject *parent);
    KOFAKE_EXPORT static KAction *paste(QObject *parent);
    KOFAKE_EXPORT static KAction *clear(QObject *parent);
    KOFAKE_EXPORT static KAction *selectAll(QObject *parent);
    KOFAKE_EXPORT static KAction *cut(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *copy(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *paste(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *pasteText(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *clear(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *selectAll(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *deselect(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *find(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *findNext(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *findPrev(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *replace(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *actualSize(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *fitToPage(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *fitToWidth(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *fitToHeight(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *zoomIn(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *zoomOut(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *zoom(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *redisplay(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *up(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *back(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *forward(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *home(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *prior(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *next(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *goTo(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *gotoPage(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *gotoLine(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *firstPage(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *lastPage(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *documentBack(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *documentForward(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *addBookmark(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *editBookmarks(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *spelling(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KToggleAction *showMenubar(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KToggleAction *showStatusbar(const QObject *recvr, const char *slot, QObject *parent);
    //KOFAKE_EXPORT static KToggleFullScreenAction *fullScreen(const QObject *recvr, const char *slot, QWidget *window, QObject *parent) { return new KToggleFullScreenAction(parent); }
    KOFAKE_EXPORT static KAction *saveOptions(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *keyBindings(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *preferences(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *configureToolbars(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *configureNotifications(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *help(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *helpContents(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *whatsThis(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *tipOfDay(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *reportBug(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *aboutApp(const QObject *recvr, const char *slot, QObject *parent);
    KOFAKE_EXPORT static KAction *aboutKDE(const QObject *recvr, const char *slot, QObject *parent);

};

#endif
