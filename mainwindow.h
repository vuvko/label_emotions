#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QStringListModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool showIntro(void);

public slots:
    void nextImage(void);
    void prevImage(void);
    void showInstructions(void);
    void save(void);
    void load(void);

private:
    Ui::MainWindow *ui;
    QString imageFolder;
    QString resultsFile;
    QString currentImage;
    size_t current_idx;
    QHash<QFileInfo, QVector<int>>::iterator current_it;
    size_t num_images;
    QHash<QFileInfo, QVector<int>> images;

    QStringListModel labelsM;
    QStringListModel typesM;

    bool show_intro;

    QPair<QString, QVector<int>> parseLine(const QString &line);
    void updateImage(void);
};

uint qHash(const QFileInfo &key, uint seed=0);

#endif // MAINWINDOW_H
