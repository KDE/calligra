/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef kword_undo_h
#define kword_undo_h

#include <qstring.h>
#include <qlist.h>
#include <qobject.h>

#include "parag.h"

enum KWCommandType {TEXT_CHANGE};
#define MAX_UNDO_REDO 100

class KWordDocument;

/******************************************************************/
/* Class: KWCommand                                               */
/******************************************************************/

class KWCommand
{
public:
    KWCommand( QString _name ) : name( _name ) {}
    virtual ~KWCommand() {}

    virtual KWCommandType getType() = 0;

    virtual void execute() = 0;
    virtual void unexecute() = 0;

    QString getName() { return name; }

protected:
    QString name;

};

/******************************************************************/
/* Class: KWTextChangeCommand                                     */
/******************************************************************/

class KWTextChangeCommand : public KWCommand
{
public:
    KWTextChangeCommand( QString _name, KWordDocument *_doc, KWFormatContext *_fc, unsigned int _textPos )
        : KWCommand( _name ) { doc = _doc; fc = _fc; textPos = _textPos; parags.setAutoDelete( false ); }

    virtual KWCommandType getType() { return TEXT_CHANGE; }

    virtual void execute();
    virtual void unexecute();

    void addParag( KWParag &_parag ) { parags.append( new KWParag( _parag ) ); }
    void setBefore( QString _before ) { before = _before; }
    void setAfter( QString _after ) { after = _after; }

    void setFrameSet( int _num ) { frameset = _num; }

protected:
    QList<KWParag> parags;
    QString before, after;
    int frameset;
    KWordDocument *doc;
    KWFormatContext *fc;
    unsigned int textPos;

};

/******************************************************************/
/* Class: KWCommandHistory                                        */
/******************************************************************/

class KWCommandHistory : public QObject
{
    Q_OBJECT

public:
    KWCommandHistory();

    void addCommand( KWCommand *_command );
    void undo();
    void redo();

    QString getUndoName();
    QString getRedoName();

protected:
    QList<KWCommand> history;
    int current;

signals:
    void undoRedoChanged( QString, QString );

};

#endif
