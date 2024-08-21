/*
** A program to convert the XML rendered by Words into LATEX.
**
** SPDX-FileCopyrightText: 2000, 2002, 2003 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#include "fileheader.h"

#include <stdlib.h>

#include "LatexDebug.h"
#include "config.h"

#include <QTextStream>

FileHeader* FileHeader::_instance = nullptr;

/*******************************************/
/* Constructor                             */
/*******************************************/
FileHeader::FileHeader()
{
    _hasHeader    = false;
    _hasFooter    = false;
    _hasColor     = false;
    _hasUnderline = false;
    _hasEnumerate = false;
    _hasGraphics  = false;
    _hasTable     = false;
    _standardPage = 0;
    _processing   = TP_NORMAL;
    //setFileHeader(this);  /* for xmlParser class. */
}

/*******************************************/
/* Destructor                              */
/*******************************************/
FileHeader::~FileHeader()
{
    debugLatex << "FileHeader Destructor";
}

/*******************************************/
/* AnalyzePaperParam                       */
/*******************************************/
void FileHeader::analyzePaperParam(const QDomNode node)
{
    setFormat(getAttr(node, "format").toInt());
    _width = getAttr(node, "width").toInt();
    _height = getAttr(node, "height").toInt();
    setOrientation(getAttr(node, "orientation").toInt());
    setColumns(getAttr(node, "columns").toInt());
    _columnSpacing = getAttr(node, "columnspacing").toInt();
    setHeadType(getAttr(node, "hType").toInt());
    setFootType(getAttr(node, "fType").toInt());
    _headBody = getAttr(node, "spHeadBody").toInt();
    _footBody = getAttr(node, "spFootBody").toInt();
    //getAttr(node, "zoom").toInt();
}

/*******************************************/
/* AnalyzePaper                            */
/*******************************************/
void FileHeader::analyzePaper(const QDomNode node)
{
    analyzePaperParam(node);

    // Analyze child markup --> PAPERBORDERS
    QDomNode childNode = getChild(node, "PAPERBORDERS");
    _leftBorder = getAttr(childNode, "left").toInt();
    _rightBorder = getAttr(childNode, "right").toInt();
    _bottomBorder = getAttr(childNode, "bottom").toInt();
    _topBorder = getAttr(childNode, "top").toInt();
}

/*******************************************/
/* AnalyzeAttributes                        */
/*******************************************/
void FileHeader::analyzeAttributes(const QDomNode node)
{
    setProcessing(getAttr(node, "processing").toInt());
    setStandardPge(getAttr(node, "standardpage").toInt());
    setTOC(getAttr(node, "hasTOC").toInt());
    _hasHeader = getAttr(node, "hasHeader").toInt();
    _hasFooter = getAttr(node, "hasFooter").toInt();
    setUnit(getAttr(node, "unit").toInt());
}

/*******************************************/
/* Generate                                */
/*******************************************/
void FileHeader::generate(QTextStream &out)
{
    debugLatex << "GENERATION OF THE FILE HEADER";
    if (Config::instance()->mustUseLatin1())
        generateLatinPreamble(out);
    else if (Config::instance()->mustUseUnicode())
        generateUnicodePreamble(out);

    generatePackage(out);
    if (getFormat() == TF_CUSTOM)
        generatePaper(out);
    out << "%%%%%%%%%%%%%%%%%% END OF PREAMBLE %%%%%%%%%%%%%%%%%%" << Qt::endl << Qt::endl;
}

/*******************************************/
/* GeneratePaper                           */
/*******************************************/
void FileHeader::generatePaper(QTextStream &out)
{
    out << "% Format of paper" << Qt::endl;
    debugLatex << "Generate custom size paper";
    /* paper size */
    out << "\\setlength{\\paperwidth}{"  << _width  << "pt}" << Qt::endl;
    out << "\\setlength{\\paperheight}{" << _height << "pt}" << Qt::endl;
    /* FileHeader and footer */
    out << "\\setlength{\\headsep}{" << _headBody << "pt}" << Qt::endl;
    out << "\\setlength{\\footskip}{" << _footBody + _bottomBorder << "pt}" << Qt::endl;
    /* Margin */
    out << "\\setlength{\\topmargin}{" << _topBorder << "pt}" << Qt::endl;
    out << "\\setlength{\\textwidth}{" << _width - _rightBorder - _leftBorder << "pt}" << Qt::endl;
    out << Qt::endl;
}

