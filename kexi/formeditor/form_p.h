/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2014 Jarosław Staniek <staniek@kde.org>

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

#ifndef KFORMDESIGNERFORM_P_H
#define KFORMDESIGNERFORM_P_H

#include <kundo2stack.h>
#include <KActionCollection>

#include "resizehandle.h"
#include "commands.h"
#include "form.h"

//! @todo pixmapcollection
#ifndef KEXI_NO_PIXMAPCOLLECTION
#include "pixmapcollection.h"
#endif

#include <KPropertySet>
#include <KProperty>

#include <QProxyStyle>

class QStyleOption;

namespace KFormDesigner
{

class ObjectTree;

//! Used to alter the widget's style at design time
class DesignModeStyle : public QProxyStyle
{
public:
    explicit DesignModeStyle(const QString &baseStyleName);

    //! Reimplemented to remove handling of the State_MouseOver state.
    virtual void drawControl(ControlElement element, const QStyleOption *option,
                             QPainter *p, const QWidget *w = 0) const;

private:
    QStyleOption* alterOption(ControlElement element, const QStyleOption *option) const;
};

//--------------

//! @internal
class FormPrivate
{
public:
    FormPrivate(Form *form, WidgetLibrary* _library);
    ~FormPrivate();

    void enableAction(const char* name, bool enable);

    void initPropertiesDescription();

    QString propertyCaption(const QByteArray &name);

    QString valueCaption(const QByteArray &name);

    void addPropertyCaption(const QByteArray &property, const QString &caption);

    void addValueCaption(const QByteArray &value, const QString &caption);

    KPropertyListData* createValueList(WidgetInfo *winfo, const QStringList &list);

    //! Sets color of selected widget(s) to value of @a p.
    //! @a roleMethod can be backgroundColor or foregroundColor.
    //! Makes background inherited if @a roleMethod if background and value is null.
    void setColorProperty(KProperty& p,
                          QPalette::ColorRole (QWidget::*roleMethod)() const,
                          const QVariant& value);

    Form::Mode mode;
    Form::State state;
    Form::Features features;
    QPoint insertionPoint;
    QPointer<Container>  toplevel;
    ObjectTree *topTree;
    QPointer<QWidget> widget;
    KPropertySet propertySet;
    QWidgetList selected;
    ResizeHandleSet::Hash resizeHandles;
    QByteArray selectedClass;
    bool modified;
    bool interactive;
    bool isUndoing;
    bool isRedoing;
    bool snapToGrid;
    int gridSize;
    QString  filename;
    KUndo2Stack undoStack;
    KActionCollection internalCollection;
    KActionCollection *collection;
    KFormDesigner::ActionGroup* widgetActionGroup;

    ObjectTreeList  tabstops;
    bool autoTabstops;
#ifdef KFD_SIGSLOTS
    ConnectionBuffer  *connBuffer;
#endif

#ifndef KEXI_NO_PIXMAPCOLLECTION
    PixmapCollection  *pixcollection;
#endif

    //! This map is used to store cursor shapes before inserting (so we can restore them later)
    QHash<QObject*, QCursor> cursors;

    //! This string list is used to store the widgets which hasMouseTracking() == true (eg lineedits)
    QStringList *mouseTrackers;

    FormWidget  *formWidget;

    //! A set of head properties to be stored in a .ui file.
    //! This includes KFD format version.
    QHash<QByteArray, QString> headerProperties;

    //! Format version, set by FormIO or on creating a new form.
    uint formatVersion;
    //! Format version, set by FormIO's loader or on creating a new form.
    uint originalFormatVersion;

#ifdef KFD_SIGSLOTS
    //! true is slot connection is curently being painted
    Connection *connection;
#endif

    //! used to update command's value when undoing
    PropertyCommand  *lastCommand;
    PropertyCommandGroup  *lastCommandGroup;
    uint idOfPropertyCommand;

    //! Command that being executed through Form::addCommand()
    const Command *executingCommand;
    bool slotPropertyChangedEnabled;
    bool slotPropertyChanged_addCommandEnabled;
    bool insideAddPropertyCommand;
    bool selectWidgetEnabled;

    // i18n stuff
    QMap<QByteArray, QString> propCaption;
    QMap<QByteArray, QString> propValCaption;

    QStyle *designModeStyle;

    QPointer<QWidget> inlineEditor;
    QPointer<Container> inlineEditorContainer;
    QByteArray editedWidgetClass;
    QString originalInlineText;
    bool pixmapsStoredInline;

    WidgetLibrary * const library;

private:
    Form *q;
};
}

#endif
