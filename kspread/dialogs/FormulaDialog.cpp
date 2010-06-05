/* This file is part of the KDE project
   Copyright (C) 2002-2003 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 1999-2003 Laurent Montel <montel@kde.org>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Daniel Herring <herring@eecs.ku.edu>
             (C) 2000-2001 Werner Trobin <trobin@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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

// Local
#include "FormulaDialog.h"

#include <KTabWidget>
#include <QApplication>
#include <QCloseEvent>
#include <QGridLayout>
#include <QVBoxLayout>

#include "CalculationSettings.h"
#include "Cell.h"
#include "Util.h"
#include "ui/Editors.h"
#include "Localization.h"
#include "Map.h"
#include "Selection.h"
#include "Sheet.h"
#include "Functions.h"

#include <kcombobox.h>
#include <kdebug.h>
#include <ktextbrowser.h>

#include <knumvalidator.h>
#include <QEvent>
#include <QLabel>
#include <QPushButton>
#include <klineedit.h>
#include <QLayout>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>

using namespace KSpread;

FormulaDialog::FormulaDialog(QWidget* parent, Selection* selection, CellEditor* editor, const QString& formulaName)
        : KDialog(parent)
{
    setCaption(i18n("Function"));
    setButtons(Ok | Cancel);
    //setWFlags( Qt::WDestructiveClose );

    m_selection = selection;
    m_editor = editor;
    m_focus = 0;
    m_desc = 0;

    Cell cell(m_selection->activeSheet(), m_selection->marker());
    m_oldText = cell.userInput();
    // Make sure that there is a cell editor running.
    if (cell.userInput().isEmpty())
        m_editor->setText("=");
    else
        if (cell.userInput().at(0) != '=')
            m_editor->setText('=' + cell.userInput());
        else
            m_editor->setText(cell.userInput());

    QWidget *page = new QWidget(this);
    setMainWidget(page);

    QGridLayout *grid1 = new QGridLayout(page);
    grid1->setMargin(KDialog::marginHint());
    grid1->setSpacing(KDialog::spacingHint());

    searchFunct = new KLineEdit(page);
    searchFunct->setClearButtonShown(true);
    searchFunct->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

    grid1->addWidget(searchFunct, 0, 0);

    typeFunction = new KComboBox(page);
    QStringList cats = FunctionRepository::self()->groups();
    cats.prepend(i18n("All"));
    typeFunction->setMaxVisibleItems(15);
    typeFunction->insertItems(0, cats);
    grid1->addWidget(typeFunction, 1, 0);

    functions = new QListView(page);
    functions->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    functions->setSelectionMode(QAbstractItemView::SingleSelection);
    functions->setEditTriggers(QAbstractItemView::NoEditTriggers);
    grid1->addWidget(functions, 2, 0);

    functionsModel = new QStringListModel(this);
    proxyModel = new QSortFilterProxyModel(functions);
    proxyModel->setSourceModel(functionsModel);
    proxyModel->setFilterKeyColumn(0);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    functions->setModel(proxyModel);

    QItemSelectionModel* selectionmodel = new QItemSelectionModel(proxyModel, this);
    functions->setSelectionModel(selectionmodel);
    connect(selectionmodel, SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            this, SLOT(slotSelected()));
    // When items are activated on single click, also change the help page on mouse-over, otherwise there is no (easy) way to get
    // the help without inserting the function
    if (functions->style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick, 0, functions)) {
        connect(functions, SIGNAL(entered(QModelIndex)), this, SLOT(slotIndexSelected(QModelIndex)));
        functions->setMouseTracking(true);
    }
    //connect(proxyModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));

    selectFunction = new QPushButton(page);
    selectFunction->setToolTip(i18n("Insert function"));
    selectFunction->setIcon(BarIcon("go-down", KIconLoader::SizeSmall));
    grid1->addWidget(selectFunction, 3, 0);

    result = new KLineEdit(page);
    grid1->addWidget(result, 4, 0, 1, -1);

    m_tabwidget = new KTabWidget(page);
    m_tabwidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    grid1->addWidget(m_tabwidget, 0, 1, 4, 1);

    m_browser = new KTextBrowser(m_tabwidget, true);
    m_browser->document()->setDefaultStyleSheet("h1 { font-size:x-large; } h2 { font-size:large; } h3 { font-size:medium; }");
    m_browser->setMinimumWidth(300);

    m_tabwidget->addTab(m_browser, i18n("Help"));
    int index = m_tabwidget->currentIndex();

    m_input = new QWidget(m_tabwidget);
    QVBoxLayout *grid2 = new QVBoxLayout(m_input);
    grid2->setMargin(KDialog::marginHint());
    grid2->setSpacing(KDialog::spacingHint());

    // grid2->setResizeMode (QLayout::Minimum);

    label1 = new QLabel(m_input);
    grid2->addWidget(label1);

    firstElement = new KLineEdit(m_input);
    grid2->addWidget(firstElement);

    label2 = new QLabel(m_input);
    grid2->addWidget(label2);

    secondElement = new KLineEdit(m_input);
    grid2->addWidget(secondElement);

    label3 = new QLabel(m_input);
    grid2->addWidget(label3);

    thirdElement = new KLineEdit(m_input);
    grid2->addWidget(thirdElement);

    label4 = new QLabel(m_input);
    grid2->addWidget(label4);

    fourElement = new KLineEdit(m_input);
    grid2->addWidget(fourElement);

    label5 = new QLabel(m_input);
    grid2->addWidget(label5);

    fiveElement = new KLineEdit(m_input);
    grid2->addWidget(fiveElement);

    grid2->addStretch(10);

    m_tabwidget->addTab(m_input, i18n("Parameters"));
    m_tabwidget->setTabEnabled(m_tabwidget->indexOf(m_input), false);

    m_tabwidget->setCurrentIndex(index);

    refresh_result = true;

    connect(this, SIGNAL(cancelClicked()), this, SLOT(slotClose()));
    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    connect(typeFunction, SIGNAL(activated(const QString &)),
            this, SLOT(slotActivated(const QString &)));
    /*
        connect( functions, SIGNAL( highlighted(const QString &) ),
                 this, SLOT( slotSelected(const QString &) ) );
        connect( functions, SIGNAL( selected(const QString &) ),
                 this, SLOT( slotSelected(const QString &) ) );
    */
    connect(functions, SIGNAL(activated(QModelIndex)),
            this , SLOT(slotDoubleClicked(QModelIndex)));

    slotActivated(i18n("All"));

    connect(selectFunction, SIGNAL(clicked()),
            this, SLOT(slotSelectButton()));

    connect(firstElement, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotChangeText(const QString &)));
    connect(secondElement, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotChangeText(const QString &)));
    connect(thirdElement, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotChangeText(const QString &)));
    connect(fourElement, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotChangeText(const QString &)));
    connect(fiveElement, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotChangeText(const QString &)));

    connect(m_selection, SIGNAL(changed(const Region&)),
            this, SLOT(slotSelectionChanged()));

    connect(m_browser, SIGNAL(urlClick(const QString&)),
            this, SLOT(slotShowFunction(const QString&)));

    // Save the name of the active sheet.
    m_sheetName = m_selection->activeSheet()->sheetName();
    // Save the cells current text.
    QString tmp_oldText = m_editor->text();
    // Position of the cell.
    m_column = m_selection->marker().x();
    m_row = m_selection->marker().y();

    if (tmp_oldText.isEmpty())
        result->setText("=");
    else {
        if (tmp_oldText.at(0) != '=')
            result->setText('=' + tmp_oldText);
        else
            result->setText(tmp_oldText);
    }

    // Allow the user to select cells on the spreadsheet.
    m_selection->startReferenceSelection();

    qApp->installEventFilter(this);

    // Was a function name passed along with the constructor ? Then activate it.
    if (!formulaName.isEmpty()) {
        kDebug() << "formulaName=" << formulaName;
#if 0
        QList<QListWidgetItem *> items = functions->findItems(formulaName, Qt::MatchFixedString);
        if (items.count() > 0) {
            functions->setCurrentItem(items[0]);
            slotDoubleClicked(items[0]);
        }
#else
        int row = functionsModel->stringList().indexOf(formulaName);
        const QModelIndex sourcemodelindex = functionsModel->index(row, 0);
        const QModelIndex proxymodelindex = proxyModel->mapFromSource(sourcemodelindex);
        if (proxymodelindex.isValid()) {
            functions->setCurrentIndex(proxymodelindex);
            slotDoubleClicked(proxymodelindex);
        }
#endif
    } else {
        // Set keyboard focus to allow selection of a formula.
        searchFunct->setFocus();
    }

    // Add auto completion.
    searchFunct->setCompletionMode(KGlobalSettings::CompletionAuto);
    searchFunct->setCompletionObject(&listFunct, true);

    if (functions->currentIndex().isValid())
        selectFunction->setEnabled(false);

    connect(searchFunct, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotSearchText(const QString &)));
    connect(searchFunct, SIGNAL(returnPressed()),
            this, SLOT(slotPressReturn()));

    resize(QSize(660, 520).expandedTo(minimumSizeHint()));
}

