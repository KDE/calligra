#include "kis_lindenmayer_production.h"

#include "kis_vec.h"

#include "kis_lindenmayer_letter.h"
#include "kis_lindenmayer_paintop.h"

#include <QDebug>

KisLindenmayerProduction::KisLindenmayerProduction(const KisLindenmayerPaintOp& paintOp) : m_paintOp(paintOp)
{
    m_isScriptError = false;
}

// angles are relative to the vertical axis, +90 degrees points to right, -90 to left. positive +90+180=+270 points to the left. watch out, what happens at +180 or -180

//default parameters:
// position, angle, length, age

// computed parameters:
// endPosition, angleToSun (points towards the mouse position), distanceToSun

QList<KisLindenmayerLetter*> KisLindenmayerProduction::produce(KisLindenmayerLetter* letter) {
    m_retList.clear();

    m_letter = letter;
    m_newLetter = 0;//letter[distanceToSun] > 50 &&
    QString code = "if{letter[distanceToSun] > 50 && letter[branched] == false && letter[stem]==false && letter[leaf]==false} {"
            "newLetter = newLetter();"
            "newLetter[position] = variant(letter[endPosition]);"
            "newLetter[angle] = variant(newLetter[angleToSun]);"
            "newLetter[length] = float(30);"
            "letter[branched] = bool(true);"
            ""
            "newLetter = newLetter();"
            "newLetter[position] = variant(letter[endPosition]);"
            "newLetter[angle] = float(newLetter[angleToSun]+45);"
            "newLetter[length] = float(30);"
            "newLetter[stem] = bool(true);"
            ""
            "newLetter = newLetter();"
            "newLetter[position] = variant(letter[endPosition]);"
            "newLetter[angle] = float(newLetter[angleToSun]-45);"
            "newLetter[length] = float(30);"
            "newLetter[stem] = bool(true);"
            "}"
            ""

            "if{letter[stem]} {"
            "newLetter = newLetter();"
            "newLetter[length] = float(10);"
            "newLetter[angle] = float(letter[angle]);"
            "newLetter[position] = variant(letter[endPosition]);"
            "newLetter[leaf] = bool(true);"

            "newLetter = newLetter();"
            "newLetter[length] = float(10);"
            "newLetter[angle] = float(letter[angle]+15);"
            "newLetter[position] = variant(letter[endPosition]);"
            "newLetter[leaf] = bool(true);"

            "newLetter = newLetter();"
            "newLetter[length] = float(10);"
            "newLetter[angle] = float(letter[angle]+30);"
            "newLetter[position] = variant(letter[endPosition]);"
            "newLetter[leaf] = bool(true);"

            "newLetter = newLetter();"
            "newLetter[length] = float(10);"
            "newLetter[angle] = float(letter[angle]-15);"
            "newLetter[position] = variant(letter[endPosition]);"
            "newLetter[leaf] = bool(true);"

            "newLetter = newLetter();"
            "newLetter[length] = float(10);"
            "newLetter[angle] = float(letter[angle]-30);"
            "newLetter[position] = variant(letter[endPosition]);"
            "newLetter[leaf] = bool(true);"
            "}"
            ""

            "if{letter[drawn] && letter[branched]}{letter.delete();}"
            "if{letter[drawn] && letter[stem]}{letter.delete();}"
            "if{letter[drawn] && letter[leaf]}{letter.delete();}";

    exec(code);

    if(m_letter->markedForRemoving()) {
        delete m_letter;
        m_letter = 0;
    }
    else {
        m_retList.append(m_letter);
    }

    return m_retList;
}


inline QString cutOut(QString clause, QString start, QString end) {
    Q_ASSERT(clause.startsWith(start));
    Q_ASSERT(clause.endsWith(end));
    return clause.mid(start.length(), clause.length()-start.length()-end.length());
}

inline QString cutOut(QString clause, int start, int end) {
    Q_ASSERT(start <= end && start >= 0 && end <= clause.length());
    return clause.mid(start, end-start);
}

