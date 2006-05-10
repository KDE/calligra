/* This file is part of the KDE project
   Copyright (C) 2002 Robert JACOLIN <rjacolin@ifrance.com>

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
   This file has been made from xsltproc.c from libxslt.
*/

/*
 * xsltproc.c: user program for the XSL Transformation 1.0 engine
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */
#include "xsltproc.h"

#include <QFile>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <libxml/debugXML.h>
#include <libxml/xmlIO.h>
#include <libxml/parserInternals.h>

#include <libexslt/exsltconfig.h>

#ifdef WIN32
#ifdef _MSC_VER
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define gettimeofday(p1,p2)
#endif /* _MS_VER */
#else /* WIN32 */
#endif /* WIN32 */

#ifndef HAVE_STAT
#  ifdef HAVE__STAT
     /* MS C library seems to define stat and _stat. The definition
      *         is identical. Still, mapping them to each other causes a warning. */
#    ifndef _MSC_VER
#      define stat(x,y) _stat(x,y)
#    endif
#    define HAVE_STAT
#  endif
#endif

xmlParserInputPtr xmlNoNetExternalEntityLoader(const char *URL,
	                                       const char *ID,
					       xmlParserCtxtPtr ctxt);

#if 0
XSLTProc::XSLTProc(const char* fileIn, const char* fileOut, const char* xsltsheet)
{
	_fileIn = fileIn;
	_fileOut = fileOut;
	_stylesheet = xsltsheet;
	nbparams = 0;
	debug = 0;
	repeat = 0;
	novalid = 0;
	output = NULL;
}
#endif

XSLTProc::XSLTProc(QString fileIn, QString fileOut, QString xsltsheet)
{
	_fileIn = QFile::encodeName(fileIn);
	_fileOut = QFile::encodeName(fileOut);
	_stylesheet = QFile::encodeName(xsltsheet);
	nbparams = 0;
	debug = 0;
	repeat = 0;
	novalid = 0;
	output = NULL;
}

/*static int debug = 0;
static int repeat = 0;
static int novalid = 0;*/

/*const char *XSLTProc::params[16 + 1];
int XSLTProc::nbparams = 0;
static const char *output = NULL;*/

void
XSLTProc::xsltProcess(xmlDocPtr doc, xsltStylesheetPtr cur, const char *filename)
{
    xmlDocPtr res;

    if (output == NULL)
	{
		
		if (repeat)
		{
		    int j;

		    for (j = 1; j < repeat; j++)
			{
				res = xsltApplyStylesheet(cur, doc, params);
				xmlFreeDoc(res);
				xmlFreeDoc(doc);
			    doc = xmlParseFile(filename);
		    }
		}
	    res = xsltApplyStylesheet(cur, doc, params);
		xmlFreeDoc(doc);
	
		if (res == NULL)
		{
		    fprintf(stderr, "no result for %s\n", filename);
		    return;
		}
#ifdef LIBXML_DEBUG_ENABLED
		if (debug)
	    	xmlDebugDumpDocument(stdout, res);
		else
		{
#endif
	    	if (cur->methodURI == NULL)
			{
				xsltSaveResultToFile(stdout, res, cur);
		    }
			else
			{
				if (xmlStrEqual
				    (cur->method, (const xmlChar *) "xhtml"))
				{
				    fprintf(stderr, "non standard output xhtml\n");
				    xsltSaveResultToFile(stdout, res, cur);
				}
				else
				{
				    fprintf(stderr,
						    "Unsupported non standard output %s\n",
						    cur->method);
				}
	    	}
#ifdef LIBXML_DEBUG_ENABLED
		}
#endif
		xmlFreeDoc(res);
    }
	else
	{
		xsltRunStylesheet(cur, doc, params, output, NULL, NULL);
		xmlFreeDoc(doc);
    }
}

/*static void usage(const char *name) {
    printf("Usage: %s [options] stylesheet file [file ...]\n", name);
    printf("   Options:\n");
    printf("      --version or -V: show the version of libxml and libxslt used\n");
    printf("      --verbose or -v: show logs of what's happening\n");
    printf("      --output file or -o file: save to a given file\n");
    printf("      --timing: display the time used\n");
    printf("      --repeat: run the transformation 20 times\n");
    printf("      --debug: dump the tree of the result instead\n");
    printf("      --novalid: skip the Dtd loading phase\n");
    printf("      --noout: do not dump the result\n");
    printf("      --maxdepth val : increase the maximum depth\n");
#ifdef LIBXML_HTML_ENABLED
    printf("      --html: the input document is(are) an HTML file(s)\n");
#endif
#ifdef LIBXML_DOCB_ENABLED
    printf("      --docbook: the input document is SGML docbook\n");
#endif
    printf("      --param name value : pass a (parameter,value) pair\n");
    printf("            string values must be quoted like \"'string'\"\n");
    printf("      --nonet refuse to fetch DTDs or entities over network\n");
#ifdef LIBXML_CATALOG_ENABLED
    printf("      --catalogs : use the catalogs from $SGML_CATALOG_FILES\n");
#endif
#ifdef LIBXML_XINCLUDE_ENABLED
    printf("      --xinclude : do XInclude processing on document intput\n");
#endif
    printf("      --profile or --norman : dump profiling informations \n");
}*/

void XSLTProc::addParam(QString name, QString value)
{
	if(nbparams < NB_PARAMETER_MAX)
	{
		params[nbparams] = strdup(name.latin1());
		params[nbparams + 1] = strdup(value.latin1());
		fprintf(stderr, "%s => ", params[0]);
		fprintf(stderr, "%s\n", params[1]);
		nbparams = nbparams + 2;
	}
}

int XSLTProc::parse()
{
    int i = 0;
    xsltStylesheetPtr cur = NULL;
    xmlDocPtr doc, style;

    xmlInitMemory();

    LIBXML_TEST_VERSION

    //xmlLineNumbersDefault(1);

    if (novalid == 0)
        xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
    else
        xmlLoadExtDtdDefaultValue = 0;

	/* Options */
	debug = debug + 1;
	novalid = novalid + 1;
	output = _fileOut;
	repeat = 20;
	xsltMaxDepth = 5;
    params[nbparams] = NULL;

    /*
     * Replace entities with their content.
     */
    xmlSubstituteEntitiesDefault(1);

    /*
     * Register the EXSLT extensions and the test module
     */
    //exsltRegisterAll();
    xsltRegisterTestModule();

    
   	style = xmlParseFile((const char *) _stylesheet);
  	if (style == NULL)
	{
		fprintf(stderr,  "cannot parse %s\n", _stylesheet.data());
		cur = NULL;
	}
	else
	{
		cur = xsltLoadStylesheetPI(style);
		if (cur != NULL)
		{
		    /* it is an embedded stylesheet */
		    xsltProcess(style, cur, _stylesheet);
		    xsltFreeStylesheet(cur);
		    goto done;
		}
		cur = xsltParseStylesheetDoc(style);
		if (cur != NULL)
		{
		    if (cur->indent == 1)
				xmlIndentTreeOutput = 1;
		    else
				xmlIndentTreeOutput = 0;
			i++;
		}
		else
		{
		    xmlFreeDoc(style);
		    goto done;
		}
	}
   
    /*
     * disable CDATA from being built in the document tree
     */
    xmlDefaultSAXHandlerInit();
    xmlDefaultSAXHandler.cdataBlock = NULL;

    if ((cur != NULL) && (cur->errors == 0))
	{
		doc = NULL;
        doc = xmlParseFile(_fileIn);
        if (doc == NULL)
			fprintf(stderr, "unable to parse %s\n", _fileIn.data());
		else
			xsltProcess(doc, cur, _fileIn);
    }
    if (cur != NULL)
		xsltFreeStylesheet(cur);
done:
    xsltCleanupGlobals();
    xmlCleanupParser();
    xmlMemoryDump();
    return (0);
}

