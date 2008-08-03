/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include <qmap.h>
#include <q3dict.h>
#include <q3ptrlist.h>
#include <q3ptrdict.h>
#include <qvariant.h>
#include <qdom.h>
#include <Q3CString>

#include <k3command.h>
#include "utils.h"

class QRect;
class QPoint;
class QStringList;

namespace KFormDesigner
{

class WidgetPropertySet;
class ObjectTreeItem;
class Container;
class Form;

//! Base class for KFormDesigner's commands
class KFORMEDITOR_EXPORT Command : public K3Command
{
public:
    Command();
    virtual ~Command();

    virtual void debug() = 0;
};

/*! This command is used when changing a property for one or more widgets. \a oldvalues is a QMap
 of the old values of the property for every widget, to allow reverting the change. \a value is
 the new value of the property. You can use the simpler constructor for a single widget.
 */
class KFORMEDITOR_EXPORT PropertyCommand : public Command
{
public:
    PropertyCommand(WidgetPropertySet *set, const Q3CString &wname, const QVariant &oldValue,
                    const QVariant &value, const Q3CString &property);
    PropertyCommand(WidgetPropertySet *set, const QMap<Q3CString, QVariant> &oldvalues,
                    const QVariant &value, const Q3CString &property);

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;
    Q3CString property() const {
        return m_property;
    }

    void  setValue(const QVariant &value);
    const QMap<Q3CString, QVariant>& oldValues() const {
        return m_oldvalues;
    }
    virtual void debug();

protected:
    WidgetPropertySet *m_propSet;
    QVariant m_value;
    QMap<Q3CString, QVariant> m_oldvalues;
    Q3CString m_property;
};

/*! This command is used when moving multiples widgets at the same time, while holding Ctrl or Shift.
 You need to supply a list of widget names, and the position of the cursor before moving. Use setPos()
  to tell the new cursor pos every time it changes.*/
class KFORMEDITOR_EXPORT GeometryPropertyCommand : public Command
{
public:
    GeometryPropertyCommand(WidgetPropertySet *set, const QStringList &names, const QPoint& oldPos);

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;
    void setPos(const QPoint& pos);
    virtual void debug();

protected:
    WidgetPropertySet *m_propSet;
    QStringList m_names;
    QPoint m_oldPos;
    QPoint m_pos;
};

/*! This command is used when an item in 'Align Widgets position' is selected. You just need
to give the list of widget names (the selected ones), and the
  type of alignment (see the enum for possible values). */
class KFORMEDITOR_EXPORT AlignWidgetsCommand : public Command
{
public:
    enum { AlignToGrid = 100, AlignToLeft, AlignToRight, AlignToTop, AlignToBottom };

    AlignWidgetsCommand(int type, WidgetList &list, Form *form);

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;
    virtual void debug();

protected:
    Form *m_form;
    int m_type;
    QMap<Q3CString, QPoint> m_pos;
};

/*! This command is used when an item in 'Adjust Widgets Size' is selected. You just need
 to give the list of widget names (the selected ones), and the
  type of size modification (see the enum for possible values). */
class KFORMEDITOR_EXPORT AdjustSizeCommand : public Command
{
public:
    enum { SizeToGrid = 200, SizeToFit, SizeToSmallWidth, SizeToBigWidth,
           SizeToSmallHeight, SizeToBigHeight
         };

    AdjustSizeCommand(int type, WidgetList &list, Form *form);

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;
    virtual void debug();

protected:
    QSize  getSizeFromChildren(ObjectTreeItem *item);

protected:
    Form *m_form;
    int m_type;
    QMap<Q3CString, QPoint> m_pos;
    QMap<Q3CString, QSize> m_sizes;
};

/*! This command is used when switching the layout of a Container. It remembers the old pos
 of every widget inside the Container. */
class KFORMEDITOR_EXPORT LayoutPropertyCommand : public PropertyCommand
{
public:
    LayoutPropertyCommand(WidgetPropertySet *set, const Q3CString &wname,
                          const QVariant &oldValue, const QVariant &value);

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;
    virtual void debug();

protected:
    Form *m_form;
    QMap<Q3CString, QRect>  m_geometries;
};

/*! This command is used when inserting a widger using toolbar or menu. You only need to give
the parent Container and the widget pos.
 The other information is taken from FormManager. */
class KFORMEDITOR_EXPORT InsertWidgetCommand : public Command
{
public:
    InsertWidgetCommand(Container *container);

