/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005-2009 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KFORMEDITOR_COMMANDS_H
#define KFORMEDITOR_COMMANDS_H

#include <QHash>
#include <QVariant>
#include <QUndoCommand>

#include <KDebug>
#include "utils.h"
#include "form.h"

class QRect;
class QPoint;
class QStringList;
class QDomElement;

namespace KFormDesigner
{

//removed 2.0 class WidgetPropertySet;
class ObjectTreeItem;
class Container;
class Form;

//! Base class for KFormDesigner's commands
class KFORMEDITOR_EXPORT Command : public QUndoCommand
{
public:
    explicit Command(Command *parent = 0);

    explicit Command(const QString &text, Command *parent = 0);

    virtual ~Command();

    //! Reimplemented to support effect of blockRedoOnce().
    virtual void redo();

    //! Implement instead of redo().
    virtual void execute() = 0;

    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const Command &c); 
protected:
    //! Used to block execution of redo() once, on ading the command to the stack.
    void blockRedoOnce();

    friend class Form;
    bool m_blockRedoOnce : 1; //!< Used to block redo() once
};

//! kDebug() stream operator. Writes command @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const Command &c); 

//! Command is used when changing a property for one or more widgets. 
class KFORMEDITOR_EXPORT PropertyCommand : public Command
{
public:
   /*! @a oldValue is the old property value for selected widget. 
     This enables reverting the change. @a value is the new property value. */
    PropertyCommand(Form& form, const QByteArray &wname, const QVariant &oldValue,
                    const QVariant &value, const QByteArray &propertyName, Command *parent = 0);

   /*! @a oldValues is a QHash of the old property values for every widget, 
     to allow reverting the change. @a value is the new property value.
     You can use the simpler constructor for a single widget. */
    PropertyCommand(Form& form, const QHash<QByteArray, QVariant> &oldValues,
                    const QVariant &value, const QByteArray &propertyName, Command *parent = 0);

    virtual ~PropertyCommand();

    Form* form() const;

    virtual int id() const { return 1; }

    void setUniqueId(int id);

    virtual void execute();
    
    virtual void undo();

    bool mergeWith(const QUndoCommand * command);

    QByteArray propertyName() const;

    QVariant value() const;

    void setValue(const QVariant &value);
    
    const QHash<QByteArray, QVariant>& oldValues() const;

    //! @return old value if there is single value, otherwise null value.
    QVariant oldValue() const;

    //! @return widget name in case when there is only one widget
    //! with changed property in this command
    /*! Otherwise empty value is returned. */
    QByteArray widgetName() const;

    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const PropertyCommand &c); 
protected:
    void init();
    class Private;
    Private * const d;
};

//! kDebug() stream operator. Writes command @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const PropertyCommand &c); 

//! Command used when moving multiples widgets at the same time, while holding Ctrl or Shift.
/*! You need to supply a list of widget names, and the position of the cursor before moving. Use setPos()
  to tell the new cursor pos every time it changes.*/
class KFORMEDITOR_EXPORT GeometryPropertyCommand : public Command
{
public:
    GeometryPropertyCommand(Form& form, const QStringList &names, 
                            const QPoint& oldPos, Command *parent = 0);

    virtual ~GeometryPropertyCommand();

    virtual int id() const { return 2; }

    virtual void execute();

    virtual void undo();
    
    void setPos(const QPoint& pos);
    
    QPoint pos() const;
    
    QPoint oldPos() const;

    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const GeometryPropertyCommand &c); 
protected:
    class Private;
    Private * const d;
};

//! kDebug() stream operator. Writes command @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const GeometryPropertyCommand &c); 

//! Command used when an "Align Widgets position" action is activated.
/* You just need to give the list of widget names (the selected ones), and the
  type of alignment (see the enum for possible values). */
class KFORMEDITOR_EXPORT AlignWidgetsCommand : public Command
{
public:
    AlignWidgetsCommand(Form &form, Form::WidgetAlignment alignment, const QWidgetList &list,
                        Command *parent = 0);

