
#include "gspreadimport.h"

#include "KoDocument.h"
#include "KoFilterChain.h"
#include "kspread/Map.h"
#include "kspread/Cell.h"
#include "kspread/Value.h"
#include "kspread/ValueConverter.h"
#include "kspread/RowColumnFormat.h"

#include "kspread/Util.h"

#include <kpluginfactory.h>
#include <KMessageBox>

#include <QRegExp>

#include <QDebug>


K_PLUGIN_FACTORY(GSpreadImportFactory, registerPlugin<GSpreadImport>();)
K_EXPORT_PLUGIN(GSpreadImportFactory("kofficefilters"));

GSpreadImport::GSpreadImport(QObject* parent, const QVariantList&) 
    : KoFilter(parent),
    m_rows(0),
    m_cols(0),
    m_sheet(0),
    m_doc(0)
{
}

KoFilter::ConversionStatus GSpreadImport::convert(const QByteArray& from, const QByteArray& to)
{
    QString file(m_chain->inputFile());
    KoDocument* document = m_chain->outputDocument();

    if (!document)
        return KoFilter::StupidError;

    if (!qobject_cast<const KSpread::Doc *>(document)) {
        kWarning(30501) << "document isn't a KSpread::Doc but a " << document->metaObject()->className();
        return KoFilter::NotImplemented;
    }
    if (from != "application/atom+xml" || to != "application/x-kspread") {
        kWarning(30501) << "Invalid mimetypes " << from << " " << to;
        return KoFilter::NotImplemented;
    }

    m_doc = static_cast<KSpread::Doc*>(document);   // type checked above

    if (m_doc->mimeType() != "application/x-kspread") {
        kWarning(30501) << "Invalid document mimetype " << m_doc->mimeType();
        return KoFilter::NotImplemented;
    }

    QFile in(file);
    if (!in.open(QIODevice::ReadOnly)) {
        KMessageBox::sorry(0L, i18n("Google-Spreadsheet filter cannot open input file - please report."));
        in.close();
        return KoFilter::FileNotFound;
    }
    
    QByteArray data(in.readAll());
    in.close();
    
//     m_sheet = m_doc->map()->addNewSheet();
    
    if (!readFeed(data))
    {
        return KoFilter::ParsingError;
    }
    
    const double defaultWidth = m_doc->map()->defaultColumnFormat()->width();
    QVector<double> widths(m_cols);
    for (int i = 0; i < m_cols; ++i)
        widths[i] = defaultWidth;
    
    KSpread::Cell cell(m_sheet, 1, 1);
    QFontMetrics fm(cell.style().font());
    
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            cell = KSpread::Cell(m_sheet, col + 1, row + 1);
            QString val = cell.value().asString();
            const double len = fm.width(val);
            if (len > widths[col])
                widths[col] = len;
        }
    }

    for (int i = 0; i < m_cols; ++i) {
        if (widths[i] > defaultWidth)
            m_sheet->nonDefaultColumnFormat(i + 1)->setWidth(widths[i]);
    }

    return KoFilter::OK;
}

bool GSpreadImport::readFeed(QByteArray& input)
{
    m_xmlReader.clear();
    m_xmlReader.addData(input);

    if (m_xmlReader.readNextStartElement()) {
        if (m_xmlReader.name() == "feed") {
            while (m_xmlReader.readNextStartElement()) {
                if (m_xmlReader.name() == "title") {
                    QString title = m_xmlReader.readElementText();
                    m_sheet = m_doc->map()->addNewSheet(title);
                }
                else if (m_xmlReader.name() == "rowCount")
                    m_rows = m_xmlReader.readElementText().toInt();
                else if (m_xmlReader.name() == "colCount")
                    m_cols = m_xmlReader.readElementText().toInt();
                else if (m_xmlReader.name() == "entry")
                    readEntry();
                else
                    m_xmlReader.skipCurrentElement();
            }
        }
        else
            m_xmlReader.raiseError(QObject::tr("The is not a Google Spreadsheet."));
    }

    return !m_xmlReader.error();
}

