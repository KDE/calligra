#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <kdebug.h>
#include "kword_xml2html.h"


/*
** A program to convert the XML rendered by KWord into HTML.
**
** Copyright (C) 1999 Lezlie Fort
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
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/



/*** Structures and types used by this program ****/
typedef enum HTMLObject {
    HTML_Null,
    HTML_Text,
    HTML_Table,
    HTML_Image
} HTMLObject;

typedef struct Format {
    int iFontSize;
    int iFontWeight;
    int iFontItalic;
    int iRed;
    int iGreen;
    int iBlue;
    int iLength;
    int iID;
    struct Format *pNext;
}Format;

typedef struct TextStruct {
    char *zText;
    int iFontFlow;
    int iIsBullet;
    int iIsEnumerated;
    int iHeader1;
    int iHeader2;
    int iHeader3;
    int iMargin;
    char *zFileName;
    Format *pFormat;
}tTextStruct;

typedef struct TableText {
    tTextStruct sTextAttributes;
    struct TableText *pNext;
    struct TableText *pPrevious;
}tTableText;

typedef struct Table {
    int iColumnNum;
    int iRowNum;
    tTableText *pTextAttributes;
    struct Table *pNext;
    struct Table *pPrevious;
}tTable;

typedef struct HTMLTree {
    HTMLObject ObjectType;

    union {
        /* This structure represents the contents of one <TEXT>...</TEXT> markup
         */
        tTextStruct TextStruct;
        struct {
            int iLeft;
            int iRight;
            int iTop;
            int iBottom;
            char *zFileName;
        }Image;
        tTable Table;
    }u;
    struct HTMLTree *pNext;
}HTMLTree;

typedef struct ListMarkup {
    char zOpenList[4];
    struct ListMarkup *pPrevious;
    struct ListMarkup *pNext;
}ListMarkup;

/* Read a file into memory */

char *ReadIntoMemory(const char *zFileName, int *iSize) {

    FILE *in;
    char *zBuffer;
    struct stat statBuf;
    int iItemsRead;

    if(!(in = fopen(zFileName,"r"))){
        fprintf(stderr,"Unable to open file %s\n",zFileName);
        return 0;
    }
    if( stat(zFileName,&statBuf) != 0) {
        fprintf(stderr,"Cannot locate file %s\n",zFileName);
        return 0;
    }
    zBuffer = (char *)malloc(statBuf.st_size+1);
    if((iItemsRead=fread(zBuffer,1,statBuf.st_size,in))!=statBuf.st_size) {
        fprintf(stderr,"Problem reading file %s\n",zFileName);
        return 0;
    }
    fclose(in);
    zBuffer[iItemsRead]='\0';
    *iSize = iItemsRead+1;
    return zBuffer;
}


void InitFormat(Format *pFormat) {
    pFormat->iFontSize = 0;
    pFormat->iFontWeight = 0;
    pFormat->iFontItalic = 0;
    pFormat->iRed = 0;
    pFormat->iGreen = 0;
    pFormat->iBlue = 0;
    pFormat->iLength = 0;
    pFormat->iID = 0;
    pFormat->pNext = 0;
}


void InitTextStruct(tTextStruct *pTextStruct) {
    pTextStruct->zText = 0;
    pTextStruct->iFontFlow = 0;
    pTextStruct->iIsBullet = 0;
    pTextStruct->iIsEnumerated = 0;
    pTextStruct->iHeader1 = 0;
    pTextStruct->iHeader2 = 0;
    pTextStruct->iHeader3 = 0;
    pTextStruct->iMargin = 0;
    pTextStruct->zFileName = 0;
    pTextStruct->pFormat = (Format *)malloc(sizeof(Format));
    InitFormat(pTextStruct->pFormat);
}

void InitTableText (tTableText *pTableText) {
    InitTextStruct(&(pTableText->sTextAttributes));
    pTableText->pNext = 0;
    pTableText->pPrevious = 0;
}

void InitHTMLTree(HTMLTree *pHTMLTree) {
    pHTMLTree->pNext = 0;
    pHTMLTree->ObjectType = HTML_Null;
    /* Stop here.  No need to initialize union elements because they can't exist
    ** simultaneously.
    */
}

void InitListMarkup(ListMarkup *pListMarkup) {
    pListMarkup->pPrevious = 0;
    pListMarkup->pNext = 0;
}

ListMarkup *MakeNewMarkup(const char *zMarkup, ListMarkup **pCurrent) {
    ListMarkup *pTemp;
    pTemp = (ListMarkup *)malloc(sizeof(ListMarkup));
    InitListMarkup(pTemp);
    strcpy(pTemp->zOpenList,zMarkup);
    pTemp->pPrevious = *pCurrent;
    pTemp->pNext = 0;
    (*pCurrent)->pNext = pTemp;
    return pTemp;
}

tTable *GetTable(tTable *pTable) {
    while(pTable->pNext) {
        pTable = pTable->pNext;
    }
    return pTable;
}

tTableText *GetTableText(tTableText *pTableText) {
    while(pTableText->pNext) {
        pTableText = pTableText->pNext;
    }
    return pTableText;
}

void SearchText(Token *pToken, HTMLTree *pHTMLTree) {

    QCString Scratch;
    int iCount;
    Token *pSaveToken;
    iCount = 0;
    if(!pToken) return;

    /* Always begin by making sure that we are dealing with the very latest
    ** HTMLTree. This is necessary because the SearchText function is recursive.
    ** This means that without this check, we could be calling ourselves
    ** with "back issues" of HTMLTree.  We want to make sure that we're being
    ** called with the current value.
    */
    while(pHTMLTree->pNext){
        pHTMLTree=pHTMLTree->pNext;
    }
    // What context are we in...
    if(pHTMLTree->ObjectType == HTML_Text) {
        if(pHTMLTree->u.TextStruct.iMargin) {

            /* We've got a complete text specification once "iMargin" has been
            ** set.  Create a new HTMLTree to
            ** prepare to store characteristics of the next encountered HTML object.
            */
            pHTMLTree->pNext = (HTMLTree *)malloc(sizeof(HTMLTree));
            pHTMLTree=pHTMLTree->pNext;
            InitHTMLTree(pHTMLTree);
        }
    }
    if(pHTMLTree->ObjectType == HTML_Image) {
        if(pHTMLTree->u.Image.zFileName) {

            /* We've got a complete image specification (for Kword frames).  Create
            ** a new HTMLTree to prepare to store characteristics of the next
            ** HTML object.
            */
            pHTMLTree->pNext = (HTMLTree *)malloc(sizeof(HTMLTree));
            pHTMLTree=pHTMLTree->pNext;
            InitHTMLTree(pHTMLTree);
        }
    }
    if(pHTMLTree->ObjectType == HTML_Table) {
        tTable *pTable;
        tTableText *pTableText;
        pTable = &(pHTMLTree->u.Table);
        /* Get us to the last table cell. */
        pTable = GetTable(pTable);
        pTableText = pTable->pTextAttributes;
        pTableText = GetTableText(pTableText);
        if(pTableText->sTextAttributes.iMargin) {
            /* We've got a complete text specification for the current Text
            ** specification in the current Table cell.  Add a new Text specification
            ** to the list.
            */
            pTableText->pNext = (tTableText *)malloc(sizeof(tTableText));
            pTableText->pNext->pPrevious = pTableText;
            pTableText->pNext->pNext = 0;
            InitTextStruct(&(pTableText->pNext->sTextAttributes));
        }
    }
    if((pToken->eType)!=TT_Markup) {
        /* We may encounter a simple token such as a space or a word.  If this
        ** is the case, the only choice there is is to procede to the next token.
        ** (simple tokens do not have a "pContent" field).
        **
        ** Eg:  For the following:
        **
        ** <DOC args........>
        **  <PAPER args........./>
        ** </DOC>
        **
        ** The newline and space are considered part of the contents of the
        ** DOC markup, so the Markup structure associated with the
        ** DOC tag would look like this:
        **
        ** zText="DOC"
        ** pContents=Token_NEWLINE;
        ** pContents->pNext=Token_SPACE
        ** (pContents->pNext)->pNext=Markup PAPER
        */
        SearchText(pToken->pNext,pHTMLTree);
        return;
    }

    //kdDebug(30503) << "SearchText. Markup found. Token=" << pToken->zText << endl;

    if(!(strcmp(pToken->zText,"FRAMESET"))) {
        int iValue = 0;
        int iHaveRow = 0;
        int iAmImage = 0;
        int iRowValue = 0, iColValue = 0;
        Arg *pArg = ((Markup *)pToken)->pArg;
        int iFrameInfo = 0;
        while(pArg) {
            if(!(strcmp(pArg->zName,"FRAMETYPE"))) {
                iValue = atoi(pArg->zValue);
                /* If the frameType argument of the FRAMESET markup is 2, then we have
                ** a picture within a frame.  If this is the case, set the current
                ** HTML object to type "HTML_Image".
                */
                if(iValue==2) {
                    iAmImage = 1;
                }
                kdDebug(30503) << "This frame has a frame type of " << iValue << endl;
            }
            else if(!(strcmp(pArg->zName,"ROW"))) {
                iHaveRow = 1;
                iRowValue = atoi(pArg->zValue);
            }
            else if(!(strcmp(pArg->zName,"COL")))
                iColValue = atoi(pArg->zValue);
            else if(!(strcmp(pArg->zName,"FRAMEINFO")))
                iFrameInfo = atoi(pArg->zValue);

            pArg=pArg->pNext;
        }
        /* If we have found a row (and a column), then we have a new table cell.
        ** This may be the beginning cell of a continuing table, or a whole new
        ** table altogether.  If it's a whole new table, then most likely
        ** the ObjectType will be HTML_Null.
        ** This is because, when we complete one object, we usually create
        ** a brand new HTMLTree with Null type.  The exception to this rule
        ** is encountering two tables back-to-back.  In that case,
        ** the ObjectType will still be HTML_Table (left over from the first
        ** of the two tables).  We handle that case later.
        */
        if(iHaveRow && pHTMLTree->ObjectType==HTML_Null) {
            tTable *pTable;
            pHTMLTree->ObjectType = HTML_Table;
            pTable = &(pHTMLTree->u.Table);
            pTable->iColumnNum = iColValue;
            pTable->iRowNum = iRowValue;
            pTable->pPrevious = 0;
            pTable->pNext = 0;
            pTable->pTextAttributes = (tTableText *)malloc(sizeof(tTableText));
            InitTableText(pTable->pTextAttributes);
        }
        /* In the next case, we have, as before, a new table cell.  It may be
        ** for a new table, or it may be for an existing table.
        */
        else if(iHaveRow && pHTMLTree->ObjectType!=HTML_Null) {

            /* Here's the back-to-back table case.  Create a new HTMLTree and
            ** start populating it.  Don't forget to free the extra text
            ** specification for the last cell from the previous table (see comment
            ** below for a short exaplanation of extra text specifications).
            */
            if(!iRowValue&&!iColValue) {
                tTable *pTable;
                tTableText *pTableText;
                pTable = &(pHTMLTree->u.Table);
                pTable = GetTable(pTable);
                pTableText = pTable->pTextAttributes;
                pTableText = GetTableText(pTableText);
                pTableText->pPrevious->pNext = 0;
                free(pTableText);
                pHTMLTree->pNext = (HTMLTree *)malloc(sizeof(HTMLTree));
                pHTMLTree = pHTMLTree->pNext;
                InitHTMLTree(pHTMLTree);
                pHTMLTree->ObjectType = HTML_Table;
                pTable = &(pHTMLTree->u.Table);
                pTable->pPrevious = 0;
                pTable->pNext = 0;
                pTable->iColumnNum = iColValue;
                pTable->iRowNum = iRowValue;
                pTable->pTextAttributes = (tTableText *)malloc(sizeof
                                                               (tTableText));
                InitTableText(pTable->pTextAttributes);
            }
            else {  /* We need to add a new cell to an existing table.  Remember
                    ** that the "if(pHTMLTree->ObjectType==HTML)" code added
                    ** a text specification to the end of the list for this cell.
                    ** It won't be used, so delete it. */
                tTable *pTable;
                tTableText *pTableText;
                pTable = &(pHTMLTree->u.Table);
                pTable = GetTable(pTable);
                pTableText = pTable->pTextAttributes;
                pTableText = GetTableText(pTableText);
                pTableText->pPrevious->pNext = 0;
                free(pTableText);
                pTable->pNext = (tTable *)malloc(sizeof(tTable));
                pTable->pNext->iColumnNum = iColValue;
                pTable->pNext->iRowNum = iRowValue;
                pTable->pNext->pNext = 0;
                pTable->pNext->pPrevious = pTable;
                pTable->pNext->pTextAttributes = (tTableText *)malloc(sizeof(
                    tTableText));
                InitTableText(pTable->pNext->pTextAttributes);
            }
        }
        /*
        **  We have just completed a table, and now we need to create a new
        **  non-table object.  We can tell this because we have
        **  a table object, but no row/column specification.  If this is the
        **  case, we don't need the tTableText structure that we've just
        **  created (in the code block a little ways above), so delete it
        **  from the current list of table cell text specifications.  Also, create
        **  a new HTMLTree to prepare for a new object.
        */
        if(pHTMLTree->ObjectType == HTML_Table && !iHaveRow) {
            tTableText *pTableText;
            tTable *pTable;
            pTable = &(pHTMLTree->u.Table);
            pTable = GetTable(pTable);
            pTableText = pTable->pTextAttributes;
            pTableText = GetTableText(pTableText);
            pTableText->pPrevious->pNext = 0;
            free(pTableText);
            pHTMLTree->pNext = (HTMLTree *)malloc(sizeof(HTMLTree));
            pHTMLTree=pHTMLTree->pNext;
            InitHTMLTree(pHTMLTree);
        }
        if((iAmImage)&&(pHTMLTree->ObjectType == HTML_Null)) {
            pHTMLTree->ObjectType = HTML_Image;
            pHTMLTree->u.Image.iLeft = 0;
            pHTMLTree->u.Image.iRight = 0;
            pHTMLTree->u.Image.iTop = 0;
            pHTMLTree->u.Image.iBottom = 0;
            pHTMLTree->u.Image.zFileName = 0;
        }
        // Ignore frames which are not FI_BODY (e.g. headers and footers)
        // i.e. only look into normal frames
        if ( iFrameInfo == 0 )
            SearchText(((Markup*)pToken)->pContent,pHTMLTree);

        SearchText(pToken->pNext,pHTMLTree);
    }
    else if(!(strcmp(pToken->zText,"TEXT"))) {
        kdDebug(30503) << "TEXT" << endl;
        tTableText *pTableText = 0L;
        if(pHTMLTree->ObjectType == HTML_Table) {
            /* We're inside a table.  Make sure to store all text attributes
            ** for this cell are inside the appropriate Table structure.
            */
            tTable *pTable;
            pTable = &(pHTMLTree->u.Table);

            /* Get us to the last table cell. */

            pTable = GetTable(pTable);
            pTableText = pTable->pTextAttributes;
            pTableText = GetTableText(pTableText);
        }
        else {
            kdDebug(30503) << "Not in a table. Setting object type to text" << endl;
            pHTMLTree->ObjectType = HTML_Text;
            InitTextStruct(&(pHTMLTree->u.TextStruct));
            kdDebug(30503) << "Struct initialised" << endl;
        }
        pSaveToken = pToken;
        pToken=((Markup *)pToken)->pContent;/* Go to first word in TEXT markup */
        if(!pToken){  /* If this markup has no content, then it means we've
                      ** found a newline (that's how "kword" represents it).
                      ** Set zText to '\n\0' and move on.  Note:  If we have a
                      ** picture, then no content does not denote a newline.
                      ** Fill one in anyway, because the population of the
                      ** zText buffer will help us with buffer management
                      ** (see 2nd comment in "FORMAT" section).  The newline will
                      ** not be printed.
                      */
            Scratch.append("\n");
            iCount=1;

        }
        else {
            while(pToken) {
                //kdDebug(30503) << "pToken=" << pToken << endl;
                kdDebug(30503) << "text=" << pToken->zText << endl;
                Scratch.append( pToken->zText );
                iCount += strlen( pToken->zText );
                pToken->zText += iCount;
                /*
                while(*pToken->zText) {
                    *Scratch++=*(pToken->zText++);
                    iCount++;
                    kdDebug(30503) << "iCount=" << iCount << endl;
                }
                (pToken->zText)++;   */         /* Increment past the null terminator
                                              ** for this word.  We want one single
                                              ** terminator at the very end of the
                                              ** entire concatenated string.
                                              */

                pToken=pToken->pNext;
            }
        }
        if(pHTMLTree->ObjectType==HTML_Text) {
            kdDebug(30503) << "Copying " << iCount << " chars into pHTMLTree : " << Scratch.data() << endl;
            pHTMLTree->u.TextStruct.zText=(char *)malloc(iCount+1);
            strncpy(pHTMLTree->u.TextStruct.zText,Scratch.data(),iCount);
            pHTMLTree->u.TextStruct.zText[iCount]='\0';
        }
        else {
            kdDebug(30503) << "Copying " << iCount << " chars into pTableText : " << Scratch.data() << endl;
            pTableText->sTextAttributes.zText=(char *)malloc(iCount+1);
            strncpy(pTableText->sTextAttributes.zText,Scratch.data(),iCount);
            pTableText->sTextAttributes.zText[iCount]='\0';
        }

        kdDebug(30503) << "Recursive call..." << endl;
        SearchText(pSaveToken->pNext,pHTMLTree);
    }
    else if(!(strcmp(pToken->zText,"FORMAT"))){
        /* If this object is of type HTML_Null, then FORMAT does not
        ** apply to a TEXT markup (recall that the TextStruct member of
        ** the HTMLTree union is only created and populated once a valid TEXT
        ** markup is current).
        ** This means, the format applies to e.g. a paragraph and we don't care about that.
        */
        if(pHTMLTree->ObjectType!=HTML_Null) {
            Arg *pArg;
            Format *pFormat;
            tTableText *pTableText;
            if(pHTMLTree->ObjectType == HTML_Table) {
                tTable *pTable;
                pTable = &(pHTMLTree->u.Table);
                pTable = GetTable(pTable);
                pTableText = pTable->pTextAttributes;
                pTableText = GetTableText(pTableText);
                pFormat = pTableText->sTextAttributes.pFormat;
            }
            else {
                if(pHTMLTree->ObjectType != HTML_Text)
                    kdWarning() << "Shouldn't that be a text ? " << (int)pHTMLTree->ObjectType << endl;
                pFormat = pHTMLTree->u.TextStruct.pFormat;
                kdDebug(30503) << "Format set to " << pFormat << endl;
            }
            pArg = ((Markup *)pToken)->pArg;
            while(pFormat->pNext) {
                pFormat = pFormat->pNext;
            }
            if((pFormat->iFontSize)&&
               (pFormat->iFontWeight)&&
               (pFormat->iFontItalic)&&
               (pFormat->iLength)){
                /*
                ** This may be a <FORMAT>...</FORMAT> markup that has been filled in.
                ** If this is the case, create a new markup structure.  This is
                ** something of a kludge.  It works because -- of the content tokens
                ** that we deal with for a <FORMAT> markup -- size, weight, and
                ** italic are filled in after "color".  So, if they're populated, we
                ** know "color" is populated.  Bad, I know, but it's a hack.
                */
                pFormat->pNext = (struct Format *)malloc(sizeof(struct Format));
                InitFormat(pFormat->pNext);
                pFormat = pFormat->pNext;
            }
            /* If this FORMAT markup has an id of 2, then we are dealing with a
            ** picture, and the FORMAT markup for it has been populated.
            ** Create the next markup structure.
            */
            if(pFormat->iID==2) {
                pFormat->pNext = (struct Format *)malloc(sizeof(struct Format));
                InitFormat(pFormat->pNext);
                pFormat = pFormat->pNext;
            }
            while(pArg) {
                if(!(strcmp(pArg->zName,"LEN"))) {
                    pFormat->iLength = atoi(pArg->zValue);
                }
                else if(!(strcmp(pArg->zName,"ID"))) {
                    pFormat->iID = atoi(pArg->zValue);
                }
                pArg = pArg->pNext;
            }
        }
        SearchText(((Markup *)pToken)->pContent,pHTMLTree);
        SearchText(pToken->pNext,pHTMLTree);
    }
    else if(!(strcmp(pToken->zText,"SIZE"))) {
        // Ignore e.g. paragraph formats
        if(pHTMLTree->ObjectType!=HTML_Null) {
            Arg *pArg;
            Format *pFormat;
            tTableText *pTableText;
            tTextStruct *pTextStruct;
            if(pHTMLTree->ObjectType == HTML_Table) {
                tTable *pTable;
                pTable = &(pHTMLTree->u.Table);
                pTable = GetTable(pTable);
                pTableText = pTable->pTextAttributes;
                pTableText = GetTableText(pTableText);
                pFormat = pTableText->sTextAttributes.pFormat;
                pTextStruct = &(pTableText->sTextAttributes);
            }
            else {
                pFormat = pHTMLTree->u.TextStruct.pFormat;
                pTextStruct = &(pHTMLTree->u.TextStruct);
            }
            if(!pTextStruct->zText) {}
            /* SIZE may not always refer to a "TEXT" markup.  If it does
            ** not, then we're done here.
            */
            else {
                while(pFormat->pNext) {
                    pFormat = pFormat->pNext;
                }
                pArg=((Markup*)pToken)->pArg;
                while(pArg) {
                    if(!(strcmp(pArg->zName,"VALUE"))) {
                        pFormat->iFontSize = atoi(pArg->zValue);
                        break;
                    }
                    else {
                        pArg=pArg->pNext;
                    }
                }
                if(!pFormat->iFontSize) {
                    fprintf(stderr,"Error.  SIZE markup must contain a font size.\n");
                    exit(1);
                }
            }
        }
        SearchText(pToken->pNext,pHTMLTree);
    }
    else if(!(strcmp(pToken->zText,"COLOR"))) {
        // Ignore e.g. paragraph formats
        if(pHTMLTree->ObjectType!=HTML_Null) {
            Arg *pArg;
            Format *pFormat;
            tTableText *pTableText;
            tTextStruct *pTextStruct;
            int iFoundRed = 0;
            int iFoundGreen = 0;
            int iFoundBlue = 0;
            if(pHTMLTree->ObjectType == HTML_Table) {
                tTable *pTable;
                pTable = &(pHTMLTree->u.Table);
                pTable = GetTable(pTable);
                pTableText = pTable->pTextAttributes;
                pTableText = GetTableText(pTableText);
                pFormat = pTableText->sTextAttributes.pFormat;
                pTextStruct = &(pTableText->sTextAttributes);
            }
            else {
                pFormat = pHTMLTree->u.TextStruct.pFormat;
                pTextStruct = &(pHTMLTree->u.TextStruct);
            }
            if(!pTextStruct->zText) {}
            /* COLOR may not always refer to a "TEXT" markup.  If it does
            ** not, then we're done.
            */
            else if (pFormat) {
                while(pFormat->pNext) {
                    pFormat = pFormat->pNext;
                }
                pArg=((Markup*)pToken)->pArg;
                while(pArg) {
                    if(!(strcmp(pArg->zName,"RED"))) {
                        pFormat->iRed = atoi(pArg->zValue);
                        iFoundRed++;
                    }
                    else if(!(strcmp(pArg->zName,"GREEN"))) {
                        pFormat->iGreen = atoi(pArg->zValue);
                        iFoundGreen++;
                    }
                    else if(!(strcmp(pArg->zName,"BLUE"))) {
                        pFormat->iBlue = atoi(pArg->zValue);
                        iFoundBlue++;
                    }
                    pArg=pArg->pNext;
                }
                if((!iFoundRed)||(!iFoundGreen)||(!iFoundBlue)) {
                    printf("Error.  COLOR markup must contain information for Red, Green,"
                           " and Blue\n");
                    exit(1);
                }
            }
        }
        SearchText(pToken->pNext,pHTMLTree);
    }
    else if(!(strcmp(pToken->zText,"WEIGHT"))) {
        // Ignore e.g. paragraph formats
        if(pHTMLTree->ObjectType!=HTML_Null) {
            Arg *pArg;
            tTableText *pTableText;
            tTextStruct *pTextStruct;
            Format *pFormat;
            if(pHTMLTree->ObjectType == HTML_Table) {
                tTable *pTable;
                pTable = &(pHTMLTree->u.Table);
                pTable = GetTable(pTable);
                pTableText = pTable->pTextAttributes;
                pTableText = GetTableText(pTableText);
                pFormat = pTableText->sTextAttributes.pFormat;
                pTextStruct = &(pTableText->sTextAttributes);
            }
            else {
                pFormat = pHTMLTree->u.TextStruct.pFormat;
                pTextStruct = &(pHTMLTree->u.TextStruct);
            }
            if(!pTextStruct->zText) {}
            /* WEIGHT may not always refer to a "TEXT" markup.  If it does
            ** not, then we're done here.
            */
            else {
                while(pFormat->pNext) {
                    pFormat = pFormat->pNext;
                }
                pArg = ((Markup*)pToken)->pArg;
                while(pArg){
                    if(!(strcmp(pArg->zName,"VALUE"))) {
                        pFormat->iFontWeight = atoi(pArg->zValue);
                        break;
                    }
                    else {
                        pArg=pArg->pNext;
                    }
                }
                if(!pFormat->iFontWeight) {
                    fprintf(stderr,"Error.  WEIGHT markup must contain a font weight.\n");
                    exit(1);
                }
            }
        }
        SearchText(pToken->pNext,pHTMLTree);
    }
    else if(!(strcmp(pToken->zText,"ITALIC"))) {
        // Ignore e.g. paragraph formats
        if(pHTMLTree->ObjectType!=HTML_Null) {
            Arg *pArg;
            Format *pFormat;
            tTableText *pTableText;
            tTextStruct *pTextStruct;
            if(pHTMLTree->ObjectType == HTML_Table) {
                tTable *pTable;
                pTable = &(pHTMLTree->u.Table);
                pTable = GetTable(pTable);
                pTableText = pTable->pTextAttributes;
                pTableText = GetTableText(pTableText);
                pFormat = pTableText->sTextAttributes.pFormat;
                pTextStruct = &(pTableText->sTextAttributes);
            }
            else {
                pFormat = pHTMLTree->u.TextStruct.pFormat;
                pTextStruct = &(pHTMLTree->u.TextStruct);
            }
            if(!pTextStruct->zText) {}
            /* ITALIC may not always refer to a "TEXT" markup.  If it does
            ** not, then the object may not have a type yet.
            */
            else {
                while(pFormat->pNext) {
                    pFormat = pFormat->pNext;
                }
                pArg=((Markup*)pToken)->pArg;
                while(pArg){
                    if(!(strcmp(pArg->zName,"VALUE"))) {
                        pFormat->iFontItalic = atoi(pArg->zValue)+1;
                        /* Add 1 here because we will
                        ** assume that if *iFontItalic
                        ** = 0, then the ITALIC value
                        ** hasn't been encountered
                        ** yet.  Since 0 is a valid
                        ** italic argument value,
                        ** we'll add 1 to whatever we
                        ** get.  That way, *iFontItalic
                        ** will always be > 0 if we've
                        ** reached this point in the
                        ** code.
                        */
                        break;
                    }
                    else {
                        pArg=pArg->pNext;
                    }
                }
                if(!pFormat->iFontItalic) {
                    fprintf(stderr,"Error.  ITALIC markup must contain a font direction.\n");
                    exit(1);
                }
            }
        }
        SearchText(pToken->pNext,pHTMLTree);
    }
    // TODO ? UNDERLINE and VERTALIGN
    else if(!(strcmp(pToken->zText,"FLOW"))) {
        Arg *pArg;
        tTableText *pTableText;
        tTextStruct *pTextStruct;
        if(pHTMLTree->ObjectType == HTML_Table) {
            tTable *pTable;
            pTable = &(pHTMLTree->u.Table);
            pTable = GetTable(pTable);
            pTableText = pTable->pTextAttributes;
            pTableText = GetTableText(pTableText);
            pTextStruct = &(pTableText->sTextAttributes);
        }
        else {
            pTextStruct = &(pHTMLTree->u.TextStruct);
        }
        pArg=((Markup*)pToken)->pArg;
        while(pArg){
            if(!(strcmp(pArg->zName,"VALUE"))) {
                pTextStruct->iFontFlow = atoi(pArg->zValue)+1;
                break;
            }
            else {
                pArg=pArg->pNext;
            }
        }
        if(!pTextStruct->iFontFlow) {
            fprintf(stderr,"Error.  FLOW markup must contain a font alignment.\n");
            exit(1);
        }
        SearchText(pToken->pNext,pHTMLTree);
    }
    else if(!(strcmp(pToken->zText,"NAME"))) {
        Arg *pArg;
        tTextStruct *pTextStruct;
        tTableText *pTableText;
        pArg=((Markup*)pToken)->pArg;
        if(pHTMLTree->ObjectType == HTML_Table) {
            tTable *pTable;
            pTable = &(pHTMLTree->u.Table);
            pTable = GetTable(pTable);
            pTableText = pTable->pTextAttributes;
            pTableText = GetTableText(pTableText);
            pTextStruct = &(pTableText->sTextAttributes);
        }
        else {
            pTextStruct = &(pHTMLTree->u.TextStruct);
        }
        while(pArg){
            if(!(strcmp(pArg->zName,"VALUE"))) {
                if(!(strcmp(pArg->zValue,"Bullet List"))) {
                    pTextStruct->iIsBullet = 1;
                }
                else if(!(strcmp(pArg->zValue,"Enumerated List"))) {
                    pTextStruct->iIsEnumerated = 1;
                }
                else if(!(strcmp(pArg->zValue,"Head 1"))) {
                    pTextStruct->iHeader1 = 1;
                }
                else if(!(strcmp(pArg->zValue,"Head 2"))) {
                    pTextStruct->iHeader2 = 1;
                }
                else if(!(strcmp(pArg->zValue,"Head 3"))) {
                    pTextStruct->iHeader3 = 1;
                }
                break;
            }
            else {
                pArg=pArg->pNext;
            }
        }
        SearchText(pToken->pNext,pHTMLTree);
    }
    else if(!(strcmp(pToken->zText,"ILEFT"))) {
        tTextStruct *pTextStruct;
        tTableText *pTableText;
        Arg *pArg;
        pArg=((Markup*)pToken)->pArg;
        if(pHTMLTree->ObjectType == HTML_Table) {
            tTable *pTable;
            pTable = &(pHTMLTree->u.Table);
            pTable = GetTable(pTable);
            pTableText = pTable->pTextAttributes;
            pTableText = GetTableText(pTableText);
            pTextStruct = &(pTableText->sTextAttributes);
        }
        else {
            pTextStruct = &(pHTMLTree->u.TextStruct);
        }
        if(!pTextStruct->zText){} /* ILEFT may not refer to a TEXT specification.
                                  ** If it doesn't, then we're done here. */
        else {
            while(pArg){
                if(!(strcmp(pArg->zName,"PT"))) {
                    pTextStruct->iMargin = atoi(pArg->zValue);
                    if(!pTextStruct->iMargin) {
                        /* Let's make this 1-based, and make the minimum margin equal to
                        ** 1.
                        */
                        pTextStruct->iMargin++;
                    }
                    break;
                }
                else {
                    pArg=pArg->pNext;
                }
            }
        }
        SearchText(pToken->pNext,pHTMLTree);
    }
    /* Note:  FILENAME has to do with image insertion, which is not supported
    ** within tables.  Therefore, in the following section of code, we do not
    ** test for ObjectType = HTML_Table.
    */
    else if((!(strcmp(pToken->zText,"FILENAME")))&&
            (pHTMLTree->ObjectType == HTML_Text)) {
        Arg *pArg;
        pArg=((Markup*)pToken)->pArg;
        while(pArg){
            if(!(strcmp(pArg->zName,"VALUE"))) {
                pHTMLTree->u.TextStruct.zFileName = pArg->zValue;
                break;
            }
            else {
                pArg=pArg->pNext;
            }
        }
        SearchText(pToken->pNext,pHTMLTree);
    }
    else if((!(strcmp(pToken->zText,"FILENAME")))&&
            (pHTMLTree->ObjectType == HTML_Image)) {
        Arg *pArg;
        pArg=((Markup*)pToken)->pArg;
        while(pArg) {
            if(!(strcmp(pArg->zName,"VALUE"))) {
                pHTMLTree->u.Image.zFileName = pArg->zValue;
                break;
            }
            else {
                pArg=pArg->pNext;
            }
        }
        SearchText(pToken->pNext,pHTMLTree);
    }
    else if(!(strcmp(pToken->zText,"FRAME"))) {
        if(pHTMLTree->ObjectType == HTML_Image) {
            Arg *pArg;
            pArg=((Markup *)pToken)->pArg;
            while(pArg) {
                if(!(strcmp(pArg->zName,"LEFT"))) {
                    pHTMLTree->u.Image.iLeft = atoi(pArg->zValue);
                }
                else if(!(strcmp(pArg->zName,"TOP"))) {
                    pHTMLTree->u.Image.iTop = atoi(pArg->zValue);
                }
                else if(!(strcmp(pArg->zName,"BOTTOM"))) {
                    pHTMLTree->u.Image.iBottom = atoi(pArg->zValue);
                }
                else if(!(strcmp(pArg->zName,"RIGHT"))) {
                    pHTMLTree->u.Image.iRight = atoi(pArg->zValue);
                }
                pArg=pArg->pNext;
            }
        }
        SearchText(pToken->pNext,pHTMLTree);
    }
    else {
        // Process child elements
        SearchText(((Markup *)pToken)->pContent,pHTMLTree);
        // Process next sibling
        SearchText(pToken->pNext,pHTMLTree);
    }
}


HTMLTree *ProcessTableAndText(HTMLTree *pHTMLTree,HTMLObject egObject,
                              FILE *Outfile) {
    kdDebug(30503) << "ProcessTableAndText" << endl;
    QCString Scratch;
    ListMarkup *pCurrentListMarkup = 0;
    // tTextStruct sTextStruct;
    tTable *pCurrentTableCell;
    int iMarginIncrease = 0;
    char *zWeight;
    char *zWeightEnd;
    char *zItalic;
    char *zItalicEnd;
    int iCurrentCol = 0;
    int iCurrentRow = 0;
    int iPosition = 0;
    int iBlankLine = 0;
    int iPutPre = 0;
    int iIsFirstListItem = 1;
    int iNumHead1s = 0;
    int iNumHead2s = 0;
    int iNumHead3s = 0;
    int iCurrentMargin = 0;
    int iParagraph = 0;
    int iAlign = 0;
    int iSize;
    tTableText *pCurrentTableText;
    Format *pFormat;
    zWeight = (char *)malloc(4);
    zWeightEnd = (char *)malloc(5);
    zItalic = (char *)malloc(4);
    zItalicEnd = (char *)malloc(5);
    if(egObject == HTML_Text) {
        fprintf(Outfile,"<p>"); // Let's start a paragraph
        while(pHTMLTree){
            QCString align;
            switch(pHTMLTree->u.TextStruct.iFontFlow-1) {
            case 0:
                align="left";
                break;
            case 1:
                align="right";
                break;
            case 2:
                align="center";
                break;
            default:
                break;
            }
            pFormat = pHTMLTree->u.TextStruct.pFormat;
            if(pFormat->iLength) {
                if((pHTMLTree->u.TextStruct.iIsBullet)||
                   (pHTMLTree->u.TextStruct.iIsEnumerated)) {
                    if((pHTMLTree->u.TextStruct.iIsBullet)&&
                       (iIsFirstListItem||((pHTMLTree->u.TextStruct.iMargin-1)
                                           >iCurrentMargin))) {
                        if(pCurrentListMarkup) {
                            pCurrentListMarkup = MakeNewMarkup("ul>",&pCurrentListMarkup);
                        }
                        else {
                            pCurrentListMarkup = (ListMarkup *)malloc(sizeof (ListMarkup));
                            InitListMarkup(pCurrentListMarkup);
                            strcpy(pCurrentListMarkup->zOpenList,"ul>");
                        }
                    }
                    else if((pHTMLTree->u.TextStruct.iIsEnumerated)&&
                            (iIsFirstListItem||((pHTMLTree->u.TextStruct.iMargin-1) >
                                                iCurrentMargin))) {
                        if(pCurrentListMarkup) {
                            pCurrentListMarkup = MakeNewMarkup("ol>",&pCurrentListMarkup);
                        }
                        else {
                            pCurrentListMarkup = (ListMarkup *)malloc(sizeof(ListMarkup));
                            InitListMarkup(pCurrentListMarkup);
                            strcpy(pCurrentListMarkup->zOpenList,"ol>");
                        }
                    }
                    /* Remember that TextStruct.iMargin was initialized to
                    ** 1 greater than its original value, so subtract 1 off for
                    ** the check.
                    */
                    if((pHTMLTree->u.TextStruct.iMargin-1) > iCurrentMargin){
                        /*
                        ** If we have indented from the previously-set margin,
                        ** then this means that we have a nested list.  In this case,
                        ** the HTML syntax is as follows:
                        **
                        ** If we're simply starting our list at an indented location
                        ** from the previous non-list text, then the markups would be:
                        ** <ul>
                        ** <ul>
                        ** <li>...
                        ** for a bullet list, for example.  The second <ul> will
                        ** indent us, and the lack of an <li> markup following the
                        ** first <ul> markup will ensure that a bullet with no
                        ** list item will not appear.
                        **
                        **If, on the other hand, we are indenting a list within
                        ** an existing list (ie:  we have a true nested list), then
                        ** the syntax will be:
                        **
                        ** <ul>
                        ** <li>
                        ** <li>..
                        ** <ul> -- start of nested list
                        ** <li>..
                        **
                        ** So in this case, each <ul> is followed by at least one
                        ** <li> markup.  The only way we can tell whether to include
                        ** an <li> after a <ul> (or <ol>) is to determine whether
                        ** this is the first list item or not. A first item is defined
                        ** as one that either:
                        **
                        ** 1.  Immediately follows a closed list, if that closed
                        **     list is not nested.  Eg:
                        **
                        **     <ul>
                        **      list...
                        **     <ol>
                        **      list...
                        **     </ol>
                        **     <li> -> This item is not a first item, even though it
                        **     follows a closed list. This is because the list that it
                        **     follows is nested within the overall list that this
                        **     item is apart of.
                        **
                        ** 2.  Immediately follows non-listed text.
                        **
                        ** So the beginning of a nested list does not constitute
                        ** a "first list item".
                        */
                        fprintf(Outfile,"<%s\n",pCurrentListMarkup->zOpenList);
                        iCurrentMargin = (pHTMLTree->u.TextStruct.iMargin-1);
                        iMarginIncrease = 1;
                    }
                    if(iIsFirstListItem) {
                        fprintf(Outfile,"<%s\n<li>\n",pCurrentListMarkup->zOpenList);
                        /* For the special case when we have two consecutive opening
                        ** markups (in order to accomplish an indent after non-listed
                        ** text), add an identical ListMarkup to the current linked
                        ** list.
                        */
                        if(iMarginIncrease) {
                            pCurrentListMarkup = MakeNewMarkup(
                                pCurrentListMarkup->zOpenList,
                                &pCurrentListMarkup);
                            iMarginIncrease = 0;
                        }
                        /* Save the list type corresponding to this "first" item.
                        ** We will use this variable when we want to close out
                        ** this list, because the list type may have been lost,
                        ** depending on whether this list has nested lists or not.
                        */
                        iIsFirstListItem = 0;
                    }
                    else {
                        fprintf(Outfile,"<li>\n");
                    }
                }
                fprintf(Outfile,"<p>");
                iParagraph = 1;
#ifdef SPARTAN
                /* If we want a spartan output (no fancy color, font, etc..
                ** markups, then don't print out the default alignment "left".
                ** Only print out alignment markups if they deviate from the
                ** default.
                */
                if(align=="left"){
                    fprintf(Outfile,"<%s>",align.data());
                    iAlign = 1;
                }
#else
                fprintf(Outfile,"<%s>",align.data());  /* see next comment */
                iAlign = 1;
#endif
            }
            while(pFormat) {
                /* It's possible for a <TEXT> markup to have a Format structure
                ** whose members are all 0 (ie:  iFontWeight=0, iLength=0, etc...).
                ** This happens if we have: <TEXT></TEXT>, which kword uses to
                ** represent a blank line.  In this case, kword does not emit a
                ** <FORMAT> tag at all, so the Format structure associated with
                ** this markup is never populated (<FLOW>, however, is specified,
                ** so kword_xml2html considers the <TEXT> markup to be complete).
                ** When this happens, we still need to go ahead and print out the
                ** newline, so check for a null iLength and then manually load
                ** Scratch with a newline.
                **
                ** Also, in this case, don't print out an alignment.  It majorly
                ** screws up HTML.
                **
                ** Finally, use the <pre></pre> markup to capture the
                ** newlines.  For some reason, the insertion of blank lines
                ** in HTML seems strange.  To insert 1 full blank line, only a
                ** <p>\n</p> is needed.  However, multiple blank lines cannot
                ** be added by simply more <p>\n</p> markups.  Instead, each
                ** subsequent blank line must be added with a <p><pre>\n</pre></p>
                ** combination.  Make sure to take this into account when
                ** rendering newlines.
                */
                if(!pFormat->iLength) {
                    /* It may be that iLength is 0 because this is actually
                    ** an in-line image.  Check this out.
                    */
                    if(pFormat->iID==2) {/* format ID=2 for pictures */
                        fprintf(Outfile,"<%s>\n",align.data());
                        fprintf(Outfile,"<img src=%s>\n",pHTMLTree->u.TextStruct.zFileName);
                        fprintf(Outfile,"</%s>\n",align.data());
                    }
                    else {
                        /* We will have one and only one Format structure if we're just
                        ** dealing with a blank line.
                        */
                        Scratch="\n";
                        if(iBlankLine==1) {
                            iPutPre = 1;
                            fprintf(Outfile,"<pre>");
                        }
                        else {
                            iBlankLine = 1;
                        }
                    }
                }
                else {
                    iBlankLine = 0;
                    iPutPre = 0;
                    Scratch = QCString(&(pHTMLTree->u.TextStruct.zText[iPosition]),
                              pFormat->iLength+1);
                    Scratch.resize(pFormat->iLength+1); // make room for trailing 0
                    iPosition+=pFormat->iLength;
                    /* Note:  "iPosition" walks the current text fragment, while
                    ** "pFormat->iLength" indicates the length of the current
                    ** special format within the text fragment (eg:  italicized text).
                    */
                    Scratch[pFormat->iLength]='\0'; // seems this is necessary even with QCString
                }
                if(pFormat->iID!=2) {
                    if((pFormat->iFontItalic-1)>0) {
                        strcpy(zItalic,"<i>");
                        strcpy(zItalicEnd,"</i>");
                    }
                    else {
                        *zItalic='\0';
                        *zItalicEnd='\0';
                    }
                    if(pFormat->iFontWeight>=75) {
                        strcpy(zWeight,"<b>");
                        strcpy(zWeightEnd,"</b>");
                    }
                    else {
                        *zWeight='\0';
                        *zWeightEnd='\0';
                    }
                    switch(pFormat->iFontSize) {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        iSize = 1; /* Map font sizes 1 through 7 to HTML size 1 */
                        break;
                    case 8:
                    case 9:
                    case 10:
                    case 11:
                    case 12:
                        iSize = 2; /* Map font sizes 8 and 12 to HTML size 2 */
                        break;
                    case 13:
                    case 14:
                    case 15:
                    case 16:
                    case 17:
                        iSize = 3; /* Map font sizes 13 through 17 to HTML size 3 */
                        break;
                    case 18:
                    case 19:
                    case 20:
                    case 21:
                        iSize = 4; /* Map font sizes 18 through 21 to HTML size 4 */
                        break;
                    case 22:
                    case 23:
                    case 24:
                        iSize = 5; /* Map font size 22 through 24 to HTML size 5 */
                        break;
                    default:
                        iSize = 6; /* Map all other font sizes to HTML size 6 */
                        break;
                    }

                    /* If this is a header, make sure to print out the numerical
                    ** outline value before printing out the associated text.
                    ** Currently, kword supports 3 header levels.  The current levels
                    ** are represented by the variables iNumHead1s, iNumHead2s, and
                    ** iNumHead3s.  These variables are incremented or decremented
                    ** as necessary to reflect the current outline values in the
                    ** text.
                    */
                    if((pHTMLTree->u.TextStruct.iHeader1)||
                       (pHTMLTree->u.TextStruct.iHeader2)||
                       (pHTMLTree->u.TextStruct.iHeader3)) {
                        if(pHTMLTree->u.TextStruct.iHeader1) {
                            iNumHead1s++;
                            /* Set 2nd and 3rd level headers back to zero, since we've
                            ** started with a brand new level 1 header.
                            */
                            iNumHead2s = 0;
                            iNumHead3s = 0;
#ifdef SPARTAN
                            fprintf(Outfile,"<H1>%d.  %s</H1>\n",iNumHead1s,Scratch.data());
#else
                            fprintf(Outfile,"<H1>%s%s<FONT COLOR=#%.2x%.2x%.2x>%d.\n"
                                    "%s</FONT>%s%s</H1>\n",zItalic,zWeight,pFormat->iRed,
                                    pFormat->iGreen,pFormat->iBlue,iNumHead1s,Scratch.data(),
                                    zWeightEnd,zItalicEnd);
#endif
                        }
                        else if(pHTMLTree->u.TextStruct.iHeader2) {
                            iNumHead2s++;
                            /* Set 3rd level headers back to zero, since we've started
                            ** with a brand new level 3 header.
                            */
                            iNumHead3s = 0;
#ifdef SPARTAN
                            fprintf(Outfile,"<H2>%d.%d.%s</H2>\n",iNumHead1s,iNumHead2s,Scratch.data());
#else
                            fprintf(Outfile,"<H2>%s%s<FONT COLOR=#%.2x%.2x%.2x>%d.%d.\n"
                                    "%s</FONT>%s%s</H2>\n",zItalic,zWeight,
                                    pFormat->iRed,pFormat->iGreen,pFormat->iBlue,
                                    iNumHead1s,iNumHead2s,Scratch.data(),zWeightEnd,zItalicEnd);
#endif
                        }
                        else if(pHTMLTree->u.TextStruct.iHeader3) {
                            iNumHead3s++;
#ifdef SPARTAN
                            fprintf(Outfile,"<H3>%d.%d.%d. %s</H3>\n",iNumHead1s,iNumHead2s,
                                    iNumHead3s,Scratch.data());
#else
                            fprintf(Outfile,"<H3>%s%s<FONT COLOR=#%.2x%.2x%.2x>%d.%d.%d.\n"
                                    "%s</FONT>%s%s</H3>\n",zItalic,zWeight,
                                    pFormat->iRed,pFormat->iGreen,pFormat->iBlue,
                                    iNumHead1s,iNumHead2s,iNumHead3s,Scratch.data(),zWeightEnd,
                                    zItalicEnd);
#endif
                        }
                    }
                    else {
#ifdef SPARTAN
                        fprintf(Outfile,"%s\n",Scratch.data());
#else
                        fprintf(Outfile,"%s%s<FONT SIZE=%d COLOR=#%.2x%.2x%.2x>%s</FONT>%s%s\n",
                                zItalic,zWeight,iSize,pFormat->iRed,
                                pFormat->iGreen,pFormat->iBlue,Scratch.data(),zWeightEnd,
                                zItalicEnd);
#endif
                    }
                }
                pFormat = pFormat->pNext;
            } /* End of "while(pFormat)" */
            iPosition = 0;
            if(iPutPre) {
                fprintf(Outfile,"</pre>");
            }
            if(iParagraph) {
                fprintf(Outfile,"</p>\n");
                iParagraph = 0;
            }
            if(iAlign) {
                fprintf(Outfile,"</%s>\n",align.data());
                iAlign = 0;
            }

            if(pHTMLTree->pNext) {
                ListMarkup *pTemp;
                int iMarginChange = 0;
                pTemp = pCurrentListMarkup;
                /* If we have completed an indentation, close off the <ul> or
                ** <ol> that initiated that indentation.
                */
                if((pHTMLTree->pNext)->ObjectType==HTML_Text) {
                    if((pHTMLTree->pNext->u.TextStruct.iMargin-1) < iCurrentMargin) {
                        fprintf(Outfile,"</%s\n",pCurrentListMarkup->zOpenList);
                        pCurrentListMarkup = pCurrentListMarkup->pPrevious;
                        iMarginChange = 1;
                        free(pTemp);
                    }
                }
                /*
                ** We're using a linked list of list markups (eg:  <ol>) so that
                ** we can tell which one we have to print out an end markup (eg:
                ** </ol>) for.  At this stage in the game, we have checked to
                ** see if we have just completed a nested list (indicated by
                ** a margin moving left).  If we have, then we've already
                ** added the end markup for the corresponding list type, and
                ** we've deleted that entry in the linked list of currently opened
                ** lists.  So, that means we're pointing to the information for the
                ** previous entry in the linked markup list.
                **
                ** In this case (we've just finished a nested list), we can have
                ** several conditions:
                **
                ** 1)  The next item to process is either a bullet or numbered
                ** list.  If this is true and the previous item was a list of the
                ** opposite type, then close out the previous list. We've just
                ** started a new list.  eg:
                **
                ** 1
                ** 2
                ** 3
                **     .
                **     .
                **     .
                **        1
                **        2
                **        3
                **     1
                **     2
                **     3
                **
                ** In this case, the third numerical list will trigger the end of
                ** the bullet-list.
                **
                ** 2)  The next item to process is either a bullet or numbered
                ** list. If this is true and the previous list was a list of the
                ** same type, then do not close out the previous list.  Simply
                ** continue.
                **
                ** 3)  The next item to process is not a list.  In this case
                ** close out the current list.
                **
                ** Now, it's also possible that we haven't closed out a nested
                ** list.  Maybe the next thing to examine is at the same level
                ** as the current item we've just examined.  As it turns out,
                ** this case has the same conditions as above, and is handled in
                ** the same way.
                */
                if((pCurrentListMarkup)&&
                   ((pHTMLTree->pNext)->ObjectType==HTML_Text)) {
                    pTemp = pCurrentListMarkup;
                    if(((pHTMLTree->pNext->u.TextStruct.iMargin-1)==iCurrentMargin)||
                       (iMarginChange)){
                        if((pHTMLTree->pNext->u.TextStruct.iIsEnumerated)&&
                           (!(strcmp(pCurrentListMarkup->zOpenList,"ul>")))) {
                            fprintf(Outfile,"</%s\n",pCurrentListMarkup->zOpenList);
                            iIsFirstListItem = 1;
                            pCurrentListMarkup = pCurrentListMarkup->pPrevious;
                            free(pTemp);
                        }
                        else if((pHTMLTree->pNext->u.TextStruct.iIsBullet)&&
                                (!(strcmp(pCurrentListMarkup->zOpenList,"ol>")))) {
                            fprintf(Outfile,"</%s\n",pCurrentListMarkup->zOpenList);
                            iIsFirstListItem = 1;
                            pCurrentListMarkup = pCurrentListMarkup->pPrevious;
                            free(pTemp);
                        }
                        else if((!pHTMLTree->pNext->u.TextStruct.iIsBullet)&&
                                (!pHTMLTree->pNext->u.TextStruct.iIsEnumerated)) {
                            fprintf(Outfile,"</%s\n",pCurrentListMarkup->zOpenList);
                            iIsFirstListItem = 1;
                            pCurrentListMarkup = pCurrentListMarkup->pPrevious;
                            free(pTemp);
                        }
                    }
                }
                if(iMarginChange) {
                    iCurrentMargin = (pHTMLTree->pNext->u.TextStruct.iMargin-1);
                }
            }
            pHTMLTree = pHTMLTree->pNext;
            if(!pHTMLTree) {
                /* If the lists were not nested properly, (eg:
                ** 1.
                ** 2.
                **   .
                **   .
                **     1.
                ** 3.
                ** Note here that the bullet list and the second numeric list are
                ** not terminated properly) then we may run out of text before
                ** we've unwrapped the ListMarkup list.  If so, just delete
                ** the rest of the ListMarkup list.  When we finish,
                ** pCurrentListMarkup should be zero.
                */
                if(pCurrentListMarkup){
                    ListMarkup *pListMarkup;
                    pListMarkup = pCurrentListMarkup;
                    pCurrentListMarkup = pCurrentListMarkup->pPrevious;
                    while(pListMarkup) {
                        free(pListMarkup);
                        pListMarkup = pCurrentListMarkup;
                        if(pCurrentListMarkup) {
                            pCurrentListMarkup = pCurrentListMarkup->pPrevious;
                        }
                    }
                }
            }
            if(pHTMLTree&&(pHTMLTree->ObjectType!=HTML_Text)) {
                fprintf(Outfile,"</p>\n");
                return pHTMLTree;
            }
        }
        fprintf(Outfile,"</p>\n");
        return pHTMLTree;
    }
    else {
        pCurrentTableCell = &(pHTMLTree->u.Table);
        pCurrentTableText = pHTMLTree->u.Table.pTextAttributes;
        /* Print a couple of newlines before adding the table. */
        fprintf(Outfile,"<p>\n</p>");
        fprintf(Outfile,"<p><pre>\n</pre></p>");
        fprintf(Outfile,"<center>\n");
        fprintf(Outfile,"<TABLE BORDER=1>\n");
        fprintf(Outfile,"<TR>\n<TD>\n");
        while(pCurrentTableCell) {
            while(pCurrentTableText) {
                QCString align;
                switch(pCurrentTableText->sTextAttributes.iFontFlow-1) {
                case 0:
                    align="left";
                    break;
                case 1:
                    align="right";
                    break;
                case 2:
                    align="center";
                    break;
                default:
                    break;
                }
                /* It's inefficient to copy all of this code, I know, but the
                ** differences between execution for table obejcts and text objects
                ** is intricate.  To better ensure accuracy, I've completely divorced
                ** the two paths.
                */
                if(egObject!=HTML_Table) {
                    fprintf(Outfile,"Internal error.  This should be a table object\n");
                    exit(1);
                }
                if(pCurrentTableCell->iRowNum!=iCurrentRow) {
                    /* We've just gone to a new row */
                    iCurrentCol = 0;
                    iCurrentRow++;
                    fprintf(Outfile,"<TR>\n<TD>\n");
                }
                else if(pCurrentTableCell->iColumnNum != iCurrentCol){
                    /* We've just gone to a new column */
                    iCurrentCol++;
                    fprintf(Outfile,"<TD>\n");
                }
                pFormat = pCurrentTableText->sTextAttributes.pFormat;
                if(pFormat->iLength) {
                    if((pCurrentTableText->sTextAttributes.iIsBullet)||
                       (pCurrentTableText->sTextAttributes.
                        iIsEnumerated)) {
                        if((pCurrentTableText->sTextAttributes.iIsBullet)
                           &&(iIsFirstListItem||
                              ((pCurrentTableText->sTextAttributes.iMargin-1)
                               >iCurrentMargin))) {
                            if(pCurrentListMarkup) {
                                pCurrentListMarkup = MakeNewMarkup("ul>",&pCurrentListMarkup);
                            }
                            else {
                                pCurrentListMarkup = (ListMarkup *)malloc(sizeof (ListMarkup));
                                InitListMarkup(pCurrentListMarkup);
                                strcpy(pCurrentListMarkup->zOpenList,"ul>");
                            }
                        }
                        else if((pCurrentTableText->sTextAttributes.iIsEnumerated)&&
                                (iIsFirstListItem||
                                 ((pCurrentTableText->sTextAttributes.iMargin-1)
                                  >iCurrentMargin))) {
                            if(pCurrentListMarkup) {
                                pCurrentListMarkup = MakeNewMarkup("ol>",&pCurrentListMarkup);
                            }
                            else {
                                pCurrentListMarkup = (ListMarkup *)malloc(sizeof(ListMarkup));
                                InitListMarkup(pCurrentListMarkup);
                                strcpy(pCurrentListMarkup->zOpenList,"ol>");
                            }
                        }
                        /* Remember that TextStruct.iMargin was initialized to
                        ** 1 greater than its original value, so subtract 1 off for
                        ** the check.
                        */
                        if((pCurrentTableText->sTextAttributes.iMargin-1)
                           > iCurrentMargin){
                            /*
                            ** If we have indented from the previously-set margin,
                            ** then this means that we have a nested list.  In this case,
                            ** the HTML syntax is as follows:
                            **
                            ** If we're simply starting our list at an indented location
                            ** from the previous non-list text, then the markups would be:
                            ** <ul>
                            ** <ul>
                            ** <li>...
                            ** for a bullet list, for example.  The second <ul> will
                            ** indent us, and the lack of an <li> markup following the
                            ** first <ul> markup will ensure that a bullet with no
                            ** list item will not appear.
                            **
                            **If, on the other hand, we are indenting a list within
                            ** an existing list (ie:  we have a true nested list), then
                            ** the syntax will be:
                            **
                            ** <ul>
                            ** <li>
                            ** <li>..
                            ** <ul> -- start of nested list
                            ** <li>..
                            **
                            ** So in this case, each <ul> is followed by at least one
                            ** <li> markup.  The only way we can tell whether to include
                            ** an <li> after a <ul> (or <ol>) is to determine whether
                            ** this is the first list item or not. A first item is defined
                            ** as one that either:
                            **
                            ** 1.  Immediately follows a closed list, if that closed
                            **     list is not nested.  Eg:
                            **
                            **     <ul>
                            **      list...
                            **     <ol>
                            **      list...
                            **     </ol>
                            **     <li> -> This item is not a first item, even though it
                            **     follows a closed list. This is because the list that it
                            **     follows is nested within the overall list that this
                            **     item is apart of.
                            **
                            ** 2.  Immediately follows non-listed text.
                            **
                            ** So the beginning of a nested list does not constitute
                            ** a "first list item".
                            */
                            fprintf(Outfile,"<%s\n",pCurrentListMarkup->zOpenList);
                            iCurrentMargin = (pCurrentTableText->sTextAttributes.iMargin-1);
                            iMarginIncrease = 1;
                        }
                        if(iIsFirstListItem) {
                            fprintf(Outfile,"<%s\n<li>\n",pCurrentListMarkup->zOpenList);
                            /* For the special case when we have two consecutive opening
                            ** markups (in order to accomplish an indent after non-listed
                            ** text), add an identical ListMarkup to the current linked
                            ** list.
                            */
                            if(iMarginIncrease) {
                                pCurrentListMarkup = MakeNewMarkup(
                                    pCurrentListMarkup->zOpenList,
                                    &pCurrentListMarkup);
                                iMarginIncrease = 0;
                            }
                            /* Save the list type corresponding to this "first" item.
                            ** We will use this variable when we want to close out
                            ** this list, because the list type may have been lost,
                            ** depending on whether this list has nested lists or not.
                            */
                            iIsFirstListItem = 0;
                        }
                        else {
                            fprintf(Outfile,"<li>\n");
                        }
                    }
#ifdef SPARTAN
                    /* If we want a spartan output (no fancy color, font, etc..
                    ** markups, then don't print out the default alignment "left".
                    ** Only print out alignment markups if they deviate from the
                    ** default.
                    */
                    if(align=="left"){
                        fprintf(Outfile,"<%s>",align.data());
                        iAlign = 1;
                    }
#else
                    fprintf(Outfile,"<%s>",align.data());  /* see next comment */
                    iAlign = 1;
#endif
                }
                while(pFormat) {
                    /* It's possible for a <TEXT> markup to have a Format structure
                    ** whose members are all 0 (ie:  iFontWeight=0, iLength=0, etc...).
                    ** This happens if we have: <TEXT></TEXT>, which kword uses to
                    ** represent a blank line.  In this case, kword does not emit a
                    ** <FORMAT> tag at all, so the Format structure associated with
                    ** this markup is never populated (<FLOW>, however, is specified,
                    ** so kword_xml2html considers the <TEXT> markup to be complete).
                    ** When this happens, we still need to go ahead and print out the
                    ** newline, so check for a null iLength and then manually load
                    ** Scratch with a newline.
                    **
                    ** Also, in this case, don't print out an alignment.  It majorly
                    ** screws up HTML.
                    **
                    ** Finally, use the <pre></pre> markup to capture the
                    ** newlines.  For some reason, the insertion of blank lines
                    ** in HTML seems strange.  To insert 1 full blank line, only a
                    ** <p>\n</p> is needed.  However, multiple blank lines cannot
                    ** be added by simply more <p>\n</p> markups.  Instead, each
                    ** subsequent blank line must be added with a <p><pre>\n</pre></p>
                    ** combination.  Make sure to take this into account when
                    ** rendering newlines.
                    */
                    if(!pFormat->iLength) {
                        /* It may be that iLength is 0 because this is actually
                        ** an in-line image.  Check this out.
                        */
                        if(pFormat->iID==2) {/* format ID=2 for pictures */
                            printf("Images within tables are currently not allowed\n");
                            exit(1);
                        }
                        else {
                            /* We will have one and only one Format structure if we're just
                            ** dealing with a blank line.
                            */
                            Scratch="\n";
                            fprintf(Outfile,"<p>");
                            iParagraph = 1;
                        }
                    }
                    else {
                        iPutPre = 0;
                        Scratch=QCString(&(pCurrentTableText->
                                          sTextAttributes.zText[iPosition]),pFormat->iLength+1);
                        Scratch.resize(pFormat->iLength+1); // make room for trailing 0
                        iPosition+=pFormat->iLength;
                        /* Note:  "iPosition" walks the current text fragment, while
                        ** "pFormat->iLength" indicates the length of the current
                        ** special format within the text fragment (eg:  italicized text).
                        */
                        Scratch[pFormat->iLength]='\0'; // see previous instance
                    }
                    if(pFormat->iID!=2) {
                        if((pFormat->iFontItalic-1)>0) {
                            strcpy(zItalic,"<i>");
                            strcpy(zItalicEnd,"</i>");
                        }
                        else {
                            *zItalic='\0';
                            *zItalicEnd='\0';
                        }
                        if(pFormat->iFontWeight>=75) {
                            strcpy(zWeight,"<b>");
                            strcpy(zWeightEnd,"</b>");
                        }
                        else {
                            *zWeight='\0';
                            *zWeightEnd='\0';
                        }
                        switch(pFormat->iFontSize) {
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        case 6:
                        case 7:
                            iSize = 1; /* Map font sizes 1 through 7 to HTML size 1 */
                            break;
                        case 8:
                        case 9:
                        case 10:
                        case 11:
                        case 12:
                            iSize = 2; /* Map font sizes 8 and 12 to HTML size 2 */
                            break;
                        case 13:
                        case 14:
                        case 15:
                        case 16:
                        case 17:
                            iSize = 3; /* Map font sizes 13 through 17 to HTML size 3 */
                            break;
                        case 18:
                        case 19:
                        case 20:
                        case 21:
                            iSize = 4; /* Map font sizes 18 through 21 to HTML size 4 */
                            break;
                        case 22:
                        case 23:
                        case 24:
                            iSize = 5; /* Map font size 22 through 24 to HTML size 5 */
                            break;
                        default:
                            iSize = 6; /* Map all other font sizes to HTML size 6 */
                            break;
                        }

                        /* If this is a header, make sure to print out the numerical
                        ** outline value before printing out the associated text.
                        ** Currently, kword supports 3 header levels.  The current levels
                        ** are represented by the variables iNumHead1s, iNumHead2s, and
                        ** iNumHead3s.  These variables are incremented or decremented
                        ** as necessary to reflect the current outline values in the
                        ** text.
                        */
                        if((pCurrentTableText->sTextAttributes.iHeader1)||
                           (pCurrentTableText->sTextAttributes.iHeader2)||
                           (pCurrentTableText->sTextAttributes.iHeader3)){
                            if(pCurrentTableText->sTextAttributes.iHeader1){
                                iNumHead1s++;
                                /* Set 2nd and 3rd level headers back to zero, since we've
                                ** started with a brand new level 1 header.
                                */
                                iNumHead2s = 0;
                                iNumHead3s = 0;
#ifdef SPARTAN
                                fprintf(Outfile,"<H1>%d.  %s</H1>\n",iNumHead1s,Scratch.data());
#else
                                fprintf(Outfile,"<H1>%s%s<FONT COLOR=#%.2x%.2x%.2x>%d.\n"
                                        "%s</FONT>%s%s</H1>\n",zItalic,zWeight,pFormat->iRed,
                                        pFormat->iGreen,pFormat->iBlue,iNumHead1s,Scratch.data(),
                                        zWeightEnd,zItalicEnd);
#endif
                            }
                            else if(pCurrentTableText->sTextAttributes.iHeader2) {
                                iNumHead2s++;
                                /* Set 3rd level headers back to zero, since we've started
                                ** with a brand new level 3 header.
                                */
                                iNumHead3s = 0;
#ifdef SPARTAN
                                fprintf(Outfile,"<H2>%d.%d.%s</H2>\n",iNumHead1s,iNumHead2s,Scratch.data());
#else
                                fprintf(Outfile,"<H2>%s%s<FONT COLOR=#%.2x%.2x%.2x>%d.%d.\n"
                                        "%s</FONT>%s%s</H2>\n",zItalic,zWeight,
                                        pFormat->iRed,pFormat->iGreen,pFormat->iBlue,
                                        iNumHead1s,iNumHead2s,Scratch.data(),zWeightEnd,zItalicEnd);
#endif
                            }
                            else if(pCurrentTableText->sTextAttributes.iHeader3) {
                                iNumHead3s++;
#ifdef SPARTAN
                                fprintf(Outfile,"<H3>%d.%d.%d. %s</H3>\n",iNumHead1s,iNumHead2s,
                                        iNumHead3s,Scratch.data());
#else
                                fprintf(Outfile,"<H3>%s%s<FONT COLOR=#%.2x%.2x%.2x>%d.%d.%d.\n"
                                        "%s</FONT>%s%s</H3>\n",zItalic,zWeight,
                                        pFormat->iRed,pFormat->iGreen,pFormat->iBlue,
                                        iNumHead1s,iNumHead2s,iNumHead3s,Scratch.data(),zWeightEnd,
                                        zItalicEnd);
#endif
                            }
                        }
                        else {
#ifdef SPARTAN
                            fprintf(Outfile,"%s\n",Scratch.data());
#else
                            fprintf(Outfile,"%s%s<FONT SIZE=%d COLOR=#%.2x%.2x%.2x>%s</FONT>%s%s\n",
                                    zItalic,zWeight,iSize,pFormat->iRed,
                                    pFormat->iGreen,pFormat->iBlue,Scratch.data(),zWeightEnd,
                                    zItalicEnd);
#endif
                        }
                    }
                    pFormat = pFormat->pNext;
                } /* End of "while(pFormat)" */
                iPosition = 0;
                if(iPutPre) {
                    fprintf(Outfile,"</pre>");
                }
                if(iParagraph) {
                    fprintf(Outfile,"</p>\n");
                    iParagraph = 0;
                }
                if(iAlign) {
                    fprintf(Outfile,"</%s>\n",align.data());
                    iAlign = 0;
                }

                if(pCurrentTableText->pNext) {
                    ListMarkup *pTemp;
                    int iMarginChange = 0;
                    pTemp = pCurrentListMarkup;
                    /* If we have completed an indentation, close off the <ul> or
                    ** <ol> that initiated that indentation.
                    */
                    if((pCurrentTableText->pNext->sTextAttributes.iMargin-1)<
                       iCurrentMargin) {
                        fprintf(Outfile,"</%s\n",pCurrentListMarkup->zOpenList);
                        pCurrentListMarkup = pCurrentListMarkup->pPrevious;
                        iMarginChange = 1;
                        free(pTemp);
                    }
                    /*
                    ** We're using a linked list of list markups (eg:  <ol>) so that
                    ** we can tell which one we have to print out an end markup (eg:
                    ** </ol>) for.  At this stage in the game, we have checked to
                    ** see if we have just completed a nested list (indicated by
                    ** a margin moving left).  If we have, then we've already
                    ** added the end markup for the corresponding list type, and
                    ** we've deleted that entry in the linked list of currently opened
                    ** lists.  So, that means we're pointing to the information for the
                    ** previous entry in the linked markup list.
                    **
                    ** In this case (we've just finished a nested list), we can have
                    ** several conditions:
                    **
                    ** 1)  The next item to process is either a bullet or numbered
                    ** list.  If this is true and the previous item was a list of the
                    ** opposite type, then close out the previous list. We've just
                    ** started a new list.  eg:
                    **
                    ** 1
                    ** 2
                    ** 3
                    **     .
                    **     .
                    **     .
                    **        1
                    **        2
                    **        3
                    **     1
                    **     2
                    **     3
                    **
                    ** In this case, the third numerical list will trigger the end of
                    ** the bullet-list.
                    **
                    ** 2)  The next item to process is either a bullet or numbered
                    ** list. If this is true and the previous list was a list of the
                    ** same type, then do not close out the previous list.  Simply
                    ** continue.
                    **
                    ** 3)  The next item to process is not a list.  In this case
                    ** close out the current list.
                    **
                    ** Now, it's also possible that we haven't closed out a nested
                    ** list.  Maybe the next thing to examine is at the same level
                    ** as the current item we've just examined.  As it turns out,
                    ** this case has the same conditions as above, and is handled in
                    ** the same way.
                    */
                    if((pCurrentListMarkup)) {
                        pTemp = pCurrentListMarkup;
                        if(((pCurrentTableText->pNext->sTextAttributes.iMargin-1)==
                            iCurrentMargin)||(iMarginChange)){
                            if((pCurrentTableText->pNext->sTextAttributes.iIsEnumerated)&&
                               (!(strcmp(pCurrentListMarkup->zOpenList,"ul>")))) {
                                fprintf(Outfile,"</%s\n",pCurrentListMarkup->zOpenList);
                                iIsFirstListItem = 1;
                                pCurrentListMarkup = pCurrentListMarkup->pPrevious;
                                free(pTemp);
                            }
                            else if((pCurrentTableText->pNext->sTextAttributes.iIsBullet)&&
                                    (!(strcmp(pCurrentListMarkup->zOpenList,"ol>")))) {
                                fprintf(Outfile,"</%s\n",pCurrentListMarkup->zOpenList);
                                iIsFirstListItem = 1;
                                pCurrentListMarkup = pCurrentListMarkup->pPrevious;
                                free(pTemp);
                            }
                            else if((!pCurrentTableText->pNext->sTextAttributes.iIsBullet)&&
                                    (!pCurrentTableText->pNext->sTextAttributes.iIsEnumerated)) {
                                fprintf(Outfile,"</%s\n",pCurrentListMarkup->zOpenList);
                                iIsFirstListItem = 1;
                                pCurrentListMarkup = pCurrentListMarkup->pPrevious;
                                free(pTemp);
                            }
                        }
                    }
                    if(iMarginChange) {
                        iCurrentMargin=(pCurrentTableText->pNext->
                                        sTextAttributes.iMargin-1);
                    }
                }
                pCurrentTableText = pCurrentTableText->pNext;
                if(!pCurrentTableText) {
                    /* If the lists were not nested properly, (eg:
                    ** 1.
                    ** 2.
                    **   .
                    **   .
                    **     1.
                    ** 3.
                    ** Note here that the bullet list and the second numeric list are
                    ** not terminated properly) then we may run out of text before
                    ** we've unwrapped the ListMarkup list.  If so, just delete
                    ** the rest of the ListMarkup list.  When we finish,
                    ** pCurrentListMarkup should be 0.
                    */
                    if(pCurrentListMarkup){
                        ListMarkup *pListMarkup;
                        pListMarkup = pCurrentListMarkup;
                        pCurrentListMarkup = pCurrentListMarkup->pPrevious;
                        while(pListMarkup) {
                            free(pListMarkup);
                            pListMarkup = pCurrentListMarkup;
                            if(pCurrentListMarkup) {
                                pCurrentListMarkup = pCurrentListMarkup->pPrevious;
                            }
                        }
                    }
                }
            }
            /* The column will change.  The row may, as well. */
            fprintf(Outfile,"</TD>\n");
            if((pCurrentTableCell->pNext)&&
               (pCurrentTableCell->pNext->iRowNum!=iCurrentRow)) {
                fprintf(Outfile,"</TR>\n");
            }
            pCurrentTableCell = pCurrentTableCell->pNext;
            if(pCurrentTableCell) {
                pCurrentTableText = pCurrentTableCell->pTextAttributes;
            }
            /* Here's a bit of a kludge:
            **
            ** It may be that the last cell of our last table will have an extra
            ** text specification in its text specification list.  Recall that, in
            ** the "SearchText" function, we always removed this extra text
            ** specification when we came to a new table cell, or when we were pre-
            ** paring to leave a table altogether (to handle the last cell in the
            ** table).  The problem is, this code is only executed if a FRAMESET
            ** markup is encountered.  This is fine in most cases, since everything
            ** is included in a frame (text, pictures, tables, etc...).  But what
            ** if the table is the last frame we encounter?  In that case, the last
            ** cell will still have the extra text specification.  Make sure to
            ** delete it now.
            */
            if((pCurrentTableCell)&&(!pCurrentTableCell->pNext)) {
                tTableText *pTableText;
                pTableText = pCurrentTableCell->pTextAttributes;
                pTableText = GetTableText(pTableText);
                /* If iMargin is still 0, we know that this specification has never
                ** been populated.  Delete it.
                */
                if(!pTableText->sTextAttributes.iMargin) {
                    pTableText->pPrevious->pNext = 0;
                    free(pTableText);
                }
            }
        }
        fprintf(Outfile,"</TR>\n");
        fprintf(Outfile,"</TABLE>\n");
        fprintf(Outfile,"</center>\n");
        /* Print a couple of newlines after the table. */
        fprintf(Outfile,"<p>\n</p>");
        fprintf(Outfile,"<p><pre>\n</pre></p>");
    }
    return (pHTMLTree->pNext);
}

