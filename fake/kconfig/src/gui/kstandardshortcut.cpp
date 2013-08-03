/* This file is part of the KDE libraries
    Copyright (C) 1997 Stefan Taferner (taferner@alpin.or.at)
    Copyright (C) 2000 Nicolas Hadacek (haadcek@kde.org)
    Copyright (C) 2001,2002 Ellis Whitehead (ellis@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kstandardshortcut.h"

#include "kconfig.h"
#include "ksharedconfig.h"
#include <kconfiggroup.h>

#include <QCoreApplication>
#include <QDebug>
#include <QKeySequence>

namespace KStandardShortcut
{

struct KStandardShortcutInfo
{
    //! The standard shortcut id. @see StandardShortcut
    StandardShortcut id;

    /** 
     * Unique name for the given accel. The name is used to save the user
     * settings. It's not representable. Use description for that.
     * @warning NEVER EVER CHANGE IT OR TRANSLATE IT!
     */
    const char* name;

    //! Context for the translation
    const char* translation_context;

    //! Localized label for user-visible display
    const char* description;

    //! The keys for this shortcut
    int cutDefault, cutDefault2;

    //! A shortcut that is created with @a cutDefault and @cutDefault2
    QList<QKeySequence> cut;

    //! If this struct is initialized. If not initialized @cut is not valid
    bool isInitialized;
};

//! We need to remember the context to get the correct translation.
#undef I18N_NOOP2
#define I18N_NOOP2(comment,x) comment, x

#define CTRL(x) Qt::CTRL+Qt::Key_##x
#define SHIFT(x) Qt::SHIFT+Qt::Key_##x
#define CTRLSHIFT(x) Qt::CTRL+Qt::SHIFT+Qt::Key_##x
#define ALT(x) Qt::ALT+Qt::Key_##x
#define ALTSHIFT(x) Qt::ALT+Qt::SHIFT+Qt::Key_##x

