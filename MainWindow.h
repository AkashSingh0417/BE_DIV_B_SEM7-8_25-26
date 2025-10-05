#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTableWidget>
#include <QLabel>
#include "ExecutableMonitorPage.h"
#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // ------------------------
    // Member variables
    // ------------------------
    QWidget *centralWidget;
    QWidget *sidebar;
    QStackedWidget *contentStack;
    bool isDarkMode;

    // Pages
    QWidget *dashboardPage;
    QWidget *urlDetectionPage;
    QWidget *analysisDetailsPage;  // NEW: Analysis details page
    ExecutableMonitorPage *executableMonitorPage; // NEW: Executable monitor page

    // Navigation buttons
    QPushButton *dashboardBtn;
    QPushButton *urlDetectionBtn;
    QPushButton *passwordManagerBtn;
    QPushButton *executableMonitorBtn;
    QPushButton *systemAnalysisBtn;
    QPushButton *networkAnalysisBtn;
    QPushButton *settingsBtn;
    QPushButton *themeToggleBtn;

    // URL Detection widgets
    QLineEdit *urlInput;
    QVBoxLayout *scanResultsLayout;
    QNetworkAccessManager *networkManager;
    QNetworkAccessManager *execNetworkManager; // for executable monitor polling
    QTimer *execPollTimer; // polling timer for /api/files
    QJsonArray execFiles; // latest files from backend
    QString execFilterText;
    
    // Analysis Details data
    QString currentAnalysisUrl;    // NEW: Store current URL being analyzed
    int currentRiskScore;          // NEW: Store risk score
    QString currentClassification; // NEW: Store classification text

    // Analysis Details widgets (Verdict & Details)
    QLabel *verdictLabel;                 // verdict text (e.g., Phishing / Legitimate)
    QLabel *verdictSubtitleLabel;         // small subtitle under verdict
    QLabel *legitimateCountLabel;         // count of legitimate factors
    QLabel *phishingCountLabel;           // count of phishing factors
    QLabel *neutralCountLabel;            // count of neutral factors
    QTableWidget *factorsTable;           // table of factors

    // ------------------------
    // Private Functions
    // ------------------------
    void setupUI();
    void createSidebar();
    void createContentStack();
    QWidget* createDashboardPage();
    QWidget* createUrlDetectionPage();
    QWidget* createAnalysisDetailsPage();  // NEW: Create analysis details page
    QWidget* createExecutableMonitorPage(); // NEW: Create executable monitor page
    void startExecPolling();
    void stopExecPolling();
    void refreshExecTable(const QJsonArray &files);
    void showExecDetailsFromObject(const QJsonObject &obj);
    void addSampleResults();
    void addScanResult(const QString &status, const QString &url, const QString &type);
    void setActiveNavButton(QPushButton *activeBtn);
    void applyDarkTheme();
    void applyLightTheme();
    
    // NEW: Helper functions for analysis details
    QWidget* createFeatureItem(const QString &title, const QString &description, 
                               const QString &status); // status: "safe", "warning", "danger"
    QWidget* createThreatItem(const QString &icon, const QString &title);
    QWidget* createActionItem(const QString &icon, const QString &title, const QString &description);

private slots:
    void showDashboard();
    void showUrlDetection();
    void showAnalysisDetails(const QString &url, int riskScore); // NEW: Show analysis with data
    void onUrlDetectionClicked();
    void onPasswordManagerClicked();
    void onExecutableMonitorClicked();
    void onSystemAnalysisClicked();
    void onNewScanClicked();
    void onExportReportClicked();
    void onScanUrlClicked();
    void onThemeToggle();
    void onBackButtonClicked();  // NEW: Back button handler
    void onAnalyzeUrlFinished(QNetworkReply *reply);
    // Executable monitor placeholders
    void onExecMonitoringToggled(bool enabled);
    void onExecFilterChanged(const QString &text);
    void onExecItemActivated(const QString &exeName);
    void onExecPollFinished(QNetworkReply *reply);
};

#endif // MAINWINDOW_H