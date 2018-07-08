#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clickablelabel.h"

#include <QLabel>
#include <iostream>
#include <QColorDialog>
#include <QDirIterator>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gridLayoutCounter_row(0),
    gridLayoutCounter_col(0),
    gridLayoutMaxCols(0),
    shotpath(":/shots/"),
    synsetPath(":/CNN/synset_words.txt")
{
    // Prepare UI
    ui->setupUi(this);

    // Set initial value for hsv tolerance
    ui->hsvToleranceLabel->setText(QString::number(ui->hsvToleranceSlider->value()));

    // Prepare layout for image results
    gridLayout = new QGridLayout(ui->scrollAreaWidgetContents);
    gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    gridLayout->setHorizontalSpacing(0);
    gridLayout->setVerticalSpacing(0);

    setupColorPicker();
    readSynSet();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayImage(QString filename)
{
    // Generate label and set properties
    ClickableLabel *label = new ClickableLabel(this);
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    label->filename = filename;
    label->setPixmap(QPixmap(shotpath + filename));
    label->setFixedSize(QSize(120, 90));
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
    ui->adjacentFramesButton->setText("not implemented");
}

void MainWindow::on_sendSelectedFrameButton_clicked()
{
    ui->sendSelectedFrameButton->setText("not implemented");
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
    QString line = ui->synSetFilterComboBox->currentText();
    QStringList words = line.split(" ");
    std::cout << line.toStdString() << "\t (" << words[0].toStdString() << ")" << std::endl;
    on_debugButton_clicked();
}

void MainWindow::on_colorSearchButton_clicked()
{
    ui->colorSearchButton->setText("not implemented");
    on_debugButton_clicked();
    std::cout << "H:" << colorPicker->currentColor().hue()
              << "\tS:" << colorPicker->currentColor().saturation()
              << "\tV:" << colorPicker->currentColor().value()
              << "\tTol:" << static_cast<double>((ui->hsvToleranceSlider->value() / 100.0))
              << std::endl;
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
    QString filename = selectedImage->filename;
    QString fileNameWithoutType = filename.split(".")[0]; // get rid of ".png"
    QStringList filenameComponents = fileNameWithoutType.split("@");
    ui->videoIdLabel->setText(filenameComponents[0]);
    ui->frameNumberLabel->setText(filenameComponents[1]);
    QString url = "http://demo2.itec.aau.at:80/vbs/aau/submit?team=2&video="
            + filenameComponents[0] + "&frame=" + filenameComponents[1];
    ui->urlLineEdit->setText(url);
}

void MainWindow::on_debugButton_clicked()
{
    // Change button text for user feedback
    ui->debugButton->setText(QString(rand()));

    // Delete old images
    deleteDisplayedImages();

    // Fill in a number of placeholder images for demonstration
    QDirIterator it(shotpath, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        displayImage(it.fileName());
    }
}