/** Array of predefined KStandardShortcutInfo objects, which cover all
    the "standard" accelerators. Each enum value from StandardShortcut
    should appear in this table.
*/
// STUFF WILL BREAK IF YOU DON'T READ THIS!!!
// Read the comments of the big enum in kstandardshortcut.h before you change anything!
static KStandardShortcutInfo g_infoStandardShortcut[] =
{
//Group File,
    {AccelNone, 0      , 0                   , 0       , 0      , 0           , QList<QKeySequence>(), false },
    { Open    , "Open" , I18N_NOOP2("@action", "Open") , CTRL(O), 0           , QList<QKeySequence>(), false }    ,
    { New     , "New"  , I18N_NOOP2("@action", "New")  , CTRL(N), 0           , QList<QKeySequence>(), false }    ,
    { Close   , "Close", I18N_NOOP2("@action", "Close"), CTRL(W), CTRL(Escape), QList<QKeySequence>(), false }    ,
    { Save    , "Save" , I18N_NOOP2("@action", "Save") , CTRL(S), 0           , QList<QKeySequence>(), false }    ,
    { Print   , "Print", I18N_NOOP2("@action", "Print"), CTRL(P), 0           , QList<QKeySequence>(), false }    ,
    { Quit    , "Quit" , I18N_NOOP2("@action", "Quit") , CTRL(Q), 0           , QList<QKeySequence>(), false }    ,

//Group Edit
    { Undo             , "Undo"             , I18N_NOOP2("@action", "Undo")                 , CTRL(Z)          , 0            , QList<QKeySequence>(), false },
    { Redo             , "Redo"             , I18N_NOOP2("@action", "Redo")                 , CTRLSHIFT(Z)     , 0            , QList<QKeySequence>(), false },
    { Cut              , "Cut"              , I18N_NOOP2("@action", "Cut")                  , CTRL(X)          , SHIFT(Delete), QList<QKeySequence>(), false },
    { Copy             , "Copy"             , I18N_NOOP2("@action", "Copy")                 , CTRL(C)          , CTRL(Insert) , QList<QKeySequence>(), false },
    { Paste            , "Paste"            , I18N_NOOP2("@action", "Paste")                , CTRL(V)          , SHIFT(Insert), QList<QKeySequence>(), false },
    { PasteSelection   , "Paste Selection"  , I18N_NOOP2("@action", "Paste Selection")      , CTRLSHIFT(Insert), 0            , QList<QKeySequence>(), false },

    { SelectAll        , "SelectAll"        , I18N_NOOP2("@action", "Select All")           , CTRL(A)          , 0            , QList<QKeySequence>(), false },
    { Deselect         , "Deselect"         , I18N_NOOP2("@action", "Deselect")             , CTRLSHIFT(A)     , 0            , QList<QKeySequence>(), false },
    { DeleteWordBack   , "DeleteWordBack"   , I18N_NOOP2("@action", "Delete Word Backwards"), CTRL(Backspace)  , 0            , QList<QKeySequence>(), false },
    { DeleteWordForward, "DeleteWordForward", I18N_NOOP2("@action", "Delete Word Forward")  , CTRL(Delete)     , 0            , QList<QKeySequence>(), false },

    { Find             , "Find"             , I18N_NOOP2("@action", "Find")                 , CTRL(F)          , 0            , QList<QKeySequence>(), false },
    { FindNext         , "FindNext"         , I18N_NOOP2("@action", "Find Next")            , Qt::Key_F3       , 0            , QList<QKeySequence>(), false },
    { FindPrev         , "FindPrev"         , I18N_NOOP2("@action", "Find Prev")            , SHIFT(F3)        , 0            , QList<QKeySequence>(), false },
    { Replace          , "Replace"          , I18N_NOOP2("@action", "Replace")              , CTRL(R)          , 0            , QList<QKeySequence>(), false },

//Group Navigation
    { Home           , "Home"                 , I18N_NOOP2("@action Go to main page"      , "Home")                 , ALT(Home)       , Qt::Key_HomePage  , QList<QKeySequence>(), false },
    { Begin          , "Begin"                , I18N_NOOP2("@action Beginning of document", "Begin")                , CTRL(Home)      , 0                 , QList<QKeySequence>(), false },
    { End            , "End"                  , I18N_NOOP2("@action End of document"      , "End")                  , CTRL(End)       , 0                 , QList<QKeySequence>(), false },
    { Prior          , "Prior"                , I18N_NOOP2("@action"                      , "Prior")                , Qt::Key_PageUp  , 0                 , QList<QKeySequence>(), false },
    { Next           , "Next"                 , I18N_NOOP2("@action Opposite to Prior"    , "Next")                 , Qt::Key_PageDown, 0                 , QList<QKeySequence>(), false },

    { Up             , "Up"                   , I18N_NOOP2("@action"                      , "Up")                   , ALT(Up)         , 0                 , QList<QKeySequence>(), false },
    { Back           , "Back"                 , I18N_NOOP2("@action"                      , "Back")                 , ALT(Left)       , Qt::Key_Back      , QList<QKeySequence>(), false },
    { Forward        , "Forward"              , I18N_NOOP2("@action"                      , "Forward")              , ALT(Right)      , Qt::Key_Forward   , QList<QKeySequence>(), false },
    { Reload         , "Reload"               , I18N_NOOP2("@action"                      , "Reload")               , Qt::Key_F5      , Qt::Key_Refresh   , QList<QKeySequence>(), false },

    { BeginningOfLine, "BeginningOfLine"      , I18N_NOOP2("@action"                      , "Beginning of Line")    , Qt::Key_Home    , 0                 , QList<QKeySequence>(), false },
    { EndOfLine      , "EndOfLine"            , I18N_NOOP2("@action"                      , "End of Line")          , Qt::Key_End     , 0                 , QList<QKeySequence>(), false },
    { GotoLine       , "GotoLine"             , I18N_NOOP2("@action"                      , "Go to Line")           , CTRL(G)         , 0                 , QList<QKeySequence>(), false },
    { BackwardWord   , "BackwardWord"         , I18N_NOOP2("@action"                      , "Backward Word")        , CTRL(Left)      , 0                 , QList<QKeySequence>(), false },
    { ForwardWord    , "ForwardWord"          , I18N_NOOP2("@action"                      , "Forward Word")         , CTRL(Right)     , 0                 , QList<QKeySequence>(), false },

    { AddBookmark    , "AddBookmark"          , I18N_NOOP2("@action"                      , "Add Bookmark")         , CTRL(B)         , 0                 , QList<QKeySequence>(), false },
    { ZoomIn         , "ZoomIn"               , I18N_NOOP2("@action"                      , "Zoom In")              , CTRL(Plus)      , CTRL(Equal)       , QList<QKeySequence>(), false },
    { ZoomOut        , "ZoomOut"              , I18N_NOOP2("@action"                      , "Zoom Out")             , CTRL(Minus)     , 0                 , QList<QKeySequence>(), false },
    { FullScreen     , "FullScreen"           , I18N_NOOP2("@action"                      , "Full Screen Mode")     , CTRLSHIFT(F)    , 0                 , QList<QKeySequence>(), false },

    { ShowMenubar    , "ShowMenubar"          , I18N_NOOP2("@action"                      , "Show Menu Bar")        , CTRL(M)         , 0                 , QList<QKeySequence>(), false },
    { TabNext        , "Activate Next Tab"    , I18N_NOOP2("@action"                      , "Activate Next Tab")    , CTRL(Period)    , CTRL(BracketRight), QList<QKeySequence>(), false },
    { TabPrev        , "Activate Previous Tab", I18N_NOOP2("@action"                      , "Activate Previous Tab"), CTRL(Comma)     , CTRL(BracketLeft) , QList<QKeySequence>(), false },

    //Group Help
    { Help           , "Help"                 , I18N_NOOP2("@action"                      , "Help")                 , Qt::Key_F1      , 0                 , QList<QKeySequence>(), false },
    { WhatsThis      , "WhatsThis"            , I18N_NOOP2("@action"                      , "What's This")          , SHIFT(F1)       , 0                 , QList<QKeySequence>(), false },

//Group TextCompletion
    { TextCompletion           , "TextCompletion"           , I18N_NOOP2("@action", "Text Completion")          , CTRL(E)     , 0, QList<QKeySequence>(), false },
    { PrevCompletion           , "PrevCompletion"           , I18N_NOOP2("@action", "Previous Completion Match"), CTRL(Up)    , 0, QList<QKeySequence>(), false },
    { NextCompletion           , "NextCompletion"           , I18N_NOOP2("@action", "Next Completion Match")    , CTRL(Down)  , 0, QList<QKeySequence>(), false },
    { SubstringCompletion      , "SubstringCompletion"      , I18N_NOOP2("@action", "Substring Completion")     , CTRL(T)     , 0, QList<QKeySequence>(), false },

    { RotateUp                 , "RotateUp"                 , I18N_NOOP2("@action", "Previous Item in List")    , Qt::Key_Up  , 0, QList<QKeySequence>(), false },
    { RotateDown               , "RotateDown"               , I18N_NOOP2("@action", "Next Item in List")        , Qt::Key_Down, 0, QList<QKeySequence>(), false },

    { OpenRecent               , "OpenRecent"               , I18N_NOOP2("@action", "Open Recent")               , 0           , 0, QList<QKeySequence>(), false },
    { SaveAs                   , "SaveAs"                   , I18N_NOOP2("@action", "Save As")                   , 0           , 0, QList<QKeySequence>(), false },
    { Revert                   , "Revert"                   , I18N_NOOP2("@action", "Revert")                   , 0           , 0, QList<QKeySequence>(), false },
    { PrintPreview             , "PrintPreview"             , I18N_NOOP2("@action", "Print Preview")             , 0           , 0, QList<QKeySequence>(), false },
    { Mail                     , "Mail"                     , I18N_NOOP2("@action", "Mail")                     , 0           , 0, QList<QKeySequence>(), false },
    { Clear                    , "Clear"                    , I18N_NOOP2("@action", "Clear")                    , 0           , 0, QList<QKeySequence>(), false },
    { ActualSize               , "ActualSize"               , I18N_NOOP2("@action", "Actual Size")               , 0           , 0, QList<QKeySequence>(), false },
    { FitToPage                , "FitToPage"                , I18N_NOOP2("@action", "Fit To Page")                , 0           , 0, QList<QKeySequence>(), false },
    { FitToWidth               , "FitToWidth"               , I18N_NOOP2("@action", "Fit To Width")               , 0           , 0, QList<QKeySequence>(), false },
    { FitToHeight              , "FitToHeight"              , I18N_NOOP2("@action", "Fit To Height")              , 0           , 0, QList<QKeySequence>(), false },
    { Zoom                     , "Zoom"                     , I18N_NOOP2("@action", "Zoom")                     , 0           , 0, QList<QKeySequence>(), false },
    { Goto                     , "Goto"                     , I18N_NOOP2("@action", "Goto")                     , 0           , 0, QList<QKeySequence>(), false },
    { GotoPage                 , "GotoPage"                 , I18N_NOOP2("@action", "Goto Page")                 , 0           , 0, QList<QKeySequence>(), false },
    { DocumentBack             , "DocumentBack"             , I18N_NOOP2("@action", "Document Back")             , ALTSHIFT(Left), 0, QList<QKeySequence>(), false },
    { DocumentForward          , "DocumentForward"          , I18N_NOOP2("@action", "Document Forward")          , ALTSHIFT(Right), 0, QList<QKeySequence>(), false },
    { EditBookmarks            , "EditBookmarks"            , I18N_NOOP2("@action", "Edit Bookmarks")            , 0           , 0, QList<QKeySequence>(), false },
    { Spelling                 , "Spelling"                 , I18N_NOOP2("@action", "Spelling")                 , 0           , 0, QList<QKeySequence>(), false },
    { ShowToolbar              , "ShowToolbar"              , I18N_NOOP2("@action", "Show Toolbar")              , 0           , 0, QList<QKeySequence>(), false },
    { ShowStatusbar            , "ShowStatusbar"            , I18N_NOOP2("@action", "Show Statusbar")            , 0           , 0, QList<QKeySequence>(), false },
    { SaveOptions              , "SaveOptions"              , I18N_NOOP2("@action", "Save Options")              , 0           , 0, QList<QKeySequence>(), false },
    { KeyBindings              , "KeyBindings"              , I18N_NOOP2("@action", "Key Bindings")              , 0           , 0, QList<QKeySequence>(), false },
    { Preferences              , "Preferences"              , I18N_NOOP2("@action", "Preferences")              , 0           , 0, QList<QKeySequence>(), false },
    { ConfigureToolbars        , "ConfigureToolbars"        , I18N_NOOP2("@action", "Configure Toolbars")        , 0           , 0, QList<QKeySequence>(), false },
    { ConfigureNotifications   , "ConfigureNotifications"   , I18N_NOOP2("@action", "Configure Notifications")   , 0           , 0, QList<QKeySequence>(), false },
    { TipofDay                 , "TipofDay"                 , I18N_NOOP2("@action", "Tip Of Day")                 , 0           , 0, QList<QKeySequence>(), false },
    { ReportBug                , "ReportBug"                , I18N_NOOP2("@action", "Report Bug")                , 0           , 0, QList<QKeySequence>(), false },
    { SwitchApplicationLanguage, "SwitchApplicationLanguage", I18N_NOOP2("@action", "Switch Application Language"), 0           , 0, QList<QKeySequence>(), false },
    { AboutApp                 , "AboutApp"                 , I18N_NOOP2("@action", "About Application")                 , 0           , 0, QList<QKeySequence>(), false },
    { AboutKDE                 , "AboutKDE"                 , I18N_NOOP2("@action", "About KDE")                 , 0           , 0, QList<QKeySequence>(), false },

    //dummy entry to catch simple off-by-one errors. Insert new entries before this line.
    { AccelNone                , 0                          , 0                   , 0                           , 0, 0, QList<QKeySequence>(), false }
};


