#include "dialogui.h"
#include "csvdialog.h"

#include <qtable.h>
#include <qcursor.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qtextstream.h>
#include <qbuttongroup.h>

#include <kapp.h>
#include <klocale.h>

CSVDialog::CSVDialog(QWidget* parent, QByteArray& fileArray, const QString seperator)
    : KDialogBase(parent, 0, true, QString::null, Ok|Cancel, No, true),
      m_startline(0),
      m_textquote('"'),
      m_delimiter(","),
      m_fileArray(fileArray),
      m_dialog(new DialogUI(this))
{
    kapp->restoreOverrideCursor();

    fillTable();
    fillComboBox();

    resize(sizeHint());
    setMainWidget(m_dialog);

    m_dialog->m_table->setSelectionMode(QTable::NoSelection);

    connect(m_dialog->m_formatBox, SIGNAL(clicked(int)),
            this, SLOT(formatClicked(int)));
    connect(m_dialog->m_delimiterBox, SIGNAL(clicked(int)),
            this, SLOT(delimiterClicked(int)));
    connect(m_dialog->m_delimiterEdit, SIGNAL(returnPressed()),
            this, SLOT(returnPressed()));
    connect(m_dialog->m_comboLine, SIGNAL(activated(const QString&)),
            this, SLOT(lineSelected(const QString&)));
    connect(m_dialog->m_comboQuote, SIGNAL(activated(const QString&)),
            this, SLOT(textquoteSelected(const QString&)));
    connect(m_dialog->m_table, SIGNAL(currentChanged(int, int)),
            this, SLOT(currentCellChanged(int, int)));
}

CSVDialog::~CSVDialog()
{
    kapp->setOverrideCursor(Qt::waitCursor);
}

void CSVDialog::fillTable()
{
    int row, column;
    enum { S_START, S_QUOTED_FIELD, S_MAYBE_END_OF_QUOTED_FIELD, S_END_OF_QUOTED_FIELD,
           S_MAYBE_NORMAL_FIELD, S_NORMAL_FIELD } state = S_START;

    QChar x;
    QString field = "";

    for (row = 0; row < m_dialog->m_table->numRows(); ++row)
        for (column = 0; column < m_dialog->m_table->numCols(); ++column)
            m_dialog->m_table->clearCell(row, column);

    row = column = 1;
    QTextStream inputStream(m_fileArray, IO_ReadOnly);
    inputStream.setEncoding(QTextStream::Locale);

    while (!inputStream.atEnd()) {
        inputStream >> x; // read one char

        if (x == '\r') inputStream >> x; // eat '\r', to handle DOS/LOSEDOWS files correctly

        switch (state)
        {
        case S_START :
            if (x == m_textquote)
            {
                state = S_QUOTED_FIELD;
            }
            else if (x == m_delimiter)
            {
                ++column;
            }
            else if (x == '\n')
            {
                ++row;
                column = 1;
            }
            else
            {
                field += x;
                state = S_MAYBE_NORMAL_FIELD;
            }
            break;
        case S_QUOTED_FIELD :
            if (x == m_textquote)
            {
                state = S_MAYBE_END_OF_QUOTED_FIELD;
            }
            else if (x == '\n')
            {
                setText(row - m_startline, column, field);
                field = "";
                if (x == '\n')
                {
                    ++row;
                    column = 1;
                }
                else
                {
                    ++column;
                }
                state = S_START;
            }
            else
            {
                field += x;
            }
            break;
        case S_MAYBE_END_OF_QUOTED_FIELD :
            if (x == m_textquote)
            {
                field += x;
                state = S_QUOTED_FIELD;
            }
            else if (x == m_delimiter || x == '\n')
            {
                setText(row - m_startline, column, field);
                field = "";
                if (x == '\n')
                {
                    ++row;
                    column = 1;
                }
                else
                {
                    ++column;
                }
                state = S_START;
            }
            else
            {
                state = S_END_OF_QUOTED_FIELD;
            }
            break;
        case S_END_OF_QUOTED_FIELD :
            if (x == m_delimiter || x == '\n')
            {
                setText(row - m_startline, column, field);
                field = "";
                if (x == '\n')
                {
                    ++row;
                    column = 1;
                }
                else
                {
                    ++column;
                }
                state = S_START;
            }
            else
            {
                state = S_END_OF_QUOTED_FIELD;
            }
            break;
        case S_MAYBE_NORMAL_FIELD :
            if (x == m_textquote)
            {
                field = "";
                state = S_QUOTED_FIELD;
                break;
            }
        case S_NORMAL_FIELD :
            if (x == m_delimiter || x == '\n')
            {
                setText(row - m_startline, column, field);
                field = "";
                if (x == '\n')
                {
                    ++row;
                    column = 1;
                }
                else
                {
                    ++column;
                }
                state = S_START;
            }
            else
            {
                field += x;
            }
        }
    }

    for (column = 0; column < m_dialog->m_table->numCols(); ++column)
    {
        QString header = m_dialog->m_table->horizontalHeader()->label(column);
        if (header != i18n("Text") && header != i18n("Number") &&
            header != i18n("Date") && header != i18n("Currency"))
            m_dialog->m_table->horizontalHeader()->setLabel(column, i18n("Text"));

        m_dialog->m_table->adjustColumn(column);
    }
}

