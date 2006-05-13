/* This file is part of the KDE project
   Copyright (C) 2002, Dirk Schönberger <dirk.schoenberger@sz-online.de>

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

#include <ctype.h>
#include <stdlib.h>
#include <QStringList>
#include "ailexer.h"

#define CATEGORY_WHITESPACE -1
#define CATEGORY_ALPHA -2
#define CATEGORY_DIGIT -3
#define CATEGORY_SPECIAL -4
#define CATEGORY_LETTERHEX -5
#define CATEGORY_INTTOOLONG -6

#define CATEGORY_ANY -127

#define MAX_INTLEN 9
#define MIN_HEXCHARS 6

#define STOP 0

int iswhitespace(char c){
  return (c==' ')||(c=='\n')||(c=='\t')||(c=='\r');
}

int isSpecial(char c){
  return (c=='*')||(c=='_')||(c=='?')||(c=='~')||(c=='-')||(c=='^')||(c=='`')||(c=='!')||(c=='.')||(c=='@')||(c=='&')||(c=='$')||(c=='=');
}

int isletterhex(char c){
  return (c=='A')||(c=='B')||(c=='C')||(c=='D')||(c=='E')||(c=='F');
}

const char*statetoa (State state){
  switch (state)
  {
    case State_Comment : return "comment";
    case State_Integer : return "integer";
    case State_Float : return "float";
    case State_String : return "string";
    case State_Token : return "token";
    case State_Reference : return "reference";
    case State_Start : return "start";
    case State_BlockStart : return "block start";
    case State_BlockEnd : return "block end";
    case State_ArrayStart : return "array start";
    case State_ArrayEnd : return "array end";
    case State_Byte : return "byte";
    case State_ByteArray : return "byte array";
    case State_StringEncodedChar : return "encoded char (string)";
    case State_CommentEncodedChar : return "encoded char (comment)";
    case State_ByteArray2 : return "byte array (mode 2)";
    default : return "unknown";
  }
}

typedef struct {
  State oldState;
  char c;
  State newState;
  Action action;
} Transition;

static Transition transitions[] = {
  { State_Comment, '\n', State_Start, Action_Output},
  { State_Comment, '\r', State_Start, Action_Output},
  { State_Comment, '\\', State_CommentEncodedChar, Action_InitTemp},
  { State_Comment, CATEGORY_ANY, State_Comment, Action_Copy},
  { State_Integer, CATEGORY_DIGIT, State_Integer, Action_Copy},
  { State_Integer, CATEGORY_WHITESPACE, State_Start, Action_Output},
  { State_Integer, '.', State_Float, Action_Copy},
  { State_Integer, ']', State_Start, Action_OutputUnget},
  { State_Integer, '}', State_Start, Action_OutputUnget},
  { State_Integer, '#', State_Byte, Action_Copy },
  { State_Integer, '/', State_Start, Action_OutputUnget },
  { State_Integer, '{', State_Start, Action_OutputUnget },
  { State_Integer, '%', State_Start, Action_OutputUnget },
  { State_Integer, CATEGORY_LETTERHEX, State_ByteArray2, Action_Copy },
  { State_Integer, CATEGORY_INTTOOLONG, State_ByteArray2, Action_Copy },
  { State_Integer, CATEGORY_ANY, State_Start, Action_Abort},
  { State_Float, CATEGORY_DIGIT, State_Float, Action_Copy},
  { State_Float, CATEGORY_WHITESPACE, State_Start, Action_Output},
  { State_Float, ']', State_Start, Action_OutputUnget},
  { State_Float, '}', State_Start, Action_OutputUnget},
  { State_Float, CATEGORY_ANY, State_Start, Action_Abort},
  { State_Token, CATEGORY_ALPHA, State_Token, Action_Copy},
  { State_Token, CATEGORY_DIGIT, State_Token, Action_Copy},
  { State_Token, CATEGORY_SPECIAL, State_Token, Action_Copy},
  { State_Token, '}', State_Start, Action_OutputUnget},
  { State_Token, ']', State_Start, Action_OutputUnget},
  { State_Token, '{', State_BlockStart, Action_Output},
  { State_Token, '}', State_BlockEnd, Action_Output},
  { State_Token, '/', State_Start, Action_OutputUnget},
  { State_Token, CATEGORY_WHITESPACE, State_Start, Action_Output},
  { State_Token, CATEGORY_ANY, State_Start, Action_Abort},
  { State_String, ')', State_Start, Action_Output},
  { State_String, '\\', State_StringEncodedChar, Action_InitTemp},
  { State_String, CATEGORY_ANY, State_String, Action_Copy},
//  { State_Array, CATEGORY_ALPHA, State_Array, Action_Copy},
//  { State_Array, CATEGORY_DIGIT, State_Array, Action_Copy},
//  { State_Array, ' ', State_Array, Action_Copy},
  { State_BlockStart, CATEGORY_ANY, State_Start, Action_OutputUnget },
  { State_BlockEnd, CATEGORY_ANY, State_Start, Action_OutputUnget },
  { State_ArrayStart, CATEGORY_ANY, State_Start, Action_OutputUnget },
  { State_ArrayEnd, CATEGORY_ANY, State_Start, Action_OutputUnget },
  { State_Reference, '#', State_Reference, Action_Copy },
  { State_Reference, CATEGORY_ALPHA, State_Reference, Action_Copy },
  { State_Reference, CATEGORY_DIGIT, State_Reference, Action_Copy },
  { State_Reference, CATEGORY_SPECIAL, State_Reference, Action_Copy },
  { State_Reference, CATEGORY_ANY, State_Start, Action_OutputUnget },
  { State_Byte, '/', State_Start, Action_OutputUnget },
  { State_Byte, CATEGORY_DIGIT, State_Byte, Action_Copy},
  { State_Byte, CATEGORY_ALPHA, State_Byte, Action_Copy},
  { State_Byte, CATEGORY_WHITESPACE, State_Start, Action_Output},
  { State_ByteArray, '>', State_Start, Action_Output },
  { State_ByteArray, CATEGORY_ALPHA, State_ByteArray, Action_Copy },
  { State_ByteArray, CATEGORY_DIGIT, State_ByteArray, Action_Copy },
  { State_ByteArray, CATEGORY_WHITESPACE, State_ByteArray, Action_Ignore },
  { State_ByteArray, CATEGORY_ANY, State_Start, Action_Abort },
  { State_StringEncodedChar, '\\', State_String, Action_Copy},
  { State_StringEncodedChar, CATEGORY_DIGIT, State_StringEncodedChar, Action_CopyTemp},
  { State_StringEncodedChar, CATEGORY_ANY, State_String, Action_DecodeUnget},
  { State_CommentEncodedChar, '\\', State_Comment, Action_Copy},
  { State_CommentEncodedChar, CATEGORY_DIGIT, State_CommentEncodedChar, Action_CopyTemp},
  { State_CommentEncodedChar, CATEGORY_ANY, State_Comment, Action_DecodeUnget},
  { State_ByteArray2, '\n', State_Start, Action_Output},
  { State_ByteArray2, '\r', State_Start, Action_Output},
  { State_ByteArray2, '}', State_Start, Action_ByteArraySpecial},
  { State_ByteArray2, CATEGORY_WHITESPACE, State_Start, Action_Output},
  { State_ByteArray2, CATEGORY_DIGIT, State_ByteArray2, Action_Copy},
  { State_ByteArray2, CATEGORY_LETTERHEX, State_ByteArray2, Action_Copy},
  { State_ByteArray2, CATEGORY_ALPHA, State_Token, Action_Copy},
  { State_ByteArray2, CATEGORY_ANY, State_Start, Action_Abort},
  { State_Start, '%', State_Comment, Action_Ignore},
  { State_Start, CATEGORY_DIGIT, State_Integer, Action_Copy},
  { State_Start, '-', State_Integer, Action_Copy},
  { State_Start, '+', State_Integer, Action_Copy},
  { State_Start, '.', State_Float, Action_Copy},
  { State_Start, '/', State_Reference, Action_Ignore },
  { State_Start, '(', State_String, Action_Ignore},
  { State_Start, '{', State_BlockStart, Action_Copy},
  { State_Start, '}', State_BlockEnd, Action_Copy},
  { State_Start, '[', State_ArrayStart, Action_Copy},
  { State_Start, ']', State_ArrayEnd, Action_Copy},
  { State_Start, '<', State_ByteArray, Action_Ignore},
  { State_Start, CATEGORY_ALPHA, State_Token, Action_Copy},
  { State_Start, CATEGORY_WHITESPACE, State_Start, Action_Output},
  { State_Start, CATEGORY_SPECIAL, State_Token, Action_Copy},
  { State_Start, CATEGORY_LETTERHEX, State_ByteArray2, Action_Copy},
  { State_Start, CATEGORY_ANY, State_Start, Action_Abort},
  { State_Start, STOP, State_Start, Action_Abort}
};

AILexer::AILexer(){
}
AILexer::~AILexer(){
}

bool AILexer::parse (QIODevice& fin){
  char c;

  m_buffer.clear();
  m_curState = State_Start;

  parsingStarted();

  while (!fin.atEnd())
  {
    c = fin.getch ();

//    qDebug ("got %c", c);

    State newState;
    Action action;

    nextStep (c, &newState, &action);

    switch (action)
    {
      case Action_Copy :
        m_buffer.append (c);
        break;
      case Action_CopyOutput :
        m_buffer.append (c);
        doOutput();
        break;
      case Action_Output :
        doOutput();
        break;
      case Action_OutputUnget :
        doOutput();
        fin.ungetch(c);
        break;
      case Action_Ignore :
        /* ignore */
        break;
      case Action_Abort :
        qWarning ( "state %s / %s char %c (%d)" , statetoa(m_curState), statetoa(newState), c, c );
        parsingAborted();
        return false;
        break;
      case Action_InitTemp :
        m_temp.clear();
        break;
      case Action_CopyTemp :
        m_temp.append (c);
        break;
      case Action_DecodeUnget :
        m_buffer.append (decode());
        fin.ungetch(c);
        break;
      // in Postscript Quelltext: Kombination F}
      case Action_ByteArraySpecial :
        m_curState = State_Token;
        doOutput();
        fin.ungetch(c);
        break;
      default :
        qWarning ( "unknown action: %d ", action);
    }

    m_curState = newState;
  }

  parsingFinished();
  return true;
}

