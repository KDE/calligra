// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "UserPropsDestination.h"

#include "rtfdebug.h"
#include "rtfreader.h"

namespace RtfReader
{
UserPropsDestination::UserPropsDestination(AbstractRtfOutput *output, const QString &name)
    : Destination(output, name)
    , m_nextPlainTextIsPropertyName(true)
    , m_propertyType(QMetaType::QString)
{
}

UserPropsDestination::~UserPropsDestination() = default;

void UserPropsDestination::handleControlWord(const QByteArray &controlWord, bool hasValue, const int value)
{
    if (controlWord == "propname") {
        m_nextPlainTextIsPropertyName = true;
    } else if ((controlWord == "proptype") && hasValue) {
        if (value == 30) {
            m_propertyType = QMetaType::QString;
        } else if (value == 3) {
            m_propertyType = QMetaType::Int;
        } else if (value == 5) {
            m_propertyType = QMetaType::Double;
        } else if (value == 64) {
            m_propertyType = QMetaType::QDate;
        } else if (value == 11) {
            m_propertyType = QMetaType::Bool;
        } else {
            qCDebug(lcRtf) << "unhandled value type in UserPropsDestination:" << value;
        }
    } else if (controlWord == "staticval") {
        m_nextPlainTextIsPropertyName = false;
    } else {
        qCDebug(lcRtf) << "unexpected control word in UserPropsDestination:" << controlWord;
    }
}

void UserPropsDestination::handlePlainText(const QByteArray &plainText)
{
    if (m_nextPlainTextIsPropertyName) {
        m_propertyName = QString::fromUtf8(plainText);
    } else {
        QVariant value;
        if (m_propertyType == QMetaType::QString) {
            value = QVariant(plainText);
        } else {
            // TODO: Really need some examples of this stuff - int, float, date and boolean
            qCDebug(lcRtf) << "unhandled value type in UserPropsDestination:" << m_propertyType;
        }
        m_output->addUserProp(m_propertyName, value);
    }
}
}
