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
	 
<!-- DOC. (DEFINITIONS, ...) -->
<xsl:template match="ANCHOR">
	<xsl:param name="rows">
		<xsl:value-of select="/DOC/FRAMESETS/FRAMESET[@grpMgr=$table]/@rows"/>
	</xsl:param>
	<xsl:param name="cols">
		<xsl:value-of select="/DOC/FRAMESETS/FRAMESET[@grpMgr=$table]/@cols"/>
	</xsl:param>
	<xsl:param name="table">param_table</xsl:param>
	<xsl:message>Construction de la table</xsl:message>
	
	<xsl:message><xsl:value-of select="$table"/>, Col <xsl:value-of select="$cols"/>, ligne <xsl:value-of select="$rows"/></xsl:message>
	<fo:table>
		<xsl:call-template name="ligne">
			<xsl:with-param name="row">0</xsl:with-param>
			<xsl:with-param name="rows">
				<xsl:copy-of select="$rows"/>
			</xsl:with-param>
			<xsl:with-param name="cols">
				<xsl:copy-of select="$cols"/>
			</xsl:with-param>
			<xsl:with-param name="table">
				<xsl:copy-of select="$table"/>
			</xsl:with-param>
		</xsl:call-template>
	</fo:table>
</xsl:template>

<xsl:template name="ligne">
	<xsl:param name="row">param_row</xsl:param>
	<xsl:param name="rows">param_rows</xsl:param>
	<xsl:param name="cols">param_cols</xsl:param>
	<xsl:param name="table">param_table</xsl:param>

	<xsl:message>Ligne : <xsl:value-of select="$row"/> / <xsl:value-of select="$rows"/></xsl:message>
	<fo:table-row>
		<xsl:call-template name="cellule">
			<xsl:with-param  name="col">0</xsl:with-param>
			<xsl:with-param name="cols">
				<xsl:copy-of select="$cols"/>
			</xsl:with-param>
			<xsl:with-param name="row">
				<xsl:copy-of select="$row"/>
			</xsl:with-param>
			<xsl:with-param name="rows">
				<xsl:copy-of select="$rows"/>
			</xsl:with-param>
			<xsl:with-param name="table">
				<xsl:copy-of select="$table"/>
			</xsl:with-param>
		</xsl:call-template>
	</fo:table-row>

	<xsl:if test="$row &lt; $rows">
		<xsl:call-template name="ligne">
			<xsl:with-param name="cols">
				<xsl:copy-of select="$cols"/>
			</xsl:with-param>
			<xsl:with-param name="row">
				<xsl:copy-of select="$row + 1"/>
			</xsl:with-param>
			<xsl:with-param name="rows">
				<xsl:copy-of select="$rows"/>
			</xsl:with-param>
			<xsl:with-param name="table">
				<xsl:copy-of select="$table"/>
			</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	
</xsl:template>

<xsl:template name="cellule">
	<xsl:param name="row">param_row</xsl:param>
	<xsl:param name="rows">param_rows</xsl:param>
	<xsl:param name="col">param_col</xsl:param>
	<xsl:param name="cols">param_cols</xsl:param>
	<xsl:param name="table">param_table</xsl:param>

	<xsl:message>Col : <xsl:value-of select="$col"/> / <xsl:value-of select="$cols"/></xsl:message>
	<xsl:message>Para : <xsl:value-of select="/DOC/FRAMESETS/FRAMESET[@grpMgr=$table and @row=$row and @col=$col]"/></xsl:message>
	<fo:table-cell>
			<xsl:call-template name="PARAS" select="/DOC/FRAMESETS/FRAMESET[@grpMgr=$table and @row=$row and @col=$col]/PARAGRAPH">
			<xsl:with-param name="i">1</xsl:with-param>
			<xsl:with-param name="debutListInitial">0</xsl:with-param>
		    <xsl:with-param name="nbItemInitial">0</xsl:with-param>
		</xsl:call-template>
	</fo:table-cell>

	<xsl:if test="$col &lt; $cols">
		<xsl:call-template name="cellule">
			<xsl:with-param name="col">
				<xsl:copy-of select="$col + 1"/>
			</xsl:with-param>
			<xsl:with-param name="cols">
				<xsl:copy-of select="$cols"/>
			</xsl:with-param>
			<xsl:with-param name="row">
				<xsl:copy-of select="$row"/>
			</xsl:with-param>
			<xsl:with-param name="rows">
				<xsl:copy-of select="$rows"/>
			</xsl:with-param>
			<xsl:with-param name="table">
				<xsl:copy-of select="$table"/>
			</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
</xsl:template>
	
</xsl:stylesheet>
