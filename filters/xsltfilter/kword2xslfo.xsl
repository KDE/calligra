<?xml version="1.0"  encoding="UTF-8"?>

<!-- 	
   This file is part of the KDE project
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
   Boston, MA 02110-1301, USA.

   This stylesheet convert kword document in xsl:fo.

   -->
   
<xsl:stylesheet version = '1.0' 
     xmlns:xsl='http://www.w3.org/1999/XSL/Transform'
     xmlns:fo='http://www.w3.org/1999/XSL/Format'>

<xsl:output method="xml" encoding="UTF-8" indent="yes"/>
<xsl:param name="nbTotalPara" select="30"/>

<!-- DOC. (DEFINITIONS, ...) -->
<xsl:template match="DOC"> 
	<xsl:comment>
	<xsl:value-of select="@editor"/>
	</xsl:comment>
	<fo:root>
	<fo:layout-master-set>
        <fo:simple-page-master
		margin-right="0.5cm" margin-left="0.5cm" margin-bottom="0.5cm" margin-top="0.75cm"
		page-width="21cm" page-height="29.7cm" master-name="first">
            <fo:region-body></fo:region-body>
        </fo:simple-page-master>
        <fo:simple-page-master
		margin-right="2.5cm" margin-left="2.5cm" margin-bottom="2cm" margin-top="1cm"
		page-width="21cm" page-height="29.7cm" master-name="rest">
            <fo:region-body></fo:region-body>
        </fo:simple-page-master>
		
		<fo:page-sequence-master master-name="page">
			<fo:repeatable-page-master-alternatives>
				<fo:conditional-page-master-reference master-name="first"
					page-position="first" />
				<fo:conditional-page-master-reference master-name="rest"
					page-position="rest" />
				<!-- recommended fallback procedure -->
				<fo:conditional-page-master-reference master-name="rest" />
			</fo:repeatable-page-master-alternatives>
		</fo:page-sequence-master>
    </fo:layout-master-set>
	<xsl:apply-templates select="FRAMESETS"/> 
	</fo:root>
</xsl:template>

<!-- LE DOCUMENT LUI-MEME -->
<xsl:template match="FRAMESETS">
	<fo:page-sequence master-name="page">
			<xsl:if test="/DOC/ATTRIBUTES/@hasHeader='1'">
			<xsl:apply-templates select="FRAMESET[@frameInfo=1]"/>
			<xsl:apply-templates select="FRAMESET[@frameInfo=2]"/>
			<xsl:apply-templates select="FRAMESET[@frameInfo=3]"/>
		</xsl:if>

		<xsl:if test="/DOC/ATTRIBUTES/@hasFooter='1'">
			<xsl:apply-templates select="FRAMESET[@frameInfo=4]"/>
			<xsl:apply-templates select="FRAMESET[@frameInfo=5]"/>
			<xsl:apply-templates select="FRAMESET[@frameInfo=6]"/>
		</xsl:if>
		
		<xsl:apply-templates select="FRAMESET[@frameInfo=0]"/>
	</fo:page-sequence>

</xsl:template>

<!-- CADRES POUR CHAQUE PARTIE D'UNE PAGE -->
<xsl:template match="FRAMESET">
	
	<xsl:if test="@frameInfo=0">
		<fo:flow flow-name="xsl-region-body">
			<xsl:apply-templates select="FRAME"/>
			<xsl:call-template name="PARAS" select="PARAGRAPH">
				<xsl:with-param name="i">1</xsl:with-param>
				<xsl:with-param name="debutListInitial">0</xsl:with-param>
			    <xsl:with-param name="nbItemInitial">0</xsl:with-param>
			</xsl:call-template>
		</fo:flow>
	</xsl:if>

	<xsl:if test="@frameInfo=1">
		<fo:static-content flow-name="xsl-region-bottom">
			<xsl:apply-templates select="FRAME"/>
			<xsl:apply-templates name="PARAGS" select="PARAS"/>
		</fo:static-content>
	</xsl:if>
	
