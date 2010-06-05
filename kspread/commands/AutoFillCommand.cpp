/* This file is part of the KDE project
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2002-2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2000-2002 Laurent Montel <montel@kde.org>
   Copyright 2000-2001 Werner Trobin <trobin@kde.org>
   Copyright 1999-2001 David Faure <faure@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>
   Copyright 1998-1999 Stephan Kulow <coolo@kde.org>
   Copyright 1998 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "AutoFillCommand.h"

#include "Localization.h"
#include "Map.h"
#include "Sheet.h"
#include "Value.h"
#include "ValueConverter.h"

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>

#include <QList>
#include <QRegExp>

#include <math.h>

using namespace KSpread;

QStringList *AutoFillCommand::month = 0;
QStringList *AutoFillCommand::shortMonth = 0;
QStringList *AutoFillCommand::day = 0;
QStringList *AutoFillCommand::shortDay = 0;
QStringList *AutoFillCommand::other = 0;

/**********************************************************************************
 *
 * AutoFillSequenceItem
 *
 **********************************************************************************/

namespace KSpread
{
/**
 * A cell content for auto-filling.
 */
class AutoFillSequenceItem
{
public:
    enum Type { VALUE, FORMULA, DAY, SHORTDAY, MONTH, SHORTMONTH, OTHER };

    explicit AutoFillSequenceItem(const Cell& cell);

    Value delta(AutoFillSequenceItem *_seq, bool *ok) const;

    Value nextValue(int _no, Value _delta) const;
    Value prevValue(int _no, Value _delta) const;

    Type type() const {
        return m_type;
    }
    Value value() const {
        return m_value;
    }
    int otherEnd() const {
        return m_otherEnd;
    }
    int otherBegin() const {
        return m_otherBegin;
    }

protected:
    Value   m_value;
    Type    m_type;
    int     m_otherBegin;
    int     m_otherEnd;
};
}

