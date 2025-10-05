#include "ExecutableMonitorPage.h"
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QHeaderView>

ExecutableMonitorPage::ExecutableMonitorPage(QWidget *parent)
    : QWidget(parent), monitorToggle(nullptr), filterInput(nullptr), detectedTable(nullptr),
      selectedNameLabel(nullptr), selectedPathLabel(nullptr), riskLevelLabel(nullptr),
      fileTypeLabel(nullptr), fileSizeLabel(nullptr), detectionLabel(nullptr),
      findingsContainer(nullptr), recommendationsContainer(nullptr),
      mimeLabel(nullptr), md5Label(nullptr), sha256Label(nullptr), stringsContainer(nullptr)
{
    this->setObjectName("execMonitorPage");
    QHBoxLayout *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QWidget *left = buildLeftPanel();
    QWidget *right = buildRightPanel();

    left->setFixedWidth(420);
    rootLayout->addWidget(left);
    rootLayout->addWidget(right, 1);
}

QWidget* ExecutableMonitorPage::buildLeftPanel() {
    QWidget *panel = new QWidget();
    panel->setObjectName("leftSidebar");
    QVBoxLayout *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(32, 32, 24, 32);
    layout->setSpacing(16);

    // Header
    QHBoxLayout *header = new QHBoxLayout();
    QLabel *title = new QLabel("Executable Monitor");
    QFont titleFont; titleFont.setBold(true); titleFont.setPointSize(20); title->setFont(titleFont);
    header->addWidget(title);
    header->addStretch();
    layout->addLayout(header);

    // Monitoring toggle row
    QFrame *monitorRow = new QFrame();
    monitorRow->setObjectName("analysisCard");
    QHBoxLayout *monitorLayout = new QHBoxLayout(monitorRow);
    QLabel *status = new QLabel("Monitoring Active");
    monitorToggle = new QCheckBox();
    monitorToggle->setChecked(true);
    connect(monitorToggle, &QCheckBox::toggled, this, &ExecutableMonitorPage::monitoringToggled);
    monitorLayout->addWidget(status);
    monitorLayout->addStretch();
    monitorLayout->addWidget(monitorToggle);
    layout->addWidget(monitorRow);

    // Filter input
    filterInput = new QLineEdit();
    filterInput->setPlaceholderText("Filter files...");
    filterInput->setObjectName("urlInput");
    connect(filterInput, &QLineEdit::textChanged, this, &ExecutableMonitorPage::filterChanged);
    layout->addWidget(filterInput);

    // Detected files table
    detectedTable = new QTableWidget(0, 3);
    detectedTable->setObjectName("detectedTable");
    QStringList headers; headers << "Executable" << "Status" << "When";
    detectedTable->setHorizontalHeaderLabels(headers);
    detectedTable->horizontalHeader()->setStretchLastSection(true);
    detectedTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    detectedTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    detectedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    detectedTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    detectedTable->setSelectionMode(QAbstractItemView::SingleSelection);
    detectedTable->setFocusPolicy(Qt::NoFocus);
    detectedTable->setShowGrid(false);
    detectedTable->verticalHeader()->setVisible(false);
    detectedTable->horizontalHeader()->setVisible(true);
    detectedTable->setAlternatingRowColors(true);
    detectedTable->setStyleSheet("QTableWidget#detectedTable { background: transparent; } ");
    layout->addWidget(detectedTable, 1);

    connect(detectedTable, &QTableWidget::cellDoubleClicked, this, [this](int row, int){
        const QString name = detectedTable->item(row, 0) ? detectedTable->item(row, 0)->text() : QString();
        emit itemActivated(name);
    });

    return panel;
}