</xsl:template>

<!-- CADRE -->
<xsl:template match="FRAME">

</xsl:template>

<!-- LISTE DE PARAG. -->
<xsl:template name="PARAS">
	<xsl:param name="i">param_i</xsl:param>
	<xsl:param name="nbItemInitial">param_nbItem</xsl:param>
	<xsl:param name="debutListeInitial" select="param_debutListeInitial"/>
  
	<xsl:message>parag. <xsl:value-of select="$i"/></xsl:message>
	
	<!-- test si le paragraphe est une liste -->
	<xsl:choose>
		<xsl:when test="PARAGRAPH[position()=$i]/LAYOUT/COUNTER/@numberingtype=0">
			<!-- si le parag. est une liste on groupe -->
			<xsl:message>Une liste</xsl:message>
			<xsl:variable name="nbItem" select="$nbItemInitial + 1"/>
			<!-- Rappelle le template -->
			<xsl:if test="($i + 1) &lt;=$nbTotalPara">
				<xsl:message>parag. suivant </xsl:message>
				<xsl:call-template name="PARAS">
					<xsl:with-param name="debutListeInitial" select="$debutListeInitial"/>
					<xsl:with-param name="i"><xsl:value-of select="$i + 1"/></xsl:with-param>
					<xsl:with-param name="nbItemInitial" select="$nbItem"/>
				</xsl:call-template>
			</xsl:if>
		</xsl:when>
		<!-- si le parag. est normal, on le genere. -->
		<xsl:otherwise>
			<xsl:if test="$nbItemInitial &gt; 0">
				<fo:list-block>
					<xsl:variable name="startInit">
						<xsl:choose>
							<xsl:when test="PARAGRAPH[position()=($i - 1)]/LAYOUT/COUNTER/@start">
									<xsl:copy-of select="PARAGRAPH[position()=$i]/LAYOUT/COUNTER/@start"/>
							</xsl:when>
							<xsl:otherwise>
									<xsl:if test="PARAGRAPH[position()=($i - 1)]/LAYOUT/COUNTER/@type=1">
											<xsl:copy-of select="1"/>
									</xsl:if>
									<xsl:if test="PARAGRAPH[position()=$i]/LAYOUT/COUNTER/@type=2">
										<xsl:copy-of select="a"/>
									</xsl:if>
									<xsl:if test="PARAGRAPH[position()=$i]/LAYOUT/COUNTER/@type=3">
										<xsl:copy-of select="A"/>
									</xsl:if>
									<xsl:if test="PARAGRAPH[position()=$i]/LAYOUT/COUNTER/@type=4">
										<xsl:copy-of select="i"/>
									</xsl:if>
									<xsl:if test="PARAGRAPH[position()=$i]/LAYOUT/COUNTER/@type=5">
										<xsl:copy-of select="I"/>
									</xsl:if>
									<xsl:if test="PARAGRAPH[position()=$i]/LAYOUT/COUNTER/@type=6">
										<xsl:copy-of select="PARAGRAPH[position()=$i]/LAYOUT/COUNTER/@bullet"/>
									</xsl:if>
							</xsl:otherwise>
						</xsl:choose>
					</xsl:variable>
					<xsl:call-template name="genListe">
						<xsl:with-param name="debutListe" select="$debutListeInitial"/>
						<xsl:with-param name="nbItem" select="$nbItemInitial"/>
						<xsl:with-param name="i" select="$debutListeInitial"/>
						<xsl:with-param name="start" select="$startInit"/>
					</xsl:call-template>
				</fo:list-block>
			</xsl:if>
			<xsl:variable name="nbItem">0</xsl:variable>
			<!--<xsl:variable name="nextItem"></xsl:variable>-->
			<xsl:message>Un parag.</xsl:message>
			<xsl:apply-templates select="PARAGRAPH[position()=$i]"/>
			<!-- Rappelle le template -->
			<xsl:if test="($i + 1) &lt;=$nbTotalPara">
				<xsl:message>parag. suivant </xsl:message>
				<xsl:call-template name="PARAS">
					<xsl:with-param name="debutListeInitial" select="$i + 1"/>
					<xsl:with-param name="i"><xsl:value-of select="$i + 1"/></xsl:with-param>
					<xsl:with-param name="nbItemInitial" select="$nbItem"/>
				</xsl:call-template>
			</xsl:if>
		</xsl:otherwise>
	</xsl:choose>

