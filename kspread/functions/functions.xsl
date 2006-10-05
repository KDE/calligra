<?xml version="1.0" encoding="UTF-8" ?>
<!--  XSL-T from KSpread's old XML function description format to the new one -->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>

    <xsl:template match="KSpreadFunctions">
        <xsl:element name="functions">
            <xsl:attribute name="xmlns">
                <xsl:text>http://www.koffice.org/standards/kspread/functions/1.0</xsl:text>
            </xsl:attribute>
            <xsl:attribute name="xmlns:xsi">
                <xsl:text>http://www.w3.org/2001/XMLSchema-instance</xsl:text>
            </xsl:attribute>
            <xsl:attribute name="xsi:schemaLocation">
                <xsl:text>http://www.koffice.org/standards/kspread/functions/1.0
                http://www.koffice.org/standards/kspread/functions/1.0/functions.xsd</xsl:text>
            </xsl:attribute>
            <xsl:for-each select="Group">
                <xsl:element name="group">
                    <xsl:attribute name="name">
                        <xsl:value-of select="GroupName"/>
                    </xsl:attribute>
                    <xsl:for-each select="Function">
                        <xsl:sort select="Name"/>
                        <xsl:apply-templates select="."/>
                    </xsl:for-each>
               </xsl:element>
            </xsl:for-each>
        </xsl:element>
    </xsl:template>

    <xsl:template match="Function">
        <xsl:element name="function">
            <xsl:attribute name="name">
                <xsl:value-of select="Name"/>
            </xsl:attribute>
            <xsl:attribute name="type">
                <xsl:value-of select="Type"/>
            </xsl:attribute>
            <xsl:element name="summary">
                <!-- empty -->
                <xsl:text>TODO</xsl:text>
            </xsl:element>
            <xsl:element name="parameters">
                <xsl:apply-templates select="Parameter"/>
            </xsl:element>
            <xsl:element name="description">
                <xsl:for-each select="Help/Text">
                    <xsl:value-of select="."/>
                    <xsl:text><![CDATA[<p>]]></xsl:text>
                </xsl:for-each>
            </xsl:element>
            <xsl:element name="examples">
                <xsl:for-each select="Help/Example">
                    <xsl:element name="example">
                        <xsl:value-of select="."/>
                    </xsl:element>
                </xsl:for-each>
            </xsl:element>
        </xsl:element>
    </xsl:template>

    <xsl:template match="Parameter">
        <xsl:element name="parameter">
            <xsl:attribute name="name">
                <!-- empty -->
            </xsl:attribute>
            <xsl:attribute name="type">
                <xsl:value-of select="Type"/>
            </xsl:attribute>
            <xsl:attribute name="comment">
                <xsl:value-of select="Comment"/>
            </xsl:attribute>
        </xsl:element>
    </xsl:template>

</xsl:stylesheet>
