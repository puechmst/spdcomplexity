#include <iostream>
#include <QtWidgets/QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "data/Parser.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Open_triggered() {
    std::cout << "ouvrir" << std::endl;
}

void MainWindow::on_action_Save_triggered() {
    std::cout << "sauver" << std::endl;
}

void MainWindow::on_action_Quit_triggered() {
    this->close();
}

void MainWindow::on_action_Directory_triggered() {
    // open directory chooser
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open directory"),
            "/home/stephane/DATA/ALB", QFileDialog::ShowDirsOnly);
    Parser parser;
    parser.parse_directory(dir);
}
