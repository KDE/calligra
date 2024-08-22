/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Enno Bartels <ebartels@nwn.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef APPLIXSPREADIMPORT_H
#define APPLIXSPREADIMPORT_H

#include <QTextStream>

#include <KoFilter.h>

struct t_mycolor;
struct t_rc;

class APPLIXSPREADImport : public KoFilter
{
    Q_OBJECT

public:
    APPLIXSPREADImport(QObject *parent, const QVariantList &);
    ~APPLIXSPREADImport() override = default;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

protected:
    QString nextLine(QTextStream &);
    QChar specCharfind(QChar, QChar);
    void writePen(QString &, int, int, const QString &);
    QString writeColor(t_mycolor *);
    void readTypefaceTable(QTextStream &, QStringList &);
    void readColormap(QTextStream &, QList<t_mycolor *> &);
    void readView(QTextStream &, const QString &, t_rc &);
    void filterSHFGBG(const QString &, int *, int *, int *);
    void transPenFormat(const QString &, int *, int *);
    int readHeader(QTextStream &);
    int translateColumnNumber(const QString &);
    QString convertFormula(const QString &input) const;

private:
    int m_stepsize;
    int m_instep;
    int m_progress;
    QString m_nextPendingLine;
};
#endif // APPLIXSPREADIMPORT_H