QWidget* ExecutableMonitorPage::buildRightPanel() {
    QWidget *panel = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(32, 32, 32, 32);
    layout->setSpacing(16);

    // Header area with selected file
    QLabel *selTitle = new QLabel("Analysis");
    QFont t; t.setBold(true); t.setPointSize(18); selTitle->setFont(t);
    layout->addWidget(selTitle);

    QFrame *summaryRow = new QFrame();
    summaryRow->setObjectName("analysisCard");
    QGridLayout *grid = new QGridLayout(summaryRow);
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(8);

    selectedNameLabel = new QLabel("malicious_payload.exe");
    selectedPathLabel = new QLabel("C:/Users/Admin/Downloads/malicious_payload.exe");
    selectedPathLabel->setObjectName("subtitle");

    riskLevelLabel = new QLabel("CRITICAL");
    fileTypeLabel = new QLabel("EXE");
    fileSizeLabel = new QLabel("2.3 MB");
    detectionLabel = new QLabel("AI + Rules");

    grid->addWidget(selectedNameLabel, 0, 0, 1, 2);
    grid->addWidget(selectedPathLabel, 1, 0, 1, 2);

    grid->addWidget(new QLabel("Risk Level"), 2, 0);
    grid->addWidget(riskLevelLabel, 3, 0);
    grid->addWidget(new QLabel("File Type"), 2, 1);
    grid->addWidget(fileTypeLabel, 3, 1);
    grid->addWidget(new QLabel("File Size"), 2, 2);
    grid->addWidget(fileSizeLabel, 3, 2);
    grid->addWidget(new QLabel("Detection"), 2, 3);
    grid->addWidget(detectionLabel, 3, 3);

    layout->addWidget(summaryRow);

    // Gemini AI Analysis block
    QFrame *aiFrame = new QFrame();
    aiFrame->setObjectName("analysisCard");
    QVBoxLayout *aiLayout = new QVBoxLayout(aiFrame);
    QLabel *aiTitle = new QLabel("Gemini AI Analysis");
    QFont f; f.setBold(true); f.setPointSize(14); aiTitle->setFont(f);
    aiLayout->addWidget(aiTitle);
    QLabel *aiStatus = new QLabel("Analyzing...");
    aiLayout->addWidget(aiStatus);
    layout->addWidget(aiFrame);

    // File features block
    QFrame *featuresFrame = new QFrame();
    featuresFrame->setObjectName("analysisCard");
    QVBoxLayout *featuresLayout = new QVBoxLayout(featuresFrame);
    QLabel *featuresTitle = new QLabel("File Features");
    featuresTitle->setFont(f);
    featuresLayout->addWidget(featuresTitle);

    mimeLabel = new QLabel("MIME: application/vnd.microsoft.portable-executable");
    md5Label = new QLabel("MD5: d41d8cd98f00b204e9800998ecf8427e");
    sha256Label = new QLabel("SHA256: e3b0c44298fc1c149afbf4c8...");
    featuresLayout->addWidget(mimeLabel);
    featuresLayout->addWidget(md5Label);
    featuresLayout->addWidget(sha256Label);

    QLabel *stringsTitle = new QLabel("Suspicious Strings");
    featuresLayout->addWidget(stringsTitle);
    stringsContainer = new QWidget();
    QVBoxLayout *stringsLayout = new QVBoxLayout(stringsContainer);
    stringsLayout->setContentsMargins(0,0,0,0);
    stringsLayout->setSpacing(2);
    stringsLayout->addWidget(new QLabel("CreateRemoteThread"));
    stringsLayout->addWidget(new QLabel("SetWindowsHookExA"));
    featuresLayout->addWidget(stringsContainer);

    layout->addWidget(featuresFrame);

    return panel;
}

void ExecutableMonitorPage::setDetectedFiles(const QList<QStringList> &rows) {
    detectedTable->setRowCount(0);
    for (const QStringList &r : rows) {
        int row = detectedTable->rowCount();
        detectedTable->insertRow(row);
        for (int c = 0; c < qMin(3, r.size()); ++c) {
            detectedTable->setItem(row, c, new QTableWidgetItem(r[c]));
        }
    }
}

void ExecutableMonitorPage::setAnalysisDetails(const QString &fileName,
                                               const QString &filePath,
                                               const QString &riskLevel,
                                               const QString &fileType,
                                               const QString &fileSize,
                                               const QString &detection,
                                               const QStringList &keyFindings,
                                               const QStringList &recommendations,
                                               const QString &mime,
                                               const QString &md5,
                                               const QString &sha256,
                                               const QStringList &suspiciousStrings)
{
    Q_UNUSED(keyFindings);
    Q_UNUSED(recommendations);
    if (selectedNameLabel) selectedNameLabel->setText(fileName);
    if (selectedPathLabel) selectedPathLabel->setText(filePath);
    if (riskLevelLabel) riskLevelLabel->setText(riskLevel);
    if (fileTypeLabel) fileTypeLabel->setText(fileType);
    if (fileSizeLabel) fileSizeLabel->setText(fileSize);
    if (detectionLabel) detectionLabel->setText(detection);
    if (mimeLabel) mimeLabel->setText(QString("MIME: %1").arg(mime));
    if (md5Label) md5Label->setText(QString("MD5: %1").arg(md5));
    if (sha256Label) sha256Label->setText(QString("SHA256: %1").arg(sha256));

    // Replace strings list
    if (stringsContainer) {
        auto layout = qobject_cast<QVBoxLayout*>(stringsContainer->layout());
        if (layout) {
            QLayoutItem *child;
            while ((child = layout->takeAt(0)) != nullptr) {
                delete child->widget();
                delete child;
            }
            for (const QString &s : suspiciousStrings) {
                layout->addWidget(new QLabel(s));
            }
        }
    }
}


