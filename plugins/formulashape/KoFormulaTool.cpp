/* This file is part of the KDE project
   Copyright (C) 2011 Cyrille Berger <cberger@cberger.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#include "KoFormulaTool.h"

#ifdef HAVE_M2MML
#include <m2mml.h>
#endif

#include <string>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QSpacerItem>
#include <QWidget>
#include <QWidgetAction>

#include <KoCanvasBase.h>
#include <KoIcon.h>
#include <KoXmlReader.h>

#include "FormulaDebug.h"
#include "FormulaFontFamilyAction.h"
#include "../textshape/FontSizeAction.h"
#include "KoFormulaShape.h"
#include "FormulaCommand.h"
#include "FormulaCommandUpdate.h"
#include "3rdparty/itexToMML/itex2MML.h"

KoFormulaTool::KoFormulaTool(KoCanvasBase* canvas): KoToolBase(canvas), m_textEdit(0), m_errorLabel(0), m_formulaShape(0), m_modeComboBox(0)
{
    m_actionFontFamily = new FormulaFontFamilyAction(this);
    m_actionFontSize = new FontSizeAction(this);
}

void KoFormulaTool::activate(KoToolBase::ToolActivation toolActivation, const QSet< KoShape* >& shapes)
{
    Q_UNUSED(toolActivation);

    foreach (KoShape *shape, shapes) {
        m_formulaShape = dynamic_cast<KoFormulaShape*>( shape );
        if( m_formulaShape )
            break;
    }

    if( m_formulaShape == 0 )  // none found
    {
        emit done();
        return;
    }
    
    if(m_textEdit)
    {
        m_textEdit->setText(m_formulaShape->content());
    }
}

void KoFormulaTool::mouseMoveEvent(KoPointerEvent* event)
{
    Q_UNUSED(event);
}

void KoFormulaTool::mousePressEvent(KoPointerEvent* event)
{
    Q_UNUSED(event);
}

void KoFormulaTool::mouseReleaseEvent(KoPointerEvent* event)
{
    Q_UNUSED(event);
}

void KoFormulaTool::paint(QPainter& painter, const KoViewConverter& converter)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

QWidget* KoFormulaTool::createOptionWidget()
{
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* hlayout = new QHBoxLayout();

    hlayout->addWidget(m_actionFontFamily->requestWidget(widget));

    QWidget* fontSizeComboBox = m_actionFontSize->requestWidget(widget);
    fontSizeComboBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    hlayout->addWidget(fontSizeComboBox);

    // Combobox to select among latex, matlab and mathml
    m_modeComboBox = new QComboBox;
    m_modeComboBox->addItem(i18n("LaTeX"));
    #ifdef HAVE_M2MML
    m_modeComboBox->addItem(i18n("Matlab"));
    if(m_mode == "Matlab")
    {
        m_modeComboBox->setCurrentIndex(1);
    }
    #endif
    m_modeComboBox->addItem(i18n("MathML"));
    m_modeComboBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    hlayout->addWidget(m_modeComboBox);

    layout->addLayout(hlayout);

    // Edit line
    widget->setLayout(layout);
    m_textEdit = new QTextEdit(widget);
    layout->addWidget(m_textEdit);
    
    // Error label
    m_errorLabel = new QLabel(widget);
    layout->addWidget(m_errorLabel);
    m_errorLabel->setText("");
    
    layout->addSpacerItem(new QSpacerItem(0,0));
    
    //connect(m_textEdit, SIGNAL(editingFinished()), SLOT(textEdited()));
    //connect(m_textEdit, SIGNAL(returnPressed()), SLOT(textEdited()));
    m_textEdit->setText(m_text);
    
    return widget;
}

// Not sure why but the toStdString/fromStdString in QString are not accessible
inline std::string QStringtoStdString(const QString& str)
{ const QByteArray latin1 = str.toLatin1(); return std::string(latin1.constData(), latin1.length()); }

inline QString QStringfromStdString(const std::string &s)
{ return QString::fromLatin1(s.data(), int(s.size())); }

void KoFormulaTool::textEdited()
{
    if(!m_formulaShape) return;
    if(!m_textEdit) return;

#ifdef HAVE_M2MML
    if(m_modeComboBox->currentIndex() == 1)
    {
        std::string source = QStringtoStdString(m_textEdit->toPlainText());
        std::string mathml;
        std::string errmsg;

        if(m2mml(source, mathml, &errmsg))
        {
            setMathML(QStringfromStdString(mathml), "Matlab");
        } else {
            m_errorLabel->setText(QStringfromStdString(errmsg));
        }
    } else {
#endif
        std::string source = QStringtoStdString(m_textEdit->toPlainText());
        source = '$' + source + '$';
        char * mathml = itex2MML_parse (source.c_str(), source.size());
        
        if(mathml)
        {
            setMathML(mathml, "LaTeX");
            itex2MML_free_string(mathml);
            mathml = 0;
        } else {
            m_errorLabel->setText(i18n("Parse error."));
        }
#ifdef HAVE_M2MML
    }
#endif
}

void KoFormulaTool::setMathML(const QString& mathml, const QString& mode)
{
    KoXmlDocument tmpDocument;
    tmpDocument.setContent( QString(mathml), false, 0, 0, 0 );

    debugFormula << mathml;
}