FormulaDialog::~FormulaDialog()
{
    kDebug(36001) << "FormulaDialog::~FormulaDialog()";
}

void FormulaDialog::slotPressReturn()
{
    //laurent 2001-07-07 desactivate this code
    //because kspread crash.
    //TODO fix it
    /*
    if( !functions->currentText().isEmpty() )
        slotDoubleClicked( functions->findItem( functions->currentText() ) );
    */
}

void FormulaDialog::slotSearchText(const QString &_text)
{
    proxyModel->setFilterFixedString(_text);
    if (functions->currentIndex().isValid())
        functions->scrollTo(functions->currentIndex());
}

bool FormulaDialog::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == firstElement && ev->type() == QEvent::FocusIn)
        m_focus = firstElement;
    else if (obj == secondElement && ev->type() == QEvent::FocusIn)
        m_focus = secondElement;
    else if (obj == thirdElement && ev->type() == QEvent::FocusIn)
        m_focus = thirdElement;
    else if (obj == fourElement && ev->type() == QEvent::FocusIn)
        m_focus = fourElement;
    else if (obj == fiveElement && ev->type() == QEvent::FocusIn)
        m_focus = fiveElement;
    else
        return false;

    if (m_focus)
        m_selection->startReferenceSelection();

    return false;
}

