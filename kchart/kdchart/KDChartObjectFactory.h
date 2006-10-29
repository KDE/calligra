#ifndef KDCHARTQSA_H
#define KDCHARTQSA_H
#include <qsobjectfactory.h>

class KDChartObjectFactory :public QSObjectFactory {

public:
    KDChartObjectFactory();
    virtual QObject* create( const QString& className, const QSArgumentList& args, QObject* context );
protected:
    QObject* createKDChartWidget( const QSArgumentList& args );
    QObject* createKDChartTableData( const QSArgumentList& args );
    QObject* createQDateTime( const QSArgumentList& args );
    QObject* createQDate( const QSArgumentList& args );
    QObject* createQTime( const QSArgumentList& args );
    QObject* createKDChartTextPiece( const QSArgumentList& args );
    QObject* createQFont( const QSArgumentList& args );
    QObject* createKDChartCustomBox( const QSArgumentList& args );
    QObject* createQColor(const QSArgumentList& args );
    QObject* createKDChartPropertySet(const QSArgumentList& args );

    bool isNumber( const QVariant& );
    bool checkArgCount( const QString& className, int count, int min, int max );
    bool checkArgsIsQtClass( const QSArgumentList& args, int index, const char* expected, const char* constructing );
    bool getString( const QSArgumentList& arg, int index, QString* str, const char* constructing  );

    bool getNumber( const QSArgumentList& args, int index, double* number, const char* constructing );
    bool getNumber( const QSArgumentList& args, int index, int* number, const char* constructing );
    bool getNumber( const QSArgumentList& args, int index, uint* number, const char* constructing );

    bool getBool( const QSArgumentList& arg, int index, bool* b, const char* constructing );
    bool checkIsQtVariant( const QSArgumentList& arg, int index, QVariant::Type expected, const QString& variantName, const char* constructing );
};


#endif /* KDCHARTQSA_H */

