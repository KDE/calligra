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

#include <stdlib.h>
#include <ctype.h>
#include <QStringList>
#include "pscommentlexer.h"

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
    case State_CommentEncodedChar : return "encoded char (comment)";
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
  { State_CommentEncodedChar, '\\', State_Comment, Action_Copy},
  { State_CommentEncodedChar, CATEGORY_DIGIT, State_CommentEncodedChar, Action_CopyTemp},
  { State_CommentEncodedChar, CATEGORY_ANY, State_Comment, Action_DecodeUnget},
  { State_Start, '%', State_Comment, Action_Ignore},
  { State_Start, CATEGORY_ANY, State_Start, Action_Ignore},
  { State_Start, STOP, State_Start, Action_Abort}
};

PSCommentLexer::PSCommentLexer(){
}
PSCommentLexer::~PSCommentLexer(){
}

bool PSCommentLexer::parse (QIODevice& fin){
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
      default :
        qWarning ( "unknown action: %d ", action);
    }

    m_curState = newState;
  }

  parsingFinished();
  return true;
}

void PSCommentLexer::doOutput ()
{
  if (m_buffer.length() == 0) return;
  switch (m_curState)
  {
    case State_Comment :
      gotComment (m_buffer.latin1());
      break;
    default:
      qWarning ( "unknown state: %d", m_curState );
  }

  m_buffer.clear();
}

void PSCommentLexer::gotComment (const char *value) {
  qDebug ( "gotComment: %s ", value );
}

void PSCommentLexer::parsingStarted() {
  qDebug ( "parsing started" );
}

void PSCommentLexer::parsingFinished() {
  qDebug ( "parsing finished" );
}

void PSCommentLexer::parsingAborted() {
  qDebug ( "parsing aborted" );
}

void PSCommentLexer::nextStep (char c, State *newState, Action *newAction) {
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

uchar PSCommentLexer::decode()
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

/* BoundingBoxExtractor */
BoundingBoxExtractor::	BoundingBoxExtractor() : m_llx(0), m_lly(0), m_urx(0), m_ury(0) {}
BoundingBoxExtractor::~BoundingBoxExtractor() {}

void BoundingBoxExtractor::gotComment (const char *value)
{
  QString data (value);
  if (data.find("%BoundingBox:")==-1) return;

  getRectangle (value, m_llx, m_lly, m_urx, m_ury);
}

bool BoundingBoxExtractor::getRectangle (const char* input, int &llx, int &lly, int &urx, int &ury)
{
  if (input == NULL) return false;

  QString s(input);
  if (s.contains ("(atend)")) return false;

  QString s2 = s.remove("%BoundingBox:");
  QStringList values = QStringList::split (" ", s2.latin1());
  qDebug("size is %d",values.size());
//  if (values.size() < 5) return false;
  llx = values[0].toInt();
  lly = values[1].toInt();
  urx = values[2].toInt();
  ury = values[3].toInt();

  return true;
}

