/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Cyrille Berger <cberger@cberger.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOM2MMLFORMULATOOL_H_
#define KOM2MMLFORMULATOOL_H_

#include <KoToolBase.h>
#include <KoToolFactoryBase.h>

class QComboBox;
class QLabel;
class KoFormulaShape;
class QLineEdit;

class KoM2MMLFormulaTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KoM2MMLFormulaTool(KoCanvasBase *canvas);

    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;

    void mouseReleaseEvent(KoPointerEvent *event) override;
    void mousePressEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    QWidget *createOptionWidget() override;
public Q_SLOTS:
    void textEdited();

private:
    void setMathML(const QString &mathml, const QString &mode);

private:
    QLineEdit *m_lineEdit;
    QLabel *m_errorLabel;
    KoFormulaShape *m_formulaShape;
    QString m_text;
    QComboBox *m_comboBox;
    QString m_mode;
};

class KoM2MMLFormulaToolFactory : public KoToolFactoryBase
{
public:
    /// The constructor - reimplemented from KoToolFactoryBase
    explicit KoM2MMLFormulaToolFactory();

    /// The destructor - reimplemented from KoToolFactoryBase
    ~KoM2MMLFormulaToolFactory() override;

    /// @return an instance of KoFormulaTool
    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif
