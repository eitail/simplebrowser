/****************************************************************************
**
** 网站小广告 -。- : http://www.linuxkiss.com/
** 注释者: Uncle Leo
** 日期:2016-11-29
**
****************************************************************************/

#include "browser.h"
#include "browserwindow.h"
#include "webview.h"
#include <QAuthenticator>

Browser::Browser()
{
}

Browser::~Browser()
{
    qDeleteAll(m_windows);
    m_windows.clear();
}

Browser &Browser::instance()
{
    static Browser browser;
    return browser;
}

QVector<BrowserWindow*> Browser::windows()
{
    return m_windows;
}

/*
 * 打开程序，添加一个差UN港口，里面有导航等控件。
 * contains()-->m_windows.contains(mainWindow)表示mainWindow是否在m_windows中。
 *
 * QList::prepend()在头部添加元素
 * QList::append()在列表尾部添加元素
 * QList::insert()在中间插入
 *
 * 信号槽函数，当mainWindow被摧毁时候，执行removeOne()操作！
 *
*/
void Browser::addWindow(BrowserWindow *mainWindow)
{
    if (m_windows.contains(mainWindow))
        return;
    m_windows.prepend(mainWindow);
    QObject::connect(mainWindow, &QObject::destroyed, [this, mainWindow]() {
        m_windows.removeOne(mainWindow);
    });
   mainWindow->show();
}
