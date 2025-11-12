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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSolveClicked();
    void onResizeMatrix();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QTableWidget *matrixTable;
    QPushButton *solveButton;
    QSpinBox *resizeSpinBox;
    QLabel *resultLabel;
    QTextEdit *resultView;

    void copyTableToClipboard();
    void pasteTableFromClipboard();

    void updateColumnHeaders();
    void centerTableItems();
};

#endif // MAINWINDOW_H