</xsl:template>

<xsl:template name="genListe">
	<xsl:param name="nbItem">param_nbItem</xsl:param>
	<xsl:param name="debutListe" select="param_debutListe"/>
	<xsl:param name="i">param_i</xsl:param>
	<xsl:param name="start">param_start</xsl:param>
	<xsl:message>Generation de la liste</xsl:message>
	<xsl:message>  debut   : <xsl:value-of select="$debutListe"/></xsl:message>
	<xsl:message>  courant : <xsl:value-of select="$i"/></xsl:message>
	<xsl:message>  nb      : <xsl:value-of select="$nbItem"/></xsl:message>
	<xsl:message>  start   : <xsl:value-of select="$start"/></xsl:message>
	<xsl:call-template name="createItem">
		<xsl:with-param name="i" select="$i"/>
		<xsl:with-param name="start" select="$start"/>
	</xsl:call-template>

	<xsl:if test="($i + 1) &lt; ($debutListe + $nbItem)">
		<xsl:variable name="nextStart">
			<xsl:choose>
				<xsl:when test="PARAGRAPH[position()=$i]/LAYOUT/COUNTER/@type &gt; 5">
					<xsl:copy-of select="$start"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:copy-of select="$start + 1"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:call-template name="genListe">
			<xsl:with-param name="debutListe" select="$debutListe"/>
			<xsl:with-param name="nbItem" select="$nbItem"/>
			<xsl:with-param name="i" select="$i + 1"/>
			<xsl:with-param name="start" select="$nextStart"/>
		</xsl:call-template>
	</xsl:if>
</xsl:template>

<xsl:template name="createItem">
	<xsl:param name="i">param_i</xsl:param>
	<xsl:param name="start">param_start</xsl:param>
	<fo:list-item>
		<fo:list-item-label>
			<fo:block>
					<xsl:value-of select="PARAGRAPH[position()=$i]/LAYOUT/COUNTER/@lefttext"/>
					<xsl:value-of select="$start"/>
					<xsl:value-of select="PARAGRAPH[position()=$i]/LAYOUT/COUNTER/@righttext"/>
			</fo:block>
		</fo:list-item-label>
		<fo:list-item-body>
				<xsl:apply-templates select="PARAGRAPH[position()=$i]"/>
		</fo:list-item-body>
	</fo:list-item>
</xsl:template>

<!-- PARAGRAPH -->
<xsl:template match="PARAGRAPH" name="PARAGRAPH">
	<xsl:text>
	</xsl:text>
	<fo:block>
	<!-- Formats the paragraph -->
	<xsl:apply-templates select="LAYOUT"/>

	<xsl:variable name="text"><xsl:value-of select="TEXT"/></xsl:variable>
	<!-- Formats each part of the paragraph -->
	<xsl:choose>
		<xsl:when test="FORMATS">
			<xsl:for-each select="FORMATS">
				<xsl:call-template name="FORMATS">
					<xsl:with-param name="text"><xsl:value-of select="$text"/></xsl:with-param>
				</xsl:call-template>
			</xsl:for-each>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="$text"/>
		</xsl:otherwise>
	</xsl:choose>

	</fo:block>
	<xsl:text>
	</xsl:text>
</xsl:template>

