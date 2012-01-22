#ifndef KIS_LINDENMAYER_PRODUCTION_H
#define KIS_LINDENMAYER_PRODUCTION_H

#include <QList>
#include <QVariant>

class KisLindenmayerLetter;
class KisLindenmayerPaintOp;

class KisLindenmayerProduction
{
public:
    KisLindenmayerProduction(const KisLindenmayerPaintOp& paintOp);

    // this can change the letter and return zero or more new letters.
    QList<KisLindenmayerLetter*> produce(KisLindenmayerLetter* letter);
    void setCode(QString code);

protected:
    void exec(QString code);
    void execBlock(QString blockCode);
    void execCommand(QString command);

    bool evalIfClause(QString clause);
    bool evalBoolClause(QString clause);
    float evalFloatClause(QString clause);
    QVariant evalVariantClause(QString clause);

    QVariant getPaintInfo(QString key);
    inline float compute(float old, QString numberExpr, QString sign);
public:
    void runTests();

private:
    const KisLindenmayerPaintOp& m_paintOp;

    QString m_code;

    // script environment
    QList<KisLindenmayerLetter*> m_retList;
    KisLindenmayerLetter* m_letter;
    KisLindenmayerLetter* m_newLetter;

    // errors
    bool m_isScriptError;
    QList<QPair<int, QString> > m_errorList;
    int m_scriptLineNumer;
};

#endif // KIS_LINDENMAYER_PRODUCTION_H