void CSVDialog::fillComboBox()
{
    m_dialog->m_comboLine->clear();
    for (int row = 0; row < m_dialog->m_table->numRows(); ++row)
        m_dialog->m_comboLine->insertItem(QString::number(row + 1), row);
}

int CSVDialog::getRows()
{
    return m_dialog->m_table->numRows();
}

int CSVDialog::getCols()
{
    return m_dialog->m_table->numCols();
}

int CSVDialog::getHeader(int col)
{
    QString header = m_dialog->m_table->horizontalHeader()->label(col);

    if (header == i18n("Text"))
        return TEXT;
    else if (header == i18n("Number"))
        return NUMBER;
    else if (header == i18n("Currency"))
        return CURRENCY;
    else
        return DATE;
}

QString CSVDialog::getText(int row, int col)
{
    return m_dialog->m_table->text(row, col);
}

void CSVDialog::setText(int row, int col, const QString& text)
{
    if (row < 1) // skipped by the user
        return;

    if (m_dialog->m_table->numRows() < row)
        m_dialog->m_table->setNumRows(row);

    if (m_dialog->m_table->numCols() < col)
        m_dialog->m_table->setNumCols(col);

    m_dialog->m_table->setText(row - 1, col - 1, text);
}

void CSVDialog::returnPressed()
{
    if (m_dialog->m_delimiterBox->id(m_dialog->m_delimiterBox->selected()) != 4)
        return;

    m_delimiter = m_dialog->m_delimiterEdit->text();
    fillTable();
}

void CSVDialog::formatClicked(int id)
{
    QString header;

    switch (id)
    {
    case 1: // text
        header = i18n("Text");
        break;
    case 0: // number
        header = i18n("Number");
        break;
    case 3: // date
        header = i18n("Date");
        break;
    case 2: // currency
        header = i18n("Currency");
        break;
    }

    m_dialog->m_table->horizontalHeader()->setLabel(m_dialog->m_table->currentColumn(), header);
}

void CSVDialog::delimiterClicked(int id)
{
    switch (id)
    {
    case 0: // comma
        m_delimiter = ",";
        break;
    case 4: // other
        m_delimiter = m_dialog->m_delimiterEdit->text();
        break;
    case 2: // tab
        m_delimiter = "\t";
        break;
    case 3: // space
        m_delimiter = " ";
        break;
    case 1: // semicolon
        m_delimiter = ";";
        break;
    }

    fillTable();
}

void CSVDialog::textquoteSelected(const QString& mark)
{
    if (mark == i18n("none"))
        m_textquote = 0;
    else
        m_textquote = mark[0];

    fillTable();
}

void CSVDialog::lineSelected(const QString& line)
{
    m_startline = line.toInt() - 1;
    fillTable();
}

void CSVDialog::currentCellChanged(int, int col)
{
    int id;
    QString header = m_dialog->m_table->horizontalHeader()->label(col);

    if (header == i18n("Text"))
        id = 1;
    else if (header == i18n("Number"))
        id = 0;
    else if (header == i18n("Date"))
        id = 3;
    else
        id = 2;

    m_dialog->m_formatBox->setButton(id);
}
