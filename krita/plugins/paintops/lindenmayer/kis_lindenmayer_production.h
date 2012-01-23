#ifndef KIS_LINDENMAYER_PRODUCTION_H
#define KIS_LINDENMAYER_PRODUCTION_H

#include <QList>
#include <QVariant>
#include <QPointF>

class KisLindenmayerLetter;
class KisLindenmayerPaintOp;

class KisLindenmayerProduction
{
public:
    KisLindenmayerProduction(const KisLindenmayerPaintOp& paintOp);

    // this can change the letter and return zero or more new letters.
    QList<KisLindenmayerLetter*> produce(KisLindenmayerLetter* letter);
    void setCodeAndLimits(QString code, int minDistance, int maxDistance);

private:
    void exec(QString code);
    void execBlock(QString blockCode);
    void execCommand(QString command);
    bool evalCommandValue(QString value, KisLindenmayerLetter* letter, const QString& param); // returns true, if there was a parse error
    float evalRandClause(const QString& params);
    float evalMixAnglesClause(const QString& params);
    bool evalBoolClause(QString clause);
    bool evalBoolClausePart(QString clause);
    float evalFloatClause(QString clause);
    QVariant evalVariantClause(QString clause);

    QVariant getPaintInfo(QString key);
    inline float compute(float old, QString numberExpr, QString sign);
public:
    void runTests();

private:
    const KisLindenmayerPaintOp& m_paintOp;

    QString m_code;
    int m_minDistance;
    int m_maxDistance;

    // script environment
    QList<KisLindenmayerLetter*> m_retList;
    KisLindenmayerLetter* m_letter;
    KisLindenmayerLetter* m_newLetter;

    // errors
    bool m_isScriptError;
    QList<QPair<int, QString> > m_errorList;
    int m_scriptStatementNumber;
};

#endif // KIS_LINDENMAYER_PRODUCTION_H
