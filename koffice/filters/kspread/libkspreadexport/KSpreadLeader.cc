/*
This file is part of the KDE project
Copyright (C) 2002 Fred Malabre <fmalabre@yahoo.com>

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
 * Boston, MA 02110-1301, USA.
*/

#include "KSpreadLeader.h"

using namespace KSpread;

Leader::Leader(KoFilterChain *filterChain) {
	m_worker = NULL;
	m_filterChain = filterChain;
}


Leader::Leader(KoFilterChain *filterChain, KSpreadBaseWorker *newWorker) {
	m_worker = newWorker;
	m_filterChain = filterChain;
}


Leader::~Leader() {
}


KSpreadBaseWorker *Leader::getWorker() const {
	return m_worker;
}


void Leader::setWorker(KSpreadBaseWorker *newWorker) {
	m_worker = newWorker;
}


KoFilter::ConversionStatus Leader::convert() {
	KoFilter::ConversionStatus status;

	// Validate the filter chain and the worker
	if (!m_filterChain) {
		kdWarning(30508) << "koFilterChain is NULL!" << endl;
		return KoFilter::StupidError;
	}
	if (!m_worker) {
		kdWarning(30508) << "the KSpreadWorker is NULL!" << endl;
		return KoFilter::StupidError;
	}

	// Gather data about the filter itself
	KSpreadFilterProperty docProperty;
	docProperty["outputfile"] = m_filterChain->outputFile();
	status = m_worker->startDocument(docProperty);
	if (status != KoFilter::OK)
		return status;

	// Get the document in memory
	KSpreadDoc *document = (KSpreadDoc *) m_filterChain->inputDocument();
	if (!document) {
		kdWarning(30508) << "the KSpreadDoc is NULL!" << endl;
		return KoFilter::StupidError;
	}
	if ( !::qt_cast<const KSpread::Doc *>( document ) ) {
		kdWarning(30508) << "the document is not a KSpreadDoc!" << endl;
		return KoFilter::StupidError;
	}
	if (document->mimeType() != "application/x-kspread") {
		kdWarning(30508) << "the mime type document is not application/x-kspread!" << endl;
		return KoFilter::StupidError;
	}
	KoDocumentInfo *info = document->documentInfo();
	if (!document) {
		kdWarning(30508) << "the KoDocumentInfo is NULL!" << endl;
		return KoFilter::StupidError;
	}

	// Gather data about the document info
	status = doInfo(info);
	if (status != KoFilter::OK)
		return status;

	// Gather data about the spread book
	status = doSpreadBook(document);
	if (status != KoFilter::OK)
		return status;

	// Gather data about the spread sheet
	KSpreadSheet *spreadSheet = document->map()->firstTable();
	while (spreadSheet != 0) {
		status = doSpreadSheet(spreadSheet);
		if (status != KoFilter::OK)
			return status;

		// Gather data about the cell
		for (int row = 1; row <= m_maxCellRow; ++row) {
			for (int column = 1; column <= m_maxCellColumn; ++column) {
				Cell*spreadCell = spreadSheet->cellAt(column, row);
				status = doSpreadCell(spreadCell, column, row);
				if (status != KoFilter::OK)
					return status;
			}
		}

		spreadSheet = document->map()->nextTable();
	}

	return status;
}


KoFilter::ConversionStatus Leader::doInfo(KoDocumentInfo *info) {
	KoFilter::ConversionStatus status;

#if 0 // this was never used, it's been removed now
	// Gather data about the document log
	KSpreadFilterProperty docInfoLogProperty;
	KoDocumentInfoLog *infoLog = (KoDocumentInfoLog *) info->page("log");
	docInfoLogProperty["oldlog"] = infoLog->oldLog();
	docInfoLogProperty["newlog"] = infoLog->newLog();
	status = m_worker->startInfoLog(docInfoLogProperty);
	if (status != KoFilter::OK)
		return status;
#endif

	// Gather data about the document author
	KSpreadFilterProperty docInfoAuthorProperty;
	KoDocumentInfoAuthor *infoAuthor = (KoDocumentInfoAuthor *) info->page("author");
	docInfoAuthorProperty["fullname"] = infoAuthor->fullName();
	docInfoAuthorProperty["initial"] = infoAuthor->initial();
	docInfoAuthorProperty["title"] = infoAuthor->title();
	docInfoAuthorProperty["company"] = infoAuthor->company();
	docInfoAuthorProperty["email"] = infoAuthor->email();
	docInfoAuthorProperty["telephone"] = infoAuthor->telephone();
	docInfoAuthorProperty["fax"] = infoAuthor->fax();
	docInfoAuthorProperty["country"] = infoAuthor->country();
	docInfoAuthorProperty["postalcode"] = infoAuthor->postalCode();
	docInfoAuthorProperty["city"] = infoAuthor->city();
	docInfoAuthorProperty["street"] = infoAuthor->street();
	status = m_worker->startInfoAuthor(docInfoAuthorProperty);
	if (status != KoFilter::OK)
		return status;

	// Gather data about the document about
	KSpreadFilterProperty docInfoAboutProperty;
	KoDocumentInfoAbout *infoAbout = (KoDocumentInfoAbout *) info->page("about");
	docInfoAboutProperty["title"] = infoAbout->title();
	docInfoAboutProperty["author"] = infoAbout->abstract();
	status = m_worker->startInfoAbout(docInfoAboutProperty);
	return status;
}


