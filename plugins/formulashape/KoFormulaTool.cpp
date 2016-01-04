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
#include <KTextEdit>
#include <QSpacerItem>
#include <QWidget>
#include <QWidgetAction>
#include <QPushButton>

#include <KoCanvasBase.h>
#include <KoIcon.h>
#include <KoXmlReader.h>

#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include "FormulaDebug.h"
#include "FormulaFontFamilyAction.h"
#include "../textshape/FontSizeAction.h"
#include "KoFormulaShape.h"
#include "FormulaCommand.h"
#include "3rdparty/itexToMML/itex2MML.h"
#include "BasicXMLSyntaxHighlighter.h"

enum EditMode { MathML, Latex };

KoFormulaTool::KoFormulaTool(KoCanvasBase* canvas): KoToolBase(canvas), m_textEdit(0), m_syntaxHighlighter(0), m_errorLabel(0), m_formulaShape(0), m_modeComboBox(0)
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
        m_syntaxHighlighter = new BasicXMLSyntaxHighlighter(m_textEdit->document());
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
    m_modeComboBox->insertItem(EditMode::MathML, i18n("MathML"));
    m_modeComboBox->insertItem(EditMode::Latex, i18n("LaTeX"));
    m_modeComboBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    connect(m_modeComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
        m_modeComboBox->setCurrentIndex(index);
        if (index == EditMode::MathML) {
            m_textEdit->setText(m_formulaShape->content());
            m_syntaxHighlighter->setDocument(m_textEdit->document());
        } else if (index == EditMode::Latex) {
            QString mmltex = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                    "calligra/formula/mmltex.xsl");
            xmlChar* texStr = 0;
            int size = 0;
            if (QFileInfo::exists(mmltex)) {
                xsltStylesheetPtr stylesheet = xsltParseStylesheetFile(
                        (const xmlChar *) mmltex.toUtf8().constData()
                );
                xmlDocPtr doc = xmlParseMemory(
                        m_formulaShape->content().toUtf8().constData(),
                        m_formulaShape->content().toUtf8().size()
                );
                xmlDocPtr res = xsltApplyStylesheet(stylesheet, doc, 0);
                xsltSaveResultToString(&texStr, &size, res, stylesheet);
                xsltFreeStylesheet(stylesheet);
                xmlFreeDoc(doc);
                xmlFreeDoc(res);
            }
            QString textEditStr = QString::fromUtf8((const char*)texStr, size);
            textEditStr.chop(3); // remove "\n\]" from end
            m_textEdit->setText(textEditStr.mid(5)); // exclude "\n\[\n\t" from begin
            m_syntaxHighlighter->setDocument(0);
            free(texStr);
        }
    });
    hlayout->addWidget(m_modeComboBox);

    layout->addLayout(hlayout);

    // Edit line
    widget->setLayout(layout);
    m_textEdit = new KTextEdit(widget);
    m_textEdit->setAcceptRichText(false);
    m_textEdit->setCheckSpellingEnabled(false);
    m_textEdit->setPlaceholderText(i18n("Input formula here …"));
    layout->addWidget(m_textEdit);

    // Update button
    m_updateButton = new QPushButton(i18n("Update Formula"), widget);
    connect(m_updateButton, &QAbstractButton::clicked, this, &KoFormulaTool::updateFormula);
    layout->addWidget(m_updateButton);
    
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

void KoFormulaTool::updateFormula() {
    // TODO support setting formula colors
    QString mathML;
    if (m_modeComboBox->currentIndex() == EditMode::Latex) {
        std::string source = QStringtoStdString(m_textEdit->toPlainText());
        source = '$' + source + '$';
        char * mmlStr = itex2MML_parse(source.c_str(), source.size());
        if(mmlStr)
        {
            mathML = QString::fromUtf8(mmlStr);
            itex2MML_free_string(mmlStr);
        } else {
            mathML = "";
            m_errorLabel->setText(i18n("Parse error."));
        }
    } else {
        mathML = m_textEdit->toPlainText();
    }

    canvas()->addCommand(new FormulaCommand(
            m_formulaShape,
            mathML,
            QFont(m_actionFontFamily->fontFamily(), m_actionFontSize->fontSize()),
            m_formulaShape->foregroundColor(),
            m_formulaShape->backgroundColor()
    ));
}
