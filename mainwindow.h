#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QDirIterator>
#include <QStringListModel>
#include <QFileDialog>

#include <algorithm>

#include <QDebug>

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
    void loadFiles(void);

public slots:
    void checkLabel(void);
    void gotoImage(void);
    void nextImage(void);
    void nextUnlabeledImage(void);
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
//    QHash<QFileInfo, QVector<int>>::iterator current_it;
    QMap<QString, QVector<int>>::iterator current_it;
    size_t num_images;
//    QHash<QFileInfo, QVector<int>> images;
    QMap<QString, QVector<int>> images;

    QStringListModel labelsM;
    QStringListModel typesM;

    bool show_intro;

    QPair<QString, QVector<int>> parseLine(const QString &line);
    void updateImage(void);

    void incImage(void);
    void decImage(void);
};

//uint qHash(const QFileInfo &key, uint seed=0);

#endif // MAINWINDOW_H
