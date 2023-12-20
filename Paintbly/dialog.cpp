#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    image=QImage(this->size(),QImage::Format_RGB32);
    image.fill(Qt::white);
    drawing=false;
    brushColor=Qt::black;
    brushsize=12;
    lastPoint.setX(390);
    lastPoint.setY(285);

    arduino = new QSerialPort;
    arduino_is_available = false;
    arduino_port_name = "";

    qDebug() << "Number of Ports: " <<QSerialPortInfo::availablePorts().length();
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
            qDebug() <<"Vendor ID: " << serialPortInfo.vendorIdentifier();
            qDebug() <<"Product ID: " << serialPortInfo.productIdentifier();
        }
    }

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
            if(serialPortInfo.vendorIdentifier() == 9025 && (serialPortInfo.productIdentifier() == 67 || serialPortInfo.productIdentifier() == 66)){
                arduino_port_name = serialPortInfo.portName();
                arduino_is_available = true;
                qDebug() << "Control conectado en: " << serialPortInfo.portName();
            }
        }
    }

    if(arduino_is_available){
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);

    }else{
        QMessageBox::warning(this,"Port Error","No se puede encontrar control");
    }

    connect(arduino,SIGNAL(readyRead()),this,SLOT(read_serial()));
}

Dialog::~Dialog()
{
    if(arduino->isOpen()){
        qDebug() << "Apagando conexion del control";
        arduino->close();
    }
    delete ui;
}

