
#ifndef _FILTER_KWORD_1_3__KWORDFRAMESET_H
#define _FILTER_KWORD_1_3__KWORDFRAMESET_H

class QTextStream;

#include <qstring.h>
#include <qmap.h>

#include "kword13paragraph.h"

class KWordFrameset
{
public:
    KWordFrameset( int frameType, int frameInfo, const QString& name );
    virtual ~KWordFrameset( void );
public:
    virtual bool addParagraph(const KWordParagraph& para);
    virtual void xmldump( QTextStream& iostream );
public:
    int m_numFrames; ///< Number of frames
    QMap<QString,QString> m_frameData; ///< Data of frames (### TODO: do better!)
protected:
    int m_frameType;
    int m_frameInfo;
    QString m_name; ///< Name of frameset
};

class KWordTextFrameset : public KWordFrameset
{
public:
    KWordTextFrameset( int frameType, int frameInfo, const QString& name );
    virtual ~KWordTextFrameset( void );
public:
    virtual bool addParagraph(const KWordParagraph& para);
    virtual void xmldump( QTextStream& iostream );
public:
    KWordParagraphGroup m_paragraphGroup;
};

#endif // _FILTER_KWORD_1_3__KWORDFRAMESET_H
