/** This file is part of the KDE project
 *  SPDX-FileCopyrightText: 2023 dag Andersen <dag.andersen@kdemail.net>
 *  SPDX-License-Identifier: LGPL-2.0-only
 */

#ifndef CALLIGRA_SHEETS_TIME_TEST
#define CALLIGRA_SHEETS_TIME_TEST

#include <QObject>

class KoXmlDocument;
class QString;

namespace Calligra
{
namespace Sheets
{

class TestTime : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void positiveDuration();
    void positiveConstructors();
    void casting();
    void operators();
    void qtime();
    void negativeDuration();
    void negativeConstructors();
    void formatting();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TIME_TEST