void KisLindenmayerProduction::exec(QString code) {
    code = code.trimmed();

    QStringList split = code.split("}");
    for(int i=0; i<split.size(); i++) {
        QString current = split.at(i).trimmed();
        if(current.length() < 2) {
            continue;
        }

        if(current.startsWith("if{")) {
            QString ifClause = current.mid(3, current.length());
            i++;
            if(i >= split.size() || split.at(i).length() < 2) {
                m_errorList.append(QPair<int , QString>(m_scriptLineNumer, "parse error, expected block with '{' and '}' and, and.. but i got nothing.."));
                m_isScriptError = true;
            }

            if(evalIfClause(ifClause) == true) {
                QString next = split.at(i).trimmed();
                if(next.at(0) != '{') {
                    m_errorList.append(QPair<int , QString>(m_scriptLineNumer, "parse error, expected {"));
                    m_isScriptError = true;
                }
                else {
                    next = next.mid(1, next.length());
                    execBlock(next);
                }
            }
        }
        else {
            if(current.at(0) != '{') {
                m_errorList.append(QPair<int , QString>(m_scriptLineNumer, "parse error, expected {"));
                m_isScriptError = true;
            }
            else {
                current = current.mid(1, current.length());
                execBlock(current);
            }
        }
    }
}

void KisLindenmayerProduction::execBlock(QString blockCode) {
    QStringList blocks = blockCode.split(";");
    foreach (QString block, blocks) {
        if(block.length()==0)
            continue;

        execCommand(block);
    }
}

void KisLindenmayerProduction::execCommand(QString command) {
    bool parseError = false;
    command = command.trimmed();

    if(command.startsWith("letter[") || command.startsWith("newLetter[")) {
        int index = command.indexOf("]");
        QString param = cutOut(command, command.indexOf("[")+1, index);
        index = command.indexOf("=")+1;

        KisLindenmayerLetter* letter;
        if(command.at(0) == 'l') {
            letter = m_letter;
        }
        else {
            if(m_newLetter == 0) {
                m_isScriptError = true;
                m_errorList.append(QPair<int , QString>(m_scriptLineNumer, "error: newLetter was not initialised in '" + command + "'"));
                return;
            }
            letter = m_newLetter;
        }

        QString value = cutOut(command, index, command.length()).trimmed();

        if(value.endsWith(")")) {
            if(value.startsWith("bool(")) {
                value = cutOut(value, "bool(", ")");
                letter->setParameter(param, evalBoolClause(value));
            }
            else if(value.startsWith("float(")) {
                value = cutOut(value, "float(", ")");
                letter->setParameter(param, evalFloatClause(value));
            }
            else if(value.startsWith("variant(")) {
                value = cutOut(value, "variant(", ")");
                letter->setParameter(param, evalVariantClause(value));
            }
            else {
                parseError = true;
            }
        }
        else {
            parseError = true;
        }
    }
    else if(command == "letter.delete()") {
        m_letter->markForRemoving();
    }
    else if(command == "newLetter = letter.clone()") {
        m_newLetter = m_letter->createCopy();
        m_retList.append(m_newLetter);
    }
    else if(command == "newLetter = newLetter()") {
        m_newLetter = new KisLindenmayerLetter(m_letter->position(), m_letter->getParameter("angle").toFloat(), &m_paintOp);
        m_retList.append(m_newLetter);
    }
    else {
        parseError = true;
    }

    if(parseError) {
        m_errorList.append(QPair<int , QString>(m_scriptLineNumer, "parse error in '" + command + "'"));
        m_isScriptError = true;
    }
}


bool KisLindenmayerProduction::evalIfClause(QString clause) {
    QStringList clauseList = clause.split("&&");
    foreach (QString c, clauseList) {
        if(evalBoolClause(c) == false)
            return false;
    }
    return true;
}

bool KisLindenmayerProduction::evalBoolClause(QString clause) {
    clause = clause.trimmed();

    QRegExp regexp = QRegExp("([<>]|==|!=)");
    int index = clause.indexOf(regexp);
    if(index == -1) {
        return evalVariantClause(clause).toBool();
    }
    QString op = regexp.cap();
    QVariant first = evalVariantClause(cutOut(clause, 0, index));
    index += op.length();
    QVariant second= evalVariantClause(cutOut(clause, index, clause.length()));

    if(op == "==") {
        return first == second;
    }
    if(op == "!=") {
        return first != second;
    }

    bool result;
    bool ok1;
    bool ok2;
    if(op == ">") {
        result = first.toFloat(&ok1) > second.toFloat(&ok2);
    }
    else if(op == "<") {
        result = first.toFloat(&ok1) < second.toFloat(&ok2);
    }
    else {
        m_errorList.append(QPair<int , QString>(m_scriptLineNumer, "error: operator '" + op + "' not supported."));
        m_isScriptError = true;
        return false;
    }

    if(ok1 && ok2) {
        return result;
    }
    else {
        m_errorList.append(QPair<int , QString>(m_scriptLineNumer, "parse error for '" + clause + "' (couldn't convert to float)"));
        m_isScriptError = true;
        return false;
    }
}

