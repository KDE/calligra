/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Robert JACOLIN <rjacolin@ifrance.com>
   SPDX-FileCopyrightText: 2006 Fredrik Edemar <f_edemar@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __LATEXEXPORTADAPTOR_H__
#define __LATEXEXPORTADAPTOR_H__

#include <QDBusAbstractAdaptor>
#include <QObject>

class LatexExportDialog;

class LatexExportAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.calligra.filter.kspread.latex")

public:
    explicit LatexExportAdaptor(LatexExportDialog *dialog);

    ~LatexExportAdaptor() override;

public Q_SLOTS: // METHODS
    Q_SCRIPTABLE void useDefaultConfig();

private:
    LatexExportDialog *_dialog;
};

#endif /* __LATEXEXPORTADAPTOR_H__ */
