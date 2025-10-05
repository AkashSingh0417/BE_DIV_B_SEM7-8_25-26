#include "MainWindow.h"
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QMessageBox>
#include <QFrame>
#include <QGridLayout>
#include <QLineEdit>
#include <QEvent>
#include <functional>
#include <random> 
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include "ExecutableMonitorPage.h"

// ==============================
// ClickableFrame Event Filter
// ==============================
class ClickableFrame : public QObject {
    Q_OBJECT
public:
    ClickableFrame(QObject *parent, std::function<void()> callback) 
        : QObject(parent), m_callback(callback) {}
    
protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::MouseButtonRelease) {
            m_callback();
            return true;
        }
        return QObject::eventFilter(obj, event);
    }
    
private:
    std::function<void()> m_callback;
};

// ==============================
// Constructor & Destructor
// ==============================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isDarkMode(true), currentRiskScore(0)
{
    setupUI();
    networkManager = new QNetworkAccessManager(this);
    execNetworkManager = new QNetworkAccessManager(this);
    execPollTimer = new QTimer(this);
    execPollTimer->setInterval(2000);
    connect(execPollTimer, &QTimer::timeout, this, [this]() {
        QNetworkRequest req(QUrl("http://127.0.0.1:8000/api/files"));
        execNetworkManager->get(req);
    });
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onAnalyzeUrlFinished);
    connect(execNetworkManager, &QNetworkAccessManager::finished, this, &MainWindow::onExecPollFinished);
    applyDarkTheme();
}

MainWindow::~MainWindow() {}

// ==============================
// UI Setup
// ==============================

void MainWindow::setupUI() {
    centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);

    QHBoxLayout *horizontalLayout = new QHBoxLayout(centralWidget);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    horizontalLayout->setSpacing(0);

    createSidebar();
    createContentStack();

    horizontalLayout->addWidget(sidebar);
    horizontalLayout->addWidget(contentStack, 1);

    setWindowTitle("SecureGuard");
    resize(1200, 800);
}

void MainWindow::createSidebar() {
    sidebar = new QWidget();
    sidebar->setFixedWidth(256);
    sidebar->setObjectName("sidebar");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    // Header
    QWidget *header = new QWidget();
    header->setFixedHeight(64);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(16, 0, 16, 0);
    
    QLabel *icon = new QLabel("🔒");
    icon->setFont(QFont("Arial", 20));
    
    QLabel *title = new QLabel("SecureGuard");
    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(14);
    title->setFont(titleFont);
    
    headerLayout->addWidget(icon);
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    
    sidebarLayout->addWidget(header);

    // Navigation
    QWidget *navWidget = new QWidget();
    QVBoxLayout *navLayout = new QVBoxLayout(navWidget);
    navLayout->setSpacing(8);
    navLayout->setContentsMargins(16, 16, 16, 16);

    dashboardBtn = new QPushButton("  Dashboard");
    dashboardBtn->setObjectName("activeNavBtn");
    dashboardBtn->setFixedHeight(40);
    connect(dashboardBtn, &QPushButton::clicked, this, &MainWindow::showDashboard);

    urlDetectionBtn = new QPushButton("  URL Detection");
    urlDetectionBtn->setObjectName("navBtn");
    urlDetectionBtn->setFixedHeight(40);
    connect(urlDetectionBtn, &QPushButton::clicked, this, &MainWindow::showUrlDetection);

    passwordManagerBtn = new QPushButton("  Password Manager");
    passwordManagerBtn->setObjectName("navBtn");
    passwordManagerBtn->setFixedHeight(40);
    connect(passwordManagerBtn, &QPushButton::clicked, this, &MainWindow::onPasswordManagerClicked);

    executableMonitorBtn = new QPushButton("  Executable Monitor");
    executableMonitorBtn->setObjectName("navBtn");
    executableMonitorBtn->setFixedHeight(40);
    connect(executableMonitorBtn, &QPushButton::clicked, this, &MainWindow::onExecutableMonitorClicked);

    systemAnalysisBtn = new QPushButton("  System Analysis");
    systemAnalysisBtn->setObjectName("navBtn");
    systemAnalysisBtn->setFixedHeight(40);
    connect(systemAnalysisBtn, &QPushButton::clicked, this, &MainWindow::onSystemAnalysisClicked);

    networkAnalysisBtn = new QPushButton("  Network Analysis");
    networkAnalysisBtn->setObjectName("navBtn");
    networkAnalysisBtn->setFixedHeight(40);

    navLayout->addWidget(dashboardBtn);
    navLayout->addWidget(urlDetectionBtn);
    navLayout->addWidget(passwordManagerBtn);
    navLayout->addWidget(executableMonitorBtn);
    navLayout->addWidget(systemAnalysisBtn);
    navLayout->addWidget(networkAnalysisBtn);
    navLayout->addStretch();

    sidebarLayout->addWidget(navWidget, 1);

    // Footer
    QWidget *footer = new QWidget();
    QVBoxLayout *footerLayout = new QVBoxLayout(footer);
    footerLayout->setContentsMargins(16, 16, 16, 16);

    QPushButton *chromeExtBtn = new QPushButton("🧩 Get Chrome Extension");
    chromeExtBtn->setObjectName("chromeExtBtn");
    chromeExtBtn->setFixedHeight(44);

    settingsBtn = new QPushButton("  Settings");
    settingsBtn->setObjectName("navBtn");
    settingsBtn->setFixedHeight(40);

    themeToggleBtn = new QPushButton("Toggle Theme");
    themeToggleBtn->setFixedHeight(32);
    connect(themeToggleBtn, &QPushButton::clicked, this, &MainWindow::onThemeToggle);

    footerLayout->addWidget(chromeExtBtn);
    footerLayout->addWidget(settingsBtn);
    footerLayout->addWidget(themeToggleBtn);

    sidebarLayout->addWidget(footer);
}

