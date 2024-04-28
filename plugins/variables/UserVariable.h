/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <mail@dipe.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef USERVARIABLE_H
#define USERVARIABLE_H

#include <KoOdfNumberStyles.h>
#include <KoVariable.h>
#include <QObject>

class KoShapeSavingContext;
class KoVariableManager;

/**
 * This is a KoVariable for user defined variables.
 *
 * This implements the ODF attributes text:user-field-get and
 * text:user-field-input which are fetching variables defined
 * via text:user-field-decls and text:user-field-decl.
 */
class UserVariable : public KoVariable
{
    Q_OBJECT
public:
    UserVariable();

    QWidget *createOptionsWidget() override;

    void readProperties(const KoProperties *props);
    void propertyChanged(Property property, const QVariant &value) override;

    void saveOdf(KoShapeSavingContext &context) override;
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    KoVariableManager *variableManager();

    int property() const;

    const QString &name() const;
    void setName(const QString &name);

    KoOdfNumberStyles::NumericStyleFormat numberstyle() const;
    void setNumberStyle(KoOdfNumberStyles::NumericStyleFormat numberstyle);

private Q_SLOTS:
    void valueChanged();

private:
    void resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd) override;

    KoVariableManager *m_variableManager;
    int m_property;
    QString m_name;
    KoOdfNumberStyles::NumericStyleFormat m_numberstyle;
};

#endif
