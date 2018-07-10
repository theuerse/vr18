#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clickablelabel.h"

#include <QLabel>
#include <iostream>
#include <QColorDialog>
#include <QDirIterator>
#include <QDebug>
#include <QMessageBox>
extern "C" {
#include <curl/curl.h>
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gridLayoutCounter_row(0),
    gridLayoutCounter_col(0),
    gridLayoutMaxCols(0),
    imageWidth(120),
    imageHeight(90),
    shotpath("../VideoRetrievalBrowser/resources/shots/"),
    synsetPath(":/CNN/synset_words.txt")
{
    //Prepare database
    dataBase = new SqliteDb("../VideoRetrievalBrowser/resources/vr.db");

    // Prepare network manager
    // https://stackoverflow.com/questions/46943134/qt-http-get-request
    networkManager = new QNetworkAccessManager();
    QObject::connect(networkManager, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply)
    {
        if (reply->error()) {
            qDebug() << reply->errorString();
            return;
        }
        QString answer = reply->readAll();
        QMessageBox msgBox;
        msgBox.setText(answer);
        msgBox.setFixedHeight(240);
        msgBox.setFixedWidth(600);
        msgBox.exec();
    });

    // Prepare UI
    ui->setupUi(this);

    // Set initial value for slider labels
    ui->hsvToleranceLabel->setText(QString::number(ui->hsvToleranceSlider->value()));
    ui->numberOfResultsSliderValue->setText((QString::number(ui->numberOfResultsSlider->value())));

    // Set initial value for URL
    urlIp = ui->ipAddressLineEdit->text();

    // Prepare layout for image results
    gridLayout = new QGridLayout(ui->scrollAreaWidgetContents);
    gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    gridLayout->setHorizontalSpacing(0);
    gridLayout->setVerticalSpacing(0);

    // Prepare layout for image results
    hBoxlayout = new QHBoxLayout(ui->videoScrollAreaWidgetContents);
    hBoxlayout->setAlignment(Qt::AlignCenter | Qt::AlignCenter);

    // Setup video bar
    ui->videoFrame->setFixedHeight(imageHeight * 1.5);

    // Hide debug button
    ui->debugButton->setVisible(false);

    setupColorPicker();
    readSynSet();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayImage(imgstruct imageInfo)
{
    // Generate label and set properties
    ClickableLabel *label = new ClickableLabel(this);
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    label->imageInfo = imageInfo;
    label->setPixmap(QPixmap(shotpath + imageInfo.filename));
    label->setFixedSize(QSize(imageWidth, imageHeight));
    label->setScaledContents(true);
    label->setVisible(true);
    connect(label, SIGNAL(clicked()), this, SLOT(on_ClickableLabel_clicked()));

    /*
     * Since the width of the container is not properly reset after
     * changing window size manually, the number of images is only adjusted
     * correctly when expanding the window, not when decreasing its size.
     */
    // Calculate the number of images that fit into the window
    int containerWidth = ui->scrollAreaWidgetContents->width();
    int imageWidth = label->width();
    int spaceBetweenImages = gridLayout->horizontalSpacing();
    gridLayoutMaxCols = containerWidth / (imageWidth + spaceBetweenImages);

    // Handle the counters for image position
    if (gridLayoutCounter_col >= gridLayoutMaxCols) {
        gridLayoutCounter_col = 0;
        gridLayoutCounter_row++;
    }
    // Add label to layout
    gridLayout->addWidget(label, gridLayoutCounter_row, gridLayoutCounter_col++);
}

void MainWindow::displayVideoImage(imgstruct imageInfo)
{
    // Generate label and set properties
    ClickableLabel *label = new ClickableLabel(this);
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    label->imageInfo = imageInfo;
    label->setPixmap(QPixmap(shotpath + imageInfo.filename));
    label->setFixedSize(QSize(imageWidth, imageHeight));
    label->setScaledContents(true);
    label->setVisible(true);
    connect(label, SIGNAL(clicked()), this, SLOT(on_ClickableLabel_clicked()));

    // Add label to layout
    hBoxlayout->addWidget(label);
}

void MainWindow::deleteDisplayedImages()
{
    if (ui->scrollAreaWidgetContents->children().size() >= 1) {
        qDeleteAll(ui->scrollAreaWidgetContents->children());
        gridLayout = new QGridLayout(ui->scrollAreaWidgetContents);
        gridLayoutCounter_row = 0;
        gridLayoutCounter_col = 0;
    }
    // Clear selection
    lastSelectedImage = nullptr;
}

void MainWindow::deleteDisplayedVideoImages()
{
    if (ui->videoScrollAreaWidgetContents->children().size() >= 1) {
        qDeleteAll(ui->videoScrollAreaWidgetContents->children());
        hBoxlayout = new QHBoxLayout(ui->videoScrollAreaWidgetContents);
    }
    // Clear selection
    lastSelectedImage = nullptr;
}

void MainWindow::setupColorPicker()
{
    // Generate Colorpicker and set options
    colorPicker = new QColorDialog();
    colorPicker->setOption(QColorDialog::ColorDialogOption::DontUseNativeDialog, true);
    colorPicker->setOption(QColorDialog::ColorDialogOption::NoButtons, true);

    // Delete all the unwanted standard widgets in the left half of Colorpicker
    QWidget *tmpWidget = new QWidget(ui->colorPickerFrame);
    QLayout *tmpLayout = colorPicker->layout()->itemAt(0)->layout()->itemAt(0)->layout();
    tmpWidget->setLayout(tmpLayout);
    tmpLayout->setParent(tmpWidget);

    for (int i = 0; i < tmpLayout->count(); ++i){
        QWidget *widget = tmpLayout->itemAt(i)->widget();
        if (widget != nullptr) {delete widget;}
    }
    delete tmpWidget;

    // Trim surrounding layouts for minimal whitespace
    colorPicker->layout()->itemAt(0)->layout()->setContentsMargins(0,0,0,0);
    colorPicker->layout()->itemAt(0)->layout()->setSpacing(0);
    colorPicker->layout()->setContentsMargins(0,0,0,0);
    colorPicker->layout()->setSpacing(0);

    // Add Color picker to its place in the toolbox
    QVBoxLayout *colorPickerAreaLayout = new QVBoxLayout();
    colorPickerAreaLayout->addWidget(colorPicker);
    colorPickerAreaLayout->setContentsMargins(0,0,0,0);
    colorPickerAreaLayout->setSpacing(0);
    colorPickerAreaLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui->colorPickerFrame->setLayout(colorPickerAreaLayout);

    // Make sure the toolbox is always wide enough for color picker
    ui->toolsFrame->setFixedWidth(static_cast<int>(colorPicker->width()/1.77));
}

void MainWindow::readSynSet(){
    QFile file(synsetPath);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(nullptr, "error", file.errorString());
    }
    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        synset.push_back(line);
    }
    file.close();
}

