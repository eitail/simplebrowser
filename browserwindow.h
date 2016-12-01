/****************************************************************************
**
** 网站小广告 -。- : http://www.linuxkiss.com/
** 注释者: Uncle Leo
** 日期:2016-11-29
**
****************************************************************************/

#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
#include <QWebEnginePage>

class QProgressBar;

class TabWidget;
class UrlLineEdit;
class WebView;

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    BrowserWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
    ~BrowserWindow();
    QSize sizeHint() const override;
    TabWidget *tabWidget() const;
    WebView *currentTab() const;

    //下面三个是加载页面
    void loadPage(const QString &url);
    void loadPage(const QUrl &url);
    void loadHomePage();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    //处理新窗口触发
    void handleNewWindowTriggered();
    //处理文件打开触发
    void handleFileOpenTriggered();
    //处理窗口展示触发
    void handleShowWindowTriggered();
    //处理网页加载进度
    void handleWebViewLoadProgress(int);
    //处理网页标题的改变
    void handleWebViewTitleChanged(const QString &title);
    //处理网页url地址的改变
    void handleWebViewUrlChanged(const QUrl &url);
    //处理网页icon图的变化
    void handleWebViewIconChanged(const QIcon &icon);
    //处理网页行为启用或者不启动
    void handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled);

private:
    //创建文件导航
    QMenu *createFileMenu(TabWidget *tabWidget);
    //创建试图导航
    QMenu *createViewMenu(QToolBar *toolBar);
    //创建窗口导航
    QMenu *createWindowMenu(TabWidget *tabWidget);
    //创建help导航
    QMenu *createHelpMenu();
    //创建工具栏
    QToolBar *createToolBar();

private:

    TabWidget *m_tabWidget;
    QProgressBar *m_progressBar;
    QAction *m_historyBackAction;
    QAction *m_historyForwardAction;
    QAction *m_stopAction;
    QAction *m_reloadAction;
    QAction *m_stopReloadAction;
    UrlLineEdit *m_urlLineEdit;
};

#endif // BROWSERWINDOW_H