void FormulaDialog::slotOk()
{
    // Pretend none of the text edits have focus; otherwise the next line will change the
    // value of whatever parameter has focus to the name of the cell we're editing
    m_focus = 0;

    m_selection->endReferenceSelection();

    // Revert the marker to its original position
    m_selection->initialize(QPoint(m_column, m_row));

    // If there is still an editor then set the text.
    // Usually the editor is always in place.
    if (m_editor != 0) {
        Q_ASSERT(m_editor);
        QString tmp = result->text();
        if (tmp.at(0) != '=')
            tmp = '=' + tmp;
        int pos = m_editor->cursorPosition() + tmp.length();
        m_editor->setText(tmp);
        m_editor->setFocus();
        m_editor->setCursorPosition(pos);
    }

    m_selection->emitModified();
    accept();
    deleteLater();
}

void FormulaDialog::slotClose()
{
    deleteLater();

    m_selection->endReferenceSelection();

    // Revert the marker to its original position
    m_selection->initialize(QPoint(m_column, m_row));

    // If there is still an editor then reset the text.
    // Usually the editor is always in place.
    if (m_editor != 0) {
        Q_ASSERT(m_editor);
        m_editor->setText(m_oldText);
        m_editor->setFocus();
    }

    m_selection->emitModified();
    reject();
}

void FormulaDialog::slotSelectButton()
{
    if (functions->currentIndex().isValid()) {
//slotDoubleClicked(functions->findItem(functions->text(functions->currentItem())));
        slotDoubleClicked(functions->currentIndex());
    }
}

void FormulaDialog::slotChangeText(const QString&)
{
    // Test the lock
    if (!refresh_result)
        return;

    if (m_focus == 0)
        return;

    QString tmp = m_leftText + m_funcName + '(';
    tmp += createFormula();
    tmp = tmp + ')' + m_rightText;

    result->setText(tmp);
}

QString FormulaDialog::createFormula()
{
    QString tmp("");

    if (!m_desc)
        return QString();

    bool first = true;

    int count = m_desc->params();

    if (!firstElement->text().isEmpty() && count >= 1) {
        tmp = tmp + createParameter(firstElement->text(), 0);
        first = false;
    }

    if (!secondElement->text().isEmpty() && count >= 2) {
        first = false;
        if (!first)
            tmp = tmp + ';' + createParameter(secondElement->text(), 1);
        else
            tmp = tmp + createParameter(secondElement->text(), 1);
    }
    if (!thirdElement->text().isEmpty() && count >= 3) {
        first = false;
        if (!first)
            tmp = tmp + ';' + createParameter(thirdElement->text(), 2);
        else
            tmp = tmp + createParameter(thirdElement->text(), 2);
    }
    if (!fourElement->text().isEmpty() && count >= 4) {
        first = false;
        if (!first)
            tmp = tmp + ';' + createParameter(fourElement->text(), 3);
        else
            tmp = tmp + createParameter(fourElement->text(), 3);
    }
    if (!fiveElement->text().isEmpty() && count >= 5) {
        first = false;
        if (!first)
            tmp = tmp + ';' + createParameter(fiveElement->text(), 4);
        else
            tmp = tmp + createParameter(fiveElement->text(), 4);
    }

    return(tmp);
}

