#ifndef __kspread_autofill_h__
#define __kspread_autofill_h__

class AutoFillDeltaSequence;
class AutoFillSequenceItem;
class AutoFillSequence;

#include <qarray.h>
#include <qstring.h>
#include <qlist.h>
#include <qstrlist.h>

class KSpreadCell;

class AutoFillSequenceItem
{
public:
    enum Type { INTEGER, FLOAT, STRING, DAY, MONTH, FORMULAR };
    
    AutoFillSequenceItem( int _i );
    AutoFillSequenceItem( double _d );
    AutoFillSequenceItem( const char *_str );

    bool getDelta( AutoFillSequenceItem *_seq, double &delta );

    QString getSuccessor( int _no, double _delta );

    Type getType() { return type; }
    int getIValue() { return ivalue; }
    double getDValue() { return dvalue; }
    const char* getString() { return string.data(); }
    
protected:
    int ivalue;
    double dvalue;
    QString string;
    
    Type type;

    static QStrList *month;
    static QStrList *day;
};

class AutoFillSequence
{
public:
    AutoFillSequence( KSpreadCell *_obj );
    
    int count() { return sequence.count(); }
    
    AutoFillSequenceItem* getFirst() { return sequence.first(); }
    AutoFillSequenceItem* getNext() { return sequence.next(); }

    bool matches( AutoFillSequence* _seq, AutoFillDeltaSequence *_delta );
    
    void fillCell( KSpreadCell *src, KSpreadCell *dest, AutoFillDeltaSequence *delta, int _block );

protected:
    QList<AutoFillSequenceItem> sequence;
};

class AutoFillDeltaSequence
{
public:
    AutoFillDeltaSequence( AutoFillSequence *_first, AutoFillSequence *_next );
    ~AutoFillDeltaSequence();
    
    bool isOk() { return ok; }
    
    bool equals( AutoFillDeltaSequence *_delta );
    
    QArray<double>* getSequence() { return sequence; }

    double getItemDelta( int _pos );
    
protected:
    bool ok;    
    QArray<double>* sequence;
};

#endif
