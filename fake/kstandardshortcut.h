#ifndef FAKE_KSTANDARDSHORTCUT_H
#define FAKE_KSTANDARDSHORTCUT_H

#include <kaction.h>
#include <kshortcut.h>

#include <QObject>
#include <QMetaObject>
#include <QMetaEnum>

class KStandardShortcut
{
public:
    enum StandardShortcut {
        //C++ requires that the value of an enum symbol be one more than the previous one.
        //This means that everything will be well-ordered from here on.
        AccelNone=0,
        // File menu
        Open, New, Close, Save,
        // The Print item
        Print,
        Quit,
        // Edit menu
        Undo, Redo, Cut, Copy, Paste,     PasteSelection,
        SelectAll, Deselect, DeleteWordBack, DeleteWordForward,
        Find, FindNext, FindPrev, Replace,
        // Navigation
        Home, Begin, End, Prior, Next,
        Up, Back, Forward, Reload,
        // Text Navigation
        BeginningOfLine, EndOfLine, GotoLine,
        BackwardWord, ForwardWord,
        // View parameters
        AddBookmark, ZoomIn, ZoomOut, FullScreen,
        ShowMenubar,
        // Tabular navigation
        TabNext, TabPrev,
        // Help menu
        Help, WhatsThis,
        // Text completion
        TextCompletion, PrevCompletion, NextCompletion, SubstringCompletion,

        RotateUp, RotateDown,

        OpenRecent,
        SaveAs,
        Revert,
        PrintPreview,
        Mail,
        Clear,
        ActualSize,
        FitToPage,
        FitToWidth,
        FitToHeight,
        Zoom,
        Goto,
        GotoPage,
        DocumentBack,
        DocumentForward,
        EditBookmarks,
        Spelling,
        ShowToolbar,
        ShowStatusbar,
        SaveOptions,
        KeyBindings,
        Preferences,
        ConfigureToolbars,
        ConfigureNotifications,
        TipofDay,
        ReportBug,
        SwitchApplicationLanguage,
        AboutApp,
        AboutKDE,

        // Insert new items here!

        StandardShortcutCount // number of standard shortcuts
    };
    
    static QKeySequence::StandardKey open() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey openNew() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey close() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey save() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey print() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey quit() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey undo() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey redo() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey cut() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey copy() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey paste() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey pasteSelection() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey selectAll() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey deleteWordBack() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey deleteWordForward() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey find() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey findNext() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey findPrev() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey replace() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey zoomIn() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey zoomOut() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey insert() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey home() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey begin() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey end() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey beginningOfLine() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey endOfLine() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey prior() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey next() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey gotoLine() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey addBookmark() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey tabNext() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey tabPrev() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey fullScreen() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey help() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey completion() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey prevCompletion() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey nextCompletion() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey substringCompletion() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey rotateUp() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey rotateDown() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey whatsThis() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey reload() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey up() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey back() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey forward() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey backwardWord() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey forwardWord() { return QKeySequence::StandardKey(); }
    static QKeySequence::StandardKey showMenubar() { return QKeySequence::StandardKey(); }
};

#endif