float KisLindenmayerProduction::compute(float old, QString numberExpression, QString sign) {
    QVariant numberVariant = evalVariantClause(numberExpression);

    if(numberVariant.convert(QVariant::Double)) {
        float number = numberVariant.toFloat();

        if(sign=="+") {
            return old + number;
        }
        if(sign=="-") {
            return old - number;
        }
        if(sign=="*") {
            return old * number;
        }
        if(sign=="/" && number != 0) {
            return old / number;
        }
        else {
            m_errorList.append(QPair<int , QString>(m_scriptLineNumer, "division by zero error for expression '" + numberExpression + "'"));
            m_isScriptError = true;
            return 0.0f;
        }
    }
    else {
        m_errorList.append(QPair<int , QString>(m_scriptLineNumer, "parse error for '" + numberExpression + "' (couldn't convert to float)"));
        m_isScriptError = true;
    }

    return 0.0;
}

float KisLindenmayerProduction::evalFloatClause(QString clause) {
    float result = 0;
    int lastPos = 0;
    QString lastSign = "+";
    int currentSignPos;
    // the minus sign must be escaped (otherwise it would mean a range): \-
    // c++ compilers eat '\', so we have to use \\. hence \\- matches a minus sign.
    QRegExp regexp = QRegExp("[+\\-*/]");

    currentSignPos = clause.indexOf(regexp, lastPos);
    while(currentSignPos != -1) {
        QString currentExpression = cutOut(clause, lastPos, currentSignPos);
        result = compute(result, currentExpression, lastSign);

        lastSign = regexp.cap();
        lastPos = currentSignPos+1;

        currentSignPos = clause.indexOf(regexp, lastPos);
    }

    QString currentExpression = cutOut(clause, lastPos, clause.length());
    result = compute(result, currentExpression, lastSign);

    return result;
}

QVariant KisLindenmayerProduction::evalVariantClause(QString clause) {
    clause = clause.trimmed();

    // untested yet, so disable for now. if you see this, please test it and enable..
//    if(clause.startsWith("float(") && clause.endsWith(")")) {
//        clause = cutOut(clause, "float(", ")");
//        return evalFloatClause(clause);
//    }

    if(clause.endsWith("]")) {
        if(clause.startsWith("letter[")) {
            return m_letter->getParameter(cutOut(clause, "letter[", "]"));
        }
        else if(clause.startsWith("newLetter[")) {
            if(m_newLetter != 0) {
                return m_newLetter->getParameter(cutOut(clause, "newLetter[", "]"));
            }
            else {
                m_isScriptError = true;
                m_errorList.append(QPair<int , QString>(m_scriptLineNumer, "error: newLetter was not initialised in '" + clause + "'"));
                return QVariant();
            }
        }
        else if(clause.startsWith("paintInfo[")) {
            QVariant i = getPaintInfo(cutOut(clause, "paintInfo[", "]"));
            if(i.isValid()) {
                return i;
            }
            else {
                m_isScriptError = true;
                m_errorList.append(QPair<int , QString>(m_scriptLineNumer, "error: there is no such paintInfo in '" + clause + "'"));
                return QVariant();
            }
        }
    }
    else if(clause == "true") return true;
    else if(clause == "false") return false;

    bool check;
    float number = clause.toFloat(&check);
    if(check) {
        return QVariant(number);
    }

    m_errorList.append(QPair<int , QString>(m_scriptLineNumer, "parse error for '" + clause + "'"));
    m_isScriptError = true;
    return QVariant();
}


