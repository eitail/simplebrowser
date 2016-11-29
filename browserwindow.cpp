/****************************************************************************
**
** 网站小广告 -。- : http://www.linuxkiss.com/
** 注释者: Uncle Leo
** 日期:2016-11-29
**
** 疑问：
** 需要深入研究的问题，当前页搜索下列文字定位
** stop功能是停止(具体是什么功能暂时不明白，深入研究之后在补充说明)
** 这两个菜单，没有显示，得研究
**
****************************************************************************/

#include "browser.h"
#include "browserwindow.h"
#include "tabwidget.h"
#include "urllineedit.h"
#include "webview.h"
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>

/*
 * 成员初始化列表
 * 1、对于const数据成员，必须在执行到构造函数体之前，即创建对象时进行初始经。
 * 2、成员初始经列表由逗号分隔的初始化列表组成(前面带冒号)。它位于参数列表的右括号之后、函数体左括号之前。
 * 3、初值可以是常量或构造函数的参数列表中的参数。这种方法并不限于初始化常量。
 * 4、只有构造函数可以使用这种初始化列表语法。对于const类成员，必须使用这种语法。另外，对于被声明为引用的类成员，也必须使用这种语法。
 * 这是因为引用和cosnt数据类似，只能在被创建时进行初始经。对于简单数据成员使用成员初始化列表和在函数体中使用赋值没有什么区别。
 * 5、对于本身就是类对象的成员来说，使用成员初始化列表效率更高。
 * 6、从概念上说，这些初始化工作是在对象创建时完成的。此时还未执行括号中的任何代码。
 * 7、不能将成员初始化列表语法用于构造函数之外的其他类方法。
 * 8、必须用这种格式来初始化非静态const数据成员
 * 9、必须用这种格式来初始化引用数据成员。
 *
 * nullptr类似于null，可自行百度查看区别
*/

BrowserWindow::BrowserWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_tabWidget(new TabWidget(this))
    , m_progressBar(new QProgressBar(this))
    , m_historyBackAction(nullptr)
    , m_historyForwardAction(nullptr)
    , m_stopAction(nullptr)
    , m_reloadAction(nullptr)
    , m_stopReloadAction(nullptr)
    , m_urlLineEdit(new UrlLineEdit(this))
{
    /*
     * 设置按钮风格，只显示一个图标，文本或文本位于图标旁边、下方。默认值是Qt::ToolButtonIconOnly。
     * Qt::ToolButtonFollowStyle：遵循QStyle::StyleHint
    */
    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    /*
     * 该方法的意思是当退出窗口时候，Qt会delete，销毁窗口，所以实例化窗口时，
     * 在栈上实例化，使用了该方法，退出窗口时会报错，
     * 而在堆上实例化窗口，则不会报错，因为堆上需要我们手动delete释放内存
    */
    setAttribute(Qt::WA_DeleteOnClose, true);

    /*
     * 创建一个ToolBar,在这里就是搜索框所在的那一条，可以拖动的控件
    */
    QToolBar *toolbar = createToolBar();
    addToolBar(toolbar);

    //添加菜单栏
    menuBar()->addMenu(createFileMenu(m_tabWidget));
    menuBar()->addMenu(createViewMenu(toolbar));
    //这两个菜单，没有显示，得研究
    //menuBar()->addMenu(createWindowMenu(m_tabWidget));
    //menuBar()->addMenu(createHelpMenu());

    //实例化一个widget
    QWidget *centralWidget = new QWidget(this);

    //进行布局设置,先实例化一个QVBoxLayout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);//表示各个控件之间的上下间距
    layout->setMargin(0);//表示控件与窗体的左右边距
    addToolBarBreak();//添加一个Break（破裂; 间断; 或译为区域），这时，新添加的工具条将不再紧跟前一个工具条，而是另起一行。
    /*
    * setMaximumHeight(1)：设置进度条，高度为1
    * setTextVisible(false)：设置进度条，百分之几这个数字不显示，true就是显示
    * setStyleSheet():设置进度条样式
    */
    m_progressBar->setMaximumHeight(1);
    m_progressBar->setTextVisible(false);
    m_progressBar->setStyleSheet(QStringLiteral("QProgressBar {border: 0px } QProgressBar::chunk { background-color: red; }"));
    layout->addWidget(m_progressBar);

    //添加一个widget,将tab页放进去
    layout->addWidget(m_tabWidget);

    //将上一句的布局放到，上面实例化的QWidget中
    centralWidget->setLayout(layout);

    //主窗口类的setCentralWidget()方法,它将把widget设置为主窗口的中心窗口部件。具体了解可以百度。
    setCentralWidget(centralWidget);

    //------一系列的信号槽函数------
    //tab页改变，窗口的title改变
    connect(m_tabWidget, &TabWidget::titleChanged, this, &BrowserWindow::handleWebViewTitleChanged);
    //鼠标移动到连接上面statusBar部分会显示相应连接内容
    connect(m_tabWidget, &TabWidget::linkHovered, [this](const QString& url) {
        statusBar()->showMessage(url);
    });
    //加载页面的时候，进度条进行读取
    connect(m_tabWidget, &TabWidget::loadProgress, this, &BrowserWindow::handleWebViewLoadProgress);
    //url改变的时候，输入框的URL就要进行修改
    connect(m_tabWidget, &TabWidget::urlChanged, this, &BrowserWindow::handleWebViewUrlChanged);
    //icon图改变的时候，输入框的ico图就要进行修改
    connect(m_tabWidget, &TabWidget::iconChanged, this, &BrowserWindow::handleWebViewIconChanged);
    //
    connect(m_tabWidget, &TabWidget::webActionEnabledChanged, this, &BrowserWindow::handleWebActionEnabledChanged);
    //输入框输入地址，按回车进入相应页面，并且在输入框显示默认图标
    connect(m_urlLineEdit, &QLineEdit::returnPressed, this, [this]() {
        m_urlLineEdit->setFavIcon(QIcon(QStringLiteral(":defaulticon.png")));
        loadPage(m_urlLineEdit->url());
    });
    //刚打开浏览器加载页面的时候，url输入框显示默认icon图
    m_urlLineEdit->setFavIcon(QIcon(QStringLiteral(":defaulticon.png")));
    //初始化的时候设置窗口title，个人感觉这句不写也行。
    handleWebViewTitleChanged(tr("Qt Simple Browser"));
    //创建一个初始的tab页面
    m_tabWidget->createTab();
}

BrowserWindow::~BrowserWindow()
{
}

/*
 * 重载sizeHint()方法
 * QRect:矩形类
 * QApplication::desktop()->screenGeometry();-->QT获取系统屏幕分辨率
 * qreal是Qt的数据类型，在桌面操作系统中（比如Windows， XNix等）qreal其实就是double类型；而在嵌入设备系统中，qreal则等同于float 类型。
 * QSize 类代表一个矩形区域的大小，实现在 QtCore 共享库中。它可以认为是由一个整型的宽度和整型的高度组合而成的。
 * 例子：
 * rect 的x1=0 y1=15 x2=459 y2=334
 * size 的wd=460 ht=320
 * 文档注释
 * right() = left() + width() - 1
 * bottom() = top() + height() - 1
 * 综上所以
 * width() = right() - left() +1
 * height() = bottom() - top() +1
 *
*/
QSize BrowserWindow::sizeHint() const
{
    QRect desktopRect = QApplication::desktop()->screenGeometry();
    QSize size = desktopRect.size() * qreal(0.9);
    return size;
}

//菜单栏第一个菜单File
QMenu *BrowserWindow::createFileMenu(TabWidget *tabWidget)
{
    //设置显示的名称File
    QMenu *fileMenu = new QMenu(tr("&File"));
    //添加子菜单New Window，调用创建新窗口handleNewWindowTriggered这个函数，并且设置快捷键
    fileMenu->addAction(tr("&New Window"), this, &BrowserWindow::handleNewWindowTriggered, QKeySequence::New);
    /*
    * 设置子菜单 new tab
    * new tab的图片，显示文字
    * 快捷键(添加setShortcuts()设置快捷键之后，在菜单后面就会显示相应的快捷键)
    * icon图不显示
    * 信号槽函数触发，connect(new tab子菜单，触发，tabwidget,创建一个tab页)
    * 将newTabAction添加到菜单列表
    */
    QAction *newTabAction = new QAction(QIcon(QLatin1String(":addtab.png")), tr("New &Tab"), this);
    newTabAction->setShortcuts(QKeySequence::AddTab);
    newTabAction->setIconVisibleInMenu(false);
    connect(newTabAction, &QAction::triggered, tabWidget, &TabWidget::createTab);
    fileMenu->addAction(newTabAction);

    //添加子菜单Open File，调用打开文件handleFileOpenTriggered函数，设置快捷键
    fileMenu->addAction(tr("&Open File..."), this, &BrowserWindow::handleFileOpenTriggered, QKeySequence::Open);

    //添加一条横线分隔
    fileMenu->addSeparator();

    //添加子菜单关闭当前tab(各句含义参考上面)
    QAction *closeTabAction = new QAction(QIcon(QLatin1String(":closetab.png")), tr("&Close Tab"), this);
    closeTabAction->setShortcuts(QKeySequence::Close);
    closeTabAction->setIconVisibleInMenu(false);
    connect(closeTabAction, &QAction::triggered, [tabWidget]() {
        tabWidget->closeTab(tabWidget->currentIndex());
    });
    fileMenu->addAction(closeTabAction);

    //添加子菜单退出(各句含义参考上面)
    QAction *closeAction = new QAction(tr("&Quit"),this);
    closeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
    connect(closeAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(closeAction);

    connect(fileMenu, &QMenu::aboutToShow, [closeAction]() {
        if (Browser::instance().windows().count() == 1)
            closeAction->setText(tr("&Quit"));
        else
            closeAction->setText(tr("&Close Window"));
    });
    //返回创建好的第一个菜单
    return fileMenu;
}

QMenu *BrowserWindow::createViewMenu(QToolBar *toolbar)
{
    //新建第二个大菜单
    QMenu *viewMenu = new QMenu(tr("&View"));
    //添加第一个子菜单，stop功能是停止(具体是什么功能暂时不明白，深入研究之后在补充说明)
    m_stopAction = viewMenu->addAction(tr("&Stop"));
    //QList类，他相当于c上的一个链表数据结构，也是c++上的一种容器类型（个人认为）
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Period));
    shortcuts.append(Qt::Key_Escape);
    m_stopAction->setShortcuts(shortcuts);
    connect(m_stopAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Stop);
    });

    //添加第二个子菜单，重载页面
    m_reloadAction = viewMenu->addAction(tr("Reload Page"));
    m_reloadAction->setShortcuts(QKeySequence::Refresh);
    connect(m_reloadAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Reload);
    });

    /*
    * 设置页面的百分比(放大)
    * currentTab()->setZoomFactor(currentTab()->zoomFactor() + 0.1);
    * 当前tab页->设置ZoomFactor这个属性(当前tab页的ZoomFactor属性 + 0.1)
    */
    QAction *zoomIn = viewMenu->addAction(tr("Zoom &In"));
    zoomIn->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus));
    connect(zoomIn, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(currentTab()->zoomFactor() + 0.1);
    });

    //设置页面的百分比(缩小)
    QAction *zoomOut = viewMenu->addAction(tr("Zoom &Out"));
    zoomOut->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    connect(zoomOut, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(currentTab()->zoomFactor() - 0.1);
    });

    //设置页面的百分比(设置成默认的100%)
    QAction *resetZoom = viewMenu->addAction(tr("Reset &Zoom"));
    resetZoom->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(resetZoom, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(1.0);
    });

    //加一条横线
    viewMenu->addSeparator();

    //隐藏/显示ToolBar
    QAction *viewToolbarAction = new QAction(tr("Hide Toolbar"),this);
    viewToolbarAction->setShortcut(tr("Ctrl+|"));
    connect(viewToolbarAction, &QAction::triggered, [toolbar,viewToolbarAction]() {
        if (toolbar->isVisible()) {
            viewToolbarAction->setText(tr("Show Toolbar"));
            toolbar->close();
        } else {
            viewToolbarAction->setText(tr("Hide Toolbar"));
            toolbar->show();
        }
    });
    viewMenu->addAction(viewToolbarAction);

    //隐藏/显示 Status Bar (Status Bar就是网页最下面的一个Bar)
    QAction *viewStatusbarAction = new QAction(tr("Hide Status Bar"), this);
    viewStatusbarAction->setShortcut(tr("Ctrl+/"));
    connect(viewStatusbarAction, &QAction::triggered, [this, viewStatusbarAction]() {
        if (statusBar()->isVisible()) {
            viewStatusbarAction->setText(tr("Show Status Bar"));
            statusBar()->close();
        } else {
            viewStatusbarAction->setText(tr("Hide Status Bar"));
            statusBar()->show();
        }
    });
    viewMenu->addAction(viewStatusbarAction);

    //返回第二个大菜单
    return viewMenu;
}