void MainWindow::createContentStack() {
    contentStack = new QStackedWidget();
    
    dashboardPage = createDashboardPage();
    urlDetectionPage = createUrlDetectionPage();
    analysisDetailsPage = createAnalysisDetailsPage();
    executableMonitorPage = static_cast<ExecutableMonitorPage*>(createExecutableMonitorPage());
    
    contentStack->addWidget(dashboardPage);
    contentStack->addWidget(urlDetectionPage);
    contentStack->addWidget(analysisDetailsPage);
    contentStack->addWidget(executableMonitorPage);
    
    contentStack->setCurrentWidget(dashboardPage);
}

QWidget* MainWindow::createDashboardPage() {
    QWidget *page = new QWidget();
    page->setObjectName("mainContent");

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollWidget);
    contentLayout->setContentsMargins(32, 32, 32, 32);
    contentLayout->setSpacing(32);

    // Header Section
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);

    QVBoxLayout *titleLayout = new QVBoxLayout();
    QLabel *pageTitle = new QLabel("Dashboard");
    QFont pageTitleFont;
    pageTitleFont.setBold(true);
    pageTitleFont.setPointSize(20);
    pageTitle->setFont(pageTitleFont);

    QLabel *subtitle = new QLabel("Overview of your security status and recent activity.");
    subtitle->setObjectName("subtitle");

    titleLayout->addWidget(pageTitle);
    titleLayout->addWidget(subtitle);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QPushButton *newScanBtn = new QPushButton("+ New Scan");
    newScanBtn->setObjectName("secondaryBtn");
    newScanBtn->setFixedHeight(40);
    connect(newScanBtn, &QPushButton::clicked, this, &MainWindow::onNewScanClicked);

    QPushButton *exportBtn = new QPushButton("↓ Export Report");
    exportBtn->setObjectName("infoBtn");
    exportBtn->setFixedHeight(40);
    connect(exportBtn, &QPushButton::clicked, this, &MainWindow::onExportReportClicked);

    buttonLayout->addWidget(newScanBtn);
    buttonLayout->addWidget(exportBtn);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    headerLayout->addLayout(buttonLayout);

    contentLayout->addWidget(headerWidget);

    // Quick Access Section
    QLabel *quickAccessTitle = new QLabel("Quick Access");
    QFont sectionFont;
    sectionFont.setBold(true);
    sectionFont.setPointSize(14);
    quickAccessTitle->setFont(sectionFont);
    contentLayout->addWidget(quickAccessTitle);

    QGridLayout *cardGrid = new QGridLayout();
    cardGrid->setSpacing(24);

    QStringList cardTitles = {"URL Detection", "Password Manager", "Executable Monitor", "System Analysis"};
    QStringList cardDescriptions = {
        "Scan URLs for malicious content.",
        "Manage and secure your passwords.",
        "Monitor running processes.",
        "Analyze system performance."
    };

    for (int i = 0; i < 4; i++) {
        QFrame *card = new QFrame();
        card->setObjectName("card");
        card->setFixedHeight(150);

        QVBoxLayout *cardLayout = new QVBoxLayout(card);

        QLabel *cardTitle = new QLabel(cardTitles[i]);
        QFont cardTitleFont;
        cardTitleFont.setBold(true);
        cardTitleFont.setPointSize(11);
        cardTitle->setFont(cardTitleFont);

        QLabel *cardDesc = new QLabel(cardDescriptions[i]);
        cardDesc->setObjectName("cardDesc");
        cardDesc->setWordWrap(true);

        QPushButton *launchBtn = new QPushButton("Launch →");
        launchBtn->setObjectName("launchBtn");
        
        if (i == 0) {
            connect(launchBtn, &QPushButton::clicked, this, &MainWindow::showUrlDetection);
        }

        cardLayout->addWidget(cardTitle);
        cardLayout->addWidget(cardDesc);
        cardLayout->addStretch();
        cardLayout->addWidget(launchBtn);

        cardGrid->addWidget(card, i / 2, i % 2);
    }

    contentLayout->addLayout(cardGrid);

    // Recent Activity Section
    QLabel *activityTitle = new QLabel("Recent Activity");
    activityTitle->setFont(sectionFont);
    contentLayout->addWidget(activityTitle);

    QFrame *activityFrame = new QFrame();
    activityFrame->setObjectName("activityFrame");
    QVBoxLayout *activityLayout = new QVBoxLayout(activityFrame);
    activityLayout->setSpacing(0);

    QStringList activities = {
        "Malicious URL detected and blocked|2 hours ago|High Priority",
        "Password Manager updated|Yesterday|Completed",
        "System Analysis completed|2 days ago|Completed",
        "URL Detection scan finished|3 days ago|3 issues found"
    };

    for (const QString &activity : activities) {
        QStringList parts = activity.split("|");

        QWidget *activityItem = new QWidget();
        activityItem->setObjectName("activityItem");
        activityItem->setFixedHeight(70);

        QHBoxLayout *itemLayout = new QHBoxLayout(activityItem);

        QLabel *icon = new QLabel("🔒");
        icon->setFixedSize(40, 40);

        QVBoxLayout *textLayout = new QVBoxLayout();
        QLabel *activityText = new QLabel(parts[0]);
        activityText->setObjectName("activityText");
        QLabel *timeText = new QLabel(parts[1]);
        timeText->setObjectName("timeText");

        textLayout->addWidget(activityText);
        textLayout->addWidget(timeText);

        QLabel *statusLabel = new QLabel(parts[2]);
        statusLabel->setObjectName("statusLabel");

        itemLayout->addWidget(icon);
        itemLayout->addLayout(textLayout);
        itemLayout->addStretch();
        itemLayout->addWidget(statusLabel);

        activityLayout->addWidget(activityItem);
    }

    contentLayout->addWidget(activityFrame);
    contentLayout->addStretch();

    scrollArea->setWidget(scrollWidget);

    QVBoxLayout *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->addWidget(scrollArea);
    
    return page;
}