/** Search for the KStandardShortcutInfo object associated with the given @p id.
    Return a dummy entry with no name and an empty shortcut if @p id is invalid.
*/
static KStandardShortcutInfo *guardedStandardShortcutInfo(StandardShortcut id)
{
    if (id >= static_cast<int>(sizeof(g_infoStandardShortcut) / sizeof(KStandardShortcutInfo)) ||
             id < 0) {
        qWarning() << "KStandardShortcut: id not found!";
        return &g_infoStandardShortcut[AccelNone];
    } else
        return &g_infoStandardShortcut[id];
}

/** Initialize the accelerator @p id by checking if it is overridden
    in the configuration file (and if it isn't, use the default).
    On X11, if QApplication was initialized with GUI disabled,
    the default will always be used.
*/
static void initialize(StandardShortcut id)
{
    KStandardShortcutInfo *info = guardedStandardShortcutInfo(id);

    // All three are needed.
    if (info->id!=AccelNone) {
        Q_ASSERT(info->description);
        Q_ASSERT(info->translation_context);
        Q_ASSERT(info->name);
    }

    KConfigGroup cg(KSharedConfig::openConfig(), "Shortcuts");

    if (cg.hasKey(info->name)) {
        QString s = cg.readEntry(info->name);
        if (s != "none")
            info->cut = QKeySequence::listFromString(s);
        else
            info->cut = QList<QKeySequence>();
    } else {
        info->cut = hardcodedDefaultShortcut(id);
    }

    info->isInitialized = true;
}

void saveShortcut(StandardShortcut id, const QList<QKeySequence> &newShortcut)
{
    KStandardShortcutInfo *info = guardedStandardShortcutInfo(id);
    // If the action has no standard shortcut associated there is nothing to
    // save
    if(info->id == AccelNone)
        return;

    KConfigGroup cg(KSharedConfig::openConfig(), "Shortcuts");

    info->cut = newShortcut;
    bool sameAsDefault = (newShortcut == hardcodedDefaultShortcut(id));

    if (sameAsDefault) {
        // If the shortcut is the equal to the hardcoded one we remove it from
        // kdeglobal if necessary and return.
        if(cg.hasKey(info->name))
            cg.deleteEntry(info->name, KConfig::Global|KConfig::Persistent);

        return;
    }

    // Write the changed shortcut to kdeglobals
    cg.writeEntry(info->name, QKeySequence::listToString(info->cut), KConfig::Global|KConfig::Persistent);
}

QString name(StandardShortcut id)
{
    return guardedStandardShortcutInfo(id)->name;
}

QString label(StandardShortcut id)
{
    KStandardShortcutInfo *info = guardedStandardShortcutInfo( id );
    return QCoreApplication::translate("KStandardShortcut",
                                       info->description,
                                       info->translation_context);
}

// TODO: Add psWhatsThis entry to KStandardShortcutInfo
QString whatsThis( StandardShortcut /*id*/ )
{
//  KStandardShortcutInfo* info = guardedStandardShortcutInfo( id );
//  if( info && info->whatsThis )
//      return i18n(info->whatsThis);
//  else
        return QString();
}