AutoFillSequenceItem::AutoFillSequenceItem(const Cell& cell)
        : m_value()
        , m_type(VALUE)
        , m_otherBegin(0)
        , m_otherEnd(0)
{
    if (cell.isFormula()) {
        m_value = Value(cell.encodeFormula());
        m_type = FORMULA;
    } else if (cell.isDate()) {
        m_value = cell.sheet()->map()->converter()->asDate(cell.value());
        m_type = VALUE;
    } else if (cell.isTime() || cell.value().format() == Value::fmt_DateTime) {
        m_value = cell.sheet()->map()->converter()->asDateTime(cell.value());
        m_type = VALUE;
    } else if (cell.value().isNumber()) {
        m_value = cell.value();
        m_type = VALUE;
    } else {
        m_value = cell.value();
        m_type = VALUE;

        if (AutoFillCommand::month == 0) {
            AutoFillCommand::month = new QStringList();
            AutoFillCommand::month->append(i18n("January"));
            AutoFillCommand::month->append(i18n("February"));
            AutoFillCommand::month->append(i18n("March"));
            AutoFillCommand::month->append(i18n("April"));
            AutoFillCommand::month->append(i18n("May"));
            AutoFillCommand::month->append(i18n("June"));
            AutoFillCommand::month->append(i18n("July"));
            AutoFillCommand::month->append(i18n("August"));
            AutoFillCommand::month->append(i18n("September"));
            AutoFillCommand::month->append(i18n("October"));
            AutoFillCommand::month->append(i18n("November"));
            AutoFillCommand::month->append(i18n("December"));
        }

        if (AutoFillCommand::shortMonth == 0) {
            AutoFillCommand::shortMonth = new QStringList();
            AutoFillCommand::shortMonth->append(i18n("Jan"));
            AutoFillCommand::shortMonth->append(i18n("Feb"));
            AutoFillCommand::shortMonth->append(i18n("Mar"));
            AutoFillCommand::shortMonth->append(i18n("Apr"));
            AutoFillCommand::shortMonth->append(i18nc("May short", "May"));
            AutoFillCommand::shortMonth->append(i18n("Jun"));
            AutoFillCommand::shortMonth->append(i18n("Jul"));
            AutoFillCommand::shortMonth->append(i18n("Aug"));
            AutoFillCommand::shortMonth->append(i18n("Sep"));
            AutoFillCommand::shortMonth->append(i18n("Oct"));
            AutoFillCommand::shortMonth->append(i18n("Nov"));
            AutoFillCommand::shortMonth->append(i18n("Dec"));
        }

        if (AutoFillCommand::day == 0) {
            AutoFillCommand::day = new QStringList();
            AutoFillCommand::day->append(i18n("Monday"));
            AutoFillCommand::day->append(i18n("Tuesday"));
            AutoFillCommand::day->append(i18n("Wednesday"));
            AutoFillCommand::day->append(i18n("Thursday"));
            AutoFillCommand::day->append(i18n("Friday"));
            AutoFillCommand::day->append(i18n("Saturday"));
            AutoFillCommand::day->append(i18n("Sunday"));
        }

        if (AutoFillCommand::shortDay == 0) {
            AutoFillCommand::shortDay = new QStringList();
            AutoFillCommand::shortDay->append(i18n("Mon"));
            AutoFillCommand::shortDay->append(i18n("Tue"));
            AutoFillCommand::shortDay->append(i18n("Wed"));
            AutoFillCommand::shortDay->append(i18n("Thu"));
            AutoFillCommand::shortDay->append(i18n("Fri"));
            AutoFillCommand::shortDay->append(i18n("Sat"));
            AutoFillCommand::shortDay->append(i18n("Sun"));
        }

        if (AutoFillCommand::other == 0) {
            // AutoFillCommand::other = new QStringList();
            KSharedConfigPtr config = KGlobal::activeComponent().config();
            AutoFillCommand::other = new QStringList(config->group("Parameters").readEntry("Other list", QStringList()));
        }

        if (AutoFillCommand::month->contains(m_value.asString())) {
            m_type = MONTH;
            return;
        }

        if (AutoFillCommand::shortMonth->contains(m_value.asString())) {
            m_type = SHORTMONTH;
            return;
        }

        if (AutoFillCommand::day->contains(m_value.asString())) {
            m_type = DAY;
            return;
        }

        if (AutoFillCommand::shortDay->contains(m_value.asString())) {
            m_type = SHORTDAY;
            return;
        }

        if (AutoFillCommand::other->contains(m_value.asString())) {
            m_type = OTHER;
            int index = AutoFillCommand::other->indexOf(m_value.asString());
            int otherBegin = AutoFillCommand::other->lastIndexOf("\\", index); // backward
            int otherEnd = AutoFillCommand::other->indexOf("\\", index); // forward
            m_otherBegin = (otherBegin != -1) ? otherBegin : 0;
            m_otherEnd = (otherEnd != -1) ? otherEnd : AutoFillCommand::other->count();
            return;
        }
    }
}

Value AutoFillSequenceItem::delta(AutoFillSequenceItem *seq, bool *ok) const
{
    if (seq->type() != m_type) {
        *ok = false;
        return Value();
    }

    *ok = true;

    switch (m_type) {
    case VALUE:
    case FORMULA: {
        switch (m_value.type()) {
        case Value::Boolean: {
            // delta indicates a flipping of the boolean
            if (seq->value().type() != Value::Boolean)
                *ok = false;
            return Value(seq->value().asBoolean() != m_value.asBoolean());
        }
        case Value::Integer: {
            if (seq->value().type() == Value::Empty)
                *ok = false;
            Value value(seq->value().asInteger() - m_value.asInteger());
            value.setFormat(m_value.format()); // may be a date format
            return value;
        }
        case Value::Float: {
            if (seq->value().type() == Value::Empty)
                *ok = false;
            Value value(seq->value().asFloat() - m_value.asFloat());
            value.setFormat(m_value.format()); // may be a time format
            return value;
        }
        case Value::Complex: {
            if (seq->value().type() == Value::Empty)
                *ok = false;
            return Value(seq->value().asComplex() - m_value.asComplex());
        }
        case Value::Empty:
        case Value::String:
        case Value::Array:
        case Value::CellRange:
        case Value::Error: {
            *ok = (m_value == seq->value());
            return Value();
        }
        }
    }
    case MONTH: {
        const int i = AutoFillCommand::month->indexOf(m_value.asString());
        const int j = AutoFillCommand::month->indexOf(seq->value().asString());
        return Value(j - i);
    }
    case SHORTMONTH: {
        const int i = AutoFillCommand::shortMonth->indexOf(m_value.asString());
        const int j = AutoFillCommand::shortMonth->indexOf(seq->value().asString());
        return Value(j - i);
    }
    case DAY: {
        const int i = AutoFillCommand::day->indexOf(m_value.asString());
        const int j = AutoFillCommand::day->indexOf(seq->value().asString());
        return Value(j - i);
    }
    case SHORTDAY: {
        const int i = AutoFillCommand::shortDay->indexOf(m_value.asString());
        const int j = AutoFillCommand::shortDay->indexOf(seq->value().asString());
        return Value(j - i);
    }
    case OTHER: {
        *ok = (m_otherEnd != seq->otherEnd() || m_otherBegin != seq->otherBegin());
        const int i = AutoFillCommand::other->indexOf(m_value.asString());
        const int j = AutoFillCommand::other->indexOf(seq->value().asString());
        int k = j;
        if (j < i)
            k += (m_otherEnd - m_otherBegin - 1);
        /*        if (j + 1 == i)
                    return -1.0;
                else*/
        return Value(k - i);
    }
    default:
        *ok = false;
    }
    return Value();
}