void MainWindow::on_adjacentFramesButton_clicked()
{
    // Delete images on video bar
    deleteDisplayedVideoImages();

    // Add new images ot video bar
    std::vector<imgstruct> result = dataBase->nameSearch(ui->videoIdLabel->text());

    // Sort result
    std::sort(result.begin(), result.end());

    // Display images
    int counter = 0;
    for (imgstruct & img : result) {
        if(counter >= ui->numberOfResultsSlider->value()) {break;}
        displayVideoImage(img);
        counter++;
    }
}

void MainWindow::on_sendSelectedFrameButton_clicked()
{
    networkRequest.setUrl(QUrl(ui->urlLineEdit->text()));
    networkManager->get(networkRequest);
    ui->checkBox->setChecked(false);
}

void MainWindow::on_synSetFilterButton_clicked()
{
    // Clear previous entries
    ui->synSetFilterComboBox->clear();

    // Iterate through whole synset
    for (QString line : synset)
    {
        QStringList words = line.split(" ");
        QString filter = ui->synSetFilterlineEdit->text();
        bool containsFilter = false;
        // Search all words in a line for filter
        for (QString word : words) {
            if (word.startsWith(filter)) {
                containsFilter = true;
            }
        }
        // Add line if filter was in one of the words
        if (containsFilter) {
            ui->synSetFilterComboBox->addItem(line);
        }
    }
}

void MainWindow::on_CNNsearchButton_clicked()
{
    // Delete old images
    deleteDisplayedImages();

    // Query database
    QString line = ui->synSetFilterComboBox->currentText();
    QStringList words = line.split(" ");
    QString synsetId = words[0];
    std::cout << synsetId.toStdString() << std::endl;
    std::vector<imgstruct> result = dataBase->cnnSearch(synsetId);

    // Sort results in descending order
    std::sort(result.rbegin(), result.rend());

    // Display images
    int counter = 0;
    for (imgstruct & img : result) {
        if(counter >= ui->numberOfResultsSlider->value()) {break;}
        displayImage(img);
        counter++;
    }
}

void MainWindow::on_colorSearchButton_clicked()
{
    // Delete old images
    deleteDisplayedImages();

    // Query database
    double tol = static_cast<double>(ui->hsvToleranceSlider->value() / 100.0);
    std::vector<imgstruct> result = dataBase->hsvSearch(colorPicker->currentColor(), tol);

    // Sort results in ascending order
    std::sort(result.begin(), result.end());

    // Display images
    int counter = 0;
    for (imgstruct & img : result) {
        if(counter >= ui->numberOfResultsSlider->value()) {break;}
        displayImage(img);
        counter++;
    }
}

void MainWindow::on_edgeSearchButton_clicked()
{
    ui->edgeSearchButton->setText("not implemented");
}

void MainWindow::on_keypointSearchButton_clicked()
{
    ui->keypointSearchButton->setText("not implemented");
}

void MainWindow::on_ClickableLabel_clicked(){
    // Highlight clicked image
    ClickableLabel *selectedImage = qobject_cast<ClickableLabel*>(sender());
    selectedImage->setFrameShape(QFrame::Box);
    selectedImage->setLineWidth(3);

    // Unhighlight last clicked image (if any)
    if (lastSelectedImage != nullptr) {
        lastSelectedImage->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        lastSelectedImage->setLineWidth(1);
    }
    lastSelectedImage = selectedImage;

    // Update Values based on selected image
    imgstruct imageInfo = selectedImage->imageInfo;
    QString filename = imageInfo.filename;
    QString fileNameWithoutType = filename.split(".")[0]; // get rid of ".png"
    QStringList filenameComponents = fileNameWithoutType.split("@");
    ui->videoIdLabel->setText(filenameComponents[0]);
    ui->frameNumberLabel->setText(filenameComponents[1]);
    ui->synsetLabel->setText(imageInfo.concept);
    ui->hueLabel->setText(QString::number(imageInfo.h));
    ui->satLabel->setText(QString::number(imageInfo.s / 100.0 * 255));
    ui->valLabel->setText(QString::number(imageInfo.v / 100.0 * 255));
    urlParams = "?team=2&video=" + filenameComponents[0] + "&frame=" + filenameComponents[1];
    ui->urlLineEdit->setText(urlIp + urlParams);
}

void MainWindow::on_debugButton_clicked()
{
    // Change button text for user feedback
    ui->debugButton->setText(QString(rand()));

}




void MainWindow::on_ipAddressLineEdit_textEdited(const QString &arg1)
{
    urlIp = ui->ipAddressLineEdit->text();
    ui->urlLineEdit->setText(urlIp + urlParams);
}
