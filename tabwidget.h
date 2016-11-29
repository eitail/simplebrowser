/****************************************************************************
**
** 网站小广告 -。- : http://www.linuxkiss.com/
** 注释者: Uncle Leo
** 日期:2016-11-29
**
****************************************************************************/

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QWebEnginePage>

class QUrl;

class WebView;

class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    TabWidget(QWidget *parent = nullptr);
    ~TabWidget();

    WebView *currentWebView() const;

signals:
    // current tab/page signals
    void linkHovered(const QString &link);
    void loadProgress(int progress);
    void titleChanged(const QString &title);
    void urlChanged(const QUrl &url);
    void iconChanged(const QIcon &icon);
    void webActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled);

public slots:
    // current tab/page slots
    void setUrl(const QUrl &url);
    void triggerWebPageAction(QWebEnginePage::WebAction action);

    WebView *createTab(bool makeCurrent = true);
    void closeTab(int index);
    void nextTab();
    void previousTab();

private slots:
    void handleCurrentChanged(int index);
    void handleContextMenuRequested(const QPoint &pos);
    void cloneTab(int index);
    void closeOtherTabs(int index);
    void reloadAllTabs();
    void reloadTab(int index);

private:
    WebView *webView(int index) const;
    void setupView(WebView *webView);
};

#endif // TABWIDGET_H