QMenu *BrowserWindow::createWindowMenu(TabWidget *tabWidget)
{
    QMenu *menu = new QMenu(tr("&Window"));

    QAction *nextTabAction = new QAction(tr("Show Next Tab"), this);
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceRight));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageDown));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketRight));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Less));
    nextTabAction->setShortcuts(shortcuts);
    connect(nextTabAction, &QAction::triggered, tabWidget, &TabWidget::nextTab);

    QAction *previousTabAction = new QAction(tr("Show Previous Tab"), this);
    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceLeft));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageUp));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Greater));
    previousTabAction->setShortcuts(shortcuts);
    connect(previousTabAction, &QAction::triggered, tabWidget, &TabWidget::previousTab);

    connect(menu, &QMenu::aboutToShow, [this, menu, nextTabAction, previousTabAction]() {
        menu->clear();
        menu->addAction(nextTabAction);
        menu->addAction(previousTabAction);
        menu->addSeparator();

        QVector<BrowserWindow*> windows = Browser::instance().windows();
        int index(-1);
        for (auto window : windows) {
            QAction *action = menu->addAction(window->windowTitle(), this, &BrowserWindow::handleShowWindowTriggered);
            action->setData(++index);
            action->setCheckable(true);
            if (window == this)
                action->setChecked(true);
        }
    });
    return menu;
}