const QList<QKeySequence> &shortcut(StandardShortcut id)
{
    KStandardShortcutInfo *info = guardedStandardShortcutInfo(id);

    if(!info->isInitialized)
        initialize(id);

    return info->cut;
}

StandardShortcut find(const QKeySequence &seq)
{
    if( !seq.isEmpty() ) {
        for(uint i = 0; i < sizeof(g_infoStandardShortcut) / sizeof(KStandardShortcutInfo); i++) {
            StandardShortcut id = g_infoStandardShortcut[i].id;
            if( id != AccelNone ) {
                if(!g_infoStandardShortcut[i].isInitialized)
                    initialize(id);
                if(g_infoStandardShortcut[i].cut.contains(seq))
                    return id;
            }
        }
    }
    return AccelNone;
}

StandardShortcut find(const char *keyName)
{
    for(uint i = 0; i < sizeof(g_infoStandardShortcut) / sizeof(KStandardShortcutInfo); i++)
        if (qstrcmp(g_infoStandardShortcut[i].name, keyName))
            return g_infoStandardShortcut[i].id;

    return AccelNone;
}

QList<QKeySequence> hardcodedDefaultShortcut(StandardShortcut id)
{
    QList<QKeySequence> cut;
    KStandardShortcutInfo *info = guardedStandardShortcutInfo(id);

    if (info->cutDefault != 0)
        cut << info->cutDefault;

    if (info->cutDefault2 != 0) {
        if (cut.isEmpty())
            cut << QKeySequence();

        cut << info->cutDefault2;
    }

    return cut;
}