Value AutoFillSequenceItem::nextValue(int _no, Value _delta) const
{
    switch (m_type) {
    case VALUE:
    case FORMULA: {
        if (m_value.isBoolean()) {
            if (!_delta.asBoolean() || _delta.isEmpty()) // no change?
                return m_value;
            return Value(_no % 2 ? !m_value.asBoolean() : m_value.asBoolean());
        } else if (m_value.isInteger()) {
            Value value(m_value.asInteger() + _no * _delta.asInteger());
            value.setFormat(_delta.format());
            return value;
        } else if (m_value.isFloat()) {
            Value value(m_value.asFloat() + (long double)_no * _delta.asFloat());
            value.setFormat(_delta.format());
            return value;
        } else if (m_value.isComplex()) {
            Value value(m_value.asComplex() + (long double)_no * _delta.asComplex());
            value.setFormat(_delta.format());
            return value;
        } else // string or empty
            return m_value;
    }
    case MONTH: {
        int i = AutoFillCommand::month->indexOf(m_value.asString());
        int j = i + _no * _delta.asInteger();
        while (j < 0)
            j += AutoFillCommand::month->count();
        int k = j % AutoFillCommand::month->count();
        return Value(AutoFillCommand::month->at(k));
    }
    case SHORTMONTH: {
        int i = AutoFillCommand::shortMonth->indexOf(m_value.asString());
        int j = i + _no * _delta.asInteger();
        while (j < 0)
            j += AutoFillCommand::shortMonth->count();
        int k = j % AutoFillCommand::shortMonth->count();
        return Value(AutoFillCommand::shortMonth->at(k));
    }
    case DAY: {
        int i = AutoFillCommand::day->indexOf(m_value.asString());
        int j = i + _no * _delta.asInteger();
        while (j < 0)
            j += AutoFillCommand::day->count();
        int k = j % AutoFillCommand::day->count();
        return Value(AutoFillCommand::day->at(k));
    }
    case SHORTDAY: {
        int i = AutoFillCommand::shortDay->indexOf(m_value.asString());
        int j = i + _no * _delta.asInteger();
        while (j < 0)
            j += AutoFillCommand::shortDay->count();
        int k = j % AutoFillCommand::shortDay->count();
        return Value(AutoFillCommand::shortDay->at(k));
    }
    case OTHER: {
        int i = AutoFillCommand::other->indexOf(m_value.asString()) - (m_otherBegin + 1);
        int j = i + _no * _delta.asInteger();
        int k = j % (m_otherEnd - m_otherBegin - 1);
        return Value(AutoFillCommand::other->at((k + m_otherBegin + 1)));
    }
    default:
        break;
    }
    return Value();
}