QWidget* MainWindow::createExecutableMonitorPage() {
    ExecutableMonitorPage *page = new ExecutableMonitorPage();
    // Wire signals
    connect(page, &ExecutableMonitorPage::monitoringToggled, this, &MainWindow::onExecMonitoringToggled);
    connect(page, &ExecutableMonitorPage::filterChanged, this, &MainWindow::onExecFilterChanged);
    connect(page, &ExecutableMonitorPage::itemActivated, this, &MainWindow::onExecItemActivated);
    // Seed with some sample rows similar to the provided HTML
    QList<QStringList> rows = {
        {"svchost.exe", "Safe", "1h ago"},
        {"malicious_payload.exe", "Critical", "2h ago"},
        {"explorer.exe", "Safe", ""},
        {"unknown_installer.msi", "Suspicious", "5h ago"}
    };
    page->setDetectedFiles(rows);
    return page;
}

void MainWindow::onExecPollFinished(QNetworkReply *reply) {
    if (!reply) return;
    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return;
    }
    if (reply->url().toString().endsWith("/api/files")) {
        const QByteArray data = reply->readAll();
        QJsonParseError err{};
        QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if (err.error == QJsonParseError::NoError && doc.isArray()) {
            execFiles = doc.array();
            QList<QStringList> rows;
            for (const QJsonValue &v : execFiles) {
                QJsonObject obj = v.toObject();
                const QString name = obj.value("name").toString();
                if (!execFilterText.isEmpty() && !name.contains(execFilterText, Qt::CaseInsensitive)) continue;
                const QString type = obj.value("type").toString();
                const QString when = obj.value("details").toObject().value("created_at").toString();
                QString status;
                if (type.compare("suspicious", Qt::CaseInsensitive) == 0) status = "Suspicious";
                else if (type.compare("error", Qt::CaseInsensitive) == 0) status = "Error";
                else status = "Safe";
                rows.append({name, status, when});
            }
            if (executableMonitorPage) executableMonitorPage->setDetectedFiles(rows);
        }
    }
    reply->deleteLater();
}

// ==============================
// Executable monitor slots (placeholders)
// ==============================
void MainWindow::onExecMonitoringToggled(bool enabled) {
    Q_UNUSED(enabled);
    // TODO: Will connect to backend process monitoring toggle
}

void MainWindow::onExecFilterChanged(const QString &text) {
    execFilterText = text;
    // Re-render using cached execFiles
    QList<QStringList> rows;
    for (const QJsonValue &v : execFiles) {
        QJsonObject obj = v.toObject();
        const QString name = obj.value("name").toString();
        if (!execFilterText.isEmpty() && !name.contains(execFilterText, Qt::CaseInsensitive)) continue;
        const QString type = obj.value("type").toString();
        const QString when = obj.value("details").toObject().value("created_at").toString();
        QString status;
        if (type.compare("suspicious", Qt::CaseInsensitive) == 0) status = "Suspicious";
        else if (type.compare("error", Qt::CaseInsensitive) == 0) status = "Error";
        else status = "Safe";
        rows.append({name, status, when});
    }
    if (executableMonitorPage) executableMonitorPage->setDetectedFiles(rows);
}

void MainWindow::onExecItemActivated(const QString &exeName) {
    // Find the object and populate right panel
    for (const QJsonValue &v : execFiles) {
        QJsonObject obj = v.toObject();
        if (obj.value("name").toString().compare(exeName, Qt::CaseInsensitive) == 0) {
            // Map details to page
            const QJsonObject d = obj.value("details").toObject();
            if (executableMonitorPage) {
                QStringList suspiciousStrings;
                for (const QJsonValue &sv : d.value("suspicious_strings").toArray()) {
                    suspiciousStrings.append(sv.toString());
                }
                executableMonitorPage->setAnalysisDetails(
                    obj.value("name").toString(),
                    obj.value("path").toString(),
                    obj.value("type").toString().toUpper(),
                    d.value("ext").toString().toUpper(),
                    d.value("size").toString(),
                    d.value("rule").toString(),
                    QStringList(),
                    QStringList(),
                    d.value("mime").toString(),
                    d.value("hash").toString(),
                    d.value("sha256").toString().left(32) + "...",
                    suspiciousStrings
                );
            }
            break;
        }
    }
}

QWidget* MainWindow::createUrlDetectionPage() {
    QWidget *page = new QWidget();
    page->setObjectName("mainContent");
    
    QVBoxLayout *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(32, 32, 32, 32);
    pageLayout->setSpacing(32);
    
    // Header
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    
    QLabel *pageTitle = new QLabel("URL Detection");
    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(24);
    pageTitle->setFont(titleFont);
    
    QPushButton *helpBtn = new QPushButton("?");
    helpBtn->setObjectName("helpBtn");
    helpBtn->setFixedSize(40, 40);
    
    QLabel *avatar = new QLabel();
    avatar->setFixedSize(40, 40);
    avatar->setStyleSheet("background-color: #EF7722; border-radius: 20px;");
    
    headerLayout->addWidget(pageTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(helpBtn);
    headerLayout->addWidget(avatar);
    
    pageLayout->addWidget(headerWidget);
    
    // URL Input Section
    QWidget *inputWidget = new QWidget();
    QHBoxLayout *inputLayout = new QHBoxLayout(inputWidget);
    inputLayout->setSpacing(16);
    
    urlInput = new QLineEdit();
    urlInput->setPlaceholderText("Enter URL to scan");
    urlInput->setObjectName("urlInput");
    urlInput->setFixedHeight(48);
    
    QPushButton *scanBtn = new QPushButton("Scan URL");
    scanBtn->setObjectName("scanBtn");
    scanBtn->setFixedHeight(48);
    scanBtn->setMinimumWidth(120);
    connect(scanBtn, &QPushButton::clicked, this, &MainWindow::onScanUrlClicked);
    
    inputLayout->addWidget(urlInput, 1);
    inputLayout->addWidget(scanBtn);
    
    pageLayout->addWidget(inputWidget);
    
    // Scan Results Section
    QLabel *resultsTitle = new QLabel("Scan Results");
    QFont sectionFont;
    sectionFont.setBold(true);
    sectionFont.setPointSize(16);
    resultsTitle->setFont(sectionFont);
    pageLayout->addWidget(resultsTitle);
    
    // Scroll area for results
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget *scrollWidget = new QWidget();
    scanResultsLayout = new QVBoxLayout(scrollWidget);
    scanResultsLayout->setSpacing(8);
    
    addSampleResults();
    
    scanResultsLayout->addStretch();
    scrollArea->setWidget(scrollWidget);
    
    pageLayout->addWidget(scrollArea, 1);
    
    return page;
}

QWidget* MainWindow::createAnalysisDetailsPage() {
    QWidget *page = new QWidget();
    page->setObjectName("mainContent");
    
    QVBoxLayout *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(32, 32, 32, 32);
    pageLayout->setSpacing(24);
    
    // Header with back button
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    
    QPushButton *backBtn = new QPushButton("←");
    backBtn->setObjectName("backBtn");
    backBtn->setFixedSize(40, 40);
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::onBackButtonClicked);
    
    QLabel *pageTitle = new QLabel("Analysis Details");
    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(24);
    pageTitle->setFont(titleFont);
    
    QPushButton *helpBtn = new QPushButton("?");
    helpBtn->setObjectName("helpBtn");
    helpBtn->setFixedSize(40, 40);
    
    QLabel *avatar = new QLabel();
    avatar->setFixedSize(40, 40);
    avatar->setStyleSheet("background-color: #EF7722; border-radius: 20px;");
    
    headerLayout->addWidget(backBtn);
    headerLayout->addWidget(pageTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(helpBtn);
    headerLayout->addWidget(avatar);
    
    pageLayout->addWidget(headerWidget);
    
    // Scroll area for content
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollWidget);
    contentLayout->setSpacing(24);
    
    // Verdict & Details only
    QFrame *verdictFrame = new QFrame();
    verdictFrame->setObjectName("analysisCard");
    QVBoxLayout *verdictLayout = new QVBoxLayout(verdictFrame);

    QLabel *verdictHeader = new QLabel("Verdict & Details");
    QFont verdictHeaderFont;
    verdictHeaderFont.setBold(true);
    verdictHeaderFont.setPointSize(16);
    verdictHeader->setFont(verdictHeaderFont);
    verdictLayout->addWidget(verdictHeader);

    QHBoxLayout *verdictRow = new QHBoxLayout();
    verdictLabel = new QLabel("Phishing");
    QFont verdictFont; verdictFont.setBold(true); verdictFont.setPointSize(18);
    verdictLabel->setFont(verdictFont);
    verdictLabel->setStyleSheet("color: #EF4444;");
    verdictSubtitleLabel = new QLabel("This URL is identified as a phishing attempt.");
    verdictSubtitleLabel->setObjectName("subtitle");

    QVBoxLayout *verdictTextCol = new QVBoxLayout();
    verdictTextCol->addWidget(verdictLabel);
    verdictTextCol->addWidget(verdictSubtitleLabel);
    verdictRow->addLayout(verdictTextCol, 1);

    QHBoxLayout *statCards = new QHBoxLayout();
    statCards->setSpacing(12);

    auto makeStat = [&](const QString &colorHex, QLabel* &countLabel, const QString &title){
        QFrame *card = new QFrame();
        card->setObjectName("analysisCard");
        QVBoxLayout *cl = new QVBoxLayout(card);
        countLabel = new QLabel("0");
        QFont f; f.setBold(true); f.setPointSize(18); countLabel->setFont(f);
        countLabel->setStyleSheet(QString("color:%1;").arg(colorHex));
        QLabel *t = new QLabel(title);
        t->setStyleSheet(QString("color:%1; font-weight:600; font-size:12px;").arg(colorHex));
        t->setWordWrap(true);
        cl->addWidget(countLabel);
        cl->addWidget(t);
        cl->setAlignment(Qt::AlignCenter);
        card->setMinimumWidth(120);
        statCards->addWidget(card);
    };

    makeStat("#22C55E", legitimateCountLabel, "Legitimate Factors");
    makeStat("#EF4444", phishingCountLabel, "Phishing Factors");
    makeStat("#F59E0B", neutralCountLabel, "Neutral Factors");

    verdictRow->addLayout(statCards);
    verdictLayout->addLayout(verdictRow);
    contentLayout->addWidget(verdictFrame);

    // Factors Table
    QFrame *tableFrame = new QFrame();
    tableFrame->setObjectName("analysisCard");
    QVBoxLayout *tableLayout = new QVBoxLayout(tableFrame);
    QLabel *tableTitle = new QLabel("Factors");
    QFont tableFont; tableFont.setBold(true); tableFont.setPointSize(14); tableTitle->setFont(tableFont);
    tableLayout->addWidget(tableTitle);
    factorsTable = new QTableWidget(0, 3);
    QStringList headers; headers << "Factor" << "Status" << "Description";
    factorsTable->setHorizontalHeaderLabels(headers);
    factorsTable->horizontalHeader()->setStretchLastSection(true);
    factorsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    factorsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    factorsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    factorsTable->setSelectionMode(QAbstractItemView::NoSelection);
    factorsTable->setFocusPolicy(Qt::NoFocus);
    tableLayout->addWidget(factorsTable);
    contentLayout->addWidget(tableFrame);
    
    scrollArea->setWidget(scrollWidget);
    pageLayout->addWidget(scrollArea);
    
    return page;
}

// ==============================
// Helper Functions
// ==============================

QWidget* MainWindow::createFeatureItem(const QString &title, const QString &description, const QString &status) {
    QFrame *item = new QFrame();
    item->setObjectName("featureItem");
    item->setFixedHeight(70);
    
    QHBoxLayout *layout = new QHBoxLayout(item);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(12);
    
    QLabel *icon = new QLabel();
    icon->setFixedSize(24, 24);
    icon->setAlignment(Qt::AlignCenter);
    
    QString iconStyle;
    QString iconText;
    if (status == "safe") {
        iconStyle = "background-color: rgba(34, 197, 94, 0.1); color: #22C55E; border-radius: 12px; font-size: 16px; font-weight: bold;";
        iconText = "✓";
    } else if (status == "danger") {
        iconStyle = "background-color: rgba(239, 68, 68, 0.1); color: #EF4444; border-radius: 12px; font-size: 16px; font-weight: bold;";
        iconText = "✗";
    } else {
        iconStyle = "background-color: rgba(250, 165, 51, 0.2); color: #FAA533; border-radius: 12px; font-size: 16px; font-weight: bold;";
        iconText = "⚠";
    }
    
    icon->setStyleSheet(iconStyle);
    icon->setText(iconText);
    
    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(2);
    
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("featureTitle");
    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(10);
    titleLabel->setFont(titleFont);
    
    QLabel *descLabel = new QLabel(description);
    descLabel->setObjectName("featureDesc");
    descLabel->setWordWrap(true);
    
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(descLabel);
    
    layout->addWidget(icon);
    layout->addLayout(textLayout, 1);
    
    return item;
}

QWidget* MainWindow::createThreatItem(const QString &icon, const QString &title) {
    QWidget *item = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(item);
    layout->setContentsMargins(0, 8, 0, 8);
    layout->setSpacing(12);
    
    QLabel *iconLabel = new QLabel(icon);
    iconLabel->setFont(QFont("Arial", 18));
    iconLabel->setFixedSize(24, 24);
    
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("threatTitle");
    QFont titleFont;
    titleFont.setPointSize(11);
    titleLabel->setFont(titleFont);
    
    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addStretch();
    
    return item;
}

QWidget* MainWindow::createActionItem(const QString &icon, const QString &title, const QString &description) {
    QWidget *item = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(item);
    layout->setContentsMargins(0, 8, 0, 8);
    layout->setSpacing(12);
    
    QLabel *iconLabel = new QLabel(icon);
    iconLabel->setFont(QFont("Arial", 18));
    iconLabel->setFixedSize(24, 24);
    iconLabel->setAlignment(Qt::AlignTop);
    
    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(4);
    
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("actionTitle");
    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(11);
    titleLabel->setFont(titleFont);
    
    QLabel *descLabel = new QLabel(description);
    descLabel->setObjectName("actionDesc");
    descLabel->setWordWrap(true);
    
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(descLabel);
    
    layout->addWidget(iconLabel);
    layout->addLayout(textLayout, 1);
    
    return item;
}

void MainWindow::addSampleResults() {
    addScanResult("Safe", "https://example.com/login", "safe");
    addScanResult("Malicious", "https://malicious.site/phishing", "malicious");
    addScanResult("Suspicious", "https://suspicious.link/unknown", "suspicious");
}

void MainWindow::addScanResult(const QString &status, const QString &url, const QString &type) {
    QFrame *resultItem = new QFrame();
    resultItem->setObjectName("scanResultItem");
    resultItem->setFixedHeight(80);
    resultItem->setCursor(Qt::PointingHandCursor);
    
    QHBoxLayout *itemLayout = new QHBoxLayout(resultItem);
    itemLayout->setContentsMargins(16, 12, 16, 12);
    
    QLabel *iconLabel = new QLabel();
    iconLabel->setFixedSize(48, 48);
    iconLabel->setAlignment(Qt::AlignCenter);
    
    QString iconStyle;
    QString iconText;
    int riskScore = 0;
    
    if (type == "safe") {
        iconStyle = "background-color: rgba(34, 197, 94, 0.1); color: #22C55E; border-radius: 8px;";
        iconText = "✓";
        riskScore = 25;
    } else if (type == "malicious") {
        iconStyle = "background-color: rgba(239, 68, 68, 0.1); color: #EF4444; border-radius: 8px;";
        iconText = "✗";
        riskScore = 85;
    } else {
        iconStyle = "background-color: rgba(250, 165, 51, 0.2); color: #FAA533; border-radius: 8px;";
        iconText = "⚠";
        riskScore = 55;
    }
    
    iconLabel->setStyleSheet(iconStyle);
    QFont iconFont;
    iconFont.setPointSize(20);
    iconFont.setBold(true);
    iconLabel->setFont(iconFont);
    iconLabel->setText(iconText);
    
    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(4);
    
    QLabel *statusLabel = new QLabel(status);
    statusLabel->setObjectName("resultStatus");
    QFont statusFont;
    statusFont.setBold(true);
    statusFont.setPointSize(11);
    statusLabel->setFont(statusFont);
    
    if (type == "safe") {
        statusLabel->setStyleSheet("color: #22C55E;");
    } else if (type == "malicious") {
        statusLabel->setStyleSheet("color: #EF4444;");
    } else {
        statusLabel->setStyleSheet("color: #FAA533;");
    }
    
    QLabel *urlLabel = new QLabel(url);
    urlLabel->setObjectName("resultUrl");
    
    textLayout->addWidget(statusLabel);
    textLayout->addWidget(urlLabel);
    
    QLabel *arrowLabel = new QLabel("→");
    arrowLabel->setObjectName("resultArrow");
    
    itemLayout->addWidget(iconLabel);
    itemLayout->addLayout(textLayout, 1);
    itemLayout->addWidget(arrowLabel);
    
    resultItem->installEventFilter(new ClickableFrame(resultItem, [this, url, riskScore]() {
        showAnalysisDetails(url, riskScore);
    }));
    
    scanResultsLayout->insertWidget(scanResultsLayout->count() - 1, resultItem);
}

