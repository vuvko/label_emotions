#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList labels;
    labels << "Неразборчиво" << "Нейтральное" << "Радость" << "Страх" << "Удивление"
           << "Грусть" << "Отвращение" << "Злость";
    labelsM.setStringList(labels);
//    ui->LabelBox->addItems(labels);
    ui->LabelView->setModel(&labelsM);
    QStringList types;
    types << "Слабо выражено" << "Заметно выражено" << "Ярко выражено";
    typesM.setStringList(types);
//    ui->TypeBox->addItems(types);
    ui->TypeView->setModel(&typesM);
    imageFolder = "images";
    resultsFile = "current_results.yml";
    show_intro = false;
    current_idx = 0;
    if (!QFileInfo(resultsFile).exists()) {
        QFile results(resultsFile);
        results.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream resultsStream(&results);
        resultsStream << "# Automatically generated file, do not modify it!" << endl;
        results.close();
        show_intro = true;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void
MainWindow::nextImage(void)
{
    save();
    if (images.isEmpty()) {
        return;
    }
    images.removeFirst();
    updateImage();
}

bool
MainWindow::showIntro(void)
{
    return show_intro;
}

void
MainWindow::save(void)
{
    if (currentImage.isEmpty()) {
        return;
    }
//    int currentLabel = ui->LabelBox->currentIndex();
//    int currentType = ui->TypeBox->currentIndex();
    int currentLabel = ui->LabelView->currentIndex().row();
    int currentType = ui->TypeView->currentIndex().row();
    QFile results(resultsFile);
    results.open(QIODevice::Append | QIODevice::Text);
    QTextStream resultsStream(&results);
    resultsStream << " - " << currentImage << ": [" << currentLabel << ", " << currentType << "]" << endl;
    results.close();
}

void
MainWindow::load(void)
{
    QDir imagesDir(imageFolder);
    images = imagesDir.entryInfoList(QDir::Files | QDir::Readable);
    num_images = images.length();
    QFile results(resultsFile);
    results.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream resultsStream(&results);
    while (!resultsStream.atEnd()) {
        QString line = resultsStream.readLine();
        auto comment_idx = line.indexOf('#');
        if (comment_idx >= 0) {
            line = line.remove(comment_idx, line.length() - comment_idx);
        }
        if (line.length() <= 0) {
            continue;
        }
        parseLine(line);
    }
    results.close();
    updateImage();
}

void
MainWindow::parseLine(const QString &line)
{
    QRegExp splitExpr("\\s*-\\s*(\\S+):.+");
    if (splitExpr.indexIn(line) >= 0) {
        QString img_name = splitExpr.cap(1);
        images.removeOne(QFileInfo(img_name));
        current_idx += 1;
    }
}

void
MainWindow::updateImage(void)
{
    current_idx += 1;
    ui->MarkedLabel->setText(QString("Размечено: %1/%2").arg(current_idx).arg(num_images));
    if (images.isEmpty()) {
        ui->Canvas->setText("Больше нет изображений");
        ui->NextButton->setEnabled(false);
    } else {
        currentImage = images.first().filePath();
        ui->Canvas->setPixmap(QPixmap(currentImage));
//        ui->LabelBox->setCurrentIndex(0);
//        ui->TypeBox->setCurrentIndex(0);
        auto sel = labelsM.index(0);
        ui->LabelView->setCurrentIndex(sel);
        sel = typesM.index(0);
        ui->TypeView->setCurrentIndex(sel);
    }
}

void
MainWindow::showInstructions(void)
{
    QString text = "<h2>Инструкции по разметке</h2>"
            "<p>"
            "При запуске приложения в интерфейсе появится первое изображение "
            "из коллекции для разметки. Справа от изображения есть два "
            "выпадающих списка: верхний для разметки эмоции, нижний для "
            "выраженности эмоции. Для разметки текущего изображения "
            "следует выбрать сначала эмоцию, которая соответствует "
            "отображаемому лицу, затем выбрать степень выраженности "
            "эмоции. По окончанию выбора следует нажать на кнопку "
            "\"Следующая\", находяющуюся в нижнем правом углу интерфейса."
            "</p>"
            "<h2>Примеры</h2>"
            "<p>"
            "Для наглядности ниже продемонстрированы несколько примеров разных эмоций."
            "</p>"
            "<table><tr>"
            "<td><img src=\":images/for_intro/neutral.jpg\" width=\"100\" height=\"100\" /></td>"
            "<td><img src=\":images/for_intro/smile.jpg\" width=\"100\" height=\"100\" /></td>"
            "<td><img src=\":images/for_intro/disgust.jpg\" width=\"100\" height=\"100\" /></td>"
            "<td><img src=\":images/for_intro/surprise.jpg\" width=\"100\" height=\"100\" /></td>"
            "</tr><tr>"
            "<td><center>Нейтральное</center></td>"
            "<td><center>Радость</center></td>"
            "<td><center>Отвращение</center></td>"
            "<td><center>Удивление</center></td>"
            "</tr></table>"
            "<h2>Уточнения</h2>"
            "<p>"
            "Изображения могут быть плохо различимы или изображённое лицо "
            "может быть перекрытым. В случае затруднения определения эмоции "
            "следует выбрать вариант \"Неразборчиво\" из верхнего выпадающего "
            "списка."
            "</p>"
            ;
    QMessageBox instructions;
    instructions.setWindowTitle("Инструкции");
    instructions.setInformativeText(text);
    instructions.exec();
    show_intro = false;
}
