/*
  SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importkmailautocorrection.h"

#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

ImportKMailAutocorrection::ImportKMailAutocorrection() = default;

ImportKMailAutocorrection::~ImportKMailAutocorrection() = default;

bool ImportKMailAutocorrection::import(const QString &fileName, QString &errorMessage, LoadAttribute loadAttribute)
{
    QFile xmlFile(fileName);
    if (!xmlFile.open(QIODevice::ReadOnly)) {
        qDebug() << Q_FUNC_INFO << "failed, not open:" << fileName;
        return false;
    }
    QXmlStreamReader xml(&xmlFile);
    mMaxFindStringLenght = 0;
    mMinFindStringLenght = 0;
    if (xml.readNextStartElement()) {
        while (xml.readNextStartElement()) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            const QStringRef xmlName = xml.name();
#else
            const QStringView xmlName = xml.name();
#endif
            // qInfo()<<Q_FUNC_INFO<<xml.name();
            if (xmlName == QLatin1String("UpperCaseExceptions")) {
                if (loadAttribute == All) {
                    while (xml.readNextStartElement()) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                        const QStringRef tagname = xml.name();
#else
                        const QStringView tagname = xml.name();
#endif
                        if (tagname == QLatin1String("word")) {
                            if (xml.attributes().hasAttribute(QStringLiteral("exception"))) {
                                const QString exception = xml.attributes().value(QStringLiteral("exception")).toString();
                                mUpperCaseExceptions += exception;
                                xml.skipCurrentElement();
                            }
                        }
                    }
                } else {
                    xml.skipCurrentElement();
                }
            } else if (xmlName == QLatin1String("TwoUpperLetterExceptions")) {
                if (loadAttribute == All) {
                    while (xml.readNextStartElement()) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                        const QStringRef tagname = xml.name();
#else
                        const QStringView tagname = xml.name();
#endif
                        if (tagname == QLatin1String("word")) {
                            if (xml.attributes().hasAttribute(QStringLiteral("exception"))) {
                                const QString exception = xml.attributes().value(QStringLiteral("exception")).toString();
                                mTwoUpperLetterExceptions += exception;
                                xml.skipCurrentElement();
                            }
                        } else {
                            xml.skipCurrentElement();
                        }
                    }
                } else {
                    xml.skipCurrentElement();
                }
            } else if (xmlName == QLatin1String("DoubleQuote")) {
                if (loadAttribute == All) {
                    if (xml.readNextStartElement()) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                        const QStringRef tagname = xml.name();
#else
                        const QStringView tagname = xml.name();
#endif
                        if (tagname == QLatin1String("doublequote")) {
                            auto s = xml.attributes().value(QStringLiteral("begin")).toString();
                            if (!s.isEmpty()) {
                                mTypographicDoubleQuotes.begin = s.at(0);
                            }
                            s = xml.attributes().value(QStringLiteral("end")).toString();
                            if (!s.isEmpty()) {
                                mTypographicDoubleQuotes.end = s.at(0);
                            }
                            xml.skipCurrentElement();
                        } else {
                            xml.skipCurrentElement();
                        }
                        xml.skipCurrentElement();
                    }
                } else {
                    xml.skipCurrentElement();
                }
            } else if (xmlName == QLatin1String("SimpleQuote")) {
                if (loadAttribute == All) {
                    if (xml.readNextStartElement()) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                        const QStringRef tagname = xml.name();
#else
                        const QStringView tagname = xml.name();
#endif
                        if (tagname == QLatin1String("simplequote")) {
                            const QString simpleQuoteBegin = xml.attributes().value(QStringLiteral("begin")).toString();
                            if (!simpleQuoteBegin.isEmpty()) { // crash when we have old data with bug.
                                mTypographicSingleQuotes.begin = simpleQuoteBegin.at(0);
                            }
                            const QString simpleQuoteEnd = xml.attributes().value(QStringLiteral("end")).toString();
                            if (!simpleQuoteEnd.isEmpty()) { // crash when we have old data with bug.
                                mTypographicSingleQuotes.end = simpleQuoteEnd.at(0);
                            }
                            xml.skipCurrentElement();
                        } else {
                            xml.skipCurrentElement();
                        }
                        xml.skipCurrentElement();
                    }
                } else {
                    xml.skipCurrentElement();
                }
            } else if (xmlName == QLatin1String("SuperScript")) {
                // qInfo()<<Q_FUNC_INFO<<1<<"SuperScriptEntries";
                if (loadAttribute == All || loadAttribute == SuperScript) {
                    while (xml.readNextStartElement()) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                        const QStringRef tagname = xml.name();
#else
                        const QStringView tagname = xml.name();
#endif
                        if (tagname == QLatin1String("superscript")) {
                            const QString find = xml.attributes().value(QStringLiteral("find")).toString();
                            const QString super = xml.attributes().value(QStringLiteral("super")).toString();
                            mSuperScriptEntries.insert(find, super);
                            // qInfo()<<Q_FUNC_INFO<<"add:"<<find<<super<<mSuperScriptEntries;
                            xml.skipCurrentElement();
                        } else {
                            xml.skipCurrentElement();
                        }
                    }
                } else {
                    xml.skipCurrentElement();
                }
            } else if (xmlName == QLatin1String("items")) {
                if (loadAttribute == All) {
                    while (xml.readNextStartElement()) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                        const QStringRef tagname = xml.name();
#else
                        const QStringView tagname = xml.name();
#endif
                        if (tagname == QLatin1String("item")) {
                            const QString find = xml.attributes().value(QStringLiteral("find")).toString();
                            const QString replace = xml.attributes().value(QStringLiteral("replace")).toString();
                            const int findLenght(find.length());
                            mMaxFindStringLenght = qMax(findLenght, mMaxFindStringLenght);
                            mMinFindStringLenght = qMin(findLenght, mMinFindStringLenght);
                            mAutocorrectEntries.insert(find, replace);
                            xml.skipCurrentElement();
                        } else {
                            xml.skipCurrentElement();
                        }
                    }
                } else {
                    xml.skipCurrentElement();
                }
            } else {
                // TODO verify
                xml.skipCurrentElement();
            }
        }
    }
    return true;
}
