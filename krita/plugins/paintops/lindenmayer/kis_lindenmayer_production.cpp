#include "kis_lindenmayer_production.h"

#include "kis_vec.h"
#include <Eigen/Core>
#include <Eigen/Geometry>
//USING_PART_OF_NAMESPACE_EIGEN

#include "kis_lindenmayer_letter.h"
#include "kis_lindenmayer_paintop.h"
#include "kis_lindenmayer_script_error_repeater.h"

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

    exec(m_code);

    m_letter->setParameter("age", m_letter->getParameter("age").toFloat() + 1);

    if(m_isScriptError) {
        KisLindenmayerScriptErrorRepeater::instance()->repeatErrors(m_errorList);
        m_errorList.clear();
        m_isScriptError = false;
    }

    if(m_letter->markedForRemoving()) {
        delete m_letter;
        m_letter = 0;
    }
    else {
        m_retList.append(m_letter);
    }

    return m_retList;
}

void KisLindenmayerProduction::setCode(QString code) {
    m_code = code;
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
#ifndef QT_NO_DEBUG
    if(code == "runTests") {
        runTests();
        return;
    }
#endif


    code = code.trimmed();
    m_scriptStatementNumber = 0;

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
                m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "parse error, expected block with '{' and '}' and, and.. but i got nothing.."));
                m_isScriptError = true;
            }

            if(evalBoolClause(ifClause) == true) {
                QString next = split.at(i).trimmed();
                if(next.at(0) != '{') {
                    m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "parse error, expected {"));
                    m_isScriptError = true;
                    m_scriptStatementNumber += next.count(";");
                }
                else {
                    next = next.mid(1, next.length());
                    execBlock(next);
                }
            }
        }
        else {
            if(current.at(0) != '{') {
                m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "parse error, expected {"));
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
    m_scriptStatementNumber++;
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
                m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "error: newLetter was not initialised in '" + command + "'"));
                return;
            }
            letter = m_newLetter;
        }

        QString value = cutOut(command, index, command.length()).trimmed();
        parseError = evalCommandValue(value, letter, param);
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
        m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "parse error in '" + command + "'"));
        m_isScriptError = true;
    }
}

float KisLindenmayerProduction::evalRandClause(const QString& params) {
    QStringList str = params.split(",");
    float lowerBound = 0.0f;
    float upperBound = 1.0f;
    if(str.size() == 2) {
        lowerBound = evalFloatClause(str.at(0));
        upperBound = evalFloatClause(str.at(1));
    }
    else if(str.size() == 1 && str.at(0) != "") {
        upperBound = evalFloatClause(str.at(0));
    }

    if(lowerBound < upperBound) {
        float rand = qrand() % RAND_MAX; // just to be sure, that the qt lib has not a bigger random range..
        rand /= (float) RAND_MAX;
        rand *= upperBound - lowerBound; // scale
        rand += lowerBound;
        return rand;
    }
    else {
        m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "parse error, while evaluating rand(" + params + "), lower bound greater higher bound."));
        m_isScriptError = true;
        return 0.0f;
    }
}

//inline Eigen::Vector2d angleToVector(double angle) {
//    double angleInRadians = (M_PI/180.0)*angle;
//    Eigen::Rotation2D<double> rot = Eigen::Rotation2D<double>(angleInRadians);

//    Eigen::Vector2d upVector(0.0, -1.0); // coord system is negative at the top

//    return rot*upVector;
//}

inline float normaliseAngle(float angle) {
    while(angle < -180) {
        angle+=360;
    }

    while(angle > 180) {
        angle-= 360;
    }
    return angle;
}

float KisLindenmayerProduction::evalMixAnglesClause(const QString& params) {
    QStringList str = params.split(",");
    if(str.size() != 3) {
        m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "parse error, while evaluating mixAngles("
                                                + params + "), parameter count is wrong (usage: mixAngles(firstAngle, secondAngle, percentageOfFirstAngle); percentage is from 0 to 1)."));
        m_isScriptError = true;
        return 0.0f;
    }

    float firstAngle = evalFloatClause(str.at(0));
    float secondAngle = evalFloatClause(str.at(1));
    float mixRatio = evalFloatClause(str.at(2)); // 0.3 means 30 % of the first angle
    if(mixRatio < 0)
        mixRatio = 0;
    if(mixRatio > 1)
        mixRatio = 1;

    firstAngle = normaliseAngle(firstAngle);
    secondAngle = normaliseAngle(secondAngle);

    float firstAnglePlus = (firstAngle > 0) ? firstAngle : firstAngle + 360;
    float secondAnglePlus = (secondAngle > 0) ? secondAngle : secondAngle + 360;

    if(fabs(firstAnglePlus - secondAnglePlus) <= 180) {
        return normaliseAngle((firstAnglePlus - secondAnglePlus)*mixRatio + secondAnglePlus);
    }

    return mixRatio * firstAngle + (1-mixRatio) * secondAngle;

