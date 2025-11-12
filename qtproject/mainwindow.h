#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSolveClicked();

private:
    QTextEdit *inputMatrix;
    QTextEdit *inputVector;
    QPushButton *solveButton;
    QTextEdit *resultView;
};

#endif // MAINWINDOW_H
