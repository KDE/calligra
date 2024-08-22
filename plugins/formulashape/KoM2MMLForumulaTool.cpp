/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Cyrille Berger <cberger@cberger.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoM2MMLForumulaTool.h"

#ifdef HAVE_M2MML
#include <m2mml.h>
#endif

#include <string>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include <QWidget>

#include <KoCanvasBase.h>
#include <KoIcon.h>
#include <KoXmlReader.h>

#include "3rdparty/itexToMML/itex2MML.h"
#include "FormulaCommand.h"
#include "FormulaCommandUpdate.h"
#include "FormulaDebug.h"
#include "KoFormulaShape.h"
#include <AnnotationElement.h>

KoM2MMLFormulaTool::KoM2MMLFormulaTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
    , m_lineEdit(nullptr)
    , m_errorLabel(nullptr)
    , m_formulaShape(nullptr)
    , m_comboBox(nullptr)
{
}

void KoM2MMLFormulaTool::activate(KoToolBase::ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    Q_UNUSED(toolActivation);

    foreach (KoShape *shape, shapes) {
        m_formulaShape = dynamic_cast<KoFormulaShape *>(shape);
        if (m_formulaShape)
            break;
    }

    if (m_formulaShape == nullptr) // none found
    {
        Q_EMIT done();
        return;
    }
    FormulaElement *element = m_formulaShape->formulaData()->formulaElement();
    foreach (BasicElement *elt, element->childElements()) {
        if (elt->elementType() == Annotation) {
            AnnotationElement *annot = static_cast<AnnotationElement *>(elt);
            m_text = annot->content();
            m_mode = annot->attribute("mode");
        }
    }

    if (m_lineEdit) {
        m_lineEdit->setText(m_text);
    }
}

void KoM2MMLFormulaTool::mouseMoveEvent(KoPointerEvent *event)
{
    Q_UNUSED(event);
}

void KoM2MMLFormulaTool::mousePressEvent(KoPointerEvent *event)
{
    Q_UNUSED(event);
}

void KoM2MMLFormulaTool::mouseReleaseEvent(KoPointerEvent *event)
{
    Q_UNUSED(event);
}

void KoM2MMLFormulaTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

QWidget *KoM2MMLFormulaTool::createOptionWidget()
{
    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;

    // Combobox to select between latex or matlab
    QLabel *modeLabel = new QLabel(i18n("Mode: "));
    m_comboBox = new QComboBox;

    m_comboBox->addItem(i18n("LaTeX"));
#ifdef HAVE_M2MML
    m_comboBox->addItem(i18n("Matlab"));

    if (m_mode == "Matlab") {
        m_comboBox->setCurrentIndex(1);
    }
#endif

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(modeLabel);
    hlayout->addWidget(m_comboBox);
    layout->addLayout(hlayout);

    // Edit line
    widget->setLayout(layout);
    m_lineEdit = new QLineEdit(widget);
    layout->addWidget(m_lineEdit);

    // Error label
    m_errorLabel = new QLabel(widget);
    layout->addWidget(m_errorLabel);
    m_errorLabel->setText("");

    layout->addSpacerItem(new QSpacerItem(0, 0));

    connect(m_lineEdit, &QLineEdit::editingFinished, this, &KoM2MMLFormulaTool::textEdited);
    connect(m_lineEdit, &QLineEdit::returnPressed, this, &KoM2MMLFormulaTool::textEdited);
    m_lineEdit->setText(m_text);

    return widget;
}

// Not sure why but the toStdString/fromStdString in QString are not accessible
inline std::string QStringtoStdString(const QString &str)
{
    const QByteArray latin1 = str.toLatin1();
    return std::string(latin1.constData(), latin1.length());
}

inline QString QStringfromStdString(const std::string &s)
{
    return QString::fromLatin1(s.data(), int(s.size()));
}

void KoM2MMLFormulaTool::textEdited()
{
    if (!m_formulaShape)
        return;
    if (!m_lineEdit)
        return;

#ifdef HAVE_M2MML
    if (m_comboBox->currentIndex() == 1) {
        std::string source = QStringtoStdString(m_lineEdit->text());
        std::string mathml;
        std::string errmsg;

        if (m2mml(source, mathml, &errmsg)) {
            setMathML(QStringfromStdString(mathml), "Matlab");
        } else {
            m_errorLabel->setText(QStringfromStdString(errmsg));
        }
    } else {
#endif
        std::string source = QStringtoStdString(m_lineEdit->text());
        source = '$' + source + '$';
        char *mathml = itex2MML_parse(source.c_str(), source.size());

        if (mathml) {
            setMathML(mathml, "LaTeX");
            itex2MML_free_string(mathml);
            mathml = nullptr;
        } else {
            m_errorLabel->setText(i18n("Parse error."));
        }
#ifdef HAVE_M2MML
    }
#endif
}

void KoM2MMLFormulaTool::setMathML(const QString &mathml, const QString &mode)
{
    KoXmlDocument tmpDocument;
    tmpDocument.setContent(QString(mathml), false, nullptr, nullptr, nullptr);
    FormulaElement *formulaElement = new FormulaElement(); // create a new root element
    formulaElement->readMathML(tmpDocument.documentElement()); // and load the new formula

    AnnotationElement *annot = new AnnotationElement;
    annot->setContent(m_lineEdit->text());
    annot->setAttribute("mode", mode);
    formulaElement->insertChild(0, annot);

    debugFormula << mathml;

    canvas()->addCommand(new FormulaCommandUpdate(m_formulaShape, new FormulaCommandLoad(m_formulaShape->formulaData(), formulaElement)));
    m_errorLabel->setText("");
}

KoM2MMLFormulaToolFactory::KoM2MMLFormulaToolFactory()
    : KoToolFactoryBase("KoM2MMLFormulaToolFactoryId")
{
#ifdef HAVE_M2MML
    setToolTip(i18n("Edit formula with LaTeX/Matlab syntax"));
#else
    setToolTip(i18n("Edit formula with LaTeX syntax"));
#endif
    setToolType(dynamicToolType());
    setIconName(koIconName("edittext"));
    setPriority(1);
    setActivationShapeId(KoFormulaShapeId);
}

KoM2MMLFormulaToolFactory::~KoM2MMLFormulaToolFactory() = default;

KoToolBase *KoM2MMLFormulaToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KoM2MMLFormulaTool(canvas);
}
