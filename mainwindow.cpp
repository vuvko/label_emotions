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
    resultsFile = "images_results.yml";
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
MainWindow::checkLabel(void)
{
    if (current_it != images.end()) {
        if (current_it.value()[0] == 0) {
            ++current_idx;
        }
        save();
    }
    nextImage();
}

void
MainWindow::gotoImage(void)
{
    if (images.isEmpty()) {
        return;
    }
    int image_idx = ui->imageIndexBox->value();
    if (image_idx >= images.size()) {
        image_idx = images.size() - 1;
    }
    current_it = images.begin() + image_idx;
    updateImage();
}

void
MainWindow::nextUnlabeledImage(void)
{
    if (images.isEmpty()) {
        return;
    }
    int image_idx = 1;
    for (current_it = images.begin();
         current_it != images.end();
         ++current_it, ++image_idx) {
        if (current_it.value()[0] == 0) {
            break;
        }
    }
    ui->imageIndexBox->setValue(image_idx);
    updateImage();
}

void
MainWindow::nextImage(void)
{
    if (images.isEmpty()) {
        return;
    }
    incImage();
    updateImage();
}

void
MainWindow::prevImage(void)
{
    if (current_it == images.begin()) {
        ui->PrevButton->setEnabled(false);
    }
    if (current_it != images.end()) {
        save();
        if (current_it.value()[0] == 0) {
            ++current_idx;
        }
    }
    decImage();
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
    QVector<int> label(3);
    label[0] = 1;
    label[1] = currentLabel;
    label[2] = currentType;
    if (label == current_it.value()) {
        return;
    }
    *current_it = label;
    QFile results(resultsFile);
    results.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream resultsStream(&results);
    resultsStream << "# Automatically generated file, do not modify it!" << endl;
    for (auto el = images.begin(); el != images.end(); ++el) {
        if (el.value()[0]) {
            resultsStream << " - " << el.key() << ": [" << el.value()[1] << ", " <<
                             el.value()[2] << "]" << endl;
        }
    }
    results.close();
}

void
MainWindow::load(void)
{
    QString newImageFolder = QFileDialog::getExistingDirectory(this, tr("Выбрать директорию"),
                                                    imageFolder,
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (newImageFolder.isEmpty() || newImageFolder == "") {
        return;
    }
    imageFolder = newImageFolder;
    QStringList path_parts = imageFolder.split(QDir::separator(), QString::SkipEmptyParts);
    resultsFile = path_parts[path_parts.length() - 1] + QString("_results.yml");
    loadFiles();
}

void
MainWindow::loadFiles(void)
{
    current_idx = 0;
    images.clear();
    QDirIterator dirIt(imageFolder, QDirIterator::Subdirectories);
    while(dirIt.hasNext()) {
        dirIt.next();
        if (QFileInfo(dirIt.filePath()).isFile()) {
            images.insert(dirIt.filePath(), QVector<int>(3, 0));
        }
    }
    current_it = images.begin();
    num_images = images.size();
    ui->imageIndexBox->setMaximum(num_images);
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
            QVector<int> label(3, 1);
            label[1] = res.second[0];
            label[2] = res.second[1];
            images[res.first] = label;
            ++current_idx;
        }
    }
    results.close();
    int image_idx = 1;
    while((current_it != images.end()) && (current_it.value()[0] >= 1)) {
        ++current_it;
        ++image_idx;
    }
    ui->imageIndexBox->setValue(image_idx);
    updateImage();
}

QPair<QString, QVector<int>>
MainWindow::parseLine(const QString &line)
{
    QRegExp splitExpr("\\s*-\\s*(\\S+):\\s*\\[(\\d)\\s*,\\s*(\\d)\\]");
    QPair<QString, QVector<int>> ret("", QVector<int>(2));
    if (splitExpr.indexIn(line) >= 0) {
        ret.first = splitExpr.cap(1);
        ret.second[0] = splitExpr.cap(2).toInt();
        ret.second[1] = splitExpr.cap(3).toInt();
    }
    return ret;
}

void
MainWindow::updateImage(void)
{
    ui->MarkedLabel->setText(QString("Размечено: %1/%2").arg(current_idx).arg(num_images));
    if (current_it == images.begin()) {
        ui->PrevButton->setEnabled(false);
    } else {
        ui->PrevButton->setEnabled(true);
    }
    if (current_it == images.end()) {
        ui->Canvas->setText("Больше нет изображений");
        ui->NextButton->setEnabled(false);
    } else {
        currentImage = current_it.key();
        ui->Canvas->setPixmap(QPixmap(currentImage));
        auto sel = labelsM.index(current_it.value()[1]);
        ui->LabelView->setCurrentIndex(sel);
        sel = typesM.index(current_it.value()[2]);
        ui->TypeView->setCurrentIndex(sel);
        ui->NextButton->setEnabled(true);
    }
}

void
MainWindow::incImage(void)
{
    ++current_it;
    int image_idx = ui->imageIndexBox->value();
    ui->imageIndexBox->setValue(image_idx + 1);
}

void
MainWindow::decImage(void)
{
    --current_it;
    int image_idx = ui->imageIndexBox->value();
    ui->imageIndexBox->setValue(image_idx - 1);
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
