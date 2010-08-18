<?xml version="1.0" encoding="UTF-8"?>
<!--
This file is part of the KOffice project
   Copyright (C) 2010 Pramod S G <pramod.xyle@gmail.com>
   Copyright (C) 2010 Srihari Prasad G V <sri-hari@live.com>
   
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
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    version="2.0">


    <xsl:param xmlns:dom="http://www.w3.org/2001/xml-events" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" name="param_no_css"/>
    <xsl:param xmlns:dom="http://www.w3.org/2001/xml-events" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" name="param_css_only"/>
    <xsl:param xmlns:dom="http://www.w3.org/2001/xml-events" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" name="scale">1</xsl:param>
    <xsl:param xmlns:dom="http://www.w3.org/2001/xml-events" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" name="style.background-color">#A0A0A0</xsl:param>
    <xsl:param xmlns:dom="http://www.w3.org/2001/xml-events" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" name="CSS.debug">0</xsl:param>
    <xsl:variable xmlns:dom="http://www.w3.org/2001/xml-events" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" name="linebreak">    
    </xsl:variable>


    <xsl:template match="office:document">
    <xsl:apply-templates/>   
    </xsl:template>

    <xsl:template match="/office:document">
    <html>
        <head>
            <!-- meta must be first -->
            <xsl:apply-templates select="office:document-meta"/>
            <!-- must be second -->
            <style>
            <xsl:apply-templates select="office:document-styles"/>
            </style>
        </head>
        <!-- body must be after head -->
        <body>
            <xsl:apply-templates select="office:document-content"/>
        </body>
    </html>
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
    <xsl:template match="dc:title">
        <title><xsl:apply-templates/></title>
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
                position: absolute;
            }
            body
            {
                position: absolute;
            }
        }
        table
        {
            border: thin solid gray;
            border-collapse: collapse;
            empty-cells: show;
            font-size: 10pt;
            table-layout: fixed;
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
        td p
        {
            max-height: 2.5ex;
            overflow: hidden;
        }
        td p:hover
        {
            max-height: none;
        }
        p
        {
            margin-top: 0;
            margin-bottom: 0;
        }
        .page-break
        {
            margin: 1em;
        }
    
   
    </xsl:template>
    
    <xsl:template match="office:document-styles">
        <!-- office:document-styles begin -->
        
        <xsl:call-template name="office:document-styles">
        </xsl:call-template>
        <xsl:apply-templates></xsl:apply-templates>    
            
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

    <xsl:template match="office:master-styles">
        <xsl:text><!-- office:master-styles begin --></xsl:text>
        <xsl:apply-templates/>
        <xsl:text><!-- office:master-styles end --></xsl:text>
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
    
  <xsl:template match="text:p">
  <p><xsl:apply-templates/></p>
  </xsl:template>

  <xsl:template match="text:span[@text:style-name='T1']">
  <b><xsl:apply-templates/></b>
  </xsl:template>

  <xsl:template match="table:table">
  <table border = "1">
 
      <xsl:for-each select="table:table-row">
      <tr>
      <xsl:for-each select="table:table-cell">
      <td>
      <xsl:value-of select="text:p"/>
      </td>
      </xsl:for-each>
      </tr>
      </xsl:for-each>

  </table>
  </xsl:template>
  
</xsl:stylesheet>



 
