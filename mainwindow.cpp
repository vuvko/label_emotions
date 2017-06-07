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
    ui->LabelView->setModel(&labelsM);
    QStringList types;
    types << "Слабо выражено" << "Заметно выражено" << "Ярко выражено";
    typesM.setStringList(types);
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
//    images.removeFirst();
    current_idx += 1;
    ++current_it;
    updateImage();
}

void
MainWindow::prevImage(void)
{
    if (current_it == images.begin()) {
        ui->PrevButton->setEnabled(false);
    }
    current_idx -= 1;
    --current_it;
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
    int currentLabel = ui->LabelView->currentIndex().row();
    int currentType = ui->TypeView->currentIndex().row();
    QVector<int> label(2);
    label[0] = currentLabel;
    label[1] = currentType;
    *current_it = label;
    QFile results(resultsFile);
    results.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream resultsStream(&results);
    resultsStream << "# Automatically generated file, do not modify it!" << endl;
    for (auto el = images.begin(); el != images.end(); ++el) {
        if ((el.value()[0] >= 0) && (el.value()[1] >= 0)) {
            resultsStream << " - " << el.key().filePath() << ": [" << el.value()[0] << ", " <<
                             el.value()[1] << "]" << endl;
        }
    }
    results.close();
}

void
MainWindow::load(void)
{
    QDir imagesDir(imageFolder);
    for (auto img_file : imagesDir.entryInfoList(QDir::Files | QDir::Readable)) {
        images.insert(img_file, QVector<int>(2, -1));
    }
    current_it = images.begin();
    num_images = images.size();
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
        auto res = parseLine(line);
        if (res.first != "") {
            images[QFileInfo(res.first)] = res.second;
            current_idx += 1;
        }
    }
    results.close();
    current_it += current_idx;
    updateImage();
}

QPair<QString, QVector<int>>
MainWindow::parseLine(const QString &line)
{
    QRegExp splitExpr("\\s*-\\s*(\\S+):\\s*\\[(\\d)\\s*,\\s*(\\d)\\]");
    QPair<QString, QVector<int>> ret("", QVector<int>(2));
    if (splitExpr.indexIn(line) >= 0) {
        ret.first = splitExpr.cap(1);
//        images.removeOne(QFileInfo(img_name));
//        QVector<int> cur_label(2, 0);
        ret.second[0] = splitExpr.cap(2).toInt();
        ret.second[1] = splitExpr.cap(3).toInt();
//        images[QFileInfo(img_name)] = cur_label;
//        current_idx += 1;
    }
    return ret;
}

void
MainWindow::updateImage(void)
{
    ui->MarkedLabel->setText(QString("Размечено: %1/%2").arg(current_idx).arg(num_images));
//    ++current_it;
    if (current_it == images.begin()) {
        ui->PrevButton->setEnabled(false);
    } else {
        ui->PrevButton->setEnabled(true);
    }
    if (current_it == images.end()) {
        ui->Canvas->setText("Больше нет изображений");
        ui->NextButton->setEnabled(false);
    } else {
//        currentImage = images.first().filePath();
        currentImage = current_it.key().filePath();
        for (int i = 0; i < current_it.value().length(); ++i) {
            if (current_it.value()[i] < 0) {
                (*current_it)[i] = 0;
            }
        }
        ui->Canvas->setPixmap(QPixmap(currentImage));
        auto sel = labelsM.index(current_it.value()[0]);
        ui->LabelView->setCurrentIndex(sel);
        sel = typesM.index(current_it.value()[1]);
        ui->TypeView->setCurrentIndex(sel);
        ui->NextButton->setEnabled(true);
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
            "<td><img src=\":images/for_intro/angry.jpg\" width=\"100\" height=\"100\" /></td>"
            "<td><img src=\":images/for_intro/surprise.jpg\" width=\"100\" height=\"100\" /></td>"
            "</tr><tr>"
            "<td><center>Нейтральное</center></td>"
            "<td><center>Радость</center></td>"
            "<td><center>Злость</center></td>"
            "<td><center>Удивление</center></td>"
            "</tr></table>"
            "<h2>Уточнения</h2>"
            "<p>"
            "Изображения могут быть плохо различимы или изображённое лицо "
            "может быть перекрытым. В случае затруднения определения эмоции "
            "следует выбрать вариант \"Неразборчиво\" из верхнего выпадающего "
            "списка."
            "</p>"
            "<p>"
            "В случае нескольких неперекрывающих друг друга лиц, основным "
            "(по которому выбирается эмоция) считается центральное лицо. "
            "В случае затруднения определения, следует выбрать вариант "
            "\"Неразборчиво\"."
            "</p>"
            ;
    QMessageBox instructions;
    instructions.setWindowTitle("Инструкции");
    instructions.setInformativeText(text);
    instructions.exec();
    show_intro = false;
}

uint
qHash(const QFileInfo &key, uint seed)
{
    return qHash(key.filePath(), seed);
}
