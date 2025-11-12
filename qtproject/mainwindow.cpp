#include "mainwindow.h"
#include "s21_matrix.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringList>
#include <QMessageBox>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QLabel *labelA = new QLabel("Матрица A (коэффициенты):", this);
    QLabel *labelB = new QLabel("Вектор b (свободные члены):", this);
    QLabel *labelResult = new QLabel("Решение x:", this);

    inputMatrix = new QTextEdit(this);
    inputMatrix->setPlaceholderText(
        "Введите матрицу A (разделитель: пробел, строки: Enter)\n"
        "Пример:\n"
        "2 1 -1\n"
        "-3 -1 2\n"
        "-2 1 2"
        );

    inputVector = new QTextEdit(this);
    inputVector->setMaximumHeight(60);
    inputVector->setPlaceholderText(
        "Введите вектор b (разделитель: пробел, одна строка)\n"
        "Пример:\n"
        "8 -11 -3"
        );

    solveButton = new QPushButton("Решить", this);

    resultView = new QTextEdit(this);
    resultView->setReadOnly(true);

    mainLayout->addWidget(labelA);
    mainLayout->addWidget(inputMatrix);
    mainLayout->addWidget(labelB);
    mainLayout->addWidget(inputVector);
    mainLayout->addWidget(solveButton);
    mainLayout->addWidget(labelResult);
    mainLayout->addWidget(resultView);

    connect(solveButton, &QPushButton::clicked, this, &MainWindow::onSolveClicked);

    setWindowTitle("Калькулятор СЛАУ");
    resize(600, 500);
}

MainWindow::~MainWindow()
{
}

void removeMatrixes(QVector<matrix_t*> data)
{
    for (int i = 0; i < data.count(); ++i) {
        s21_remove_matrix(data[i]);
    }
}

void MainWindow::onSolveClicked()
{
    QStringList rows = inputMatrix->toPlainText().trimmed().split('\n');
    if (rows.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите матрицу A");
        return;
    }

    int n = rows.size();
    matrix_t A, b, A_inv, x;
    int code;

    code = s21_create_matrix(n, n, &A);
    if (code != 0) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать матрицу A");
        return;
    }

    for (int i = 0; i < n; ++i) {
        QStringList nums = rows[i].split(' ', Qt::SkipEmptyParts);
        if (nums.size() != n) {
            QMessageBox::warning(this, "Ошибка", QString("Неверное количество элементов в строке %1").arg(i + 1));
            s21_remove_matrix(&A);
            return;
        }
        for (int j = 0; j < n; ++j) {
            A.matrix[i][j] = nums[j].toDouble();
        }
    }

    QStringList bList = inputVector->toPlainText().trimmed().split(' ', Qt::SkipEmptyParts);
    if (bList.size() != n) {
        QMessageBox::warning(this, "Ошибка", "Размер вектора b не совпадает с размером матрицы");
        s21_remove_matrix(&A);
        return;
    }

    code = s21_create_matrix(n, 1, &b);
    if (code != 0) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать вектор b");
        s21_remove_matrix(&A);
        return;
    }

    for (int i = 0; i < n; ++i) {
        b.matrix[i][0] = bList[i].toDouble();
    }

    double det;
    code = s21_determinant(&A, &det);
    if (code != 0 || det == 0) {
        QMessageBox::critical(this, "Ошибка", "Матрица A вырождена (det = 0), решение невозможно.");
        removeMatrixes({&A, &b});
        return;
    }

    code = s21_inverse_matrix(&A, &A_inv);
    if (code != 0) {
        QMessageBox::critical(this, "Ошибка", "Не удалось вычислить обратную матрицу");
        removeMatrixes({&A, &b, &A_inv});
        return;
    }

    code = s21_mult_matrix(&A_inv, &b, &x);
    if (code != 0) {
        QMessageBox::critical(this, "Ошибка", "Ошибка при умножении матриц");
        removeMatrixes({&A, &b, &A_inv});
        return;
    }

    QString result = "Решение x:\n";
    for (int i = 0; i < n; ++i) {
        result += QString("x%1 = %2\n").arg(i+1).arg(x.matrix[i][0]);
    }

    resultView->setText(result);

    removeMatrixes({&A, &b, &A_inv, &x});
}