QMenu *BrowserWindow::createHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"));
    helpMenu->addAction(tr("About &Qt"), qApp, QApplication::aboutQt);
    return helpMenu;
}

/*
 * 创建ToolBar
*/
QToolBar *BrowserWindow::createToolBar()
{
    //右击ToolBar工具栏显示"Leo的工具栏"则几个字
    QToolBar *navigationBar = new QToolBar(tr("Leo的工具栏"));
    /*
    * 设置ToolBar的位置，默认打开的时候是在顶端的
    * 括号里设置 TopToolBarArea,Bottom,Left,Right格式如下
    * 只有设置了相应的位置拖东工具栏时，工具栏才能放到相应的位置，
    * 如果没有设置TopToolBarArea，即使默认打开是在顶端，当你拖动之后，就放不回顶端了。
    */
    navigationBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    //设置工具栏是未选中状态
    navigationBar->toggleViewAction()->setEnabled(false);

    //添加工具栏返回上一页事件
    m_historyBackAction = new QAction(this);
    //设置返回快捷键
    m_historyBackAction->setShortcuts(QKeySequence::Back);
    /*
     * 菜单项前面可以添加图标。
     * 这一点是使用QAction::setIcon()实现的。
     * 但是，如果我们将这个QAction放在菜单项中，Windows 和 KDE 平台会显示出图标；
     * Mac OS X 则不会；GNOME 则根据设置与否决定是否显示。
     * 不过，这一点也是由 Qt 帮我们完成的，我们不需要为此编写特别的代码。
     * 我们可以使用QAction::setIconVisibleInMenu(bool)
     * 或者QApplication::setAttribute(Qt::AA_DontShowIconsInMenus)来覆盖系统默认设置。
     * 默认图标在菜单中是可见的
     * 设置false是设置成不可见,true是可见
    */
    m_historyBackAction->setIconVisibleInMenu(false);
    //设置该行为的图标
    m_historyBackAction->setIcon(QIcon(QStringLiteral(":go-previous.png")));
    /*
     * 信号槽函数，用lambda表达式设置为back返回上一页的行为
     * m_tabWidget表示新建的窗口显示页
     * triggerWebPageAction(QWebEnginePage::Back)，可以理解为触发网页的方法是back，返回上一页,这个格式是固定写法记住就好，以后有用到直接拿来用就行
    */
    connect(m_historyBackAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Back);
    });
    //将设置好的back行为添加到新建的工具栏属性中
    navigationBar->addAction(m_historyBackAction);

    //添加工具栏下一页事件(参考上一页事件)
    m_historyForwardAction = new QAction(this);
    m_historyForwardAction->setShortcuts(QKeySequence::Forward);
    m_historyForwardAction->setIconVisibleInMenu(false);
    m_historyForwardAction->setIcon(QIcon(QStringLiteral(":go-next.png")));
    connect(m_historyForwardAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Forward);
    });
    navigationBar->addAction(m_historyForwardAction);

    //添加工具栏刷新页面事件
    //注意最后要转化成toInt()！！！
    m_stopReloadAction = new QAction(this);
    connect(m_stopReloadAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::WebAction(m_stopReloadAction->data().toInt()));
    });
    navigationBar->addAction(m_stopReloadAction);

    /*
    * 添加搜索框
    * sizeHint()-->获取搜索框 sizeHint 属性的值，sizeHint().height()就是获取搜索框的高
    */
    navigationBar->addWidget(m_urlLineEdit);
    int size = m_urlLineEdit->sizeHint().height();

    //设置工具栏的Icon图的尺寸的宽和高，都是搜索框的高这个值
    navigationBar->setIconSize(QSize(size, size));

    //返回一个ToolBar工具栏
    return navigationBar;
}