Value AutoFillSequenceItem::prevValue(int _no, Value _delta) const
{
    switch (m_type) {
    case VALUE:
    case FORMULA: {
        if (m_value.isBoolean()) {
            if (!_delta.asBoolean() || _delta.isEmpty()) // no change?
                return m_value;
            return Value(_no % 2 ? !m_value.asBoolean() : m_value.asBoolean());
        } else if (m_value.isInteger()) {
            Value value(m_value.asInteger() - _no * _delta.asInteger());
            value.setFormat(_delta.format());
            return value;
        } else if (m_value.isFloat()) {
            Value value(m_value.asFloat() - (long double)_no * _delta.asFloat());
            value.setFormat(_delta.format());
            return value;
        } else if (m_value.isComplex()) {
            Value value(m_value.asComplex() - (long double)_no * _delta.asComplex());
            value.setFormat(_delta.format());
            return value;
        } else // string or empty
            return m_value;
    }
    case MONTH: {
        int i = AutoFillCommand::month->indexOf(m_value.asString());
        int j = i - _no * _delta.asInteger();
        while (j < 0)
            j += AutoFillCommand::month->count();
        int k = j % AutoFillCommand::month->count();
        return Value(AutoFillCommand::month->at(k));
    }
    case SHORTMONTH: {
        int i = AutoFillCommand::shortMonth->indexOf(m_value.asString());
        int j = i - _no * _delta.asInteger();
        while (j < 0)
            j += AutoFillCommand::shortMonth->count();
        int k = j % AutoFillCommand::shortMonth->count();
        return Value(AutoFillCommand::shortMonth->at(k));
    }
    case DAY: {
        int i = AutoFillCommand::day->indexOf(m_value.asString());
        int j = i - _no * _delta.asInteger();
        while (j < 0)
            j += AutoFillCommand::day->count();
        int k = j % AutoFillCommand::day->count();
        return Value(AutoFillCommand::day->at(k));
    }
    case SHORTDAY: {
        int i = AutoFillCommand::shortDay->indexOf(m_value.asString());
        int j = i - _no * _delta.asInteger();
        while (j < 0)
            j += AutoFillCommand::shortDay->count();
        int k = j % AutoFillCommand::shortDay->count();
        return Value(AutoFillCommand::shortDay->at(k));
    }
    case OTHER: {
        int i = AutoFillCommand::other->indexOf(m_value.asString()) - (m_otherBegin + 1);
        int j = i - _no * _delta.asInteger();
        while (j < 0)
            j += (m_otherEnd - m_otherBegin - 1);
        int k = j % (m_otherEnd - m_otherBegin - 1);
        return Value(AutoFillCommand::other->at((k + m_otherBegin + 1)));
    }
    default:
        break;
    }
    return Value();
}


/**********************************************************************************
 *
 * AutoFillSequence
 *
 **********************************************************************************/

namespace KSpread
{
/**
 * A sequence of cell contents for auto-filling.
 */
class AutoFillSequence : public QList<AutoFillSequenceItem*>
{
public:
    AutoFillSequence();
    AutoFillSequence(const QList<AutoFillSequenceItem*>&);
    ~AutoFillSequence();

    QList<Value> createDeltaSequence(int intervalLength) const;
};
}

AutoFillSequence::AutoFillSequence()
{
}

AutoFillSequence::AutoFillSequence(const QList<AutoFillSequenceItem*>& list)
        : QList<AutoFillSequenceItem*>(list)
{
}

AutoFillSequence::~AutoFillSequence()
{
}

QList<Value> AutoFillSequence::createDeltaSequence(int intervalLength) const
{
    bool ok = true;
    QList<Value> deltaSequence;

    // Guess the delta by looking at cells 0...2*intervalLength-1
    //
    // Since the interval may be of length 'intervalLength' we calculate the delta
    // between cells 0 and intervalLength, 1 and intervalLength+1, ...., intervalLength-1 and 2*intervalLength-1.
    for (int t = 0; t < intervalLength /*&& t + intervalLength < count()*/; ++t) {
        deltaSequence.append(value(t)->delta(value((t + intervalLength) % count()), &ok));
        if (!ok)
            return QList<Value>();
    }

    // fill to the interval length
    while (deltaSequence.count() < intervalLength)
        deltaSequence.append(Value());

    return deltaSequence;
}


