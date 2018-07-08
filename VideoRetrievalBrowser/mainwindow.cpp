#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <iostream>
#include <QColorDialog>
#include "clickablelabel.h"




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gridLayoutCounter_row(0),
    gridLayoutCounter_col(0),
    gridLayoutMaxCols(3)
{
    ui->setupUi(this);

    // Prepare layout for image results
    gridLayout = new QGridLayout(ui->scrollAreaWidgetContents);
    gridLayout->setHorizontalSpacing(1);
    gridLayout->setVerticalSpacing(1);

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

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_debugButton_clicked()
{
    // Change button text for user feedback
    ui->debugButton->setText(QString(rand()));

    // Delete old images
    deleteDisplayedImages();

    // Fill in a number of placeholder images for demonstration
    for (int i = 0; i < 100; i++) {
        QPixmap img(":/placeholders/320x240.png");
        displayImage(img);
    }
}

void MainWindow::on_ClickableLabel_clicked(){
    // Highlight clicked image
    ClickableLabel *selectedImage = qobject_cast<ClickableLabel*>(sender());
    selectedImage->setFrameShape(QFrame::Box);
    selectedImage->setLineWidth(3);

    // Unhighlight last clicked image (if any)
    if (lastSelectedImage != nullptr) {
        lastSelectedImage->setFrameShape(QFrame::NoFrame);
        lastSelectedImage->setLineWidth(1);
    }
    lastSelectedImage = selectedImage;
}

void MainWindow::displayImage(QPixmap img)
{
    // Generate label and set properties
    ClickableLabel *label = new ClickableLabel(this);
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    label->setPixmap(img);
    label->setFixedSize(QSize(160, 120));
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
