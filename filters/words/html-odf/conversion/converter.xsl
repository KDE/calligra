<?xml version="1.0" encoding="UTF-8"?>
<!--
This file is part of the Calligra project
   Copyright (C) 2010 Pramod S G <pramod.xyle@gmail.com>
   Copyright (C) 2010 Srihari Prasad G V <sri-hari@live.com>
   Copyright (C) 2011 Stuart Dickson <stuart@kogmbh.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
-->
<xsl:stylesheet xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0"
    xmlns:meta="urn:oasis:names:tc:opendocument:xmlns:meta:1.0"
    xmlns:config="urn:oasis:names:tc:opendocument:xmlns:config:1.0"
    xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"
    xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0"
    xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0"
    xmlns:presentation="urn:oasis:names:tc:opendocument:xmlns:presentation:1.0"
    xmlns:dr3d="urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0"
    xmlns:chart="urn:oasis:names:tc:opendocument:xmlns:chart:1.0"
    xmlns:form="urn:oasis:names:tc:opendocument:xmlns:form:1.0"
    xmlns:script="urn:oasis:names:tc:opendocument:xmlns:script:1.0"
    xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0"
    xmlns:number="urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0"
    xmlns:anim="urn:oasis:names:tc:opendocument:xmlns:animation:1.0"
    xmlns:dc="http://purl.org/dc/elements/1.1/"
    xmlns:xlink="http://www.w3.org/1999/xlink"
    xmlns:math="http://www.w3.org/1998/Math/MathML"
    xmlns:xforms="http://www.w3.org/2002/xforms"
    xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"
    xmlns:svg="urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0"
    xmlns:smil="urn:oasis:names:tc:opendocument:xmlns:smil-compatible:1.0"
    xmlns:ooo="http://openoffice.org/2004/office"
    xmlns:ooow="http://openoffice.org/2004/writer"
    xmlns:oooc="http://openoffice.org/2004/calc"
    xmlns:int="http://opendocumentfellowship.org/internal"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    version="2.0">

    <xsl:param xmlns:dom="http://www.w3.org/2001/xml-events" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" name="param_no_css"/>
    <xsl:param xmlns:dom="http://www.w3.org/2001/xml-events" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" name="param_css_only"/>
    <xsl:param xmlns:dom="http://www.w3.org/2001/xml-events" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" name="scale">1</xsl:param>
    <xsl:param xmlns:dom="http://www.w3.org/2001/xml-events" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" name="style.background-color">#F0F0F0</xsl:param>
    <xsl:param xmlns:dom="http://www.w3.org/2001/xml-events" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" name="CSS.debug">0</xsl:param>
    <xsl:variable xmlns:dom="http://www.w3.org/2001/xml-events" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" name="linebreak">&#10;</xsl:variable>


    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes"/>


    <xsl:template match="office:document">
    <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="/office:document">
    <xsl:element name="html">
        <xsl:element name="head">
            <!-- meta must be first -->
            <xsl:apply-templates select="office:document-meta"/>
            <!-- must be second -->
            <xsl:element name="style">
                <xsl:apply-templates select="office:document-styles"/>
            </xsl:element>
        </xsl:element>
        <!-- body must be after head -->
        <xsl:element name="body">
            <xsl:apply-templates select="office:document-styles/office:master-styles/style:master-page/style:header"/>
            <xsl:apply-templates select="office:document-content"/>
            <xsl:apply-templates select="office:document-styles/office:master-styles/style:master-page/style:footer"/>
        </xsl:element>
    </xsl:element>
    </xsl:template>

    <xsl:template match="office:document-meta">
        <xsl:apply-templates/>
    </xsl:template>
    <xsl:template match="office:meta">
        <xsl:comment>office:metadata begin</xsl:comment>
        <xsl:apply-templates select="dc:title"/>
        <xsl:apply-templates select="dc:creator"/>
        <xsl:apply-templates select="dc:date"/>
        <xsl:apply-templates select="dc:language"/>
        <xsl:apply-templates select="dc:description"/>
        <xsl:apply-templates select="meta:keyword"/>
        <xsl:apply-templates select="meta:generator"/>
        <meta http-equiv="Content-Type" content="application/xhtml+xml;charset=utf-8"/>
        <xsl:comment>office:metadata end</xsl:comment>
    </xsl:template>

    <!-- Without care, the XSLT would output <title/> for an empty title, which is incorrectly
        parsed by many modern browsers.
        To ensure that some value for title is defined, if it is not set in the document,
        we write the filename into the title.
        -->
    <xsl:template match="dc:title">
        <xsl:element name="title">
            <xsl:choose>
                <xsl:when test="current()!=''">
                    <xsl:apply-templates/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$html-odf-fileName"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>

    <xsl:template match="dc:language">
        <meta http-equiv="content-language" content="{current()}"/>
    </xsl:template>
    <xsl:template match="dc:creator">
        <meta name="author" content="{current()}"/>
        <meta name="DC.creator" content="{current()}"/>
    </xsl:template>
    <xsl:template match="dc:description">
        <meta name="description" content="{current()}"/>
    </xsl:template>
    <xsl:template match="dc:date">
        <meta name="revised" content="{current()}"/>
        <meta name="DC.date" content="{current()}"/>
    </xsl:template>
    <xsl:template match="meta:keyword">
        <meta name="keywords" content="{current()}"/>
    </xsl:template>
    <xsl:template match="meta:generator">
        <meta name="generator" content="{current()}"/>
    </xsl:template>


    <xsl:template name="office:document-styles">
        html
        {
            font-family: Verdana, SunSans-Regular, Sans-Serif;
            font-size: <xsl:value-of select="$scale * 14"/>pt;
        }
        @media print
        {
            html
            {
            }
        }
        @media screen
        {
            html
            {
                background-color: <xsl:value-of select="$style.background-color"/>;
                margin: 1.5em;
            }
            body
            {
                max-width: 40em;
                margin:0 auto;  /* center document content */
            }
        }
        table
        {
            border: thin solid gray;
            border-collapse: collapse;
            empty-cells: show;
        }
        td
        {
            border: thin solid gray;
            vertical-align: bottom;
        }
        .cell_string
        {
            text-align: left;
        }
        .cell_time
        {
            text-align: right;
        }

        td p:hover
        {
            max-height: none;
        }

        .page-break
        {
            margin: 1em;
        }
    </xsl:template>

    <xsl:template match="office:document-styles">
        <!-- office:document-styles begin -->

        <xsl:call-template name="office:document-styles" />
        <xsl:apply-templates/>

        <!-- office:document-styles end -->
    </xsl:template>

    <xsl:template match="office:styles">
        <xsl:text><!-- office:styles begin --></xsl:text>
        <xsl:apply-templates />
        <xsl:text><!-- office:styles end --></xsl:text>
    </xsl:template>

    <xsl:template match="office:automatic-styles">
        <xsl:text><!-- office:automatic-styles begin1 --></xsl:text>
        <xsl:apply-templates/>
        <xsl:text><!-- office:automatic-styles end --></xsl:text>
    </xsl:template>

    <xsl:template match="style:default-style">
            <xsl:text>p{</xsl:text>
            <xsl:text>}</xsl:text>
    </xsl:template>

    <xsl:template match="office:styles/style:style">
        <xsl:text>.</xsl:text><xsl:value-of select="@style:family"></xsl:value-of><xsl:text>_</xsl:text>
        <xsl:value-of select="@style:name"></xsl:value-of>
        <xsl:text>
                {</xsl:text>
                <xsl:apply-templates/>
        <xsl:text>}
        </xsl:text>
    </xsl:template>

    <xsl:template match="office:styles/style:paragraph-properties">
        <xsl:text> padding-top:</xsl:text>
        <xsl:value-of select="@fo:padding-top"></xsl:value-of><xsl:text>; </xsl:text>
        <xsl:text> font-size:</xsl:text>
        <xsl:value-of select="@fo:font-size"></xsl:value-of><xsl:text>; </xsl:text>
    </xsl:template>
    <xsl:template match="office:styles/style:text-properties">
        <xsl:text> font-size:</xsl:text>
        <xsl:value-of select="@fo:font-size"></xsl:value-of><xsl:text>; </xsl:text>
        <xsl:text> font-weight:</xsl:text>
        <xsl:value-of select="@fo:font-weight"></xsl:value-of><xsl:text>; </xsl:text>
        <xsl:text> font-family:</xsl:text>
        <xsl:value-of select="@fo:font-family"></xsl:value-of><xsl:text>; </xsl:text>
        <xsl:text> font-style:</xsl:text>
        <xsl:value-of select="@fo:font-style"></xsl:value-of><xsl:text>; </xsl:text>
    </xsl:template>

    <xsl:template match="office:document-content">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="office:body">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="office:text">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="text:p">
        <xsl:element name="p">
            <xsl:apply-templates/>
        </xsl:element>
    </xsl:template>

    <xsl:template match="text:span[@text:style-name='T1']">
        <xsl:element name="strong">
            <xsl:apply-templates/>
        </xsl:element>
    </xsl:template>

    <xsl:template match="text:span">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="table:table-columns">
        <xsl:apply-templates />
    </xsl:template>

    <!-- table headers need to be processed slightly differtly in order to output <th> elements
        instead of <td> elements. As both <table-header-rows> and <table-row> contain
        <table-cell> elements, we need to set a "table_header" mode to apply the appropriate
        template
    -->
    <xsl:template match="table:table-header-rows">
        <xsl:element name="thead">
            <xsl:for-each select="table:table-row">
                <xsl:element name="tr">
                    <xsl:apply-templates mode="table_header" />
                </xsl:element>
            </xsl:for-each>
        </xsl:element>
    </xsl:template>

    <!-- Regular table row -->
    <xsl:template match="table:table-row">
        <xsl:element name="tr">
            <xsl:apply-templates />
        </xsl:element>
    </xsl:template>

    <!-- Table cell inside a table header -->
    <xsl:template match="table:table-cell" mode="table_header">
        <xsl:element name="th">
        <xsl:if test="string(@table:number-columns-spanned)">
            <xsl:attribute name="colspan">
                <xsl:value-of select="@table:number-columns-spanned" />
            </xsl:attribute>
        </xsl:if>
        <xsl:if test="string(@table:number-rows-spanned)">
            <xsl:attribute name="rowspan">
                <xsl:value-of select="@table:number-rows-spanned" />
            </xsl:attribute>
        </xsl:if>
        <xsl:apply-templates select="text:p"/>
        </xsl:element>
    </xsl:template>

    <!-- Regular table cell -->
    <xsl:template match="table:table-cell">
        <xsl:element name="td">
        <xsl:if test="string(@table:number-columns-spanned)">
            <xsl:attribute name="colspan">
                <xsl:value-of select="@table:number-columns-spanned" />
            </xsl:attribute>
        </xsl:if>
        <xsl:if test="string(@table:number-rows-spanned)">
            <xsl:attribute name="rowspan">
                <xsl:value-of select="@table:number-rows-spanned" />
            </xsl:attribute>
        </xsl:if>
        <xsl:apply-templates select="text:p"/>
        </xsl:element>
    </xsl:template>

    <xsl:template match="table:table">
        <table border="1">
        <xsl:value-of select="$linebreak"/>
            <xsl:apply-templates />
        </table>
    </xsl:template>

    <!-- Images -->
    <xsl:template match="draw:frame">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="draw:image">
        <img src="{$html-odf-resourcesPath}/{@xlink:href}"/>
    </xsl:template>


    <!--
        We may need to remove non-printing character sequence EF BF BC (0xfffc)
        This is a UTF-8 encoding for QChar::ObjectRepalcementMarker
    -->
    <xsl:template match="text()">
        <xsl:value-of select="replace(.,'\xEF\xBF\xBC','')"/>
    </xsl:template>


</xsl:stylesheet>