/**********************************************************************************
 *
 * File static helper functions
 *
 **********************************************************************************/

static QList<Value> findInterval(const AutoFillSequence& _seqList)
{
    // What is the interval (block)? If your sheet looks like this:
    // 1 3 5 7 9
    // then the interval has the length 1 and the delta list is [2].
    // 2 200 3 300 4 400
    // Here the interval has length 2 and the delta list is [1,100]

    QList<Value> deltaSequence;

    kDebug() << "Sequence length:" << _seqList.count();

    // How big is the interval. It is in the range from [1...n].
    //
    // We try to find the shortest interval.
    int intervalLength = 1;
    for (intervalLength = 1; intervalLength < _seqList.count(); ++intervalLength) {
        kDebug() << "Checking interval of length:" << intervalLength;

        // Create the delta list.
        deltaSequence = _seqList.createDeltaSequence(intervalLength);

        QString str("Deltas: [ ");
        foreach(Value v, deltaSequence) {
            if (v.isBoolean())
                str += v.asBoolean() ? "change " : "nochange ";
            else if (v.isInteger())
                str += QString::number(v.asInteger()) + ' ';
            else if (v.isFloat())
                str += QString::number((double) v.asFloat()) + ' ';
            else
                str += v.asString() + ' ';
        }
        str += ']';
        kDebug() << str;

        // Verify the delta by looking at cells intervalLength.._seqList.count().
        // We only looked at the cells 0..2*intervalLength-1.
        // Now test whether the cells from "(i-1) * intervalLength + s" share the same delta
        // with the cell "i * intervalLength + s" for all test=1..._seqList.count()/intervalLength
        // and for all s=0...intervalLength-1.
        for (int i = 1; (i + 1) * intervalLength < _seqList.count(); ++i) {
            AutoFillSequence tail = _seqList.mid(i * intervalLength);
//             kDebug() <<"Verifying for sequence after" << i * intervalLength <<", length:" << tail.count();
            QList<Value> otherDeltaSequence = tail.createDeltaSequence(intervalLength);
            if (deltaSequence != otherDeltaSequence) {
                kDebug() << "Interval does not match.";
                deltaSequence.clear();
                break;
            }
        }

        // Did we find a valid interval?
        if (!deltaSequence.isEmpty())
            break;
    }

    // if the full interval has to be taken fill the delta sequence with zeros
    if (intervalLength == _seqList.count()) {
        while (intervalLength--)
            deltaSequence.append(Value());

        QString str("Deltas: [ ");
        foreach(Value v, deltaSequence) {
            if (v.isBoolean())
                str += v.asBoolean() ? "change " : "nochange ";
            else if (v.isInteger())
                str += QString::number(v.asInteger()) + ' ';
            else if (v.isFloat())
                str += QString::number((double) v.asFloat()) + ' ';
            else
                str += v.asString() + ' ';
        }
        str += ']';
        kDebug() << str;
    }

    return deltaSequence;
}

