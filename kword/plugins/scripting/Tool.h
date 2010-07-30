/*
 * This file is part of KWord
 *
 * Copyright (c) 2007 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTING_TOOL_H
#define SCRIPTING_TOOL_H

#include <QSignalMapper>
#include <KAction>

#include <kdebug.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoTextEditor.h>
#include <KWView.h>
#include <KoCanvasBase.h>

#include "Module.h"
#include "TextCursor.h"

class KWDocument;

namespace Scripting
{

/**
* The Tool class provides access to functionality like handling for
* example current/active selections.
*
* Python sample code that walks through the actions the Tool provides
* and executes each of them;
* \code
* import KWord
* tool = KWord.tool()
* def triggered(actionname):
*     print "Action %s executed" % actionname
* tool.connect("actionTriggered(QString)",triggered)
* for n in tool.actionNames():
*     print "Trying to execute %s" % tool.actionText(n)
*     tool.triggerAction(n)
* \endcode
*/
class Tool : public QObject
{
    Q_OBJECT
public:
    explicit Tool(Module* module) : QObject(module), m_module(module) {
        KWView* v = dynamic_cast< KWView* >(m_module->view());
        KoCanvasBase* c = v ? v->canvasBase() : 0;
        m_toolproxy = c ? c->toolProxy() : 0;

        m_signalMapper = new QSignalMapper(this);
        QHash<QString, KAction*> actionhash = actions();
        for (QHash<QString, KAction*>::const_iterator it = actionhash.constBegin(); it != actionhash.constEnd(); ++it) {
            connect(it.value(), SIGNAL(triggered()), m_signalMapper, SLOT(map()));
            m_signalMapper->setMapping(it.value() , it.key());
        }
        connect(m_signalMapper, SIGNAL(mapped(const QString&)), this, SIGNAL(actionTriggered(const QString&)));

        connect(KoToolManager::instance(), SIGNAL(changedTool(KoCanvasController*, int)), this, SIGNAL(changedTool()));
    }
    virtual ~Tool() {}

    KoToolSelection* toolSelection() const {
        return m_toolproxy ? m_toolproxy->selection() : 0;
    }
    KoTextEditor* textSelection() const {
        return dynamic_cast< KoTextEditor* >(toolSelection());
    }
    QHash<QString, KAction*> actions() const {
        return m_toolproxy ? m_toolproxy->actions() : QHash<QString, KAction*>();
    }

public slots:

    /** Return true if there is actualy a selection. */
    bool hasSelection() const {
        return toolSelection() != 0;
    }

    /** Return true if the selected object is a text object. */
    bool hasTextSelection() const {
        return textSelection() != 0;
    }

    /** Return the selected text. */
    QString selectedText() const {
        KoTextEditor* h = textSelection();
        return h ? h->selectedText() : QString();
    }
//TODO return the textEditor
    /** Return the active/current \a TextCursor object. */
/*    QObject* cursor() {
        KoTextEditor* h = textSelection();
        return h ? new TextCursor(this, h->caret()) : 0;
    }
*/
    /** Set the active/current \a TextCursor object. */
/*    bool setCursor(QObject* cursor) {
        kDebug(32010) << "Scripting::Selection::setCursor";
        TextCursor* textcursor = dynamic_cast< TextCursor* >(cursor);
        if (! textcursor) return false;
        KWView* v = dynamic_cast< KWView* >(m_module->view());
        KoCanvasBase* c = v ? v->KoCanvasBase() : 0;
        KoResourceManager* r = c ? c->resourceManager() : 0;
        if (! r) return false;
        QVariant variant;
        variant.setValue((QObject*) &textcursor->cursor());
//TODO store TextEditor?
// the above can't work;  storing a pointer to a value based object (QTextCursor).
// I don't even think its possible to store a QTextCursor in any form in a QVarient. (TZ)
        //r->setResource(KWord::CurrentTextCursor, variant);
        return true;
    }
*/
    /** Return a list of the action names. */
    QStringList actionNames() {
        return QStringList(actions().keys());
    }
    /** Return the text the action with \p actionname has. */
    QString actionText(const QString& actionname) {
        QAction* a = actions()[ actionname ];
        return a ? a->text() : QString();
    }
    /** Trigger the action with \p actionname . */
    void triggerAction(const QString& actionname) {
        QAction* a = actions()[ actionname ];
        if (a) a->trigger();
    }

signals:

    /** This signal got emitted if an action was triggered. */
    void actionTriggered(const QString& actionname);

    /** This signal got emitted if the tool changed. */
    void changedTool();

private:
    Module* m_module;
    KoToolProxy* m_toolproxy;
    QSignalMapper* m_signalMapper;
};

}

#endif
