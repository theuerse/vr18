#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <iostream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gridLayoutCounter_row(0),
    gridLayoutCounter_col(0),
    gridLayoutMaxRows(10),
    gridLayoutMaxCols(3)
{
    ui->setupUi(this);
    gridLayout = new QGridLayout(ui->scrollAreaWidgetContents);
    gridLayout->setHorizontalSpacing(1);
    gridLayout->setVerticalSpacing(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_debugButton_clicked
 * This method (and button) is only for debugging and testing and should be removed in the final built
 *
 * @todo override QWidget::resizeEvent() to get correct col numbers after resize.
 */
void MainWindow::on_debugButton_clicked()
{
    // Change button text for user feedback
    ui->debugButton->setText(QString(rand()));

    // Delete old images
    if (ui->scrollAreaWidgetContents->children().size() >= 1) {
        qDeleteAll(ui->scrollAreaWidgetContents->children());
        gridLayout = new QGridLayout(ui->scrollAreaWidgetContents);
        gridLayoutCounter_row = 0;
        gridLayoutCounter_col = 0;
    }

    // Fill in a number of placeholder images for demonstration
    for (int i = 0; i < 100; i++) {

        // Generate label and set properties
        QPixmap img(":/placeholders/320x240.png");
        QLabel *label = new QLabel(this);
        label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
        label->setPixmap(img);
        label->setFixedSize(QSize(160, 120));
        label->setScaledContents(true);
        label->setVisible(true);

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
        // Add label label to layout
        gridLayout->addWidget(label, gridLayoutCounter_row, gridLayoutCounter_col++);
    }
}
