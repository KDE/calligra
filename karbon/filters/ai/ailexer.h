/***************************************************************************
                          ailexer.h  -  description
                             -------------------
    begin                : Wed Jul 18 2001
    copyright            : (C) 2001 by Dirk Schönberger
    email                : schoenberger@signsoft.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AILEXER_H
#define AILEXER_H

#include <qiodevice.h>
#include <qstring.h>

/**
  *@author Dirk Schönberger
  */
typedef enum {
  State_Comment=0,
  State_Integer,
  State_Float,
  State_String,
  State_Token,
  State_Reference,
  State_Start,
  State_BlockStart,
  State_BlockEnd,
  State_ArrayStart,
  State_ArrayEnd,
  State_Byte,
  State_ByteArray,
} State;

typedef enum {
  Action_Copy=1,
  Action_CopyOutput,
  Action_Output,
  Action_Ignore,
  Action_Abort,
  Action_OutputUnget } Action;

class AILexer {
public: 
	AILexer();
	virtual ~AILexer();

  virtual bool parse (QIODevice& fin);
private:
  State m_curState;
  QString m_buffer;

/*  State nextState (char c);
  Action nextAction (char c);  */

  void nextStep (char c, State* newState, Action* newAction);

  void doOutput ();

protected:
  virtual void parsingStarted();
  virtual void parsingFinished();
  virtual void parsingAborted();

  virtual void gotComment (const char *value);
  virtual void gotIntValue (int value);
  virtual void gotDoubleValue (double value);
  virtual void gotStringValue (const char *value);
  virtual void gotToken (const char *value);
  virtual void gotReference (const char *value);
  virtual void gotBlockStart ();
  virtual void gotBlockEnd ();
  virtual void gotArrayStart ();
  virtual void gotArrayEnd ();

};

#endif

