/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Dirk Schönberger <dirk.schoenberger@sz-online.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PSCOMMENTLEXER_H
#define PSCOMMENTLEXER_H

#include <QIODevice>
#include <QString>

/**
 *@author Dirk Schönberger
 */
typedef enum { State_Comment = 0, State_CommentEncodedChar, State_Start } State;

typedef enum {
    Action_Copy = 1,
    Action_CopyOutput,
    Action_Output,
    Action_Ignore,
    Action_Abort,
    Action_OutputUnget,
    Action_InitTemp,
    Action_CopyTemp,
    Action_DecodeUnget,
    Action_ByteArraySpecial
} Action;

// TODO: Looks like this class is duplicated in ailexer.h
class StringBuffer
{
public:
    StringBuffer();
    virtual ~StringBuffer();

    void append(char c);
    void clear();
    QString toString() const;
    uint length() const;
    double toFloat();
    int toInt();
    const char *toLatin1() const;
    QString mid(uint index, uint len = 0xffffffff) const;

private:
    char *m_buffer;
    uint m_length;
    int m_capacity;

    void ensureCapacity(int p_capacity);
};

class PSCommentLexer
{
public:
    PSCommentLexer();
    virtual ~PSCommentLexer();

    virtual bool parse(QIODevice &fin);

private:
    State m_curState;
    StringBuffer m_buffer;
    StringBuffer m_temp;

    void nextStep(char c, State *newState, Action *newAction);

    void doOutput();
    uchar decode();

protected:
    virtual void parsingStarted();
    virtual void parsingFinished();
    virtual void parsingAborted();

    virtual void gotComment(const char *value);
};

class BoundingBoxExtractor : public PSCommentLexer
{
public:
    BoundingBoxExtractor();
    ~BoundingBoxExtractor() override;

    int llx() const
    {
        return m_llx;
    }
    int lly() const
    {
        return m_lly;
    }
    int urx() const
    {
        return m_urx;
    }
    int ury() const
    {
        return m_ury;
    }

private:
    int m_llx, m_lly, m_urx, m_ury;
    bool getRectangle(const char *input, int &llx, int &lly, int &urx, int &ury);

protected:
    void gotComment(const char *value) override;
};

#endif