const QList<QKeySequence> &open()                  { return shortcut( Open ); }
const QList<QKeySequence> &openNew()               { return shortcut( New ); }
const QList<QKeySequence> &close()                 { return shortcut( Close ); }
const QList<QKeySequence> &save()                  { return shortcut( Save ); }
const QList<QKeySequence> &print()                 { return shortcut( Print ); }
const QList<QKeySequence> &quit()                  { return shortcut( Quit ); }
const QList<QKeySequence> &cut()                   { return shortcut( Cut ); }
const QList<QKeySequence> &copy()                  { return shortcut( Copy ); }
const QList<QKeySequence> &paste()                 { return shortcut( Paste ); }
const QList<QKeySequence> &pasteSelection()        { return shortcut( PasteSelection ); }
const QList<QKeySequence> &deleteWordBack()        { return shortcut( DeleteWordBack ); }
const QList<QKeySequence> &deleteWordForward()     { return shortcut( DeleteWordForward ); }
const QList<QKeySequence> &undo()                  { return shortcut( Undo ); }
const QList<QKeySequence> &redo()                  { return shortcut( Redo ); }
const QList<QKeySequence> &find()                  { return shortcut( Find ); }
const QList<QKeySequence> &findNext()              { return shortcut( FindNext ); }
const QList<QKeySequence> &findPrev()              { return shortcut( FindPrev ); }
const QList<QKeySequence> &replace()               { return shortcut( Replace ); }
const QList<QKeySequence> &home()                  { return shortcut( Home ); }
const QList<QKeySequence> &begin()                 { return shortcut( Begin ); }
const QList<QKeySequence> &end()                   { return shortcut( End ); }
const QList<QKeySequence> &beginningOfLine()       { return shortcut( BeginningOfLine ); }
const QList<QKeySequence> &endOfLine()             { return shortcut( EndOfLine ); }
const QList<QKeySequence> &prior()                 { return shortcut( Prior ); }
const QList<QKeySequence> &next()                  { return shortcut( Next ); }
const QList<QKeySequence> &backwardWord()          { return shortcut( BackwardWord ); }
const QList<QKeySequence> &forwardWord()           { return shortcut( ForwardWord ); }
const QList<QKeySequence> &gotoLine()              { return shortcut( GotoLine ); }
const QList<QKeySequence> &addBookmark()           { return shortcut( AddBookmark ); }
const QList<QKeySequence> &tabNext()               { return shortcut( TabNext ); }
const QList<QKeySequence> &tabPrev()               { return shortcut( TabPrev ); }
const QList<QKeySequence> &fullScreen()            { return shortcut( FullScreen ); }
const QList<QKeySequence> &zoomIn()                { return shortcut( ZoomIn ); }
const QList<QKeySequence> &zoomOut()               { return shortcut( ZoomOut ); }
const QList<QKeySequence> &help()                  { return shortcut( Help ); }
const QList<QKeySequence> &completion()            { return shortcut( TextCompletion ); }
const QList<QKeySequence> &prevCompletion()        { return shortcut( PrevCompletion ); }
const QList<QKeySequence> &nextCompletion()        { return shortcut( NextCompletion ); }
const QList<QKeySequence> &rotateUp()              { return shortcut( RotateUp ); }
const QList<QKeySequence> &rotateDown()            { return shortcut( RotateDown ); }
const QList<QKeySequence> &substringCompletion()   { return shortcut( SubstringCompletion ); }
const QList<QKeySequence> &whatsThis()             { return shortcut( WhatsThis ); }
const QList<QKeySequence> &reload()                { return shortcut( Reload ); }
const QList<QKeySequence> &selectAll()             { return shortcut( SelectAll ); }
const QList<QKeySequence> &up()                    { return shortcut( Up ); }
const QList<QKeySequence> &back()                  { return shortcut( Back ); }
const QList<QKeySequence> &forward()               { return shortcut( Forward ); }
const QList<QKeySequence> &showMenubar()           { return shortcut( ShowMenubar ); }

}
