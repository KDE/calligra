#ifndef FAKE_KSTANDARDACTION_H
#define FAKE_KSTANDARDACTION_H

#include <kaction.h>
#include <ktoggleaction.h>
#include <kstandardshortcut.h>
#include <krecentfilesaction.h>

#include <QObject>
#include <QMetaObject>
#include <QMetaEnum>

class KStandardAction : public QObject
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

    static QString name(StandardAction a)
    {
        KStandardAction sa;
        int idx = sa.metaObject()->indexOfEnumerator("StandardAction");
        Q_ASSERT(idx >= 0);
        QMetaEnum e = sa.metaObject()->enumerator(idx);
        return QString::fromLatin1(e.valueToKey(int(a)));
    }

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

    static KAction *openNew(const QObject *recvr, const char *slot, QObject *parent)
    {
        return new KAction(parent);
    }

    static KAction *open(const QObject *recvr, const char *slot, QObject *parent)
    {
        return new KAction(parent);
    }

    static KRecentFilesAction *openRecent(const QObject *recvr, const char *slot, QObject *parent)
    {
        KRecentFilesAction *a = new KRecentFilesAction(i18n("Open recent"), parent);
        connect(a, SIGNAL(triggered(bool)), recvr, slot);
        return a;
    }

    static KAction *save(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *saveAs(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *revert(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *close(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *print(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *printPreview(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *mail(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *quit(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *undo(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *redo(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *cut(QObject *parent) { return new KAction(parent); }
    static KAction *copy(QObject *parent) { return new KAction(parent); }
    static KAction *paste(QObject *parent) { return new KAction(parent); }
    static KAction *clear(QObject *parent) { return new KAction(parent); }
    static KAction *selectAll(QObject *parent) { return new KAction(parent); }
    static KAction *cut(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *copy(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *paste(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *pasteText(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *clear(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *selectAll(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *deselect(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *find(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *findNext(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *findPrev(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *replace(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *actualSize(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *fitToPage(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *fitToWidth(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *fitToHeight(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *zoomIn(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *zoomOut(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *zoom(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *redisplay(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *up(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *back(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *forward(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *home(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *prior(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *next(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *goTo(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *gotoPage(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *gotoLine(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *firstPage(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *lastPage(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *documentBack(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *documentForward(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *addBookmark(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *editBookmarks(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *spelling(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KToggleAction *showMenubar(const QObject *recvr, const char *slot, QObject *parent) { return new KToggleAction(parent); }
    static KToggleAction *showStatusbar(const QObject *recvr, const char *slot, QObject *parent) { return new KToggleAction(parent); }
    //static KToggleFullScreenAction *fullScreen(const QObject *recvr, const char *slot, QWidget *window, QObject *parent) { return new KToggleFullScreenAction(parent); }

    static KAction *saveOptions(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *keyBindings(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *preferences(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *configureToolbars(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *configureNotifications(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *help(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *helpContents(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *whatsThis(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *tipOfDay(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *reportBug(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *aboutApp(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
    static KAction *aboutKDE(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }

};

#endif