void MainWindow::setActiveNavButton(QPushButton *activeBtn) {
    dashboardBtn->setObjectName("navBtn");
    urlDetectionBtn->setObjectName("navBtn");
    passwordManagerBtn->setObjectName("navBtn");
    executableMonitorBtn->setObjectName("navBtn");
    systemAnalysisBtn->setObjectName("navBtn");
    networkAnalysisBtn->setObjectName("navBtn");
    
    activeBtn->setObjectName("activeNavBtn");
    
    if (isDarkMode) {
        applyDarkTheme();
    } else {
        applyLightTheme();
    }
}

// ==============================
// Slots
// ==============================

void MainWindow::showDashboard() {
    contentStack->setCurrentWidget(dashboardPage);
    setActiveNavButton(dashboardBtn);
    if (execPollTimer && execPollTimer->isActive()) execPollTimer->stop();
}

void MainWindow::showUrlDetection() {
    contentStack->setCurrentWidget(urlDetectionPage);
    setActiveNavButton(urlDetectionBtn);
    if (execPollTimer && execPollTimer->isActive()) execPollTimer->stop();
}

void MainWindow::showAnalysisDetails(const QString &url, int riskScore) {
    currentAnalysisUrl = url;
    currentRiskScore = riskScore;
    
    QList<QLabel*> labels = analysisDetailsPage->findChildren<QLabel*>();
    for (QLabel *label : labels) {
        if (label->property("currentUrl").toBool()) {
            label->setText(url);
        }
        if (label->property("currentScore").toBool()) {
            label->setText(QString::number(riskScore));
        }
        if (label->property("currentLevel").toBool()) {
            if (riskScore >= 70) {
                label->setText("High Risk");
                label->setStyleSheet("color: #EF4444; font-weight: bold;");
            } else if (riskScore >= 40) {
                label->setText("Medium Risk");
                label->setStyleSheet("color: #FAA533; font-weight: bold;");
            } else {
                label->setText("Low Risk");
                label->setStyleSheet("color: #22C55E; font-weight: bold;");
            }
        }
    }
    
    contentStack->setCurrentWidget(analysisDetailsPage);
}

void MainWindow::onBackButtonClicked() {
    showUrlDetection();
}

void MainWindow::onUrlDetectionClicked() {
    showUrlDetection();
}

void MainWindow::onPasswordManagerClicked() {
    QMessageBox::information(this, "Password Manager", "Password Manager feature will be implemented here.");
}

void MainWindow::onExecutableMonitorClicked() {
    if (executableMonitorPage) {
        contentStack->setCurrentWidget(executableMonitorPage);
        setActiveNavButton(executableMonitorBtn);
        if (execPollTimer && !execPollTimer->isActive()) execPollTimer->start();
    }
}

void MainWindow::onSystemAnalysisClicked() {
    QMessageBox::information(this, "System Analysis", "System Analysis feature will be implemented here.");
}

void MainWindow::onNewScanClicked() {
    showUrlDetection();
}

void MainWindow::onExportReportClicked() {
    QMessageBox::information(this, "Export Report", "Exporting security report...");
}

void MainWindow::onScanUrlClicked() {
    QString url = urlInput->text().trimmed();

    if (url.isEmpty()) {
        QMessageBox::warning(this, "Empty URL", "Please enter a URL to scan.");
        return;
    }

    // Build JSON payload
    QJsonObject payload;
    payload["url"] = url;
    QJsonDocument doc(payload);

    // Send POST to FastAPI
    QNetworkRequest req(QUrl("http://127.0.0.1:8000/analyze_url"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->post(req, doc.toJson());
}

void MainWindow::onAnalyzeUrlFinished(QNetworkReply *reply) {
    if (!reply) return;
    QByteArray data = reply->readAll();
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, "Scan Error", QString("Request failed: %1").arg(reply->errorString()));
        return;
    }

    QJsonParseError parseError{};
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        QMessageBox::critical(this, "Scan Error", "Invalid JSON response from server.");
        return;
    }

    QJsonObject obj = doc.object();
    const QString classification = obj.value("classification").toString();
    const QString conclusion = obj.value("conclusion").toString();
    const QJsonArray features = obj.value("features_table").toArray();
    const QString scannedUrl = urlInput ? urlInput->text().trimmed() : QString();

    QString type;
    QString status;
    int risk = 0;
    if (classification.compare("Legitimate", Qt::CaseInsensitive) == 0) {
        type = "safe";
        status = "Safe";
        risk = 20;
    } else if (classification.compare("Phishing", Qt::CaseInsensitive) == 0) {
        type = "malicious";
        status = "Malicious";
        risk = 85;
    } else {
        type = "suspicious";
        status = "Suspicious";
        risk = 55;
    }

    addScanResult(status, scannedUrl.isEmpty() ? "(unknown)" : scannedUrl, type);

    // Populate Analysis Details page with server data
    // Update URL
    QList<QLabel*> labels = analysisDetailsPage->findChildren<QLabel*>();
    for (QLabel *label : labels) {
        if (label->property("currentUrl").toBool()) {
            label->setText(scannedUrl);
        }
    }

    // Update verdict labels
    currentClassification = classification;
    if (verdictLabel) {
        if (classification.compare("Phishing", Qt::CaseInsensitive) == 0) {
            verdictLabel->setText("Phishing");
            verdictLabel->setStyleSheet("color:#EF4444; font-weight:700;");
            if (verdictSubtitleLabel) verdictSubtitleLabel->setText("This URL is identified as a phishing attempt.");
        } else {
            verdictLabel->setText("Legitimate");
            verdictLabel->setStyleSheet("color:#22C55E; font-weight:700;");
            if (verdictSubtitleLabel) verdictSubtitleLabel->setText("This URL appears legitimate.");
        }
    }

    // Fill table and counts
    int legit = 0, phish = 0, neutral = 0;
    factorsTable->setRowCount(0);
    auto addRow = [&](const QString &factor, const QString &statusText, const QString &desc, const QString &badgeColor){
        int r = factorsTable->rowCount();
        factorsTable->insertRow(r);
        auto *fItem = new QTableWidgetItem(factor);
        auto *sItem = new QTableWidgetItem(statusText);
        auto *dItem = new QTableWidgetItem(desc);
        sItem->setData(Qt::UserRole, badgeColor);
        factorsTable->setItem(r, 0, fItem);
        factorsTable->setItem(r, 1, sItem);
        factorsTable->setItem(r, 2, dItem);
    };

    for (const QJsonValue &v : features) {
        QJsonObject fo = v.toObject();
        const QString factor = fo.value("feature").toString();
        const QString meaning = fo.value("description").toString();
        QString statusText;
        QString badgeColor;
        if (meaning.startsWith("✅")) { statusText = "Legitimate"; badgeColor = "#22C55E"; legit++; }
        else if (meaning.startsWith("⚠️")) { statusText = "Phishing"; badgeColor = "#EF4444"; phish++; }
        else { statusText = "Neutral"; badgeColor = "#F59E0B"; neutral++; }
        addRow(factor, statusText, meaning, badgeColor);
    }

    if (legitimateCountLabel) legitimateCountLabel->setText(QString::number(legit));
    if (phishingCountLabel) phishingCountLabel->setText(QString::number(phish));
    if (neutralCountLabel) neutralCountLabel->setText(QString::number(neutral));

    // Resize columns to fit content
    factorsTable->resizeColumnsToContents();

    // Navigate to details page populated with data
    showAnalysisDetails(scannedUrl, risk);

    if (!conclusion.isEmpty()) {
        QMessageBox::information(this, "Scan Complete", conclusion);
    }
    if (urlInput) urlInput->clear();
}