//    Eigen::Vector2d firstVector = angleToVector(firstAngle) * mixRatio;
//    Eigen::Vector2d secondVector = angleToVector(secondAngle) * (1 - mixRatio);

//    Eigen::Vector2d sum = firstVector + secondVector;

//    if(sum.norm() < 0.01)
//        return 0.0f;

//    sum.normalize();

//    double radianAngle = acos(sum.dot(Eigen::Vector2d(0.0, -1.0)));
//    if(sum.x() < 0) {
//        radianAngle*=-1.0;
//    }
//    float ret = (180.0/M_PI)*radianAngle;
//    return ret;
}

bool KisLindenmayerProduction::evalCommandValue(QString value, KisLindenmayerLetter* letter, const QString& param) {
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
        else if(value.startsWith("rand(")) {
            value = cutOut(value, "rand(", ")");
            letter->setParameter(param, evalRandClause(value));
        }
        else if(value.startsWith("mixAngles(")) {
            value = cutOut(value, "mixAngles(", ")");
            letter->setParameter(param, evalMixAnglesClause(value));
        }
        else {
            return true;
        }
    }
    else {
        letter->setParameter(param, evalVariantClause(value));
    }
    return false;
}

bool KisLindenmayerProduction::evalBoolClause(QString clause) {
    m_scriptStatementNumber++;
    QStringList clauseList = clause.split("&&");
    foreach (QString c, clauseList) {
        if(evalBoolClausePart(c) == false)
            return false;
    }
    return true;
}