void AILexer::doOutput ()
{
  if (m_buffer.length() == 0) return;
  switch (m_curState)
  {
    case State_Comment :
      gotComment (m_buffer.latin1());
      break;
    case State_Integer :
      gotIntValue (m_buffer.toInt());
      break;
    case State_Float :
      gotDoubleValue (m_buffer.toFloat());
      break;
    case State_String :
      gotStringValue (m_buffer.latin1());
      break;
    case State_Token :
      gotToken (m_buffer.latin1());
      break;
    case State_Reference :
      gotReference (m_buffer.latin1());
      break;
    case State_BlockStart :
      gotBlockStart ();
      break;
    case State_BlockEnd :
      gotBlockEnd ();
      break;
    case State_Start :
      break;
    case State_ArrayStart :
      gotArrayStart ();
      break;
    case State_ArrayEnd :
      gotArrayEnd ();
      break;
    case State_Byte :
      gotByte (getByte());
      break;
    case State_ByteArray :
    case State_ByteArray2 :
      doHandleByteArray ();
      break;
    default:
      qWarning ( "unknown state: %d", m_curState );
  }

  m_buffer.clear();
}

void AILexer::gotComment (const char *value) {
  qDebug ( "gotComment: %s ", value );
}

void AILexer::gotIntValue (int value) {
  qDebug ( "gotInt: %d ", value );
}