HTMLTree *ProcessImage(HTMLTree *pHTMLTree,HTMLObject /*egObject*/,FILE *Outfile) {
    /* We've got a picture in a frame.  For now, default to center
    ** alignment.
    */
    if(pHTMLTree->ObjectType!=HTML_Image) {
        fprintf(Outfile,"Internal error.  This object should be an image\n");
        exit(1);
    }
    fprintf(Outfile,"<center>\n");
    fprintf(Outfile,"<img src=%s height=%d width=%d>\n",
            pHTMLTree->u.Image.zFileName,
            (pHTMLTree->u.Image.iBottom-pHTMLTree->u.Image.iTop),
            (pHTMLTree->u.Image.iRight-pHTMLTree->u.Image.iLeft));
    fprintf(Outfile,"</center>\n");
    pHTMLTree = pHTMLTree->pNext;
    return pHTMLTree;
}

/* Main Function.
**
*/
void mainFunc( const char *data, const char *charset ) {
  // ListMarkup *pCurrentListMarkup = 0;
    // tTextStruct sTextStruct;
    // int iMarginIncrease = 0;
    /*#ifdef SPARTAN
        const char *Usage="Usage:  xml2html_sp file1 <file2...fileN> output file1\n"
          "        <output file2...output fileN>\n";
    #else
        const char *Usage="Usage:  xml2html file1 <file2...fileN> output file1\n"
          "        <output file2...output fileN>\n";
      #endif*/
    const char *zXmlFile;
    FILE *OutputFile;
    HTMLTree *pHTMLTree;
    HTMLTree *pSaveHTMLTree;
    Token *pToken;
    int j = 0;
    pHTMLTree = (HTMLTree *)malloc(sizeof(HTMLTree));
    pSaveHTMLTree = pHTMLTree;
    InitHTMLTree(pHTMLTree);

    zXmlFile = data;
    if(!zXmlFile)
        return;

    OutputFile = fopen( "/tmp/kword2html" , "w" );
    fprintf(OutputFile,"<HTML>\n");
    fprintf(OutputFile,"<HEAD>\n");
    fprintf(OutputFile,"<META HTTP-EQUIV=\"Content-Type\" content=\"text/html; charset=%s\">\n", charset);
    fprintf(OutputFile,"</HEAD>\n");
#ifndef SPARTAN
    fprintf(OutputFile,"<body bgcolor=white>\n");
#endif
    kdDebug(30503) << "Calling ParseXml" << endl;

    /* Since the entire XML content that we are parsing is contained within
    ** one giant <DOC>......</DOC> markup, we only need to call the XML
    ** parser once (the parser will return after it encounters a completion
    ** of the first encountered markup, and will continue to call itself
    ** recursively until it does).
    */
    pToken = ParseXml(zXmlFile,&j);

    // Debug
    //PrintXml( pToken, 0 );

    SearchText(pToken,pHTMLTree);

    while((pHTMLTree)&&(pHTMLTree->ObjectType!=HTML_Null)) {
        if((pHTMLTree->ObjectType==HTML_Text)||
           (pHTMLTree->ObjectType==HTML_Table)) {
            pHTMLTree=ProcessTableAndText(pHTMLTree,pHTMLTree->ObjectType,
                                          OutputFile);
        }
        else if(pHTMLTree->ObjectType==HTML_Image) {
            pHTMLTree=ProcessImage(pHTMLTree,pHTMLTree->ObjectType,OutputFile);
        }
    }
    free(pSaveHTMLTree);
    pHTMLTree = (HTMLTree *)malloc(sizeof(HTMLTree));
    pSaveHTMLTree = pHTMLTree;
    InitHTMLTree(pHTMLTree);
    fprintf(OutputFile,"</HTML>\n");
    fclose(OutputFile);
    j = 0;
}