void MainWindow::onThemeToggle() {
    isDarkMode = !isDarkMode;
    if (isDarkMode)
        applyDarkTheme();
    else
        applyLightTheme();
}

// ==============================
// Styling
// ==============================

void MainWindow::applyDarkTheme() {
    QString darkStyle = R"(
        QMainWindow, QWidget { background-color: #1A1A1A; color: #E5E5E5; }
        #sidebar { background-color: rgba(0, 0, 0, 0.2); border-right: 1px solid #333; }
        #leftSidebar { background-color: #24282F; border-right: 1px solid #3A3F4B; }
        #mainContent { background-color: #1C1C1C; }
        #activeNavBtn { background-color: rgba(239, 119, 34, 0.1); color: #EF7722; border: none; border-radius: 8px; text-align: left; padding-left: 12px; font-weight: bold; }
        #navBtn { background-color: transparent; color: #999; border: none; border-radius: 8px; text-align: left; padding-left: 12px; }
        #navBtn:hover { background-color: rgba(255, 255, 255, 0.05); }
        #chromeExtBtn { background-color: #0BA6DF; color: white; border: none; border-radius: 8px; font-weight: 600; }
        #chromeExtBtn:hover { background-color: #0A95CE; }
        #secondaryBtn { background-color: #FAA533; color: white; border: none; border-radius: 8px; padding: 8px 16px; font-weight: bold; }
        #secondaryBtn:hover { background-color: #E89422; }
        #infoBtn { background-color: #0BA6DF; color: white; border: none; border-radius: 8px; padding: 8px 16px; font-weight: bold; }
        #infoBtn:hover { background-color: #0A95CE; }
        #scanBtn { background-color: #EF7722; color: white; border: none; border-radius: 8px; font-weight: bold; }
        #scanBtn:hover { background-color: #E06611; }
        #backBtn { background-color: rgba(255, 255, 255, 0.05); color: #E5E5E5; border: none; border-radius: 8px; font-size: 20px; font-weight: bold; }
        #backBtn:hover { background-color: rgba(255, 255, 255, 0.1); }
        #helpBtn { background-color: transparent; color: #999; border: none; border-radius: 20px; font-size: 18px; font-weight: bold; }
        #helpBtn:hover { background-color: rgba(255, 255, 255, 0.05); color: #EF7722; }
        #card { background-color: rgba(0, 0, 0, 0.3); border-radius: 12px; padding: 16px; }
        #card:hover { background-color: rgba(0, 0, 0, 0.4); }
        #cardDesc { color: #999; font-size: 12px; }
        #launchBtn { background-color: transparent; color: #EF7722; border: none; text-align: left; padding: 0; font-weight: bold; }
        #analysisCard { background-color: rgba(0, 0, 0, 0.3); border-radius: 12px; padding: 16px; border: 1px solid rgba(255, 255, 255, 0.05); }
        #riskScoreCard { background-color: rgba(0, 0, 0, 0.3); border-radius: 12px; padding: 24px; border: 1px solid rgba(255, 255, 255, 0.05); }
        #riskScoreLabel { color: #EF4444; }
        #riskLevelLabel { color: #999; font-size: 14px; font-weight: 600; }
        #featureItem { background-color: rgba(0, 0, 0, 0.2); border-radius: 8px; border: 1px solid rgba(255, 255, 255, 0.05); }
        #featureTitle { color: #E5E5E5; font-weight: bold; }
        #featureDesc { color: #999; font-size: 11px; }
        #urlInput { background-color: rgba(0, 0, 0, 0.2); border: 1px solid #333; border-radius: 8px; padding-left: 16px; color: #E5E5E5; }
        #urlInput:focus { border: 2px solid #EF7722; outline: none; }
        #scanResultItem { background-color: rgba(0, 0, 0, 0.2); border-radius: 12px; border: 1px solid transparent; }
        #scanResultItem:hover { background-color: rgba(0, 0, 0, 0.3); border: 1px solid rgba(239, 119, 34, 0.3); }
        #resultUrl { color: #999; font-size: 12px; }
        #resultArrow { color: #666; font-size: 18px; }
        #subtitle { color: #999; }
        #sectionLabel { color: #999; font-size: 12px; }
        #urlValueLabel { color: #EF7722; font-size: 13px; font-weight: 500; }
        #threatTitle { color: #EF4444; font-weight: 600; }
        #actionTitle { color: #E5E5E5; font-weight: bold; }
        #actionDesc { color: #999; font-size: 11px; }
        #activityFrame { background-color: rgba(0, 0, 0, 0.3); border-radius: 12px; }
        #activityItem { border-bottom: 1px solid rgba(255, 255, 255, 0.05); padding: 8px; }
        #activityItem:hover { background-color: rgba(255, 255, 255, 0.05); }
        #activityText { font-weight: bold; }
        #timeText { color: #999; font-size: 11px; }
        #statusLabel { color: #999; font-size: 12px; }
        QScrollArea { border: none; }

        /* Table styling for Executable Monitor */
        QTableWidget#detectedTable { background-color: transparent; gridline-color: transparent; }
        QTableWidget#detectedTable::item { padding: 8px; }
        QHeaderView::section { background-color: rgba(255,255,255,0.05); color: #E5E5E5; border: none; padding: 8px; font-weight: 600; }
        QTableCornerButton::section { background-color: transparent; border: none; }
        QTableView { alternate-background-color: rgba(255,255,255,0.03); selection-background-color: rgba(239,119,34,0.2); selection-color: #E5E5E5; }
    )";
    setStyleSheet(darkStyle);
}

void MainWindow::applyLightTheme() {
    QString lightStyle = R"(
        QMainWindow, QWidget { background-color: #EBEBEB; color: #333; }
        #sidebar { background-color: white; border-right: 1px solid #E0E0E0; }
        #leftSidebar { background-color: #F8F9FB; border-right: 1px solid #E0E0E0; }
        #mainContent { background-color: #EBEBEB; }
        #activeNavBtn { background-color: rgba(239, 119, 34, 0.1); color: #EF7722; border: none; border-radius: 8px; text-align: left; padding-left: 12px; font-weight: bold; }
        #navBtn { background-color: transparent; color: #666; border: none; border-radius: 8px; text-align: left; padding-left: 12px; }
        #navBtn:hover { background-color: #F5F5F5; }
        #chromeExtBtn { background-color: #0BA6DF; color: white; border: none; border-radius: 8px; font-weight: 600; }
        #chromeExtBtn:hover { background-color: #0A95CE; }
        #secondaryBtn { background-color: #FAA533; color: white; border: none; border-radius: 8px; padding: 8px 16px; font-weight: bold; }
        #secondaryBtn:hover { background-color: #E89422; }
        #infoBtn { background-color: #0BA6DF; color: white; border: none; border-radius: 8px; padding: 8px 16px; font-weight: bold; }
        #infoBtn:hover { background-color: #0A95CE; }
        #scanBtn { background-color: #EF7722; color: white; border: none; border-radius: 8px; font-weight: bold; }
        #scanBtn:hover { background-color: #E06611; }
        #backBtn { background-color: #F5F5F5; color: #333; border: none; border-radius: 8px; font-size: 20px; font-weight: bold; }
        #backBtn:hover { background-color: #E0E0E0; }
        #helpBtn { background-color: transparent; color: #666; border: none; border-radius: 20px; font-size: 18px; font-weight: bold; }
        #helpBtn:hover { background-color: #F5F5F5; color: #EF7722; }
        #card { background-color: rgba(255, 255, 255, 0.8); border-radius: 12px; padding: 16px; }
        #card:hover { background-color: white; }
        #cardDesc { color: #666; font-size: 12px; }
        #launchBtn { background-color: transparent; color: #EF7722; border: none; text-align: left; padding: 0; font-weight: bold; }
        #analysisCard { background-color: white; border-radius: 12px; padding: 16px; border: 1px solid #E0E0E0; }
        #riskScoreCard { background-color: white; border-radius: 12px; padding: 24px; border: 1px solid #E0E0E0; }
        #riskScoreLabel { color: #EF4444; }
        #riskLevelLabel { color: #666; font-size: 14px; font-weight: 600; }
        #featureItem { background-color: #F9F9F9; border-radius: 8px; border: 1px solid #E0E0E0; }
        #featureTitle { color: #333; font-weight: bold; }
        #featureDesc { color: #666; font-size: 11px; }
        #urlInput { background-color: white; border: 1px solid #DDD; border-radius: 8px; padding-left: 16px; color: #333; }
        #urlInput:focus { border: 2px solid #EF7722; outline: none; }
        #scanResultItem { background-color: white; border-radius: 12px; border: 1px solid #E0E0E0; }
        #scanResultItem:hover { background-color: #F9F9F9; border: 1px solid #EF7722; }
        #resultUrl { color: #666; font-size: 12px; }
        #resultArrow { color: #999; font-size: 18px; }
        #subtitle { color: #666; }
        #sectionLabel { color: #666; font-size: 12px; }
        #urlValueLabel { color: #EF7722; font-size: 13px; font-weight: 500; }
        #threatTitle { color: #EF4444; font-weight: 600; }
        #actionTitle { color: #333; font-weight: bold; }
        #actionDesc { color: #666; font-size: 11px; }
        #activityFrame { background-color: rgba(255, 255, 255, 0.8); border-radius: 12px; }
        #activityItem { border-bottom: 1px solid #E0E0E0; padding: 8px; }
        #activityItem:hover { background-color: rgba(0, 0, 0, 0.02); }
        #activityText { font-weight: bold; }
        #timeText { color: #666; font-size: 11px; }
        #statusLabel { color: #666; font-size: 12px; }
        QScrollArea { border: none; }

        /* Table styling for Executable Monitor */
        QTableWidget#detectedTable { background-color: white; gridline-color: #EEE; }
        QTableWidget#detectedTable::item { padding: 8px; }
        QHeaderView::section { background-color: #F3F4F6; color: #333; border: 1px solid #E5E7EB; padding: 8px; font-weight: 600; }
        QTableCornerButton::section { background-color: #F3F4F6; border: 1px solid #E5E7EB; }
        QTableView { alternate-background-color: #FAFAFA; selection-background-color: rgba(239,119,34,0.1); selection-color: #333; }
    )";
    setStyleSheet(lightStyle);
}

#include "MainWindow.moc"