<!-- FORMATS -->
<xsl:template name="FORMATS">
	<xsl:param name="text"/>
	
	<xsl:variable name="pos"><xsl:value-of select="FORMAT/@pos"/></xsl:variable>
	<!-- Add the text before the first format. -->
	<xsl:if test="$pos != 0">
		<xsl:value-of select="substring($text, 0, $pos + 1)"/>
	</xsl:if>
	
	<xsl:for-each select="FORMAT">
		<xsl:variable name="pos"><xsl:value-of select="@pos"/></xsl:variable>
		<xsl:variable name="len"><xsl:value-of select="@len"/></xsl:variable>
		<!-- Add the text in a format. -->
		<fo:inline>
			<xsl:apply-templates select="."/>
			<xsl:value-of select="substring($text, $pos + 1, $len)"/>
		</fo:inline>
		<xsl:variable name="next_pos"><xsl:value-of select="following::FORMAT/@pos"/></xsl:variable>
		<!-- Add the text when a following format exists. -->
		<xsl:if test="$next_pos > ($pos + $len)">
			<xsl:value-of select="substring($text, $pos + $len + 1, $next_pos - $pos - $len)"/>
		</xsl:if>
		<!-- Add the text when there are no more formats. -->
		<xsl:if test="$next_pos=0">
			<xsl:value-of select="substring($text, $pos + $len + 1)"/>
		</xsl:if>
	</xsl:for-each>
</xsl:template>

<!-- FORMAT -->
<xsl:template match="FORMAT" name="FORMAT">
	<xsl:choose>
		<xsl:when test="@id=1">
			<xsl:call-template name="TEXT-FORMAT"/>
		</xsl:when>
	</xsl:choose>
</xsl:template>

<!-- TEXT-FORMAT -->
<xsl:template name="TEXT-FORMAT">
	<xsl:choose>
		<xsl:when test="VERTALIGN/@value=1">
			<xsl:attribute name="vertical-align">sub</xsl:attribute>
		</xsl:when>
		<xsl:when test="VERTALIGN/@value=2">
			<xsl:attribute name="vertical-align">super</xsl:attribute>
		</xsl:when>
	</xsl:choose>
	<xsl:if test="FONT">
		<xsl:attribute name="font-family"><xsl:value-of select="FONT/@name"/></xsl:attribute>
	</xsl:if>
	<xsl:if test="UNDERLINE/@value=1">
		<xsl:attribute name="text-decoration">underline</xsl:attribute>
	</xsl:if>
	<xsl:if test="STRIKEOUT/@value=1">
		<xsl:attribute name="text-decoration">line-through</xsl:attribute>
	</xsl:if>
	<xsl:if test="CHARSET">
		<!--<xsl:attribute name="charset"><xsl:value-of select="CHARSET/@value"/></xsl:attribute>-->
	</xsl:if>
	<xsl:if test="ITALIC/@value=1">
		<xsl:attribute name="font-style">italic</xsl:attribute>
	</xsl:if>
	<xsl:if test="WEIGHT/@value=75">
		<xsl:attribute name="font-weight">bold</xsl:attribute>
	</xsl:if>
	<xsl:if test="SIZE">
		<xsl:attribute name="font-size"><xsl:value-of select="SIZE/@value"/>pt</xsl:attribute>
	</xsl:if>
</xsl:template>

<!-- OTHER-FORMAT -->

<!-- LAYOUT -->
<xsl:template match="LAYOUT">
	<xsl:if test="NAME">
		<!-- MUST USE THE STYLE WHICH THIS NAME -->
		<!-- TODO -->
		<xsl:variable name="name"><xsl:value-of select="@value"/></xsl:variable>
		<xsl:call-template name="FORMAT" select="/DOC/STYLES//STYLE/NAME[@value=$name]"/>
	</xsl:if>
	
	<xsl:if test="FLOW">
		<xsl:attribute name="text-align">
			<xsl:value-of select="FLOW/@align"/>
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="COUNTER">
		
	</xsl:if>
	<xsl:if test="INDENTS/@first">
		<xsl:attribute name="text-indent">
			<xsl:value-of select="/@first"/>
		</xsl:attribute>
	</xsl:if>

	<xsl:if test="FORMAT">
		<xsl:apply-templates select="FORMAT"/>
	</xsl:if>
</xsl:template>

</xsl:stylesheet>
