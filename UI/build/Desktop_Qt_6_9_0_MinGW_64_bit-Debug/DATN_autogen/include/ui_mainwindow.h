/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QLabel *rearRight;
    QLabel *right1;
    QLabel *right2;
    QLabel *backRight;
    QLabel *rear;
    QLabel *left2;
    QLabel *backLeft;
    QLabel *rearLeft;
    QLabel *left1;
    QLabel *back;
    QPushButton *connectButton;
    QMenuBar *menubar;
    QMenu *menuODS;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1250, 744);
        MainWindow->setMinimumSize(QSize(1080, 720));
        MainWindow->setMaximumSize(QSize(16777215, 16777215));
        MainWindow->setStyleSheet(QString::fromUtf8("background-color: rgb(58, 58, 58);"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(250, 160, 300, 150));
        QFont font;
        font.setFamilies({QString::fromUtf8("Arial")});
        font.setPointSize(36);
        label->setFont(font);
        label->setFrameShape(QFrame::Shape::Box);
        label->setLineWidth(20);
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label->setWordWrap(false);
        rearRight = new QLabel(centralwidget);
        rearRight->setObjectName("rearRight");
        rearRight->setGeometry(QRect(0, 0, 260, 170));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Arial")});
        font1.setPointSize(20);
        rearRight->setFont(font1);
        rearRight->setFrameShape(QFrame::Shape::NoFrame);
        rearRight->setLineWidth(10);
        rearRight->setAlignment(Qt::AlignmentFlag::AlignCenter);
        rearRight->setWordWrap(false);
        right1 = new QLabel(centralwidget);
        right1->setObjectName("right1");
        right1->setGeometry(QRect(250, 0, 155, 170));
        right1->setFont(font1);
        right1->setFrameShape(QFrame::Shape::NoFrame);
        right1->setLineWidth(10);
        right1->setAlignment(Qt::AlignmentFlag::AlignCenter);
        right1->setWordWrap(false);
        right2 = new QLabel(centralwidget);
        right2->setObjectName("right2");
        right2->setGeometry(QRect(395, 0, 155, 170));
        right2->setFont(font1);
        right2->setFrameShape(QFrame::Shape::NoFrame);
        right2->setLineWidth(10);
        right2->setAlignment(Qt::AlignmentFlag::AlignCenter);
        right2->setWordWrap(false);
        backRight = new QLabel(centralwidget);
        backRight->setObjectName("backRight");
        backRight->setGeometry(QRect(540, 0, 260, 170));
        backRight->setFont(font1);
        backRight->setFrameShape(QFrame::Shape::NoFrame);
        backRight->setLineWidth(10);
        backRight->setAlignment(Qt::AlignmentFlag::AlignCenter);
        backRight->setWordWrap(false);
        rear = new QLabel(centralwidget);
        rear->setObjectName("rear");
        rear->setGeometry(QRect(0, 160, 260, 151));
        rear->setFont(font1);
        rear->setFrameShape(QFrame::Shape::NoFrame);
        rear->setLineWidth(10);
        rear->setAlignment(Qt::AlignmentFlag::AlignCenter);
        rear->setWordWrap(false);
        left2 = new QLabel(centralwidget);
        left2->setObjectName("left2");
        left2->setGeometry(QRect(395, 300, 155, 170));
        left2->setFont(font1);
        left2->setFrameShape(QFrame::Shape::NoFrame);
        left2->setLineWidth(10);
        left2->setAlignment(Qt::AlignmentFlag::AlignCenter);
        left2->setWordWrap(false);
        backLeft = new QLabel(centralwidget);
        backLeft->setObjectName("backLeft");
        backLeft->setGeometry(QRect(540, 300, 260, 170));
        backLeft->setFont(font1);
        backLeft->setFrameShape(QFrame::Shape::NoFrame);
        backLeft->setLineWidth(10);
        backLeft->setAlignment(Qt::AlignmentFlag::AlignCenter);
        backLeft->setWordWrap(false);
        rearLeft = new QLabel(centralwidget);
        rearLeft->setObjectName("rearLeft");
        rearLeft->setGeometry(QRect(0, 300, 260, 170));
        rearLeft->setFont(font1);
        rearLeft->setFrameShape(QFrame::Shape::NoFrame);
        rearLeft->setLineWidth(10);
        rearLeft->setAlignment(Qt::AlignmentFlag::AlignCenter);
        rearLeft->setWordWrap(false);
        left1 = new QLabel(centralwidget);
        left1->setObjectName("left1");
        left1->setGeometry(QRect(250, 300, 155, 170));
        left1->setFont(font1);
        left1->setFrameShape(QFrame::Shape::NoFrame);
        left1->setLineWidth(10);
        left1->setAlignment(Qt::AlignmentFlag::AlignCenter);
        left1->setWordWrap(false);
        back = new QLabel(centralwidget);
        back->setObjectName("back");
        back->setGeometry(QRect(540, 160, 260, 151));
        back->setFont(font1);
        back->setFrameShape(QFrame::Shape::NoFrame);
        back->setLineWidth(10);
        back->setAlignment(Qt::AlignmentFlag::AlignCenter);
        back->setWordWrap(false);
        connectButton = new QPushButton(centralwidget);
        connectButton->setObjectName("connectButton");
        connectButton->setGeometry(QRect(880, 300, 80, 24));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Arial")});
        font2.setPointSize(12);
        connectButton->setFont(font2);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1250, 21));
        menuODS = new QMenu(menubar);
        menuODS->setObjectName("menuODS");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuODS->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-weight:700;\">ODS</span></p></body></html>", nullptr));
        rearRight->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p>NaN</p></body></html>", nullptr));
        right1->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p>NaN</p></body></html>", nullptr));
        right2->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p>NaN</p></body></html>", nullptr));
        backRight->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p>NaN</p></body></html>", nullptr));
        rear->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p>NaN</p></body></html>", nullptr));
        left2->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p>NaN</p></body></html>", nullptr));
        backLeft->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p>NaN</p></body></html>", nullptr));
        rearLeft->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p>NaN</p></body></html>", nullptr));
        left1->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p>NaN</p></body></html>", nullptr));
        back->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p>NaN</p></body></html>", nullptr));
        connectButton->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        menuODS->setTitle(QCoreApplication::translate("MainWindow", "ODS", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