void GSpreadImport::readEntry()
{
    Q_ASSERT(m_xmlReader.isStartElement() && m_xmlReader.name() == "entry");

    while (m_xmlReader.readNextStartElement()) {
        if (m_xmlReader.name() == "cell") {
            int row = m_xmlReader.attributes().value("row").toString().toInt();
            int col = m_xmlReader.attributes().value("col").toString().toInt();
            
            KSpread::Cell cell(m_sheet, col, row);
            
            QString inputValue = m_xmlReader.attributes().value("inputValue").toString();
            
            //actual Sheet-Cell contains a formula
            if (inputValue.startsWith('=')) {
                QString formula = convertFormula(inputValue, cell);
                if (formula.isEmpty()) {
                    inputValue = m_xmlReader.attributes().value("numericValue").toString();
                    KSpread::Value value(inputValue.toDouble());
                    cell.setValue(value);
                    cell.setUserInput(m_doc->map()->converter()->asString(value).asString());
                }
                else {
                    cell.parseUserInput(formula);
                    if (cell.value().isError()) {
                        //TODO formular parse error
qDebug() << "###GSpreadImport::readEntry\tFormular-Parse-Error!";
                    }
                }
            }
            else if (m_xmlReader.attributes().hasAttribute("numericValue")) {
                inputValue = m_xmlReader.attributes().value("numericValue").toString();
                KSpread::Value value(inputValue.toDouble());
                cell.setValue(value);
                cell.setUserInput(m_doc->map()->converter()->asString(value).asString());
            }
            else {
                KSpread::Value value(inputValue);
                cell.setValue(value);
                cell.setUserInput(m_doc->map()->converter()->asString(value).asString());
            }
        }
        m_xmlReader.skipCurrentElement();
    }
}

QString GSpreadImport::convertFormula(QString& formula, KSpread::Cell& cell)
{
qDebug() << "###GSpreadImport::convertCellIndicesInFormulas\tformula=" << formula << ",row=" << cell.row() << ",col=" << cell.column() << ",name=" << cell.name();

    QString newFormula = formula;
    QMap<QString, QString> newIndices;
    int pos = 0;
    int single_R_Count = 0;
    int single_C_Count = 0;
    int tmpRowNr = 0;
    bool columnIndexFollows = false;
    QString idxRow;;
    QRegExp rx("(R|C)\\[(\\-?\\d+)\\]");
    while ((pos = rx.indexIn(formula, pos)) != -1) {
// qDebug() << "###GSpreadImport::convertCellIndicesInFormulas\trx2.cap(0)=" << rx.cap(0);
// qDebug() << "###GSpreadImport::convertCellIndicesInFormulas\trx2.cap(1)=" << rx.cap(1);
// qDebug() << "###GSpreadImport::convertCellIndicesInFormulas\trx2.cap(2)=" << rx.cap(2);

        int idxOffset = 0;
        int newRowNr = cell.row(), newColNr = cell.column();
        if (rx.cap(1) == "R") {
            idxOffset = rx.cap(2).toInt();
            newRowNr += idxOffset;
            if (formula.at(pos + rx.cap(0).size()) == 'C' 
                && formula.at(pos + rx.cap(0).size() + 1) != '[') {
                newFormula.remove(pos + rx.cap(0).size() - single_C_Count++, 1);
            }
            else {
                pos += rx.matchedLength();
                tmpRowNr = newRowNr;
                idxRow = rx.cap(0);
                columnIndexFollows = true;
                continue;
            }
        }
        else if (rx.cap(1) == "C") {
            idxOffset = rx.cap(2).toInt();
            newColNr += idxOffset;
            if (formula.at(pos - 1) == 'R') {
                newFormula.remove(pos - ++single_R_Count, 1);
            }
        }
        else {
            continue;
        }
        
        QString newIndex;
        if (columnIndexFollows) {
            newRowNr = tmpRowNr;
            tmpRowNr = 0;
            columnIndexFollows = false;
            newIndex = KSpread::Cell::name(newColNr, newRowNr);
            newIndices.insert(idxRow + rx.cap(0), newIndex);
        }
        else {
            newIndex = KSpread::Cell::name(newColNr, newRowNr);
            newIndices.insert(rx.cap(0), newIndex);
        }

        pos += rx.matchedLength();
    }
    
    QMap<QString, QString>::iterator iter = newIndices.begin();
    while (iter != newIndices.end()) {
        newFormula.replace(iter.key(), iter.value());
        ++iter;
    }

    //the delimiter for the statement for the number of decimals, ',' used by Google has to change
    //to ';', which is specified in OpenFormula
//     pos = 0;
//     QRegExp rx2("(,)\\d{1}(\\.)\\d+E(\\-|\\+)\\d+\\)$");
// 
//     while ((pos = rx2.indexIn(formula, pos)) != -1) {
//         newFormula.replace(rx2.cap(1), ";");
//         newFormula.replace(rx2.cap(2), ",");
// 
//         pos += rx2.matchedLength();
//     }
    
    //TODO test, is it ok for all formulas
    newFormula.replace(',', ';');
    newFormula.replace('.', ',');
qDebug() << "###GSpreadImport::convertCellIndicesInFormulas\tnewFormula=" << newFormula;
    
    return newFormula;
}

QString GSpreadImport::errorString()
{
    return QObject::tr("%1\ncharOffset %2").arg(m_xmlReader.errorString()).arg(m_xmlReader.characterOffset());
}

#include "gspreadimport.moc"
