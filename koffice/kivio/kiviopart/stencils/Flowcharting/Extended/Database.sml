<?xml version="1.0"?>
<KivioShapeStencil creator="Kate">
	<KivioSMLStencilSpawnerInfo>
		<Author data="Kristof Borrey"/>
		<Title data="Database"/>
		<Id data="database"/>
		<Description data="Database shape"/>
		<Version data="0.1"/>
		<Web data="www.koffice.org"/>
		<Email data="borrey@kde.org"/>
		<Copyright data="Copyright (C) 2003 Kristof Borrey. All rights reserved."/>
		<AutoUpdate data="off"/>
	</KivioSMLStencilSpawnerInfo>
	<Dimensions w="80.0" h="100.0"/>
                <KivioConnectorTarget x="0.0" y="50.0"/>
                <KivioConnectorTarget x="80.0" y="50.0"/>
                <KivioConnectorTarget x="40.0" y="6.2"/>
                <KivioConnectorTarget x="40.0" y="95.0"/>
	<KivioShape type="ClosedPath" name="Direct Database Line">
		<KivioFillStyle colorStyle="1" color="#2a7fd9"/>
                <KivioLineStyle color="#000000" width="0.5" capStyle="32" joinStyle="128"/>
		<KivioPoint x="0" y="25"/>
		<KivioPoint x="0" y="75" type="bezier"/>
		<KivioPoint x="15" y="100" type="bezier"/>
		<KivioPoint x="65" y="100" type="bezier"/>
		<KivioPoint x="80" y="75" type="bezier"/>
		<KivioPoint x="80" y="25"/>
	</KivioShape>
        <KivioShape type="ClosedPath" name="Database">
		<KivioFillStyle colorStyle="1" color="#2a7fd9"/>
                <KivioLineStyle color="#000000" width="0.5" capStyle="32" joinStyle="128"/>
		<KivioPoint x="0" y="25"/>

		<KivioPoint x="0" y="25" type="bezier"/>
		<KivioPoint x="15" y="0" type="bezier"/>
		<KivioPoint x="65" y="0" type="bezier"/>
		<KivioPoint x="80" y="25" type="bezier"/>

                <KivioPoint x="80" y="25"/>
                <KivioPoint x="80" y="75"/>

		<KivioPoint x="80" y="25" type="bezier"/>
		<KivioPoint x="65" y="50" type="bezier"/>
		<KivioPoint x="15" y="50" type="bezier"/>
		<KivioPoint x="0" y="25" type="bezier"/>

		<KivioPoint x="0" y="75"/>
	</KivioShape>
	<KivioShape type="TextBox" name="Text"  x="5.0" y="40.0" w="70.0" h="50.0"/>
</KivioShapeStencil>