void Dialog::read_serial()
{
    if(arduino->isOpen()){
        while(arduino->bytesAvailable()){
            serial_data = arduino->readAll();
        }

        if(menu){
            qDebug() <<serial_data;
            if(serial_data == "D"){
                lastPoint.setY(lastPoint.y() + 12);
            }else if(serial_data == "L"){
                lastPoint.setX(lastPoint.x() - 12);
            }else if(serial_data == "U"){
                lastPoint.setY(lastPoint.y() - 12);
            }else if(serial_data == "R"){
                lastPoint.setX(lastPoint.x() + 12);
            }else if(serial_data == "S"){
                QPainter painter(&image);
                painter.setPen(QPen(brushColor,brushsize,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
                QRect rect(lastPoint.x(), lastPoint.y(), 10, 10);
                painter.drawRect(rect);
                this->update();
            }else if(serial_data == "M"){
                menu = false;
                ui->btndibujar->setFocus();
            }
        }else{
            if (ui->btndibujar->hasFocus()){
                if (serial_data == "L") {
                    qDebug() << "<-";
                    ui->btnsalir->setFocus();
                } else if (serial_data == "R") {
                    qDebug() << "->";
                    ui->btnlimpiar->setFocus();
                } else if(serial_data == "S"){
                    qDebug() << "X";
                    this->on_btndibujar_pressed();
                    menu = true;
                }
            }else if (ui->btnlimpiar->hasFocus()){
                if (serial_data == "L") {
                    qDebug() << "<-";
                    ui->btndibujar->setFocus();
                } else if (serial_data == "R") {
                    qDebug() << "->";
                    ui->btnvibrar->setFocus();
                } else if(serial_data == "S"){
                    qDebug() << "X";
                    this->on_btnlimpiar_pressed();
                    menu = true;
                }
            }else if (ui->btnvibrar->hasFocus()){
                if (serial_data == "L") {
                    qDebug() << "<-";
                    ui->btnlimpiar->setFocus();
                } else if (serial_data == "R") {
                    qDebug() << "->";
                    ui->btninfo->setFocus();
                } else if(serial_data == "S"){
                    qDebug() << "X";
                    this->on_btnvibrar_pressed();
                    menu = true;
                }
            }else if (ui->btninfo->hasFocus()){
                if (serial_data == "L") {
                    qDebug() << "<-";
                    ui->btnvibrar->setFocus();
                } else if (serial_data == "R") {
                    qDebug() << "->";
                    ui->btnsalir->setFocus();
                } else if(serial_data == "S"){
                    qDebug() << "X";
                    this->on_btninfo_pressed();
                    menu = true;
                }
            }else if (ui->btnsalir->hasFocus()){
                if (serial_data == "L") {
                    qDebug() << "<-";
                    ui->btninfo->setFocus();
                } else if (serial_data == "R") {
                    qDebug() << "->";
                    ui->btndibujar->setFocus();
                } else if(serial_data == "S"){
                    qDebug() << "X";
                    this->on_btnsalir_pressed();
                    menu = true;
                }
            }
        }

    }
}

void Dialog::paintEvent(QPaintEvent *event)
{
    QPainter canvasPainter(this);
    canvasPainter.drawImage(this->rect(),image,image.rect());
}

void Dialog::keyPressEvent(QKeyEvent *event)
{
    if(menu){
        if (event->key() == Qt::Key_F) {
            QPainter painter(&image);
            painter.setPen(QPen(brushColor,brushsize,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
            QRect rect(lastPoint.x(), lastPoint.y(), 10, 10);
            painter.drawRect(rect);
            this->update();

        }else if (event->key() == Qt::Key_A) {
            lastPoint.setX(lastPoint.x() - 12);
        } else if (event->key() == Qt::Key_D) {
            lastPoint.setX(lastPoint.x() + 12);
        } else if (event->key() == Qt::Key_W) {
            lastPoint.setY(lastPoint.y() - 12);
        } else if (event->key() == Qt::Key_S) {
            lastPoint.setY(lastPoint.y() + 12);
        } else if (event->key() == Qt::Key_E) {
            menu = false;
            ui->btndibujar->setFocus();
        }

    }else{
        if (ui->btndibujar->hasFocus()){
            if (event->key() == Qt::Key_A) {
                qDebug() << "A";
                ui->btnsalir->setFocus();
            } else if (event->key() == Qt::Key_D) {
                qDebug() << "D";
                ui->btnlimpiar->setFocus();
            } else if(event->key() == Qt::Key_F){
                qDebug() << "F";
                this->on_btndibujar_pressed();
                menu = true;
            }
        }else if (ui->btnlimpiar->hasFocus()){
            if (event->key() == Qt::Key_A) {
                qDebug() << "A";
                ui->btndibujar->setFocus();
            } else if (event->key() == Qt::Key_D) {
                qDebug() << "D";
                ui->btnvibrar->setFocus();
            } else if(event->key() == Qt::Key_F){
                qDebug() << "F";
                this->on_btnlimpiar_pressed();
                menu = true;
            }
        }else if (ui->btnvibrar->hasFocus()){
            if (event->key() == Qt::Key_A) {
                qDebug() << "A";
                ui->btnlimpiar->setFocus();
            } else if (event->key() == Qt::Key_D) {
                qDebug() << "D";
                ui->btninfo->setFocus();
            } else if(event->key() == Qt::Key_F){
                qDebug() << "F";
                this->on_btnvibrar_pressed();
                menu = true;
            }
        }else if (ui->btninfo->hasFocus()){
            if (event->key() == Qt::Key_A) {
                qDebug() << "A";
                ui->btnvibrar->setFocus();
            } else if (event->key() == Qt::Key_D) {
                qDebug() << "D";
                ui->btnsalir->setFocus();
            } else if(event->key() == Qt::Key_F){
                qDebug() << "F";
                this->on_btninfo_pressed();
                menu = true;
            }
        }else if (ui->btnsalir->hasFocus()){
            if (event->key() == Qt::Key_A) {
                qDebug() << "A";
                ui->btninfo->setFocus();
            } else if (event->key() == Qt::Key_D) {
                qDebug() << "D";
                ui->btndibujar->setFocus();
            } else if(event->key() == Qt::Key_F){
                qDebug() << "F";
                this->on_btnsalir_pressed();
                menu = true;
            }
        }
    }
}

void Dialog::on_btnlimpiar_pressed()
{
    lastPoint.setX(390);
    lastPoint.setY(285);
    image.fill(Qt::white);
    this->update();
    ui->btnlimpiar->clearFocus();
}


void Dialog::on_btndibujar_pressed()
{
    ui->btndibujar->clearFocus();
}


void Dialog::on_btnvibrar_pressed()
{

    if(arduino->isWritable()){
        QString vibrar = "V";
        arduino->write(vibrar.toStdString().c_str());
        arduino->waitForBytesWritten();

    }else{
        qDebug() << "La serial del control no esta activa";
    }
    ui->btnvibrar->clearFocus();
}


void Dialog::on_btninfo_pressed()
{
    QMessageBox messageBox;
    messageBox.setWindowTitle("Acerca de");
    messageBox.setText("Creado por:\n\nEddy Diaz - 201906558\n"
                       "Alvaro Arenas - 202102864\n"
                       "Julio Zaldaña - 202110206\n"
                       "Marco Solares - 202004822\n"
                       "Kevin Ruiz - 201903791\n"
                       "Diego Perez - 202106538\n"
                       "Mauricio Castro - 202100299\n"
                       "Diana Berducido - 202000277\n\n"
                       "Proyecto realizado en Diciembre 2023 para el curso de\n"
                       "Arquitectura de Computadores y Ensambladores 1\n"
                       "utilizando QT Creator para la elaboración de la interfaz grafica\n"
                       "tambien se utilizo arduino embebido con assambler para la\n"
                       "elaboracion de controles conectados por usb");
    messageBox.setIcon(QMessageBox::Information);
    messageBox.setStandardButtons(QMessageBox::Ok);

    messageBox.exec();
    ui->btninfo->clearFocus();

}

void Dialog::on_btnsalir_pressed()
{
    QCoreApplication::quit();
}

