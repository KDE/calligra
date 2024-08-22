/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 C. Boemann <cbo@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FormulaPart.h"
#include "FormulaDocument.h"

#include <KoComponentData.h>

#include <calligra-version.h>

#include <KAboutData>

FormulaPart::FormulaPart(QObject *parent)
    : KoPart(KoComponentData(KAboutData(QStringLiteral("koformula"), QStringLiteral("KoFormula"), QStringLiteral(CALLIGRA_VERSION_STRING))), parent)
{
}

FormulaPart::~FormulaPart() = default;

KoView *FormulaPart::createViewInstance(KoDocument *document, QWidget *parent)
{
    Q_UNUSED(document);
    Q_UNUSED(parent);

    return nullptr;
}

KoMainWindow *FormulaPart::createMainWindow()
{
    return new KoMainWindow(FORMULA_MIME_TYPE, componentData());
}
