// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.0-only

#include <engine/Formula.h>
#include <engine/FunctionModuleRegistry.h>
#include <engine/Value.h>

#include <QTest>

#include <float.h> // DBL_EPSILON
#include <math.h>

using namespace Calligra::Sheets;


namespace QTest
{
template<>
char *toString(const Value& value)
{
    QString message;
    QTextStream ts(&message, QIODevice::WriteOnly);
    ts << value;
    return qstrdup(message.toLatin1());
}
}