static void fillSequence(const QList<Cell>& _srcList,
                         const QList<Cell>& _destList,
                         const AutoFillSequence& _seqList,
                         const QList<Value>& deltaSequence,
                         bool down)
{
    const int intervalLength = deltaSequence.count();
    // starting position depends on the sequence and interval length
    int s = _srcList.count() % intervalLength;
    // Amount of intervals (blocks)
    int block = _srcList.count() / intervalLength;
    kDebug() << "Valid interval, number of intervals:" << block;

    // Start iterating with the first cell
    Cell cell;
    int destIndex = 0;
    if (down)
        cell = _destList.first();
    else {
        cell = _destList.last();
        destIndex = _destList.count() - 1;
        block -= (_srcList.count() - 1);
    }

    // Fill destination cells
    //
    while (!cell.isNull()) {
        // End of block? -> start again from beginning
        if (down) {
            if (s == intervalLength) {
                ++block;
                s = 0;
            }
        } else {
            if (s == -1) {
                s = intervalLength - 1;
                ++block;
            }
        }

        kDebug() << "Cell:" << cell.name() << ", position:" << s << ", block:" << block;

        // Calculate the new value of 'cell' by adding 'block' times the delta to the
        // value of cell 's'.
        //
        Value value;
        if (down)
            value = _seqList.value(s)->nextValue(block, deltaSequence.value(s));
        else
            value = _seqList.value(s)->prevValue(block, deltaSequence.value(s));

        // insert the new value
        //
        if (_seqList.value(s)->type() == AutoFillSequenceItem::FORMULA) {
            // Special handling for formulas
            cell.parseUserInput(cell.decodeFormula(_seqList.value(s)->value().asString()));
        } else if (value.format() == Value::fmt_Time) {
            const Value timeValue = cell.sheet()->map()->converter()->asTime(value);
            cell.setValue(timeValue);
            cell.setUserInput(cell.sheet()->map()->converter()->asString(timeValue).asString());
        } else if (value.format() == Value::fmt_Date) {
            const Value dateValue = cell.sheet()->map()->converter()->asDate(value);
            cell.setValue(dateValue);
            cell.setUserInput(cell.sheet()->map()->converter()->asString(dateValue).asString());
        } else if (value.type() == Value::Boolean ||
                   value.type() == Value::Complex ||
                   value.type() == Value::Float ||
                   value.type() == Value::Integer) {
            cell.setValue(value);
            cell.setUserInput(cell.sheet()->map()->converter()->asString(value).asString());
        } else { // if (value.type() == Value::String)
            QRegExp number("(\\d+)");
            int pos = number.indexIn(value.asString());
            if (pos != -1) {
                const int num = number.cap(1).toInt() + 1;
                cell.parseUserInput(value.asString().replace(number, QString::number(num)));
            } else if (!_srcList.at(s).link().isEmpty()) {
                cell.parseUserInput(value.asString());
                cell.setLink(_srcList.at(s).link());
            } else {
                cell.setValue(value);
                cell.setUserInput(value.asString());
            }
        }

        // copy the style of the source cell
        //
        cell.copyFormat(_srcList.at(s));

        // next/previous cell
        if (down) {
            cell = _destList.value(++destIndex);
            ++s;
        } else {
            cell = _destList.value(--destIndex);
            --s;
        }
    }
}


/**********************************************************************************
 *
 * AutoFillCommand
 *
 **********************************************************************************/

AutoFillCommand::AutoFillCommand()
{
    setText(i18n("Autofill"));
}

AutoFillCommand::~AutoFillCommand()
{
}

void AutoFillCommand::setSourceRange(const QRect& range)
{
    m_sourceRange = range;
}

void AutoFillCommand::setTargetRange(const QRect& range)
{
    m_targetRange = range;
}