bool KisLindenmayerProduction::evalBoolClausePart(QString clause) {
    clause = clause.trimmed();
    if(clause.contains("&&"))
        return evalBoolClause(clause);

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
        m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "error: operator '" + op + "' not supported."));
        m_isScriptError = true;
        return false;
    }

    if(ok1 && ok2) {
        return result;
    }
    else {
        m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "parse error for '" + clause + "' (couldn't convert to float)"));
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
            m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "division by zero error for expression '" + numberExpression + "'"));
            m_isScriptError = true;
            return 0.0f;
        }
    }
    else {
        m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "parse error for '" + numberExpression + "' (couldn't convert to float)"));
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

    clause = clause.trimmed();
    if(clause.at(0) == '-') {
        lastPos = 1;
        lastSign = "-";
    }

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

    if(clause.startsWith("float(") && clause.endsWith(")")) {
        clause = cutOut(clause, "float(", ")");
        return evalFloatClause(clause);
    }

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
                m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "error: newLetter was not initialised in '" + clause + "'"));
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
                m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "error: there is no such paintInfo in '" + clause + "'"));
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

    m_errorList.append(QPair<int , QString>(m_scriptStatementNumber, "parse error for '" + clause + "'"));
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
    m_scriptStatementNumber = 1;

    Q_ASSERT((evalVariantClause("letter[pointF]").toPointF() - QPointF(10.0, 10.0)).manhattanLength() < 0.0001); m_scriptStatementNumber++;
    Q_ASSERT(evalVariantClause("true").toBool() == true); m_scriptStatementNumber++;
    Q_ASSERT(evalVariantClause("false").toBool() == false); m_scriptStatementNumber++;
    Q_ASSERT(evalVariantClause("letter[booleanTrue]").toBool() == true); m_scriptStatementNumber++;
    Q_ASSERT(evalVariantClause("letter[booleanFalse]").toBool() == false); m_scriptStatementNumber++;
    Q_ASSERT(evalVariantClause("letter[booleanFalse").isValid() == false); m_scriptStatementNumber++;
    Q_ASSERT(evalVariantClause("newLetter[asdf]").isValid() == false); m_scriptStatementNumber++;
    Q_ASSERT(evalVariantClause("paintInfo[asdf]").isValid() == false); m_scriptStatementNumber++;
    Q_ASSERT(qFuzzyCompare(evalVariantClause("letter[floatTest50]").toFloat(), 50.0f)); m_scriptStatementNumber++;

    Q_ASSERT(qFuzzyCompare(evalFloatClause("letter[floatTest50]"), 50.0f)); m_scriptStatementNumber++;
    Q_ASSERT(qFuzzyCompare(evalFloatClause("letter[floatTest50]+50"), 100.0f)); m_scriptStatementNumber++;
    Q_ASSERT(qFuzzyCompare(evalFloatClause("letter[floatTest50]*2"), 100.0f)); m_scriptStatementNumber++;
    Q_ASSERT(qFuzzyCompare(evalFloatClause("letter[floatTest50]-20.58+10.08+10.5"), 50.0f)); m_scriptStatementNumber++;
    Q_ASSERT(qFuzzyCompare(evalFloatClause(" letter[floatTest50] +   20.58 -  10.08 - 10.5 / 2"), 25.0f)); m_scriptStatementNumber++;
    Q_ASSERT(qFuzzyCompare(evalFloatClause(" letter[floatTest50] ++ 20.58 --  10.08 //0/ 10.5 "), 0.0f)); m_scriptStatementNumber++;
    Q_ASSERT(qFuzzyCompare(evalFloatClause(" - 30 "), -30.0f)); m_scriptStatementNumber++;

    m_newLetter = new KisLindenmayerLetter(QPointF(0, 0), 30, &m_paintOp);
    Q_ASSERT(qFuzzyCompare(evalFloatClause("letter[floatTest50] +\n\t newLetter[angle]  "), 80.0f)); m_scriptStatementNumber++;
    delete m_newLetter;
    m_newLetter = 0;

    Q_ASSERT(evalBoolClause("true")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("50")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("letter[booleanTrue]")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("letter[notExistantShouldBeFalse]") == false); m_scriptStatementNumber++;

    Q_ASSERT(evalBoolClause("true == true")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("false != true")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("false == true") == false); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("true == false") == false); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("true != false") == true); m_scriptStatementNumber++;

    Q_ASSERT(evalBoolClause("42 == 24") == false); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("42 != 42") == false); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("42 == 42.0000")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("42 != 42.0000") == false); m_scriptStatementNumber++;

    Q_ASSERT(evalBoolClause("50 == letter[floatTest50]")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("letter[floatTest50] == 50.00")); m_scriptStatementNumber++;

    Q_ASSERT(evalBoolClause("letter[booleanFalse] == false")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("letter[asdf] == false")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("letter[pointF] == letter[pointF]")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("letter[booleanFalse] != letter[floatTest50]")); m_scriptStatementNumber++;

    m_letter->setParameter("floatTest40", 40.0f);
    Q_ASSERT(evalBoolClause("letter[floatTest40] < letter[floatTest50]")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("letter[floatTest50] > letter[floatTest40]")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("letter[floatTest50] != letter[floatTest40]")); m_scriptStatementNumber++;


    Q_ASSERT(evalBoolClause("false = true") == false); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("2 * 5") == false); m_scriptStatementNumber++;

    Q_ASSERT(evalBoolClause("false != true")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("false != true && true == true && 40>30")); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("false != true && true == true && 40<30") == false); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("false == true && true == true && 40>30") == false); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("false != true && true != true && 40>30") == false); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("50 == letter[floatTest50] && letter[floatTest40] < letter[floatTest50] && letter[floatTest50] != letter[floatTest40]")); m_scriptStatementNumber++;

    execCommand("letter[newBool]=bool(true)"); m_scriptStatementNumber++;
    execCommand("letter[newFloat] = float(40+50 - letter[floatTest50]  ) "); m_scriptStatementNumber++;
    execCommand("letter[newPoint] = variant(letter[pointF])"); m_scriptStatementNumber++;
    execCommand("letter[test30] = 30"); m_scriptStatementNumber++;
    execCommand("letter[test30_2] = letter[test30]"); m_scriptStatementNumber++;
    execCommand("letter[test30_2] = letter[test30]"); m_scriptStatementNumber++;
    execCommand("letter[boolTest] = bool(40>30 && true && false != true && letter[booleanTrue])"); m_scriptStatementNumber++;

    execCommand("letter[rand1] = rand()"); m_scriptStatementNumber++;
    execCommand("letter[rand2] = rand(5)"); m_scriptStatementNumber++;
    execCommand("letter[rand3] = rand(-5, 10)"); m_scriptStatementNumber++;
    execCommand("letter[rand4] = rand(10, 20)"); m_scriptStatementNumber++;

    execCommand("letter[mixAngle1] = mixAngles(10, 170, 0.5)"); m_scriptStatementNumber++;
    execCommand("letter[mixAngle2] = mixAngles(-10, -170, 0.5)"); m_scriptStatementNumber++;
    execCommand("letter[mixAngle3] = mixAngles(-90, 90, 0.5)"); m_scriptStatementNumber++;
    execCommand("letter[mixAngle4] = mixAngles(-170, 150, 0.5)"); m_scriptStatementNumber++;
    execCommand("letter[mixAngle5] = mixAngles(180, 270, 0.5)"); m_scriptStatementNumber++;
    execCommand("letter[mixAngle6] = mixAngles(270, -50, 0.5)"); m_scriptStatementNumber++;
    execCommand("letter[mixAngle7] = mixAngles(50, 100, 0.2)"); m_scriptStatementNumber++;
    execCommand("letter[mixAngle8] = mixAngles(10, -90, 0.7)"); m_scriptStatementNumber++;

    qDebug() << "rand 1 (range  0 - 1 )= " << m_letter->getParameter("rand1").toString();
    qDebug() << "rand 2 (range  0 - 5 )= " << m_letter->getParameter("rand2").toString();
    qDebug() << "rand 3 (range -5 - 10)= " << m_letter->getParameter("rand3").toString();
    qDebug() << "rand 4 (range 10 - 20)= " << m_letter->getParameter("rand4").toString();

    Q_ASSERT(m_letter->getParameter("newBool").toBool());
    Q_ASSERT(qFuzzyCompare(m_letter->getParameter("newFloat").toFloat(), 40.0f));
    Q_ASSERT(m_letter->getParameter("newPoint").toPointF() == testPointF);
    Q_ASSERT(qFuzzyCompare(m_letter->getParameter("test30").toFloat(), 30.0f));
    Q_ASSERT(qFuzzyCompare(m_letter->getParameter("test30_2").toFloat(), 30.f));
    Q_ASSERT(m_letter->getParameter("rand1").toFloat() > 0 && m_letter->getParameter("rand1").toFloat() < 1);
    Q_ASSERT(m_letter->getParameter("rand2").toFloat() > 0 && m_letter->getParameter("rand2").toFloat() < 5);
    Q_ASSERT(m_letter->getParameter("rand3").toFloat() > -5 && m_letter->getParameter("rand3").toFloat() < 10);
    Q_ASSERT(m_letter->getParameter("rand4").toFloat() > 10 && m_letter->getParameter("rand4").toFloat() < 20);

    Q_ASSERT(qFuzzyCompare(m_letter->getParameter("mixAngle1").toFloat(), 90));
    Q_ASSERT(qFuzzyCompare(m_letter->getParameter("mixAngle2").toFloat(), -90));
    Q_ASSERT(qFuzzyCompare(m_letter->getParameter("mixAngle3").toFloat(), 0) || qFuzzyCompare(m_letter->getParameter("mixAngle3").toFloat(), 180));
    Q_ASSERT(qFuzzyCompare(m_letter->getParameter("mixAngle4").toFloat(), 170));
    Q_ASSERT(qFuzzyCompare(m_letter->getParameter("mixAngle5").toFloat(), -135));
    Q_ASSERT(qFuzzyCompare(m_letter->getParameter("mixAngle6").toFloat(), -70));
    Q_ASSERT(qFuzzyCompare(m_letter->getParameter("mixAngle7").toFloat(), 90));
    Q_ASSERT(qFuzzyCompare(m_letter->getParameter("mixAngle8").toFloat(), -20));

    // some new features, placing tests at the bottom, so that parse errors are at the correct position..
    Q_ASSERT(qFuzzyCompare(evalVariantClause("float(50+30)").toFloat(), 80.0f)); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("float(letter[floatTest50] - 40) == float(0.25*letter[floatTest40])") == true); m_scriptStatementNumber++;
    Q_ASSERT(evalBoolClause("float(letter[floatTest50] - 40) == float(0.25*letter[floatTest40]) && true != false") == true); m_scriptStatementNumber++;



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
            if(pair.second != errors.at(i)) { // errors.at(i) could be out of range, if there are new errors, but we can crash, because it's an error anyway..
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