    virtual ~AlignWidgetsCommand();

    virtual int id() const { return 3; }

    virtual void execute();
    
    virtual void undo();
    
    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const AlignWidgetsCommand &c); 
protected:
    class Private;
    Private * const d;
};

//! kDebug() stream operator. Writes command @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const AlignWidgetsCommand &c); 

//! Command used when an "Adjust Widgets Size" action is activated.
/*! You just need to give the list of widget names (the selected ones), 
    and the type of size modification (see the enum for possible values). */
class KFORMEDITOR_EXPORT AdjustSizeCommand : public Command
{
public:
    enum Adjustment {
        SizeToGrid,
        SizeToFit,
        SizeToSmallWidth,
        SizeToBigWidth,
        SizeToSmallHeight,
        SizeToBigHeight
    };

    AdjustSizeCommand(Form& form, Adjustment type, const QWidgetList &list, Command *parent = 0);

    virtual ~AdjustSizeCommand();

    virtual int id() const { return 4; }

    virtual void execute();
    
    virtual void undo();
    
    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const AdjustSizeCommand &c); 
protected:
    QSize getSizeFromChildren(ObjectTreeItem *item);

protected:
    class Private;
    Private * const d;
};

//! kDebug() stream operator. Writes command @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const AdjustSizeCommand &c); 

//! Command used when switching the layout of a container.
/*! It remembers the old pos of every widget inside the container. */
class KFORMEDITOR_EXPORT LayoutPropertyCommand : public PropertyCommand
{
public:
    LayoutPropertyCommand(Form& form, const QByteArray &wname,
                          const QVariant &oldValue, const QVariant &value,
                          Command *parent = 0);

    virtual ~LayoutPropertyCommand();

    virtual int id() const { return 5; }

    virtual void execute();
    
    virtual void undo();
    
    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const LayoutPropertyCommand &c); 
protected:
    class Private;
    Private * const d;
};

//! kDebug() stream operator. Writes command @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const LayoutPropertyCommand &c); 

//! Command used when inserting a widget using toolbar or menu. 
/*! You only have to give the parent Container and the widget pos.
 The other information is taken from the form. */
class KFORMEDITOR_EXPORT InsertWidgetCommand : public Command
{
public:
    explicit InsertWidgetCommand(const Container& container, Command *parent = 0);

    /*! This ctor allows to set explicit class name and position.
     Used for dropping widgets on the form surface.
     If \a namePrefix is empty, widget's unique name is constructed using
     hint for \a className (WidgetLibrary::namePrefix()),
     otherwise, \a namePrefix is used to generate widget's name.
     This allows e.g. inserting a widgets having name constructed using
     */
    InsertWidgetCommand(const Container& container, const QByteArray& className,
                        const QPoint& pos, const QByteArray& namePrefix = QByteArray(),
                        Command *parent = 0);

    virtual ~InsertWidgetCommand();

    virtual int id() const { return 6; }

    virtual void execute();
    
    virtual void undo();
    
    //! @return inserted widget's name
    QByteArray widgetName() const;

    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const InsertWidgetCommand &c); 
protected:
    void init();

    class Private;
    Private * const d;
};

//! kDebug() stream operator. Writes command @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const InsertWidgetCommand &c);

//! Command used when creating a layout from some widgets using "Lay out in...".
/*! It remembers the old pos of every widget, and takes care of updating ObjectTree too. You need
 to supply a QWidgetList of the selected widgets. */
class KFORMEDITOR_EXPORT CreateLayoutCommand : public Command
{
public:
    CreateLayoutCommand(Form &form, Form::LayoutType layoutType, const QWidgetList &list,
                        Command *parent = 0);

    virtual ~CreateLayoutCommand();

    virtual int id() const { return 7; }

    virtual void execute();
    
    virtual void undo();
    
    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const CreateLayoutCommand &c); 