void AILexer::gotDoubleValue (double value) {
  qDebug ( "gotDouble: %f ", value );
}

void AILexer::gotStringValue (const char *value) {
  qDebug ( "gotString: %s ", value );
}

void AILexer::gotToken (const char *value) {
  qDebug ( "gotToken: %s ", value );
}

void AILexer::gotReference (const char *value) {
  qDebug ( "gotReference: %s ", value );
}

void AILexer::gotBlockStart (){
  qDebug ( "gotBlockStart" );
}

void AILexer::gotBlockEnd (){
  qDebug ( "gotBlockEnd" );
}

void AILexer::gotArrayStart (){
  qDebug ( "gotArrayStart" );
}

void AILexer::gotArrayEnd (){
  qDebug ( "gotArrayEnd" );
}

void AILexer::parsingStarted() {
  qDebug ( "parsing started" );
}

void AILexer::parsingFinished() {
  qDebug ( "parsing finished" );
}

void AILexer::parsingAborted() {
  qDebug ( "parsing aborted" );
}

void AILexer::gotByte (uchar value) {
  qDebug ( "got byte %d" , value );
}

void AILexer::gotByteArray (const QByteArray &data) {
  qDebug ( "got byte array" );
/*  for ( uint i = 0; i < data.size(); i++ )
  {
    uchar value = data[i];
    qDebug( "%d: %x", i, value );
  }
  qDebug ( "/byte array" ); */

}