KoFilter::ConversionStatus Leader::doSpreadBook(KSpreadDoc *document) {
	KSpreadFilterProperty docSpreadBookProperty;
	docSpreadBookProperty["spreadsheetcount"] = QString::number(document->map()->count());
	docSpreadBookProperty["decimalsymbol"] = document->locale()->decimalSymbol();
	docSpreadBookProperty["thousandsseparator"] = document->locale()->thousandsSeparator();
	docSpreadBookProperty["currencysymbol"] = document->locale()->currencySymbol();
	docSpreadBookProperty["monetarydecimalsymbol"] = document->locale()->monetaryDecimalSymbol();
	docSpreadBookProperty["monetarythousandsseparator"] = document->locale()->monetaryThousandsSeparator();
	docSpreadBookProperty["positivesign"] = document->locale()->positiveSign();
	docSpreadBookProperty["negativesign"] = document->locale()->negativeSign();
	docSpreadBookProperty["fracdigits"] = QString::number(document->locale()->fracDigits());
	docSpreadBookProperty["positiveprefixcurrencysymbol"] = (document->locale()->positivePrefixCurrencySymbol()==0?"false":"true");
	docSpreadBookProperty["negativeprefixcurrencysymbol"] = (document->locale()->negativePrefixCurrencySymbol()==0?"false":"true");
	docSpreadBookProperty["use12clock"] = (document->locale()->use12Clock()==0?"false":"true");
	docSpreadBookProperty["weekstartsmonday"] = (document->locale()->weekStartsMonday()==0?"false":"true");
	docSpreadBookProperty["weekstartday"] = QString::number(document->locale()->weekStartDay());
	docSpreadBookProperty["language"] = document->locale()->language();
	docSpreadBookProperty["country"] = document->locale()->country();
	docSpreadBookProperty["encoding"] = document->locale()->encoding();
	docSpreadBookProperty["dateformat"] = document->locale()->dateFormat();
	docSpreadBookProperty["dateformatshort"] = document->locale()->dateFormatShort();
	docSpreadBookProperty["timeformat"] = document->locale()->timeFormat();
	docSpreadBookProperty["defaultlanguage"] = KLocale::defaultLanguage();
	docSpreadBookProperty["defaultcountry"] = KLocale::defaultCountry();
	docSpreadBookProperty["defaultgridpencolorname"] = document->defaultGridPen().color().name();
	docSpreadBookProperty["defaultgridpencolorred"] = QString::number(document->defaultGridPen().color().red());
	docSpreadBookProperty["defaultgridpencolorgreen"] = QString::number(document->defaultGridPen().color().green());
	docSpreadBookProperty["defaultgridpencolorblue"] = QString::number(document->defaultGridPen().color().blue());
	docSpreadBookProperty["defaultgridpenwidth"] = QString::number(document->defaultGridPen().width());
	docSpreadBookProperty["showverticalscrollbar"] = (document->getShowVerticalScrollBar()==0?"false":"true");
	docSpreadBookProperty["showhorizontalscrollbar"] = (document->getShowHorizontalScrollBar()==0?"false":"true");
	docSpreadBookProperty["showcolheader"] = (document->getShowColHeader()==0?"false":"true");
	docSpreadBookProperty["showrowheader"] = (document->getShowRowHeader()==0?"false":"true");
	docSpreadBookProperty["indentvalue"] = QString::number(document->getIndentValue());
	docSpreadBookProperty["movetovalue"] = QString::number(document->getMoveToValue());
	docSpreadBookProperty["showmessageerror"] = (document->getShowMessageError()==0?"false":"true");
	docSpreadBookProperty["showtabbar"] = (document->getShowTabBar()==0?"false":"true");
	docSpreadBookProperty["pagebordercolorname"] = document->pageBorderColor().name();
	docSpreadBookProperty["pagebordercolorred"] = QString::number(document->pageBorderColor().red());
	docSpreadBookProperty["pagebordercolorgreen"] = QString::number(document->pageBorderColor().green());
	docSpreadBookProperty["pagebordercolorblue"] = QString::number(document->pageBorderColor().blue());
	docSpreadBookProperty["showcommentindicator"] = (document->getShowCommentIndicator()==0?"false":"true");
	docSpreadBookProperty["showformulabar"] = (document->getShowFormulaBar()==0?"false":"true");
	docSpreadBookProperty["dontcheckupperword"] = (document->dontCheckUpperWord()==0?"false":"true");
	docSpreadBookProperty["dontchecktitlecase"] = (document->dontCheckTitleCase()==0?"false":"true");
	docSpreadBookProperty["showstatusbar"] = (document->getShowStatusBar()==0?"false":"true");
	docSpreadBookProperty["unitname"] = document->getUnitName();
	docSpreadBookProperty["syntaxversion"] = QString::number(document->syntaxVersion());
	return m_worker->startSpreadBook(docSpreadBookProperty);
}