QString FormulaDialog::createParameter(const QString& _text, int param)
{
    if (_text.isEmpty())
        return QString("");

    if (!m_desc)
        return QString("");

    QString text;

    ParameterType elementType = m_desc->param(param).type();

    switch (elementType) {
    case KSpread_Any: {
        bool isNumber;
        double tmp = m_selection->activeSheet()->map()->calculationSettings()->locale()->readNumber(_text, &isNumber);
        Q_UNUSED(tmp);

        //In case of number or boolean return _text, else return value as KSpread_String
        if (isNumber || _text.toUpper() == "FALSE" || _text.toUpper() == "TRUE")
            return _text;
    }
    // fall through
    case KSpread_String: {
        // Does the text start with quotes?
        if (_text[0] == '"') {
            text = '\\'; // changed: was '"'

            // Escape quotes
            QString tmp = _text;
            int pos;
            int start = 1;
            while ((pos = tmp.indexOf('"', start)) != -1) {
                if (tmp[pos - 1] != '\\')
                    tmp.replace(pos, 1, "\\\"");
                else
                    start = pos + 1;
            }

            text += tmp;
            text += '"';
        } else {
            const Region region(_text, m_selection->activeSheet()->map());
            if (!region.isValid()) {
                text = '"';

                // Escape quotes
                QString tmp = _text;
                int pos;
                int start = 1;
                while ((pos = tmp.indexOf('"', start)) != -1) {
                    if (tmp[pos - 1] != '\\')
                        tmp.replace(pos, 1, "\\\"");
                    else
                        start = pos + 1;
                }

                text += tmp;
                text += '"';
            } else
                text = _text;
        }
    }
    return text;
    case KSpread_Float:
        return _text;
    case KSpread_Boolean:
        return _text;
    case KSpread_Int:
        return _text;
    }

    // Never reached
    return text;
}

static void showEntry(KLineEdit* edit, QLabel* label,
                      FunctionDescription* desc, int param)
{
    edit->show();
    label->setText(desc->param(param).helpText() + ':');
    label->show();
    ParameterType elementType = desc->param(param).type();
    KFloatValidator *validate = 0;
    switch (elementType) {
    case KSpread_String:
    case KSpread_Boolean:
    case KSpread_Any:
        edit->setValidator(0);
        break;
    case KSpread_Float:
        validate = new KFloatValidator(edit);
        validate->setAcceptLocalizedNumbers(true);
        edit->setValidator(validate);
        edit->setText("0");
        break;
    case KSpread_Int:
        edit->setValidator(new QIntValidator(edit));
        edit->setText("0");
        break;
    }

}

void FormulaDialog::slotDoubleClicked(QModelIndex item)
{
    if (!item.isValid()) {
        item = functions->currentIndex();
        if (!item.isValid())
            return;
    }
    refresh_result = false;
    if (!m_desc) {
        m_browser->setText("");
        return;
    }

    m_focus = 0;
    int old_length = result->text().length();

    // Do not change order of these function calls due to a bug in Qt 2.2
    m_browser->setText(m_desc->toQML());
    m_tabwidget->setTabEnabled(m_tabwidget->indexOf(m_input), true);
    m_tabwidget->setCurrentIndex(1);

    //
    // Show as many KLineEdits as needed.
    //
    if (m_desc->params() > 0) {
        m_focus = firstElement;
        firstElement->setFocus();

        showEntry(firstElement, label1, m_desc, 0);
    } else {
        label1->hide();
        firstElement->hide();
    }

    if (m_desc->params() > 1) {
        showEntry(secondElement, label2, m_desc, 1);
    } else {
        label2->hide();
        secondElement->hide();
    }

    if (m_desc->params() > 2) {
        showEntry(thirdElement, label3, m_desc, 2);
    } else {
        label3->hide();
        thirdElement->hide();
    }

    if (m_desc->params() > 3) {
        showEntry(fourElement, label4, m_desc, 3);
    } else {
        label4->hide();
        fourElement->hide();
    }

    if (m_desc->params() > 4) {
        showEntry(fiveElement, label5, m_desc, 4);
    } else {
        label5->hide();
        fiveElement->hide();
    }

    if (m_desc->params() > 5)
        kDebug(36001) << "Error in param->nb_param";
    refresh_result = true;

    //
    // Put the new function call in the result.
    //
    if (result->cursorPosition() < old_length) {
        m_rightText = result->text().right(old_length - result->cursorPosition());
        m_leftText = result->text().left(result->cursorPosition());
    } else {
        m_rightText = "";
        m_leftText = result->text();
    }

    int pos = result->cursorPosition();

    {
        const QString text = proxyModel->data(functions->currentIndex()).toString();
        result->setText(m_leftText + text + "()" + m_rightText);
        if (result->text()[0] != '=')
            result->setText('=' + result->text());
    }

    //
    // Put focus somewhere is there are no KLineEdits visible
    //
    if (m_desc->params() == 0) {
        label1->show();
        label1->setText(i18n("This function has no parameters."));
        result->setFocus();
        const QString text = proxyModel->data(functions->currentIndex()).toString();
        result->setCursorPosition(pos + text.length() + 2);
    }
    slotChangeText("");
}