    /*! This ctor allows to set explicit class name and position.
     Used for dropping widgets on the form surface.
     If \a namePrefix is empty, widget's unique name is constructed using
     hint for \a className (WidgetLibrary::namePrefix()),
     otherwise, \a namePrefix is used to generate widget's name.
     This allows e.g. inserting a widgets having name constructed using
     */
    InsertWidgetCommand(Container *container, const Q3CString& className,
                        const QPoint& pos, const Q3CString& namePrefix = Q3CString());

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;
    virtual void debug();

    //! \return inserted widget's name
    Q3CString widgetName() const {
        return m_name;
    }

protected:
    Form *m_form;
    QString m_containername;
    QPoint m_point;
    Q3CString m_name;
    Q3CString m_class;
    QRect m_insertRect;
};

/*! This command is used when creating a layout from some widgets using "Lay out in..." menu item.
 It remembers the old pos of every widget, and takes care of updating ObjectTree too. You need
 to supply a WidgetList of the selected widgets. */
class KFORMEDITOR_EXPORT CreateLayoutCommand : public Command
{
public:
    CreateLayoutCommand(int layoutType, WidgetList &list, Form *form);
    CreateLayoutCommand() {
        ;
    } // for BreakLayoutCommand

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;
    virtual void debug();

protected:
    Form *m_form;
    QString m_containername;
    QString m_name;
    QMap<Q3CString, QRect> m_pos;
    int m_type;
};

/*! This command is used when the 'Break Layout' menu item is selected. It does exactly the
 opposite of CreateLayoutCommand. */
class KFORMEDITOR_EXPORT BreakLayoutCommand : public CreateLayoutCommand
{
public:
    BreakLayoutCommand(Container *container);

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;
    virtual void debug();
};

/*! This command is used when pasting widgets. You need to give the QDomDocument containing
the widget(s) to paste, and optionnally the point where to paste widgets. */
class KFORMEDITOR_EXPORT PasteWidgetCommand : public Command
{
public:
    PasteWidgetCommand(QDomDocument &domDoc, Container *container, const QPoint& p = QPoint());

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;
    virtual void debug();

protected:
    /*! Internal function used to change the coordinates of a widget to \a newpos
     before pasting it (to paste it at the position of the contextual menu). It modifies
       the "geometry" property of the QDomElement representing the widget. */
    void changePos(QDomElement &widg, const QPoint &newpos);
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
    Form *m_form;
    QString m_data;
    QString m_containername;
    QPoint m_point;
    QStringList m_names;
};

/*! This command is used when deleting a widget using the "Delete" menu item.
You need to give a WidgetList of the selected widgets. */
class KFORMEDITOR_EXPORT DeleteWidgetCommand : public Command
{
public:
    DeleteWidgetCommand(WidgetList &list, Form *form);

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;
    virtual void debug();

protected:
    QDomDocument m_domDoc;
    Form *m_form;
    QMap<Q3CString, Q3CString>  m_containers;
    QMap<Q3CString, Q3CString>  m_parents;
};

/*! This command is used when cutting widgets. It is basically a DeleteWidgetCommand
which also updates the clipboard contents. */
class KFORMEDITOR_EXPORT CutWidgetCommand : public DeleteWidgetCommand
{
public:
    CutWidgetCommand(WidgetList &list, Form *form);

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;
    virtual void debug();

protected:
    QString m_data;
};

/*! A Command Group is a command that holds several subcommands.
 It will appear as one to the user and in the command history,
 but it can use the implementation of multiple commands internally.
 It extends KMacroCommand by providing the list of commands executed.
 Selected subcommands can be marked as nonexecutable by adding them using
 addCommand(KCommand *command, bool allowExecute) special method.
*/
class KFORMEDITOR_EXPORT CommandGroup : public Command
{
public:
    CommandGroup(const QString & name, WidgetPropertySet *propSet);
    virtual ~CommandGroup();

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
    virtual void unexecute();

    virtual QString name() const;

    /*! \return a list of all subcommands of this group.
     Note that if a given subcommand is a group itself,
     it will not be expanded to subcommands on this list. */
    const QList< K3Command*> commands() const;

    /*! Resets all 'allowExecute' flags that was set in addCommand().
     Call this after calling CommandGroup::execute() to ensure that
     in the future, when REDO is be executed, all subcommands will be executed. */
    void resetAllowExecuteFlags();

    virtual void debug();

protected:
    class SubCommands;
    SubCommands *m_subCommands;
    //! Used to store pointers to subcommands that shouldn't be executed
    //! on CommandGroup::execute()
    Q3PtrDict<char> m_commandsShouldntBeExecuted;
    WidgetPropertySet *m_propSet;
};

}

#endif
