
#ifndef _FILTER_KWORD_1_3__KWORDFRAMESET_H
#define _FILTER_KWORD_1_3__KWORDFRAMESET_H

#include <qstring.h>

class KWordFrameset
{
public:
    KWordFrameset( int frameType, int frameInfo, const QString& name );
    virtual ~KWordFrameset( void );
protected:
    int m_frameType;
    int m_frameInfo;
    QString m_name; ///< Name of frameset
};

class KWordNormalTextFrameset : public KWordFrameset
{
public:
    KWordNormalTextFrameset( int frameType, int frameInfo, const QString& name );
    virtual ~KWordNormalTextFrameset( void );
};

#endif // _FILTER_KWORD_1_3__KWORDFRAMESET_H