/*******************************************/
/* GenerateLatinPreamble                   */
/*******************************************/
void FileHeader::generateLatinPreamble(QTextStream &out)
{
    out << "%% Generated by Calligra Sheets. Don't modify this file but the file *.ods." << Qt::endl;
    out << "%% Send an email to rjacolin@ifrance.com for bugs, wishes, .... Thank you." << Qt::endl;
    out << "%% Compile this file with : latex filename.tex" << Qt::endl;
    out << "%% a dvi file will be generated." << Qt::endl;
    out << "%% The file uses the latex style (not the words style). " << Qt::endl;
    out << "\\documentclass[";
    switch (getFormat()) {
    case TF_A3:
        out << "";
        break;
    case TF_A4:
        out << "a4paper, ";
        break;
    case TF_A5:
        out << "a5paper, ";
        break;
    case TF_USLETTER:
        out << "letterpaper, ";
        break;
    case TF_USLEGAL:
        out << "legalpaper, ";
        break;
    case TF_SCREEN:
        out << "";
        break;
    case TF_CUSTOM:
        out << "";
        break;
    case TF_B3:
        out << "";
        break;
    case TF_USEXECUTIVE:
        out << "executivepaper, ";
        break;
    }
    if (getOrientation() == TO_LANDSCAPE)
        out << "landscape, ";
    /* To change : will use a special latexcommand to able to
     * obtain more than one column :))
     */
    switch (getColumns()) {
    case TC_1:
        //out << "onecolumn, ";
        break;
    case TC_2:
        out << "twocolumn, ";
        break;
    case TC_MORE:
        out << "";
        break;
    case TC_NONE:
        break;
    }

    out << Config::instance()->getDefaultFontSize() << "pt";
    if (Config::instance()->getQuality() == "draft")
        out << ", draft";
    out << "]{";
    out << Config::instance()->getClass() << "}" << Qt::endl;
    out << "\\usepackage[" << Config::instance()->getEncoding() << "]{inputenc}" << Qt::endl << Qt::endl;
}

/*******************************************/
/* GenerateUnicodePreamble                 */
/*******************************************/
void FileHeader::generateUnicodePreamble(QTextStream &out)
{
    out << "%% Generated by Calligra Sheets. Don't modify this file but the file *.ods." << Qt::endl;
    out << "%% Send an email to rjacolin@ifrance.com for bugs, wishes, .... Thank you." << Qt::endl;
    out << "%% Compile this file with : lambda filename.tex" << Qt::endl;
    out << "%% a dvi file will be generated." << Qt::endl;
    out << "%% Use odvips to convert it and to see it with gv" << Qt::endl;
    out << "%% The file uses the latex style (not the words style). " << Qt::endl;
    out << "\\ocp\\TexUTF=inutf8" << Qt::endl;
    out << "\\InputTranslation currentfile \\TexUTF" << Qt::endl;
    out << "\\documentclass[";
    switch (getFormat()) {
    case TF_A3:
        out << "";
        break;
    case TF_A4:
        out << "a4paper, ";
        break;
    case TF_A5:
        out << "a5paper, ";
        break;
    case TF_USLETTER:
        out << "letterpaper, ";
        break;
    case TF_USLEGAL:
        out << "legalpaper, ";
        break;
    case TF_SCREEN:
        out << "";
        break;
    case TF_CUSTOM:
        out << "";
        break;
    case TF_B3:
        out << "";
        break;
    case TF_USEXECUTIVE:
        out << "executivepaper, ";
        break;
    }
    if (getOrientation() == TO_LANDSCAPE)
        out << "landscape, ";
    /* To change : will use a special latexcommand to able to
     * obtain more than one column :))
     */
    switch (getColumns()) {
    case TC_1:
        //out << "onecolumn, ";
        break;
    case TC_2:
        out << "twocolumn, ";
        break;
    case TC_MORE:
        out << "";
        break;
    case TC_NONE:
        break;
    }

    out << Config::instance()->getDefaultFontSize() << "pt";
    if (Config::instance()->getQuality() == "draft")
        out << ", draft";
    out << "]{";
    out << Config::instance()->getClass() << "}" << Qt::endl;
}


/*******************************************/
/* GeneratePackage                         */
/*******************************************/
void FileHeader::generatePackage(QTextStream &out)
{
    out << "% Package(s) to include" << Qt::endl;
    if (Config::instance()->mustUseUnicode())
        out << "\\usepackage{omega}" << Qt::endl;
    if (getFormat() == TF_A4)
        out << "\\usepackage[a4paper]{geometry}" << Qt::endl;
    if (hasFooter() || hasHeader())
        out << "\\usepackage{fancyhdr}" << Qt::endl;
    if (hasColor())
        out << "\\usepackage{colortbl}" << Qt::endl;
    if (hasUnderline())
        out << "\\usepackage{ulem}" << Qt::endl;
    if (hasEnumerate())
        out << "\\usepackage{enumerate}" << Qt::endl;
    if (hasGraphics())
        out << "\\usepackage{graphics}" << Qt::endl;
    out << "\\usepackage{array}" << Qt::endl;
    out << "\\usepackage{multirow}" << Qt::endl;
    out << "\\usepackage{textcomp}" << Qt::endl;
    out << "\\usepackage{rotating}" << Qt::endl;
    out << Qt::endl;
    QStringList langs = Config::instance()->getLanguagesList();
    if (langs.count() > 0) {
        out << "\\usepackage[" << langs.join(", ") << "]{babel}" << Qt::endl;
    }
    out << "\\usepackage{textcomp}" << Qt::endl;
    out << Qt::endl;

    if (langs.count() > 1)
        out << "\\selectlanguage{" << Config::instance()->getDefaultLanguage()
        << "}" << Qt::endl << Qt::endl;
}

FileHeader* FileHeader::instance()
{
    if (_instance == nullptr)
        _instance = new FileHeader();
    return _instance;
}

