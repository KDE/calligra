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
#include <kundo2command.h>
#include <kundo2stack.h>

#include "kisundo2_export.h"

class QAction;
class KisUndo2CommandPrivate;
class KisUndo2Group;
class KActionCollection;

#ifndef QT_NO_UNDOCOMMAND

class KISUNDO2_EXPORT KisUndo2Command :public KUndo2Command
{
public:
    explicit KisUndo2Command(KUndo2Command *parent = 0);
    explicit KisUndo2Command(const QString &text, KUndo2Command *parent = 0);
    virtual ~KisUndo2Command();

    virtual void undoMergedCommands();
    virtual void redoMergedCommands();


    using KUndo2Command::timedId;
    virtual int timedId();
    using KUndo2Command::timedMergeWith;
    virtual bool timedMergeWith(KUndo2Command *other);

private:
    Q_DISABLE_COPY(KisUndo2Command)


};

#endif // QT_NO_UNDOCOMMAND
#endif

