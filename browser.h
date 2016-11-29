/****************************************************************************
**
** 网站小广告 -。- : http://www.linuxkiss.com/
** 注释者: Uncle Leo
** 日期:2016-11-29
**
****************************************************************************/

#ifndef BROWSER_H
#define BROWSER_H

#include <QVector>

class BrowserWindow;

class Browser
{
public:
    ~Browser();

    QVector<BrowserWindow*> windows();
    void addWindow(BrowserWindow* window);
    static Browser &instance();

private:
    Browser();
    QVector<BrowserWindow*> m_windows;
};
#endif // BROWSER_H
