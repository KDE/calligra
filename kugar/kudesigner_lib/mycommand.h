/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>
   Copyright (C) 2000 David Faure <faure@kde.org>

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

#ifndef kcommand_h
#define kcommand_h

#include <qptrlist.h>
#include <qstring.h>
#include <qobject.h>

class QAction;
class QActionCollection;
class QPopupMenu;

/**
 * The abstract base class for all Commands. Commands are used to
 * store information needed for Undo/Redo functionality...
 */
class KCommand
{
protected:
    /**
     * Creates a command.
     */
    KCommand() {}

public:
    virtual ~KCommand();

    /**
     * The main method: executes this command.
     * Implement here what this command is about, and remember to
     * record any information that will be helpful for #unexecute.
     */
    virtual void execute() = 0;
    /**
     * Unexecutes (undo) this command.
     * Implement here the steps to take for undoing the command.
     * If your application uses actions for everything (it should),
     * and if you implement unexecute correctly, the application is in the same
     * state after unexecute as it was before execute. This means, the next
     * call to execute will do the same thing as it did the first time.
     */
    virtual void unexecute() = 0;

    /**
     * @return the name of this command, translated, since it will appear
     * in the menus.
     */
    virtual QString name() const = 0;
protected:
    virtual void virtual_hook( int id, void* data );
};

/**
 * A command which stores its name.
 * This class mostly exists for compatibility with KDE-2.x
 * It is more memory-efficient to use KCommand and to implement the name() method.
 */
class KNamedCommand : public KCommand
{
protected:
    /**
     * Creates a command.
     * @param name the name of this command, translated, since it will appear
     * in the menus.
     */
    KNamedCommand(const QString &name) : KCommand(), m_name(name) {}

public:
    /**
     * @return the name of this command
     */
    virtual QString name() const { return m_name; }
    /**
     * Updates the name of this command.
     * Rarely necessary.
     */
    void setName(const QString &name) { m_name=name; }

private:
    QString m_name;
protected:
    virtual void virtual_hook( int id, void* data );
};

/**
 * A Macro Command is a command that holds several sub-commands.
 * It will appear as one to the user and in the command history,
 * but it can use the implementation of multiple commands internally.
 */
class KMacroCommand : public KNamedCommand
{
public:
    /**
     * Creates a macro command. You will then need to call @ref addCommand
     * for each subcommand to be added to this macro command.
     * @param name the name of this command, translated, since it will appear
     * in the menus.
     */
    KMacroCommand( const QString & name );
    virtual ~KMacroCommand() {}

    /**
     * Appends a command to this macro command.
     * The ownership is transfered to the macro command.
     */
    void addCommand(KCommand *command);

    /**
     * Executes this command, i.e. execute all the sub-commands
     * in the order in which they were added.
     */
    virtual void execute();
    /**
     * Undoes the execution of this command, i.e. #unexecute all the sub-commands
     * in the _reverse_ order to the one in which they were added.
     */
    virtual void unexecute();

protected:
    QPtrList<KCommand> m_commands;
protected:
    virtual void virtual_hook( int id, void* data );
};


/**
 * The command history stores a (user) configurable amount of
 * Commands. It keeps track of its size and deletes commands
 * if it gets too large. The user can set a maximum undo and
 * a maximum redo limit (e.g. max. 50 undo / 30 redo commands).
 * The KCommandHistory keeps track of the "borders" and deletes
 * commands, if appropriate. It also activates/deactivates the
 * undo/redo actions in the menu and changes the text according
 * to the name of the command.
 */
class KCommandHistory : public QObject {
    Q_OBJECT
public:
    /**
     * Creates a command history, to store commands.
     * This constructor doesn't create actions, so you need to call
     * #undo and #redo yourself.
     */
    KCommandHistory();

    /**
     * Creates a command history, to store commands.
     * This also creates an undo and a redo action, in the @p actionCollection,
     * using the standard names ("edit_undo" and "edit_redo").
     * @param withMenus if true, the actions will display a menu when plugged
     * into a toolbar.
     */
    KCommandHistory(QActionCollection *actionCollection, bool withMenus = true);

    /**
     * Destructs the command history object.
     */
    virtual ~KCommandHistory();

    /**
     * Erases all the undo/redo history.
     * Use this when reloading the data, for instance, since this invalidates
     * all the commands.
     */
    void clear();

    /**
     * Adds a command to the history. Call this for each @p command you create.
     * Unless you set @p execute to false, this will also execute the command.
     * This means, most of the application's code will look like
     *    MyCommand * cmd = new MyCommand(i18n("The name"), parameters);
     *    m_historyCommand.addCommand( cmd );
     */
    void addCommand(KCommand *command, bool execute=true);

    /**
     * @return the maximum number of items in the undo history
     */
    int undoLimit() const { return m_undoLimit; }
    /**
     * Sets the maximum number of items in the undo history.
     */
    void setUndoLimit(int limit);
    /**
     * @return the maximum number of items in the redo history
     */
    int redoLimit() const { return m_redoLimit; }
    /**
     * Sets the maximum number of items in the redo history.
     */
    void setRedoLimit(int limit);

    /**
     * Enable or disable the undo and redo actions.
     * This isn't usually necessary, but this method can be useful if
     * you disable all actions (to go to a "readonly" state), and then
     * want to come back to a readwrite mode.
     */
    void updateActions();

public slots:
    /**
     * Undoes the last action.
     * Call this if you don't use the builtin KActions.
     */
    virtual void undo();
    /**
     * Redoes the last undone action.
     * Call this if you don't use the builtin KActions.
     */
    virtual void redo();
    /**
     * Remembers when you saved the document.
     * Call this right after saving the document. As soon as
     * the history reaches the current index again (via some
     * undo/redo operations) it will emit @ref documentRestored
     * If you implemented undo/redo properly the document is
     * the same you saved before.
     */
    virtual void documentSaved();

protected slots:
    void slotUndoAboutToShow();
    void slotUndoActivated( int );
    void slotRedoAboutToShow();
    void slotRedoActivated( int );

signals:
    /**
     * Emitted every time a command is executed
     * (whether by addCommand, undo or redo).
     * You can use this to update the GUI, for instance.
     */
    void commandExecuted();
    /**
     * Emitted everytime we reach the index where you
     * saved the document for the last time. See @ref documentSaved
     */
    void documentRestored();

private:
    void clipCommands();  // ensures that the limits are kept

    QPtrList<KCommand> m_commands;
    QAction *m_undo, *m_redo;
    QPopupMenu *m_undoPopup, *m_redoPopup;
    int m_undoLimit, m_redoLimit;
    bool m_first;  // attention: it's the first command in the list!
protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KCommandHistoryPrivate;
    KCommandHistoryPrivate *d;
};

#endif