protected:
    //! Used in BreakLayoutCommand ctor.
    explicit CreateLayoutCommand(Command *parent = 0);

    void init();

    class Private;
    Private * const d;
};

//! kDebug() stream operator. Writes command @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const CreateLayoutCommand &c);

//! Command used when the 'Break Layout' menu item is selected. 
/*! It does exactly the opposite of the CreateLayoutCommand. */
class KFORMEDITOR_EXPORT BreakLayoutCommand : public CreateLayoutCommand
{
public:
    explicit BreakLayoutCommand(const Container &container, Command *parent = 0);

    virtual ~BreakLayoutCommand();

    virtual int id() const { return 8; }

    virtual void execute();
    
    virtual void undo();
    
    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const BreakLayoutCommand &c); 
};

//! kDebug() stream operator. Writes command @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const BreakLayoutCommand &c);

//! @todo add CopyWidgetCommand

//! Command used when pasting widgets. 
/*! You need to give the QDomDocument containing
    the widget(s) to paste, and optionally the point where to paste widgets. */
class KFORMEDITOR_EXPORT PasteWidgetCommand : public Command
{
public:
    explicit PasteWidgetCommand(const QDomDocument &domDoc, const Container& container, 
                                const QPoint& p = QPoint(), Command *parent = 0);

    virtual ~PasteWidgetCommand();

    virtual int id() const { return 9; }

    virtual void execute();
    
    virtual void undo();
    
    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const PasteWidgetCommand &c); 
protected:
    /*! Internal function used to change the coordinates of a widget to \a newPos
     before pasting it (to paste it at the position of the contextual menu). It modifies
       the "geometry" property of the QDomElement representing the widget. */
    void changePos(QDomElement &el, const QPoint &newPos);

    /*! Internal function used to fix the coordinates of a widget before pasting it
       (to avoid having two widgets at the same position). It moves the widget by
       (10, 10) increment (several times if there are already pasted widgets at this position). */
    void fixPos(QDomElement &el, Container *container);
    
    void moveWidgetBy(QDomElement &el, Container *container, const QPoint &p);
    
    /*! Internal function used to fix the names of the widgets before pasting them.
      It prevents from pasting a widget with
      the same name as an actual widget. The child widgets are also fixed recursively.\n
      If the name of the widget ends with a number (eg "QLineEdit1"), the new name is
      just incremented by one (eg becomes "QLineEdit2"). Otherwise, a "2" is just
      appended at the end of the name (eg "myWidget" becomes "myWidget2"). */
    void fixNames(QDomElement &el);

protected:
    class Private;
    Private * const d;
};

//! kDebug() stream operator. Writes command @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const PasteWidgetCommand &c);

//! Command used when deleting widgets using the "Delete" menu item.
/*! You need to give a QWidgetList of the selected widgets. */
class KFORMEDITOR_EXPORT DeleteWidgetCommand : public Command
{
public:
    DeleteWidgetCommand(Form& form, const QWidgetList &list, Command *parent = 0);

    virtual ~DeleteWidgetCommand();

    virtual int id() const { return 10; }

    virtual void execute();
    
    virtual void undo();
    
    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const DeleteWidgetCommand &c); 
protected:
    class Private;
    Private * const d;
};

//! kDebug() stream operator. Writes command @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const DeleteWidgetCommand &c);

//! Command used when duplicating widgets.
/*! You need to give a QWidgetList of the selected widgets. */
class KFORMEDITOR_EXPORT DuplicateWidgetCommand : public Command
{
public:
    DuplicateWidgetCommand(const Container& container, const QWidgetList &list, 
                           const QPoint& copyToPoint, Command *parent = 0);

    virtual ~DuplicateWidgetCommand();

    virtual int id() const { return 11; }

    virtual void execute();
    
    virtual void undo();
    
    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const DuplicateWidgetCommand &c); 
protected:
    class Private;
    Private * const d;
};