bool AutoFillCommand::mainProcessing()
{
    if (m_sourceRange.contains(m_targetRange))
        return false;

    if (m_reverse) {
        // reverse - use the stored value
        AbstractDataManipulator::mainProcessing();
        return true;
    }

    // Fill from left to right
    if (m_sourceRange.left() == m_targetRange.left() && m_sourceRange.right() < m_targetRange.right()) {
        for (int y = m_sourceRange.top(); y <= m_sourceRange.bottom(); y++) {
            int x;
            QList<Cell> destList;
            for (x = m_sourceRange.right() + 1; x <= m_targetRange.right(); x++)
                destList.append(Cell(m_sheet, x, y));
            QList<Cell> srcList;
            for (x = m_sourceRange.left(); x <= m_sourceRange.right(); x++)
                srcList.append(Cell(m_sheet, x, y));
            AutoFillSequence seqList;
            for (x = m_sourceRange.left(); x <= m_sourceRange.right(); x++)
                seqList.append(new AutoFillSequenceItem(Cell(m_sheet, x, y)));
            fillSequence(srcList, destList, seqList);
            qDeleteAll(seqList);
        }
    }

    // Fill from top to bottom
    if (m_sourceRange.top() == m_targetRange.top() && m_sourceRange.bottom() < m_targetRange.bottom()) {
        for (int x = m_sourceRange.left(); x <= m_targetRange.right(); x++) {
            int y;
            QList<Cell> destList;
            for (y = m_sourceRange.bottom() + 1; y <= m_targetRange.bottom(); y++)
                destList.append(Cell(m_sheet, x, y));
            QList<Cell> srcList;
            for (y = m_sourceRange.top(); y <= m_sourceRange.bottom(); y++)
                srcList.append(Cell(m_sheet, x, y));
            AutoFillSequence seqList;
            for (y = m_sourceRange.top(); y <= m_sourceRange.bottom(); y++)
                seqList.append(new AutoFillSequenceItem(Cell(m_sheet, x, y)));
            fillSequence(srcList, destList, seqList);
            qDeleteAll(seqList);
        }
    }

    // Fill from right to left
    if (m_sourceRange.left() == m_targetRange.right() && m_sourceRange.right() >= m_targetRange.right()) {
        for (int y = m_targetRange.top(); y <= m_targetRange.bottom(); y++) {
            int x;
            QList<Cell> destList;
            for (x = m_targetRange.left(); x < m_sourceRange.left(); x++)
                destList.append(Cell(m_sheet, x, y));
            QList<Cell> srcList;
            for (x = m_sourceRange.left(); x <= m_sourceRange.right(); x++)
                srcList.append(Cell(m_sheet, x, y));
            AutoFillSequence seqList;
            for (x = m_sourceRange.left(); x <= m_sourceRange.right(); x++)
                seqList.append(new AutoFillSequenceItem(Cell(m_sheet, x, y)));
            fillSequence(srcList, destList, seqList, false);
            qDeleteAll(seqList);
        }
    }

    // Fill from bottom to top
    if (m_sourceRange.top() == m_targetRange.bottom() && m_sourceRange.bottom() >= m_targetRange.bottom()) {
        const int startVal = qMin(m_targetRange.left(), m_sourceRange.left());
        const int endVal = qMax(m_sourceRange.right(), m_targetRange.right());
        for (int x = startVal; x <= endVal; x++) {
            int y;
            QList<Cell> destList;
            for (y = m_targetRange.top(); y < m_sourceRange.top(); y++)
                destList.append(Cell(m_sheet, x, y));
            QList<Cell> srcList;
            for (y = m_sourceRange.top(); y <= m_sourceRange.bottom(); ++y)
                srcList.append(Cell(m_sheet, x, y));
            AutoFillSequence seqList;
            for (y = m_sourceRange.top(); y <= m_sourceRange.bottom(); y++)
                seqList.append(new AutoFillSequenceItem(Cell(m_sheet, x, y)));
            fillSequence(srcList, destList, seqList, false);
            qDeleteAll(seqList);
        }
    }
    return true;
}

void AutoFillCommand::fillSequence(const QList<Cell>& _srcList,
                                   const QList<Cell>& _destList,
                                   const AutoFillSequence& _seqList,
                                   bool down)
{
    if (_srcList.isEmpty() || _destList.isEmpty())
        return;

    // find an interval to use to fill the sequence
    QList<Value> deltaSequence;

    //If we only have a single cell, the interval will depend upon the data type.
    //- For numeric values, set the interval to 0 as we don't know what might be useful as a sequence
    //- For time values, set the interval to one hour, as this will probably be the most useful setting
    //- For date values, set the interval to one day, as this will probably be the most useful setting
    //
    //Note that the above options were chosen for consistency with Excel.  Gnumeric (1.59) sets
    //the interval to 0 for all types, OpenOffice.org (2.00) uses increments of 1.00, 1 hour and 1 day
    //respectively
    if (_srcList.count() == 1) {
        const Cell cell = _srcList.value(0);
        if (cell.isTime() || cell.value().format() == Value::fmt_DateTime) {
            // TODO Stefan: delta depending on minimum unit of format
            deltaSequence.append(Value(QTime(1, 0), m_sheet->map()->calculationSettings()));
        } else if (cell.isDate()) {
            // TODO Stefan: delta depending on minimum unit of format
            Value value(1);
            value.setFormat(Value::fmt_Date);
            deltaSequence.append(value);
        } else
            deltaSequence.append(Value());
    } else
        deltaSequence = findInterval(_seqList);

    // fill the sequence
    ::fillSequence(_srcList, _destList, _seqList, deltaSequence, down);
}
