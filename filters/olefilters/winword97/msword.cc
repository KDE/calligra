<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<HTML>
<HEAD>
<TITLE>Справочник по KFind: Установка</TITLE>
<META HTTP-EQUIV="content-type" CONTENT="text/html; charset=koi8-r">
<META NAME="keywords" CONTENT="KDE kfind translation Russian Alexander Izyurov перевод русский Александр Изьюров">
<META NAME="description" CONTENT="Translation of KDE kfind to Russian by Alexander Izyurov">
</HEAD>
<BODY BGCOLOR="#ffffff" LINK="#aa0000" TEXT="#000000" > 
<FONT FACE="Helvetica">
<A HREF="http://www.kde.org/"><IMG SRC="logotp3.png" BORDER="0" ALT="The K Desktop Environment"></A>
<HR WIDTH="100%" SIZE=2 ALIGN="CENTER" NOSHADE>

 
<P ALIGN="RIGHT">

<A HREF="index-3.html"> Вперед </A>
<A HREF="index-1.html"> Назад </A>
<A HREF="index.html#toc2"> Содержание </A>
</P>
<H3><A NAME="s2">2. Установка</A></H3>

<P>
<P>
<H3><A NAME="ss2.1">2.1 Где взять KFind</A>
</H3>

<P>KFind -- часть проекта KDE  
<A HREF="http://www.kde.org">http://www.kde.org</A>. KFind находится на
<A HREF="ftp://ftp.kde.org/pub/kde/">ftp://ftp.kde.org/pub/kde/</A>, основном ftp-сервере проекта KDE.
Там также есть ссылки на множество зеркал.
<P>
<H3><A NAME="ss2.2">2.2 Требования</A>
</H3>

<P>Для того, чтобы успешно скомпилировать KFind, вам понадобятся последние версии
<CODE>libkdecore</CODE> и <CODE>libkfm</CODE>. Все необходимые библиотеки и сам
KFind можно взять на
<A HREF="ftp://ftp.kde.org/pub/kde/">ftp://ftp.kde.org/pub/kde/</A>.
<P>
<H3><A NAME="ss2.3">2.3 Компиляция и установка</A>
</H3>

<P><B>Kfind</B> обычно является составной частью KDE и поэтому
автоматически устанавливается вместе с другими приложениями KDE при нормальной
установке KDE.
<P>Если вам приходится компилировать и устанавливать KFind самому, 
перейдите в  каталог дистрибутива KFind  и введите следующие команды:
<BLOCKQUOTE><CODE>
<PRE>
 
% ./configure 
% make 
% make install 
</PRE>
</CODE></BLOCKQUOTE>
<P>Поскольку KFind использует <CODE>autoconf</CODE>, у вас не должно возникнуть проблем при
компиляции.
Если вы все же столкнетесь с проблемами, пожалуйста, сообщите о них в списки рассылки <B>KDE</B>.
<P>
<H3><A NAME="ss2.4">2.4 Настройка</A>
</H3>

<P>Обычно настройка выполняется в диалоговом окне Установки;
подробности в разделе для пользователя этого руководства. 
Для интересующихся приводим техническое описание настройки
<B>kfind</B> :
<P>
<P><B>Kfind</B> читает параметры конфигурации из конфигурационного файла
(обычно  <CODE>~/.kde/share/config/kfindrc</CODE>).
<P>Этот файл содержит информацию о сохранении результатов поиска,
имеющихся архиваторах и другие параметры конфигурации; 
эти параметры можно редактировать из диалогового окна
<B>Установки</B> <CODE>Kfind</CODE>. 
<P>
<P><B>Общие сведения:</B> файл ресурсов состоит из разделов.
Каждый раздел начинается с заголовка в квадратных скобках, например, <CODE>[Saving]</CODE>
<P>
<P>Информация после заголовка относится к 
конкретным аспектам работы <CODE>Kfind</CODE>.
<P>
<P><B>Настройка сохранения:</B> В конфигурационном файле содержатся имя и формат
файла, используемого для сохранения результатов поиска. Эти параметры хранятся
в следующем виде:
<BLOCKQUOTE><CODE>
<PRE>
[Saving]
Format=HTML
Filename=/root/result.html
</PRE>
</CODE></BLOCKQUOTE>

где параметр <CODE>Format</CODE> может принимать значения <CODE>HTML </CODE>или <CODE>Plain Text</CODE>.
<P>
<P><B>Настройка архиваторов:</B> Доступные <CODE>kfind</CODE> архиваторы описаны
в разделе <CODE>[Archiver Types]</CODE> <I>kfindrc</I> так:
<BLOCKQUOTE><CODE>
<PRE>
[Archiver Types]
Archivers=tar;zip;zoo;
</PRE>
</CODE></BLOCKQUOTE>
<P>Каждый архиватор подробно описывается в собственном разделе:
<BLOCKQUOTE><CODE>
<PRE>
[tar]
ExecOnCreate=tar cf %a -C %d %n
ExecOnUpdate=tar uf %a -C %d %n
Comment=Tar
</PRE>
</CODE></BLOCKQUOTE>
<P>Первая строка содержит команду, которую нужно выполнить
для создания нового архива, а вторая -- для изменения существующего архива.
Третья строка -- комментарий, который используется в качестве описания архиватора
в диалоговом окне <B>Установки</B>.
В описанных командах можно использовать переменные, которые начинаются 
со знака %.
<P>Вот список допустимых переменных командной строки:
<P>
<P><CODE>%a</CODE> Полное имя архива
<P>
<P><CODE>%f</CODE> Имя обычного файла
<P>
<P><CODE>%d</CODE> Родительский каталог. Если аргументом является файл
(не каталог), то результатом подстановки переменной будет
каталог, содержащий этот файл.
<P>
<P><CODE>%n</CODE> Имя файла. Если аргумент является каталогом, то это имя этого каталога.
<P>
<P>
<P ALIGN="RIGHT">

<A HREF="index-3.html"> Вперед </A>
<A HREF="index-1.html"> Назад </A>
<A HREF="index.html#toc2"> Содержание </A>
</P>
<CENTER>
<HR WIDTH="100%" SIZE=3 ALIGN=CENTER NOSHADE>
</CENTER>    
</FONT>

 
</BODY>
</HTML>