void AILexer::nextStep (char c, State *newState, Action *newAction) {
  int i=0;

  while (true) {
    Transition trans = transitions[i];

    if (trans.c == STOP) {
      *newState = trans.newState;
      *newAction = trans.action;
      return;
    }

    bool found = false;

    if (trans.oldState == m_curState) {
      switch (trans.c) {
        case CATEGORY_WHITESPACE : found = isspace(c); break;
        case CATEGORY_ALPHA : found = isalpha(c); break;
        case CATEGORY_DIGIT : found = isdigit(c); break;
        case CATEGORY_SPECIAL : found = isSpecial(c); break;
        case CATEGORY_LETTERHEX : found = isletterhex(c); break;
        case CATEGORY_INTTOOLONG : found = m_buffer.length() > MAX_INTLEN; break;
        case CATEGORY_ANY : found = true; break;
        default : found = (trans.c == c);
      }

      if (found) {
        *newState = trans.newState;
        *newAction = trans.action;

        return;
      }
    }


    i++;
  }
}

void AILexer::doHandleByteArray ()
{
  // Special case - too short
  if (m_buffer.length () < MIN_HEXCHARS)
  {
    gotToken (m_buffer.latin1());
    return;
  }

  uint strIdx = 0;
  uint arrayIdx = 0;

  QByteArray data (m_buffer.length() >> 1);

  while (strIdx < m_buffer.length())
  {
    const QString &item = m_buffer.mid (strIdx, 2);
    uchar val = item.toShort(NULL, 16);
    data[arrayIdx] = val;
    strIdx += 2;
    arrayIdx++;
  }

  gotByteArray (data);
}

uchar AILexer::getByte()
{
//  qDebug ("convert string to byte (%s)", m_buffer.latin1());

  QStringList list = QStringList::split ("#", m_buffer.toString());
  int radix = list[0].toShort();
  uchar value = list[1].toShort (NULL, radix);

  return value;
}

uchar AILexer::decode()
{
  uchar value = m_temp.toString().toShort(NULL, 8);
//  qDebug ("got encoded char %c",value);
  return value;
}

/* StringBuffer implementation */

int initialSize = 20;
int addSize = 10;

StringBuffer::StringBuffer () {
  m_buffer = (char*)calloc (initialSize, sizeof(char));
  m_length = 0;
  m_capacity = initialSize;
}

StringBuffer::~StringBuffer (){
  free(m_buffer);
}

void StringBuffer::append (char c){
  ensureCapacity(m_length + 1);
  m_buffer[m_length] = c;
  m_length++;
}

void StringBuffer::clear(){
  for (uint i=0; i<m_length; i++) m_buffer[i] = '\0';
  m_length = 0;
}

QString StringBuffer::toString() const {
  QString ret(m_buffer);
  return ret;
}

void StringBuffer::ensureCapacity (int p_capacity) {
  if (m_capacity >= p_capacity) return;

  int newSize = m_capacity + addSize;
  if (p_capacity > newSize) newSize = p_capacity;

  char* oldBuffer = m_buffer;
  char *newBuffer = (char*)calloc (newSize, sizeof(char));
  strcpy (newBuffer, m_buffer);
  free(oldBuffer);
  m_buffer = newBuffer;
  m_capacity = newSize;
}

uint StringBuffer::length() {
  return m_length;
}

double StringBuffer::toFloat() {
  QString data = toString();
  return data.toFloat();
}

int StringBuffer::toInt() {
  QString data = toString();
  return data.toInt();
}

const char *StringBuffer::latin1() {
  return m_buffer;
}

QString StringBuffer::mid( uint index, uint len) const {
  QString data = toString();
  return data.mid(index,len);
}
