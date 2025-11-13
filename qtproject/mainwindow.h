#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QTextEdit>
#include "dbmanager.h"
#include "loginwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSolveClicked();
    void onResizeMatrix();
    void onLoginClicked();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QTableWidget *matrixTable;
    QPushButton *solveButton;
    QPushButton *loginButton;
    QSpinBox *resizeSpinBox;
    QLabel *resultLabel;
    QTextEdit *resultView;

    void copyTableToClipboard();
    void pasteTableFromClipboard();

    void updateColumnHeaders();
    void centerTableItems();
    void updateLoginButton();

    DBManager dbManager;
    LoginWindow *loginWindow = nullptr;
    int currentUserId = -1;
};

#endif // MAINWINDOW_H
