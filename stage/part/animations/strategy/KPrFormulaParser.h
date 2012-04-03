#ifndef KPRFORMULAPARSER_H
#define KPRFORMULAPARSER_H

#include <QVector>
#include <QString>
#include <QStack>
#include "KPrValueParser.h"

class KoShape;
class KoTextBlockData;
class KPrAnimationCache;

class KPrFormulaParser
{
public:
    KPrFormulaParser(QString formula, KoShape *shape, KoTextBlockData *textBlockData);
    QString formula() const;
    qreal eval(KPrAnimationCache * cache, qreal time) const;
    bool valid() const;
protected:
    Tokens scan(QString formula);
    void compile(const Tokens& tokens) const;
    qreal identifierToValue(QString identifier, KPrAnimationCache * cache, qreal time) const;
    qreal formulaToValue(QString identifier, KPrAnimationCache * cache, qreal arg) const;
private:
    KoShape *m_shape;
    KoTextBlockData *m_textBlockData;
    QString m_formula;
    mutable bool m_fcompiled;
    mutable bool m_fvalid;
    mutable QVector<Opcode> m_codes;
    mutable QVector<qreal> m_constants;
    mutable QVector<QString> m_identifier;
    mutable QVector<QString> m_functions;
};

#endif // KPRFORMULAPARSER_H
