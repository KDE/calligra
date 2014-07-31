/*
 *  Copyright (c) 2014 Dmitry Kazakov <dimula73@gmail.com>
 *  Copyright (c) 2014 Mohit Goyal <mohit.bits2011@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

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
#include <kundo2magicstring.h>

#include "kisundo2_export.h"

class QAction;
class KisUndo2CommandPrivate;
class KisUndo2Group;
class KActionCollection;

#ifndef QT_NO_UNDOCOMMAND

class KisUndo2Command : public KUndo2Command
{
public:
    explicit KisUndo2Command(KUndo2Command *parent = 0);
    explicit KisUndo2Command(const KUndo2MagicString &text, KUndo2Command *parent = 0);
    virtual ~KisUndo2Command();

    virtual void undoMergedCommands();
    virtual void redoMergedCommands();



    virtual int timedId();
    virtual bool timedMergeWith(KUndo2Command *other);

private:
    Q_DISABLE_COPY(KisUndo2Command)


};

#endif // QT_NO_UNDOCOMMAND
#endif