KoFilter::ConversionStatus Leader::doSpreadSheet(KSpreadSheet *spreadSheet) {
	KSpreadFilterProperty docSpreadSheetProperty;
	docSpreadSheetProperty["name"] = spreadSheet->tableName();
	docSpreadSheetProperty["sizemaxx"] = QString::number(spreadSheet->sizeMaxX());
	docSpreadSheetProperty["sizemaxy"] = QString::number(spreadSheet->sizeMaxY());
	docSpreadSheetProperty["showgrid"] = (spreadSheet->getShowGrid()==0?"false":"true");
	docSpreadSheetProperty["showformula"] = (spreadSheet->getShowFormula()==0?"false":"true");
	docSpreadSheetProperty["showformulaindicator"] = (spreadSheet->getShowFormulaIndicator()==0?"false":"true");
	docSpreadSheetProperty["lcmode"] = (spreadSheet->getLcMode()==0?"false":"true");
	docSpreadSheetProperty["autocalc"] = (spreadSheet->getAutoCalc()==0?"false":"true");
	docSpreadSheetProperty["showcolumnnumber"] = (spreadSheet->getShowColumnNumber()==0?"false":"true");
	docSpreadSheetProperty["hidezero"] = (spreadSheet->getHideZero()==0?"false":"true");
	docSpreadSheetProperty["firstletterupper"] = (spreadSheet->getFirstLetterUpper()==0?"false":"true");
	docSpreadSheetProperty["ishidden"] = (spreadSheet->isHidden()==0?"false":"true");
	docSpreadSheetProperty["showpageborders"] = (spreadSheet->isShowPageBorders()==0?"false":"true");
	docSpreadSheetProperty["printablewidth"] = QString::number(spreadSheet->printableWidth());
	docSpreadSheetProperty["printableheight"] = QString::number(spreadSheet->printableHeight());
	docSpreadSheetProperty["paperwidth"] = QString::number(spreadSheet->paperWidth());
	docSpreadSheetProperty["paperheight"] = QString::number(spreadSheet->paperHeight());
	docSpreadSheetProperty["leftborder"] = QString::number(spreadSheet->leftBorder());
	docSpreadSheetProperty["rightborder"] = QString::number(spreadSheet->rightBorder());
	docSpreadSheetProperty["topborder"] = QString::number(spreadSheet->topBorder());
	docSpreadSheetProperty["bottomborder"] = QString::number(spreadSheet->bottomBorder());
	docSpreadSheetProperty["headleft"] = spreadSheet->headLeft();
	docSpreadSheetProperty["headmid"] = spreadSheet->headMid();
	docSpreadSheetProperty["headright"] = spreadSheet->headRight();
	docSpreadSheetProperty["footleft"] = spreadSheet->footLeft();
	docSpreadSheetProperty["footmid"] = spreadSheet->footMid();
	docSpreadSheetProperty["footright"] = spreadSheet->footRight();
	docSpreadSheetProperty["orientation"] = spreadSheet->orientationString();
	docSpreadSheetProperty["paperformat"] = spreadSheet->paperFormatString();
	docSpreadSheetProperty["printgrid"] = (spreadSheet->getPrintGrid()==0?"false":"true");
	docSpreadSheetProperty["printcomment"] = (spreadSheet->getPrintCommentIndicator()==0?"false":"true");
	docSpreadSheetProperty["printformula"] = (spreadSheet->getPrintFormulaIndicator()==0?"false":"true");
	updateMaxCells(spreadSheet);
	docSpreadSheetProperty["maxcellrow"] = QString::number(m_maxCellRow);
	docSpreadSheetProperty["maxcellcolumn"] = QString::number(m_maxCellColumn);
	return m_worker->startSpreadSheet(docSpreadSheetProperty);
}


