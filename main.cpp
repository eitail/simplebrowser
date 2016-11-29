/****************************************************************************
**
** 网站小广告 -。- : http://www.linuxkiss.com/
** 注释者: Uncle Leo
** 日期:2016-11-29
**
****************************************************************************/

#include "browser.h"
#include "browserwindow.h"
#include <QApplication>

//获取URL函数
QString getCommandLineUrlArgument()
{
/*
    //QCoreApplication::arguments()，
    返回命令行参数的列表
    arguments().at(0) 工程名称
    arguments().at(1) 是第一个参数
    arguments().last()是最后一个参数

    QUrl::fromUserInput(lastArg):
    可以被扣除，返回一个 userInput 指定的有效 URL；如果不能，则会返回一个空 QURL。
    大多数应用程序，可以浏览网页，允许用户输入一个 URL 形式的简单字符串。此字符串可以手动输入到地址栏，从剪贴板中获得，或者通过命令行参数传递。
    当没有一个有效的 URL 字符串时，执行最好的猜测，使各种web相关的假设。
    当字符串对应系统上的一个有效文件路径时，会构造一个 file:// URL，使用 QUrl::fromLocalFile()。
    如果不是这种情况，试图把字符串变成一个 http:// 或 ftp:// URL。这时，后者字符串从以 'ftp' 开始。
    结果然后通过 QUrl 的 tolerant 解析器传递，如果成功，则会返回一个有效的 QUrl；否则，返回 QUrl()。

    isValid():判断url是否可用
*/
    const QStringList args = QCoreApplication::arguments();
    if (args.count() > 1) {
        const QString lastArg = args.last();
        const bool isValidUrl = QUrl::fromUserInput(lastArg).isValid();
        if (isValidUrl)
            return lastArg;
    }
    return QString();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    //设置任务栏中的图标
    app.setWindowIcon(QIcon(QLatin1String(":simplebrowser.svg")));
    //实例化一个窗口类
    BrowserWindow *window = new BrowserWindow();
    //添加一个窗口，静态函数instance()返回指向自己的指针
    //addWindow()在browser.cpp中定义
    Browser::instance().addWindow(window);

    //获取url,有就跳转，没有就天赚到home页
    const QString url = getCommandLineUrlArgument();
    //如果URl不为空
    if (!url.isEmpty())
        window->loadPage(url);
    else
        //默认首页，函数在browserwindows.cpp
        window->loadHomePage();

    return app.exec();
}