void BrowserWindow::handleWebViewIconChanged(const QIcon &icon)
{
    m_urlLineEdit->setFavIcon(icon);
}

void BrowserWindow::handleWebViewUrlChanged(const QUrl &url)
{
    m_urlLineEdit->setUrl(url);
    if (url.isEmpty())
        m_urlLineEdit->setFocus();
}

void BrowserWindow::handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled)
{
    switch (action) {
    case QWebEnginePage::Back:
        m_historyBackAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Forward:
        m_historyForwardAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Reload:
        m_reloadAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Stop:
        m_stopAction->setEnabled(enabled);
        break;
    default:
        qWarning("Unhandled webActionChanged singal");
    }
}

void BrowserWindow::handleWebViewTitleChanged(const QString &title)
{
    if (title.isEmpty())
        setWindowTitle(tr("QT制作的简易浏览器"));
    else
        setWindowTitle(tr("%1 - QT制作的简易浏览器").arg(title));
}

void BrowserWindow::handleNewWindowTriggered()
{
    BrowserWindow *window = new BrowserWindow();
    Browser::instance().addWindow(window);
    window->loadHomePage();
}

void BrowserWindow::handleFileOpenTriggered()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open Web Resource"), QString(),
                                                tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));
    if (file.isEmpty())
        return;
    loadPage(file);
}

/*
 * 关闭窗口时候进行的判断的槽函数
 * 弹出个警告框
 * ignore()：告诉 Qt，这个类的事件处理函数不想要处理这个事件
 * accept()：用来告诉 Qt，这个类的事件处理函数想要处理这个事件
 * isAccepted()：来查询这个事件是不是已经被接收了
 *
 * deleteLater() 是QT的产物， 而C++标准是没有的。就是在删除一个对像时，依赖它的子对象，子对象的子对象都会关联地删除。
 * 使用deleteLater主要作用还是减少内存泄露的风险。
 *
 * delete是C++与QT公用的。
 *
*/
void BrowserWindow::closeEvent(QCloseEvent *event)
{
    if (m_tabWidget->count() > 1) {
        int ret = QMessageBox::warning(this, tr("Confirm close"),
                                       tr("你确定你要关闭窗口 ?\n"
                                          "请注意，还有 %1 个窗口开着！").arg(m_tabWidget->count()),
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::No){
            event->ignore();
            return;
        }
    }
    event->accept();
    deleteLater();
}

//默认首页
void BrowserWindow::loadHomePage()
{
    loadPage(QStringLiteral("http://www.linuxkiss.com"));
}

void BrowserWindow::loadPage(const QString &page)
{
    loadPage(QUrl::fromUserInput(page));
}

//进来的是个url
void BrowserWindow::loadPage(const QUrl &url)
{
    //判断进来的url是否可用
    if (url.isValid()) {
        m_urlLineEdit->setUrl(url);
        //打开网页
        m_tabWidget->setUrl(url);
    }
}

TabWidget *BrowserWindow::tabWidget() const
{
    return m_tabWidget;
}

WebView *BrowserWindow::currentTab() const
{
    return m_tabWidget->currentWebView();
}

//加载页面时，进度条的行为
void BrowserWindow::handleWebViewLoadProgress(int progress)
{
    static QIcon stopIcon(QStringLiteral(":process-stop.png"));
    static QIcon reloadIcon(QStringLiteral(":view-refresh.png"));

    if (progress < 100 && progress > 0) {
        m_stopReloadAction->setData(QWebEnginePage::Stop);
        m_stopReloadAction->setIcon(stopIcon);
        m_stopReloadAction->setToolTip(tr("Stop loading the current page"));
    } else {
        m_stopReloadAction->setData(QWebEnginePage::Reload);
        m_stopReloadAction->setIcon(reloadIcon);
        m_stopReloadAction->setToolTip(tr("Reload the current page"));
    }
    m_progressBar->setValue(progress < 100 ? progress : 0);
}

//
void BrowserWindow::handleShowWindowTriggered()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        int offset = action->data().toInt();
        QVector<BrowserWindow*> windows = Browser::instance().windows();
        windows.at(offset)->activateWindow();
        windows.at(offset)->currentTab()->setFocus();
    }
}
