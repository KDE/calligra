#ifndef CSVDIALOG_H
#define CSVDIALOG_H

#include <qstring.h>
#include <qcstring.h>
#include <kdialogbase.h>

class DialogUI;

class CSVDialog : public KDialogBase
{
    Q_OBJECT
public:
    enum Header { TEXT, NUMBER, DATE, CURRENCY };

    CSVDialog(QWidget* parent, QByteArray& fileArray, const QString seperator);
    ~CSVDialog();

    int getRows();
    int getCols();
    int getHeader(int col);
    QString getText(int row, int col);

private:
    void fillTable();
    void fillComboBox();
    void setText(int row, int col, const QString& text);

    int m_startline;
    QChar m_textquote;
    QString m_delimiter;
    QByteArray m_fileArray;
    DialogUI *m_dialog;

private slots:
    void returnPressed();
    void formatClicked(int id);
    void delimiterClicked(int id);
    void lineSelected(const QString& line);
    void textquoteSelected(const QString& mark);
    void currentCellChanged(int, int col);
};

#endif
