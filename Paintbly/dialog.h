#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QWidget>
#include <QMessageBox>
#include <QPainter>
#include <QKeyEvent>
#include <QToolBar>
#include <QAction>

#include <thread>
#include <chrono>

QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();
    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private slots:
    void read_serial();

    void on_btnlimpiar_pressed();

    void on_btndibujar_pressed();

    void on_btnvibrar_pressed();

    void on_btninfo_pressed();

    void on_btnsalir_pressed();

private:
    Ui::Dialog *ui;
    QSerialPort *arduino;
    QString arduino_port_name;
    bool arduino_is_available;
    QString serial_data;

    /*-----------------------*/
    /*      PAINTER          */
    QImage image;
    bool drawing;
    QPoint lastPoint;
    int brushsize;
    QColor brushColor;
    /*-----------------------*/

    bool menu = 1;
};
#endif // DIALOG_H