void FormulaDialog::slotIndexSelected(const QModelIndex& index)
{
    // This slot is only called when single-click to activate is uesd in the listbox, when the mouse moves over a item; to prevent
    // the active selection to change after the user activated one, slotSelected is only called when the current tab is the Help tab,
    // and not when the parameters tab is active
    if (m_tabwidget->currentIndex() != 0) return;
    QString function = proxyModel->data(index).toString();
    slotSelected(function);
}

void FormulaDialog::slotSelected(const QString& afunction)
{
    QString function = afunction;
    if (function.isNull())
        function = proxyModel->data(functions->currentIndex()).toString();

    FunctionDescription* desc = FunctionRepository::self()->functionInfo(function);
    if (!desc) {
        m_browser->setText(i18n("Description is not available."));
        return;
    }

    if (functions->currentIndex().isValid())
        selectFunction->setEnabled(true);

    // Lock
    refresh_result = false;

    m_funcName = function;
    m_desc = desc;

    // Set the help text
    m_browser->setText(m_desc->toQML());
    //m_browser->setContentsPos( 0, 0 );

    m_focus = 0;

    m_tabwidget->setCurrentIndex(0);
    m_tabwidget->setTabEnabled(m_tabwidget->indexOf(m_input), false);

    // Unlock
    refresh_result = true;
}

// from hyperlink in the "Related Function"
void FormulaDialog::slotShowFunction(const QString& function)
{
    FunctionDescription* desc =
        FunctionRepository::self()->functionInfo(function);
    if (!desc) return;

    // select the category
    QString category = desc->group();
    typeFunction->setCurrentIndex(typeFunction->findText(category));
    slotActivated(category);

    // select the function
    //Q3ListBoxItem* item = functions->findItem( function, QKeySequence::ExactMatch | Qt::CaseSensitive );
    //if( item ) functions->setCurrentItem( item );
    int row = functionsModel->stringList().indexOf(function);
    const QModelIndex sourcemodelindex = functionsModel->index(row, 0);
    const QModelIndex proxymodelindex = proxyModel->mapFromSource(sourcemodelindex);
    if (proxymodelindex.isValid())
        functions->setCurrentIndex(proxymodelindex);

    slotSelected(function);
}

void FormulaDialog::slotSelectionChanged()
{
    if (!m_focus)
        return;

    if (m_selection->isValid()) {
        QString area = m_selection->name();
        m_focus->setText(area);
    }
}

void FormulaDialog::slotActivated(const QString& category)
{
    QStringList lst;
    if (category == i18n("All"))
        lst = FunctionRepository::self()->functionNames();
    else
        lst = FunctionRepository::self()->functionNames(category);

    kDebug(36001) << "category:" << category << " (" << lst.count() << "functions)";

    functionsModel->setStringList(lst);

    QStringList upperList;
    for (QStringList::Iterator it = lst.begin(); it != lst.end();++it)
        upperList.append((*it).toUpper());
    listFunct.setItems(upperList);

    // Go to the first function in the list.
    const QString text = proxyModel->data(proxyModel->index(0, 0)).toString();
    slotSelected(text);
}

void FormulaDialog::closeEvent(QCloseEvent * e)
{
    deleteLater();
    e->accept();
}

#include "FormulaDialog.moc"
