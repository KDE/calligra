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

#include <QDebug>
#include <klocale.h>
#include <kstandardaction.h>
#include <kactioncollection.h>
#include "kisundo2stack.h"
#include "kisundo2stack_p.h"
#include "kisundo2group.h"
#include <KoIcon.h>
#include <QTime>

#ifndef QT_NO_UNDOCOMMAND



KisUndo2Command::KisUndo2Command(const QString &text, KUndo2Command *parent):KUndo2Command(text,parent)
{

}



KisUndo2Command::KisUndo2Command(KUndo2Command *parent):KUndo2Command(parent)
{

}



KisUndo2Command::~KisUndo2Command()
{

}




void KisUndo2Command::redoMergedCommands()
{

    if(!mergeCommandsVector().isEmpty()){

        QVectorIterator<KUndo2Command*> it(mergeCommandsVector());
        it.toBack();
        while(it.hasPrevious())
        {
            KisUndo2Command* cmd = dynamic_cast<KisUndo2Command*>(it.previous());
            cmd->redoMergedCommands();
        }
    }
    redo();
}

void KisUndo2Command::undoMergedCommands()
{
    undo();
    if(!mergeCommandsVector().isEmpty()){
        QVectorIterator<KUndo2Command*> it(mergeCommandsVector());
        it.toFront();
        while(it.hasNext())
        {
            KisUndo2Command* cmd =  dynamic_cast<KisUndo2Command*>(it.next());
            cmd->undoMergedCommands();
        }
    }
}
int KisUndo2Command::timedId()
{
    return 1;
}

bool KisUndo2Command::timedMergeWith(KUndo2Command *other)
{
    KUndo2Command::timedMergeWith(other);
    return true;
}



#endif // QT_NO_UNDOCOMMAND

