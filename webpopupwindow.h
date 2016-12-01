/****************************************************************************
**
** 网站小广告 -。- : http://www.linuxkiss.com/
** 注释者: Uncle Leo
** 日期:2016-11-29
**
****************************************************************************/

#ifndef WEBPOPUPWINDOW_H
#define WEBPOPUPWINDOW_H

#include <QWidget>

class QWebEngineProfile;
class QWebEngineView;

class WebView;
class UrlLineEdit;

class WebPopupWindow : public QWidget
{
    Q_OBJECT

public:
    WebPopupWindow(QWebEngineProfile *profile);
    QWebEngineView *view() const;
    void setUrl(const QUrl &url);

private slots:
    void handleGeometryChangeRequested(const QRect &newGeometry);
    void handleIconChanged(const QIcon &icon);

private:
    UrlLineEdit *m_addressBar;
    WebView *m_view;
};
#endif // WEBPOPUPWINDOW_H
