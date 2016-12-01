/****************************************************************************
**
** 网站小广告 -。- : http://www.linuxkiss.com/
** 注释者: Uncle Leo
** 日期:2016-11-29
**
****************************************************************************/

#include "urllineedit.h"
#include "webpage.h"
#include "webpopupwindow.h"
#include "webview.h"
#include <QIcon>
#include <QVBoxLayout>

WebPopupWindow::WebPopupWindow(QWebEngineProfile *profile)
    : m_addressBar(new UrlLineEdit(this))
    , m_view(new WebView(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    setLayout(layout);
    layout->addWidget(m_addressBar);
    layout->addWidget(m_view);

    m_view->setPage(new WebPage(profile, m_view));
    m_view->setFocus();
    m_addressBar->setReadOnly(true);
    m_addressBar->setFavIcon(QIcon(QStringLiteral(":defaulticon.png")));

    connect(m_view, &WebView::titleChanged, this, &QWidget::setWindowTitle);
    connect(m_view, &WebView::urlChanged, this, &WebPopupWindow::setUrl);
    connect(m_view->page(), &WebPage::iconChanged, this, &WebPopupWindow::handleIconChanged);
    connect(m_view->page(), &WebPage::geometryChangeRequested, this, &WebPopupWindow::handleGeometryChangeRequested);
    connect(m_view->page(), &WebPage::windowCloseRequested, this, &QWidget::close);
}

QWebEngineView *WebPopupWindow::view() const
{
    return m_view;
}

void WebPopupWindow::setUrl(const QUrl &url)
{
    m_addressBar->setUrl(url);
}

void WebPopupWindow::handleGeometryChangeRequested(const QRect &newGeometry)
{
    m_view->setMinimumSize(newGeometry.width(), newGeometry.height());
    move(newGeometry.topLeft() - m_view->pos());
    // let the layout do the magic
    resize(0, 0);
    show();
}

void WebPopupWindow::handleIconChanged(const QIcon &icon)
{
    if (icon.isNull())
        m_addressBar->setFavIcon(QIcon(QStringLiteral(":defaulticon.png")));
    else
        m_addressBar->setFavIcon(icon);
}
