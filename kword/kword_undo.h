/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Undo (Header)                                          */
/******************************************************************/

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
