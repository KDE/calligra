// SPDX-FileCopyrightText: 2008, 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "rtfreader.h"
#include "AbstractRtfOutput.h"
#include "controlword.h"
#include "rtfdebug.h"

#include "AuthorPcdataDestination.h"
#include "CategoryPcdataDestination.h"
#include "ColorTableDestination.h"
#include "CommentPcdataDestination.h"
#include "CompanyPcdataDestination.h"
#include "DocumentCommentPcdataDestination.h"
#include "DocumentDestination.h"
#include "FontTableDestination.h"
#include "GeneratorPcdataDestination.h"
#include "HLinkBasePcdataDestination.h"
#include "IgnoredDestination.h"
#include "InfoCreatedTimeDestination.h"
#include "InfoDestination.h"
#include "InfoPrintedTimeDestination.h"
#include "InfoRevisedTimeDestination.h"
#include "KeywordsPcdataDestination.h"
#include "ManagerPcdataDestination.h"
#include "OperatorPcdataDestination.h"
#include "PictDestination.h"
#include "StyleSheetDestination.h"
#include "SubjectPcdataDestination.h"
#include "TitlePcdataDestination.h"
#include "UserPropsDestination.h"

#include <QStack>
#include <QTextCursor>
#include <QUrl>

namespace RtfReader
{
Reader::Reader(QObject *parent)
    : QObject(parent)
    , m_inputDevice(nullptr)
{
}

Reader::~Reader()
{
    // Clean up any remaining objects
    qDeleteAll(m_destinationStack);
}

bool Reader::open(const QString &filename)
{
    m_inputDevice = new QFile(filename, this);

    bool result = m_inputDevice->open(QIODevice::ReadOnly);

    return result;
}

void Reader::close()
{
    if (!m_inputDevice)
        return;
    m_inputDevice->close();
    delete m_inputDevice;
    m_inputDevice = nullptr;
}

QString Reader::fileName() const
{
    if (m_inputDevice && m_inputDevice->exists()) {
        return m_inputDevice->fileName();
    } else {
        return QString();
    }
}

bool Reader::parseTo(AbstractRtfOutput *output)
{
    if ((!m_inputDevice) || (!m_inputDevice->isOpen())) {
        return false;
    }

    m_output = output;

    parseFile();

    return true;
}

void Reader::parseFile()
{
    Tokenizer tokenizer(m_inputDevice);

    if (parseFileHeader(tokenizer)) {
        parseDocument(tokenizer);
    }
}

bool Reader::parseFileHeader(Tokenizer &tokenizer)
{
    bool result = true;

    Token token = tokenizer.fetchToken();
    if (token.type != OpenGroup) {
        qCDebug(lcRtf) << "Not an RTF file";
        result = false;
    }

    token = tokenizer.fetchToken();
    if (token.type != Control) {
        qCDebug(lcRtf) << "Not an RTF file - wrong document type";
        result = false;
    }

    if (!headerFormatIsKnown(QString::fromUtf8(token.name), token.parameter.toInt())) {
        qCDebug(lcRtf) << "Not a valid RTF file - unknown header";
        result = false;
    }

    return result;
}

bool Reader::headerFormatIsKnown(const QString &tokenName, int tokenValue)
{
    if (tokenName != QLatin1String("rtf")) {
        qCDebug(lcRtf) << "unknown / unexpected header token name:" << tokenName;
        return false;
    }

    if (tokenValue != 1) {
        qCDebug(lcRtf) << "unknown / unexpected header token value:" << tokenValue;
        return false;
    }

    return true;
}

Destination *Reader::makeDestination(const QString &destinationName)
{
    if (destinationName == QLatin1String("colortbl")) {
        return new ColorTableDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("creatim")) {
        return new InfoCreatedTimeDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("printim")) {
        return new InfoPrintedTimeDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("revtim")) {
        return new InfoRevisedTimeDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("author")) {
        return new AuthorPcdataDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("company")) {
        return new CompanyPcdataDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("operator")) {
        return new OperatorPcdataDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("comment")) {
        return new CommentPcdataDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("doccomm")) {
        return new DocumentCommentPcdataDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("title")) {
        return new TitlePcdataDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("subject")) {
        return new SubjectPcdataDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("manager")) {
        return new ManagerPcdataDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("category")) {
        return new CategoryPcdataDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("keywords")) {
        return new KeywordsPcdataDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("hlinkbase")) {
        return new HLinkBasePcdataDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("generator")) {
        return new GeneratorPcdataDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("pict")) {
        return new PictDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("fonttbl")) {
        return new FontTableDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("stylesheet")) {
        return new StyleSheetDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("rtf")) {
        return new DocumentDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("info")) {
        return new InfoDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("userprops")) {
        return new UserPropsDestination(m_output, destinationName);
    } else if (destinationName == QLatin1String("ignorable")) {
        return new IgnoredDestination(m_output, destinationName);
    }
    qCDebug(lcRtf) << "creating plain old Destination for" << destinationName;
    return new Destination(m_output, destinationName);
}

void Reader::changeDestination(const QString &destinationName)
{
    if (m_destinationStack.top()->name() == QLatin1StringView("ignorable")) {
        // we don't change destinations inside ignored groups
        return;
    }
    // qCDebug(lcRtf) << m_debugIndent << "about to change destination to: " << destinationName;

    Destination *dest = makeDestination(destinationName);

    m_destinationStack.push(dest);
    m_stateStack.top().didChangeDestination = true;
    QStringList destStackElementNames;
    for (int i = 0; i < m_destinationStack.size(); ++i) {
        destStackElementNames << m_destinationStack.at(i)->name();
    }
    qCDebug(lcRtf) << m_debugIndent << "destinationStack after changeDestination (" << destStackElementNames << ")";
}

void Reader::parseDocument(Tokenizer &tokenizer)
{
    class RtfGroupState state;

    // Push an end-of-file marker onto the stack
    state.endOfFile = true;
    m_stateStack.push(state);

    // Set up the outer part of the destination stack
    Destination *dest = makeDestination(QStringLiteral("rtf"));
    m_destinationStack.push(dest);
    m_stateStack.top().didChangeDestination = true;

    m_debugIndent = QLatin1Char('\t');
    // Parse RTF document
    bool atEndOfFile = false;
    bool nextSymbolMightBeDestination = false;
    bool nextSymbolIsIgnorable = false;

    RtfReader::ControlWord controlWord(QStringLiteral(""));

    while (!atEndOfFile) {
        Token token = tokenizer.fetchToken();
        // token.dump();
        switch (token.type) {
        case Invalid:
            atEndOfFile = true;
            break;
        case OpenGroup: {
            // Store the current state on the stack
            RtfGroupState state;
            m_stateStack.push(state);
            nextSymbolMightBeDestination = true;
            m_output->startGroup();
            // qCDebug(lcRtf) << m_debugIndent << "opengroup";
            m_debugIndent.append(QLatin1Char('\t'));
            break;
        }
        case CloseGroup: {
            QStringList destStackElementNames;
            for (int i = 0; i < m_destinationStack.size(); ++i) {
                destStackElementNames << m_destinationStack.at(i)->name();
            }
            // qCDebug(lcRtf) << m_debugIndent << "closegroup ( destinationStack:" << destStackElementNames << ")";
            m_debugIndent.remove(0, 1);
            state = m_stateStack.pop();
            if (state.endOfFile) {
                atEndOfFile = true;
            } else {
                m_output->endGroup();
            }

            if (state.didChangeDestination) {
                m_destinationStack.top()->aboutToEndDestination();
                delete m_destinationStack.top();
                m_destinationStack.pop();
            }

            destStackElementNames.clear();
            for (int i = 0; i < m_destinationStack.size(); ++i) {
                destStackElementNames << m_destinationStack.at(i)->name();
            }
            // qCDebug(lcRtf) << m_debugIndent << "destinationStack after CloseGroup: (" << destStackElementNames << ")";
            nextSymbolMightBeDestination = true;
            break;
        }
        case Control:
            controlWord = ControlWord(QString::fromUtf8(token.name));
            if (!controlWord.isKnown()) {
                qCDebug(lcRtf) << "*** Unrecognised control word (not in spec 1.9.1): " << token.name;
            }
            // qCDebug(lcRtf) << m_debugIndent << "got controlWord: " << token.name;
            // qCDebug(lcRtf) << m_debugIndent << "isDestination:" << controlWord.isDestination();
            // qCDebug(lcRtf) << m_debugIndent << "isIgnorable:" << nextSymbolIsIgnorable;
            if (nextSymbolMightBeDestination && controlWord.isSupportedDestination()) {
                nextSymbolMightBeDestination = false;
                nextSymbolIsIgnorable = false;
                changeDestination(QString::fromUtf8(token.name));
            } else if (nextSymbolMightBeDestination && nextSymbolIsIgnorable) {
                // This is a control word we don't understand
                nextSymbolMightBeDestination = false;
                nextSymbolIsIgnorable = false;
                qCDebug(lcRtf) << "ignorable destination word:" << token.name;
                changeDestination(QStringLiteral("ignorable"));
            } else {
                nextSymbolMightBeDestination = false;
                if (token.name == "*") {
                    nextSymbolMightBeDestination = true;
                    nextSymbolIsIgnorable = true;
                }
                m_destinationStack.top()->handleControlWord(token.name, token.hasParameter, token.parameter.toInt());
            }
            break;
        case Plain:
            m_destinationStack.top()->handlePlainText(token.name);
            break;
        case Binary:
            qCDebug(lcRtf) << "binary data:" << token.name;
            break;
        default:
            qCDebug(lcRtf) << "Unexpected token Type";
        }
    }
}
}