void KisLindenmayerProduction::runTests() {
    qDebug() << "running KisLindenmayerProduction tests!";
    KisLindenmayerLetter* letterBackup = m_letter;
    KisLindenmayerLetter* newLetterBackup = m_newLetter;
    QPointF testPointF(10.0, 10.0);
    m_letter = new KisLindenmayerLetter(QPointF(1, 1), 90, &m_paintOp);
    m_letter->setParameter("floatTest50", 50.0f);
    m_letter->setParameter("booleanTrue", true);
    m_letter->setParameter("booleanFalse", false);
    m_letter->setParameter("pointF", testPointF);

    m_newLetter = 0;

    m_isScriptError = false;
    m_scriptLineNumer = 1;

    Q_ASSERT((evalVariantClause("letter[pointF]").toPointF() - QPointF(10.0, 10.0)).manhattanLength() < 0.0001); m_scriptLineNumer++;
    Q_ASSERT(evalVariantClause("true").toBool() == true); m_scriptLineNumer++;
    Q_ASSERT(evalVariantClause("false").toBool() == false); m_scriptLineNumer++;
    Q_ASSERT(evalVariantClause("letter[booleanTrue]").toBool() == true); m_scriptLineNumer++;
    Q_ASSERT(evalVariantClause("letter[booleanFalse]").toBool() == false); m_scriptLineNumer++;
    Q_ASSERT(evalVariantClause("letter[booleanFalse").isValid() == false); m_scriptLineNumer++;
    Q_ASSERT(evalVariantClause("newLetter[asdf]").isValid() == false); m_scriptLineNumer++;
    Q_ASSERT(evalVariantClause("paintInfo[asdf]").isValid() == false); m_scriptLineNumer++;
    Q_ASSERT(qFuzzyCompare(evalVariantClause("letter[floatTest50]").toFloat(), 50.0f)); m_scriptLineNumer++;

    Q_ASSERT(qFuzzyCompare(evalFloatClause("letter[floatTest50]"), 50.0f)); m_scriptLineNumer++;
    Q_ASSERT(qFuzzyCompare(evalFloatClause("letter[floatTest50]+50"), 100.0f)); m_scriptLineNumer++;
    Q_ASSERT(qFuzzyCompare(evalFloatClause("letter[floatTest50]*2"), 100.0f)); m_scriptLineNumer++;
    Q_ASSERT(qFuzzyCompare(evalFloatClause("letter[floatTest50]-20.58+10.08+10.5"), 50.0f)); m_scriptLineNumer++;
    Q_ASSERT(qFuzzyCompare(evalFloatClause(" letter[floatTest50] +   20.58 -  10.08 - 10.5 / 2"), 25.0f)); m_scriptLineNumer++;
    Q_ASSERT(qFuzzyCompare(evalFloatClause(" letter[floatTest50] ++ 20.58 --  10.08 //0/ 10.5 "), 0.0f)); m_scriptLineNumer++;

    m_newLetter = new KisLindenmayerLetter(QPointF(0, 0), 30, &m_paintOp);
    Q_ASSERT(qFuzzyCompare(evalFloatClause("letter[floatTest50] +\n\t newLetter[angle]  "), 80.0f)); m_scriptLineNumer++;
    delete m_newLetter;
    m_newLetter = 0;

    Q_ASSERT(evalBoolClause("true")); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("50")); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("letter[booleanTrue]")); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("letter[notExistantShouldBeFalse]") == false); m_scriptLineNumer++;

    Q_ASSERT(evalBoolClause("true == true")); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("false != true")); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("false == true") == false); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("true == false") == false); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("true != false") == true); m_scriptLineNumer++;

    Q_ASSERT(evalBoolClause("42 == 24") == false); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("42 != 42") == false); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("42 == 42.0000")); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("42 != 42.0000") == false); m_scriptLineNumer++;

    Q_ASSERT(evalBoolClause("50 == letter[floatTest50]")); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("letter[floatTest50] == 50.00")); m_scriptLineNumer++;

    Q_ASSERT(evalBoolClause("letter[booleanFalse] == false")); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("letter[asdf] == false")); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("letter[pointF] == letter[pointF]")); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("letter[booleanFalse] != letter[floatTest50]")); m_scriptLineNumer++;

    m_letter->setParameter("floatTest40", 40.0f);
    Q_ASSERT(evalBoolClause("letter[floatTest40] < letter[floatTest50]")); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("letter[floatTest50] > letter[floatTest40]")); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("letter[floatTest50] != letter[floatTest40]")); m_scriptLineNumer++;


    Q_ASSERT(evalBoolClause("false = true") == false); m_scriptLineNumer++;
    Q_ASSERT(evalBoolClause("2 * 5") == false); m_scriptLineNumer++;

    Q_ASSERT(evalIfClause("false != true")); m_scriptLineNumer++;
    Q_ASSERT(evalIfClause("false != true && true == true && 40>30")); m_scriptLineNumer++;
    Q_ASSERT(evalIfClause("false != true && true == true && 40<30") == false); m_scriptLineNumer++;
    Q_ASSERT(evalIfClause("false == true && true == true && 40>30") == false); m_scriptLineNumer++;
    Q_ASSERT(evalIfClause("false != true && true != true && 40>30") == false); m_scriptLineNumer++;
    Q_ASSERT(evalIfClause("50 == letter[floatTest50] && letter[floatTest40] < letter[floatTest50] && letter[floatTest50] != letter[floatTest40]")); m_scriptLineNumer++;

    execCommand("letter[newBool]=bool(true)"); m_scriptLineNumer++;
    execCommand("letter[newFloat] = float(40+50 - letter[floatTest50]  ) "); m_scriptLineNumer++;
    execCommand("letter[newPoint] = variant(letter[pointF])"); m_scriptLineNumer++;

    // i want to make this work:
//    m_newLetter = new KisLindenmayerLetter(QPointF(0, 0), 30, &m_paintOp);
//    execCommand("newLetter[angle] = 30"); m_scriptLineNumer++;

    Q_ASSERT(m_letter->getParameter("newBool").toBool());
    Q_ASSERT(qFuzzyCompare(m_letter->getParameter("newFloat").toFloat(), 40.0f));
    Q_ASSERT(m_letter->getParameter("newPoint").toPointF() == testPointF);

//    delete m_newLetter;
    m_newLetter = 0;

    if(m_isScriptError) {
        QStringList errors;
        errors << "parse error for 'letter[booleanFalse'";
        errors << "error: newLetter was not initialised in 'newLetter[asdf]'";
        errors << "error: there is no such paintInfo in 'paintInfo[asdf]'";
        errors << "parse error for ''";
        errors << "parse error for '' (couldn't convert to float)";
        errors << "parse error for ''";
        errors << "parse error for '' (couldn't convert to float)";
        errors << "parse error for ''";
        errors << "parse error for '' (couldn't convert to float)";
        errors << "division by zero error for expression '0'";
        errors << "parse error for 'false = true'";
        errors << "parse error for '2 * 5'";

        QPair<int, QString> pair;
        int i=0;
        foreach (pair, m_errorList) {
            if(pair.second != errors.at(i)) {
                qDebug() << QString("unexpected parse error in assert no ") + QString::number(pair.first) + QString(":") + pair.second;
                Q_ASSERT(false);
            }
            i++;
        }
        m_errorList.clear();
        m_isScriptError = false;
    }
    else {
        Q_ASSERT(false);
    }


    QString code = "if{letter[distanceToSun] > 50 && letter[branched] == false} {"
            "newLetter = newLetter();"
            "newLetter[position] = letter[endPosition];"
            "newLetter[angle] = newLetter[angleToSun];"
            "newLetter[length] = 30;"
            "letter[branched] = true;"
            "}";

    delete m_letter;
    m_letter = letterBackup;
    m_newLetter = newLetterBackup;
}

QVariant KisLindenmayerProduction::getPaintInfo(QString key) {
    const KisPaintInformation& paintInfo = m_paintOp.getSunInformations();
    if(key == "pos") return paintInfo.pos();
    if(key == "pressure") return paintInfo.pressure();
    if(key == "movement") return toQPointF(paintInfo.movement());
    if(key == "xTilt") return paintInfo.xTilt();
    if(key == "yTilt") return paintInfo.yTilt();
    if(key == "rotation") return paintInfo.rotation();
    if(key == "tangentialPressure") return paintInfo.tangentialPressure();
    if(key == "perspective") return paintInfo.perspective();
    if(key == "time") return (float) paintInfo.currentTime();

    return QVariant();
}







