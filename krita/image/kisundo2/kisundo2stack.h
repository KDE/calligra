/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef KISUNDO2STACK_H
#define KISUNDO2STACK_H

#include <QObject>
#include <QString>
#include <QList>
#include <QAction>
#include <QTime>
#include <QVector>

#include "kisundo2_export.h"

class QAction;
class KisUndo2CommandPrivate;
class KisUndo2Group;
class KActionCollection;

#ifndef QT_NO_UNDOCOMMAND

class KISUNDO2_EXPORT KisUndo2Command
{
    KisUndo2CommandPrivate *d;

public:
    explicit KisUndo2Command(KisUndo2Command *parent = 0);
    explicit KisUndo2Command(const QString &text, KisUndo2Command *parent = 0);
    virtual ~KisUndo2Command();

    virtual void undo();
    void undoMergedCommands();
    virtual void redo();
    void redoMergedCommands();

    QString actionText() const;
    QString text() const;
    void setText(const QString &text);

    virtual int id() const;
    virtual bool mergeWith(const KisUndo2Command *other);

    int childCount() const;
    const KisUndo2Command *child(int index) const;

    bool hasParent();
    void setTime();
    QTime time();
    void addToMergedCommands(KisUndo2Command* cmd);
    QVector<KisUndo2Command*> mergeCommandsVector();

private:
    Q_DISABLE_COPY(KisUndo2Command)
    friend class KisUndo2QStack;

    bool m_hasParent;
    QTime m_timeOfCreation;
    QVector<KisUndo2Command*> m_mergeCommandsVector;
};

#endif // QT_NO_UNDOCOMMAND

#ifndef QT_NO_UNDOSTACK

class KISUNDO2_EXPORT KisUndo2QStack : public QObject
{
    Q_OBJECT
//    Q_DECLARE_PRIVATE(KisUndo2QStack)
    Q_PROPERTY(bool active READ isActive WRITE setActive)
    Q_PROPERTY(int undoLimit READ undoLimit WRITE setUndoLimit)

public:
    explicit KisUndo2QStack(QObject *parent = 0);
    virtual ~KisUndo2QStack();
    void clear();

    void push(KisUndo2Command *cmd);

    bool canUndo() const;
    bool canRedo() const;
    QString undoText() const;
    QString redoText() const;

    int count() const;
    int index() const;
    QString actionText(int idx) const;
    QString text(int idx) const;

#ifndef QT_NO_ACTION
    QAction *createUndoAction(QObject *parent) const;
    QAction *createRedoAction(QObject *parent) const;
#endif // QT_NO_ACTION

    bool isActive() const;
    bool isClean() const;
    int cleanIndex() const;

    void beginMacro(const QString &text);
    void endMacro();

    void setUndoLimit(int limit);
    int undoLimit() const;

    const KisUndo2Command *command(int index) const;

public Q_SLOTS:
    void setClean();
    virtual void setIndex(int idx);
    virtual void undo();
    virtual void redo();
    void setActive(bool active = true);


Q_SIGNALS:
    void indexChanged(int idx);
    void cleanChanged(bool clean);
    void canUndoChanged(bool canUndo);
    void canRedoChanged(bool canRedo);
    void undoTextChanged(const QString &undoActionText);
    void redoTextChanged(const QString &redoActionText);

private:
    // from QUndoStackPrivate
    QList<KisUndo2Command*> m_command_list;
    QList<KisUndo2Command*> m_macro_stack;
    int m_index;
    int m_clean_index;
    KisUndo2Group *m_group;
    int m_undo_limit;

    // also from QUndoStackPrivate
    void setIndex(int idx, bool clean);
    bool checkUndoLimit();

    Q_DISABLE_COPY(KisUndo2QStack)
    friend class KisUndo2Group;
};

class KISUNDO2_EXPORT KisUndo2Stack : public KisUndo2QStack
{
public:
    explicit KisUndo2Stack(QObject *parent = 0);

    // functions from KUndoStack
    QAction* createRedoAction(KActionCollection* actionCollection, const QString& actionName = QString());
    QAction* createUndoAction(KActionCollection* actionCollection, const QString& actionName = QString());

};

#endif // QT_NO_UNDOSTACK

#endif // KISUNDO2STACK_H