KoFilter::ConversionStatus Leader::doSpreadCell(Cell*spreadCell, int column, int row) {
	KSpreadFilterProperty docSpreadCellProperty;
	docSpreadCellProperty["column"] = QString::number(column);
	docSpreadCellProperty["row"] = QString::number(row);
	docSpreadCellProperty["width"] = QString::number(spreadCell->dblWidth());
	docSpreadCellProperty["height"] = QString::number(spreadCell->dblHeight());
	docSpreadCellProperty["empty"] = (spreadCell->isEmpty()==0?"false":"true");
	if (!spreadCell->isEmpty()) {
		docSpreadCellProperty["text"] = spreadCell->text();
		docSpreadCellProperty["strouttext"] = spreadCell->strOutText();
		docSpreadCellProperty["action"] = spreadCell->action();
		docSpreadCellProperty["date"] = (spreadCell->isDate()==0?"false":"true");
		docSpreadCellProperty["time"] = (spreadCell->isTime()==0?"false":"true");
		docSpreadCellProperty["textwidth"] = QString::number(spreadCell->textWidth());
		docSpreadCellProperty["textheight"] = QString::number(spreadCell->textHeight());
		docSpreadCellProperty["forceextracells"] = (spreadCell->isForceExtraCells()==0?"false":"true");
		docSpreadCellProperty["mergedxcells"] = QString::number(spreadCell->mergedXCells());
		docSpreadCellProperty["mergedycells"] = QString::number(spreadCell->mergedYCells());
		docSpreadCellProperty["extraxcells"] = QString::number(spreadCell->extraXCells());
		docSpreadCellProperty["extraycells"] = QString::number(spreadCell->extraYCells());
		docSpreadCellProperty["extrawidth"] = QString::number(spreadCell->extraWidth());
		docSpreadCellProperty["extraheight"] = QString::number(spreadCell->extraHeight());
		docSpreadCellProperty["formula"] = (spreadCell->isFormula()==0?"false":"true");
		docSpreadCellProperty["haserror"] = (spreadCell->hasError()==0?"false":"true");
		docSpreadCellProperty["alignx"] = QString::number(spreadCell->defineAlignX());
		docSpreadCellProperty["name"] = spreadCell->name();
		docSpreadCellProperty["fullname"] = spreadCell->fullName();
		docSpreadCellProperty["content"] = QString::number(spreadCell->content());
		docSpreadCellProperty["style"] = QString::number(spreadCell->style());
		docSpreadCellProperty["valuedate"] = spreadCell->valueDate().toString();
		docSpreadCellProperty["valuetime"] = spreadCell->valueTime().toString();
		docSpreadCellProperty["leftborderwidth"] = QString::number(spreadCell->leftBorderPen(column, row).width());
		docSpreadCellProperty["leftbordercolorname"] = spreadCell->leftBorderPen(column, row).color().name();
		docSpreadCellProperty["leftbordercolorred"] = QString::number(spreadCell->leftBorderPen(column, row).color().red());
		docSpreadCellProperty["leftbordercolorgreen"] = QString::number(spreadCell->leftBorderPen(column, row).color().green());
		docSpreadCellProperty["leftbordercolorblue"] = QString::number(spreadCell->leftBorderPen(column, row).color().blue());
		docSpreadCellProperty["topborderwidth"] = QString::number(spreadCell->topBorderPen(column, row).width());
		docSpreadCellProperty["topbordercolorname"] = spreadCell->topBorderPen(column, row).color().name();
		docSpreadCellProperty["topbordercolorred"] = QString::number(spreadCell->topBorderPen(column, row).color().red());
		docSpreadCellProperty["topbordercolorgreen"] = QString::number(spreadCell->topBorderPen(column, row).color().green());
		docSpreadCellProperty["topbordercolorblue"] = QString::number(spreadCell->topBorderPen(column, row).color().blue());
		docSpreadCellProperty["rightborderwidth"] = QString::number(spreadCell->rightBorderPen(column, row).width());
		docSpreadCellProperty["rightbordercolorname"] = spreadCell->rightBorderPen(column, row).color().name();
		docSpreadCellProperty["rightbordercolorred"] = QString::number(spreadCell->rightBorderPen(column, row).color().red());
		docSpreadCellProperty["rightbordercolorgreen"] = QString::number(spreadCell->rightBorderPen(column, row).color().green());
		docSpreadCellProperty["rightbordercolorblue"] = QString::number(spreadCell->rightBorderPen(column, row).color().blue());
		docSpreadCellProperty["bottomborderwidth"] = QString::number(spreadCell->bottomBorderPen(column, row).width());
		docSpreadCellProperty["bottombordercolorname"] = spreadCell->bottomBorderPen(column, row).color().name();
		docSpreadCellProperty["bottombordercolorred"] = QString::number(spreadCell->bottomBorderPen(column, row).color().red());
		docSpreadCellProperty["bottombordercolorgreen"] = QString::number(spreadCell->bottomBorderPen(column, row).color().green());
		docSpreadCellProperty["bottombordercolorblue"] = QString::number(spreadCell->bottomBorderPen(column, row).color().blue());
		docSpreadCellProperty["bgcolorname"] = spreadCell->bgColor(column, row).name();
		docSpreadCellProperty["bgcolorred"] = QString::number(spreadCell->bgColor(column, row).red());
		docSpreadCellProperty["bgcolorgreen"] = QString::number(spreadCell->bgColor(column, row).green());
		docSpreadCellProperty["bgcolorblue"] = QString::number(spreadCell->bgColor(column, row).blue());
		docSpreadCellProperty["bgbrushstyle"] = QString::number(spreadCell->backGroundBrush(column, row).style());
		docSpreadCellProperty["valueempty"] = (spreadCell->value().isEmpty()==0?"false":"true");
		docSpreadCellProperty["valueboolean"] = (spreadCell->value().isBoolean()==0?"false":"true");
		docSpreadCellProperty["valueinteger"] = (spreadCell->value().isInteger()==0?"false":"true");
		docSpreadCellProperty["valuefloat"] = (spreadCell->value().isFloat()==0?"false":"true");
		docSpreadCellProperty["valuenumber"] = (spreadCell->value().isNumber()==0?"false":"true");
		docSpreadCellProperty["valuestring"] = (spreadCell->value().isString()==0?"false":"true");
		docSpreadCellProperty["valueerror"] = (spreadCell->value().isError()==0?"false":"true");
		docSpreadCellProperty["valueasboolean"] = (spreadCell->value().asBoolean()==0?"false":"true");
		docSpreadCellProperty["valueasinteger"] = QString::number(spreadCell->value().asInteger());
		docSpreadCellProperty["valueasfloat"] = QString::number(spreadCell->value().asFloat());
		docSpreadCellProperty["valueasstring"] = spreadCell->value().asString();
		docSpreadCellProperty["valueasdatetime"] = spreadCell->value().asDateTime().toString();
		docSpreadCellProperty["valueaserror"] = spreadCell->value().errorMessage();
	}
	return m_worker->startSpreadCell(docSpreadCellProperty);
}


void Leader::updateMaxCells(KSpreadSheet *spreadSheet) {
	m_maxCellColumn = 0;
	m_maxCellRow = 0;

	int maxColumn = spreadSheet->maxColumn();
	int maxRow = spreadSheet->maxRow();

	// Go through all the SpreadSheet to find out the minimum rectangle of cells
	// Maybe we should have something which does that in the KSpreadSheet class,
	// it would be easy to keep track of this each time a new Cellis instanciated.
	for (int row = 1; row < maxRow; ++row) {
		bool usedColumn = FALSE;
		for (int column = 1; column < maxColumn; ++column) {
			Cell*cell = spreadSheet->cellAt(column, row);
			if (!cell->isDefault() && !cell->isEmpty()) {
				if (column > m_maxCellColumn) {
					m_maxCellColumn = column;
				}
				usedColumn = TRUE;
			}
		}
		if (usedColumn) {
			m_maxCellRow = row;
		}
	}
}
