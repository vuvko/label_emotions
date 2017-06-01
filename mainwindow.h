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
    void showInstructions(void);
    void save(void);
    void load(void);

private:
    Ui::MainWindow *ui;
    QString imageFolder;
    QString resultsFile;
    QString currentImage;
    size_t current_idx;
    size_t num_images;
    QFileInfoList images;

    QStringListModel labelsM;
    QStringListModel typesM;

    bool show_intro;

    void parseLine(const QString &line);
    void updateImage(void);
};

#endif // MAINWINDOW_H
