#ifndef EXECUTABLEMONITORPAGE_H
#define EXECUTABLEMONITORPAGE_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QStringList>

class QTableWidget;
class QLineEdit;
class QCheckBox;
class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;

class ExecutableMonitorPage : public QWidget {
    Q_OBJECT
public:
    explicit ExecutableMonitorPage(QWidget *parent = nullptr);

signals:
    void monitoringToggled(bool enabled);
    void filterChanged(const QString &text);
    void itemActivated(const QString &executableName);

public slots:
    void setDetectedFiles(const QList<QStringList> &rows); // each: [name, status, time]
    void setAnalysisDetails(const QString &fileName,
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
                            const QStringList &suspiciousStrings);

private:
    QWidget *buildLeftPanel();
    QWidget *buildRightPanel();

    // Left panel
    QCheckBox *monitorToggle;
    QLineEdit *filterInput;
    QTableWidget *detectedTable;

    // Right panel (summary widgets)
    QLabel *selectedNameLabel;
    QLabel *selectedPathLabel;
    QLabel *riskLevelLabel;
    QLabel *fileTypeLabel;
    QLabel *fileSizeLabel;
    QLabel *detectionLabel;

    // Sections
    QWidget *findingsContainer;
    QWidget *recommendationsContainer;
    QLabel *mimeLabel;
    QLabel *md5Label;
    QLabel *sha256Label;
    QWidget *stringsContainer;
};

#endif // EXECUTABLEMONITORPAGE_H


