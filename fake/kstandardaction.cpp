#include "kstandardaction.h"

QString KStandardAction::name(KStandardAction::StandardAction a)
{
    KStandardAction sa;
    int idx = sa.metaObject()->indexOfEnumerator("StandardAction");
    Q_ASSERT(idx >= 0);
    QMetaEnum e = sa.metaObject()->enumerator(idx);
    return QString::fromLatin1(e.valueToKey(int(a)));
}

KAction* KStandardAction::openNew(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::open(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }

KRecentFilesAction* KStandardAction::openRecent(const QObject *recvr, const char *slot, QObject *parent)
{
    KRecentFilesAction *a = new KRecentFilesAction(i18n("Open recent"), parent);
    connect(a, SIGNAL(triggered(bool)), recvr, slot);
    return a;
}

KAction* KStandardAction::save(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::saveAs(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::revert(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::close(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::print(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::printPreview(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::mail(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::quit(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::undo(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::redo(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::cut(QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::copy(QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::paste(QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::clear(QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::selectAll(QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::cut(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::copy(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::paste(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::pasteText(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::clear(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::selectAll(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::deselect(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::find(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::findNext(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::findPrev(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::replace(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::actualSize(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::fitToPage(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::fitToWidth(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::fitToHeight(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::zoomIn(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::zoomOut(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::zoom(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::redisplay(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::up(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::back(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::forward(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::home(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::prior(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::next(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::goTo(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::gotoPage(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::gotoLine(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::firstPage(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::lastPage(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::documentBack(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::documentForward(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::addBookmark(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::editBookmarks(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::spelling(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
static KToggleAction *showMenubar(const QObject *recvr, const char *slot, QObject *parent) { return new KToggleAction(parent); }
static KToggleAction *showStatusbar(const QObject *recvr, const char *slot, QObject *parent) { return new KToggleAction(parent); }
KAction* KStandardAction::saveOptions(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::keyBindings(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::preferences(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::configureToolbars(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::configureNotifications(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::help(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::helpContents(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::whatsThis(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::tipOfDay(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::reportBug(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::aboutApp(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
KAction* KStandardAction::aboutKDE(const QObject *recvr, const char *slot, QObject *parent) { return new KAction(parent); }
