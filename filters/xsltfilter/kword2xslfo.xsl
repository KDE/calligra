<?xml version="1.0"  encoding="UTF-8"?>
<xsl:stylesheet version = '1.0' 
     xmlns:xsl='http://www.w3.org/1999/XSL/Transform'
     xmlns:fo='http://www.w3.org/1999/XSL/Format'>

<xsl:output method="xml" encoding="UTF-8"/>

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
	<xsl:apply-templates select="FRAMESET"/>
	</fo:page-sequence>

</xsl:template>

<!-- CADRES POUR CHAQUE PARTIE D'UNE PAGE -->
<xsl:template match="FRAMESET">
	<xsl:choose>
		<xsl:when test="@frameInfo=0">
			<fo:flow flow-name="xsl-region-body">
				<xsl:apply-templates select="FRAME"/>
				<xsl:for-each select="PARAGRAPH">
					<xsl:call-template name="PARAGS" select="ancestor-or-self::PARAGRAPH"/>
				</xsl:for-each>
			</fo:flow>
		</xsl:when>
		<xsl:when test="@frameInfo=1">
			<fo:static-content>
				<xsl:apply-templates select="FRAME"/>
				<xsl:for-each select="PARAGRAPH">
					<xsl:apply-templates select="."/>
				</xsl:for-each>
			</fo:static-content>
		</xsl:when>
	</xsl:choose>
</xsl:template>

<!-- CADRE -->
<xsl:template match="FRAME">
	<!--<xsl:for-each select="PARAGRAPH">-->
<!--	</xsl:for-each>-->
</xsl:template>

<!-- LISTS -->
<xsl:template name="LISTS">
	<fo:list-block>
		<xsl:call-template name="LIST-ITEM" select="."/>
		<xsl:for-each select="following-sibling::PARAGRAPH">
			<xsl:choose>
			<xsl:when test="./LAYOUT/COUNTER/@numberingtype=0">
				<xsl:call-template name="LIST-ITEM" select="."/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:call-template name="PARAGS" select="ancestor-or-self::PARAGRAPH"/>
			</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
	</fo:list-block>
</xsl:template>

<!-- PARAGS -->
<xsl:template name="PARAGS">
	<xsl:for-each select="following-sibling::PARAGRAPH">
		<xsl:choose>
			<xsl:when test="./LAYOUT/COUNTER/@numberingtype=0">
				<xsl:call-template name="LISTS" select="ancestor-or-self::PARAGRAPH"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates select="."/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:for-each>
</xsl:template>

<!-- LIST -->
<xsl:template name="LIST-ITEM">
	<fo:list-item>
		<fo:list-item-label>
			<fo:block/>
		</fo:list-item-label>
		<fo:list-item-body>
			<xsl:apply-templates select="."/>
		</fo:list-item-body>
	</fo:list-item>
</xsl:template>

<!-- PARAGRAPH -->
<xsl:template match="PARAGRAPH">
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