//! kDebug() stream operator. Writes command @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const DuplicateWidgetCommand &c);

//! Command used when cutting widgets. 
/*! It is basically a DeleteWidgetCommand which also updates the clipboard contents. */
class KFORMEDITOR_EXPORT CutWidgetCommand : public DeleteWidgetCommand
{
public:
    CutWidgetCommand(Form &form, const QWidgetList &list, Command *parent = 0);

    virtual ~CutWidgetCommand();

    virtual int id() const { return 12; }

    virtual void execute();
    
    virtual void undo();
    
    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const CutWidgetCommand &c); 
protected:
    class Private;
    Private * const d2;
};

//! kDebug() stream operator. Writes command @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const CutWidgetCommand &c);

//! Command that holds several PropertyCommand subcommands.
/*! It appears as one to the user and in the command history. */
class KFORMEDITOR_EXPORT PropertyCommandGroup : public Command
{
public:
    explicit PropertyCommandGroup(const QString &text, Command *parent = 0);

    virtual ~PropertyCommandGroup();

    virtual int id() const { return 13; }

#if 0
    /*! Like KMacroCommand::addCommand(KCommand*)
     but if \a allowExecute is false, \a command will not be executed
     as a subcommand when CommandGroup::execute() is called.

     This is useful e.g. in KexiFormView::insertAutoFields(),
     where a number of subcommands of InsertWidgetCommand type and subcommands
     is groupped using CommandGroup but some of these subcommands are executed
     before executing CommandGroup::execute().

     If \a allowExecute is true, this method behaves exactly like
     KMacroCommand::addCommand(KCommand*).

     Note that unexecute() doesn't check \a allowExecute flag:
     all subcommands will be unexecuted (in reverse order
     to the one in which they were added).
    */
    void addCommand(K3Command *command, bool allowExecute);

    /*! Executes all subcommands added to this group
     in the same order as they were added. Subcommands added with
     addCommand(KCommand *command, bool allowExecute) where allowExecute == false,
     will not be executed. */
    virtual void execute();

    /*! Unexecutes all subcommands added to this group,
     (in reversed order). */
    virtual void undo();

    virtual QString name() const;

    /*! \return a list of all subcommands of this group.
     Note that if a given subcommand is a group itself,
     it will not be expanded to subcommands on this list. */
    const QList<K3Command*> commands() const;

    /*! Resets all 'allowExecute' flags that was set in addCommand().
     Call this after calling CommandGroup::execute() to ensure that
     in the future, when REDO is be executed, all subcommands will be executed. */
    void resetAllowExecuteFlags();

    /*! merges property values from property commands stored in @a command
     with this group. 
     @return true if number, order and names of the commands match.
     Otherwise no value is copied. */
    virtual bool mergeWith(const QUndoCommand * command);
#endif

    virtual void execute();

    virtual void debug() const { kDebug() << *this; }

    friend KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const PropertyCommandGroup &c); 
protected:
    class Private;
    Private * const d;
};

//! Command is used when inline text is edited for a single widget. 
class KFORMEDITOR_EXPORT InlineTextEditingCommand : public Command
{
public:
   /*! @a oldValue is the old property value for selected widget. 
     This enables reverting the change. @a value is the new property value. */
    InlineTextEditingCommand(
        Form& form, QWidget *widget, const QByteArray &editedWidgetClass, 
        const QString &text, Command *parent = 0);

    virtual ~InlineTextEditingCommand();

    virtual int id() const { return 14; }

    virtual bool mergeWith(const QUndoCommand * command);

    virtual void execute();
    
    virtual void undo();

    Form* form() const;

    QString text() const;

    QString oldText() const;
protected:
    class Private;
    Private * const d;
};

//! kDebug() stream operator. Writes command group @a c to the debug output in a nicely formatted way.
KFORMEDITOR_EXPORT QDebug operator<<(QDebug dbg, const InlineTextEditingCommand &c); 

}

#endif
