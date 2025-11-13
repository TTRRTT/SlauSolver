#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTextEdit>
#include <QLabel>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include "dbmanager.h"
#include "s21_matrix.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    dbManager.connectToDatabase("localhost", "slau_solver", "postgres", "");

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *resizeLayout = new QHBoxLayout();

    QLabel *resizeLabel = new QLabel("Число уравнений:");
    resizeSpinBox = new QSpinBox();
    resizeSpinBox->setRange(2, 12);
    resizeSpinBox->setValue(3);

    QPushButton *resizeButton = new QPushButton("Изменить размер");
    connect(resizeButton, &QPushButton::clicked, this, &MainWindow::onResizeMatrix);

    loginButton = new QPushButton("Войти", this);
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::onLoginClicked);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(loginButton);
    topLayout->addWidget(resizeLabel);
    topLayout->addWidget(resizeSpinBox);
    topLayout->addWidget(resizeButton);

    mainLayout->addLayout(topLayout);

    QLabel *labelA = new QLabel("Матрица A и вектор b:");
    matrixTable = new QTableWidget(resizeSpinBox->value(), resizeSpinBox->value() + 1, this);
    matrixTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QFont font = matrixTable->horizontalHeader()->font();
    font.setBold(true);
    matrixTable->horizontalHeader()->setFont(font);
    matrixTable->verticalHeader()->setFont(font);
    updateColumnHeaders();
    centerTableItems();

    mainLayout->addWidget(labelA);
    mainLayout->addWidget(matrixTable);

    solveButton = new QPushButton("Решить", this);
    connect(solveButton, &QPushButton::clicked, this, &MainWindow::onSolveClicked);
    mainLayout->addWidget(solveButton);

    resultLabel = new QLabel("Результат:");
    resultView = new QTextEdit(this);
    resultView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    resultView->setMaximumHeight(150);
    resultView->setReadOnly(true);
    mainLayout->addWidget(resultLabel);
    mainLayout->addWidget(resultView);

    setWindowTitle("Калькулятор СЛАУ");
    resize(700, 600);

    onResizeMatrix();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_C) {
        copyTableToClipboard();
    } else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_V) {
        pasteTableFromClipboard();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::copyTableToClipboard()
{
    QString text;
    int rows = matrixTable->rowCount();
    int cols = matrixTable->columnCount();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            QTableWidgetItem *item = matrixTable->item(i, j);
            QString data = item ? item->text() : "";
            text += data;
            if (j < cols - 1) text += "\t";
        }
        if (i < rows - 1) text += "\n";
    }

    QApplication::clipboard()->setText(text);
}

void MainWindow::pasteTableFromClipboard()
{
    QString text = QApplication::clipboard()->text();
    QStringList rows = text.split('\n');

    int r = 0;
    for (const QString &row : std::as_const(rows)) {
        if (r >= matrixTable->rowCount()) break;
        QStringList columns = row.split('\t');
        int c = 0;
        for (const QString &col : std::as_const(columns)) {
            if (c >= matrixTable->columnCount()) break;
            QTableWidgetItem *item = matrixTable->item(r, c);
            if (!item) {
                item = new QTableWidgetItem();
                matrixTable->setItem(r, c, item);
            }
            item->setText(col.trimmed());
            c++;
        }
        r++;
    }
}

void MainWindow::updateColumnHeaders()
{
    int n = matrixTable->columnCount();
    QStringList headers;
    for (int j = 0; j < n - 1; ++j) {
        headers << QString("x%1").arg(j + 1);
    }
    headers << "b";
    matrixTable->setHorizontalHeaderLabels(headers);
}

void MainWindow::centerTableItems()
{
    int rows = matrixTable->rowCount();
    int cols = matrixTable->columnCount();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            QTableWidgetItem *item = matrixTable->item(i, j);
            if (!item) {
                item = new QTableWidgetItem();
                matrixTable->setItem(i, j, item);
            }
            item->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void MainWindow::onResizeMatrix()
{
    int n = resizeSpinBox->value();
    matrixTable->setRowCount(n);
    matrixTable->setColumnCount(n + 1);
    updateColumnHeaders();
    centerTableItems();
}

void removeMatrixes(QVector<matrix_t*> &&data)
{
    for (auto matrix : data) {
        s21_remove_matrix(matrix);
    }
}

void MainWindow::onSolveClicked()
{
    int n = matrixTable->rowCount();
    if (n <= 0) {
        QMessageBox::warning(this, "Ошибка", "Матрица пуста");
        return;
    }

    matrix_t A, b, A_inv, x;
    int code;

    code = s21_create_matrix(n, n, &A);
    if (code != 0) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать матрицу A");
        return;
    }

    code = s21_create_matrix(n, 1, &b);
    if (code != 0) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать вектор b");
        s21_remove_matrix(&A);
        return;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            QTableWidgetItem *item = matrixTable->item(i, j);
            QString text = item ? item->text() : "";
            bool ok;
            double val = text.toDouble(&ok);
            if (!ok) {
                QMessageBox::warning(this, "Ошибка", QString("Некорректное значение в ячейке [%1][%2]").arg(i+1).arg(j+1));
                removeMatrixes({&A, &b});
                return;
            }
            A.matrix[i][j] = val;
        }

        QTableWidgetItem *item = matrixTable->item(i, n);
        QString text = item ? item->text() : "";
        bool ok;
        double val = text.toDouble(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Ошибка", QString("Некорректное значение вектора b в строке %1").arg(i+1));
            removeMatrixes({&A, &b});
            return;
        }
        b.matrix[i][0] = val;
    }

    double det;
    code = s21_determinant(&A, &det);
    if (code != 0 || det == 0) {
        QMessageBox::critical(this, "Ошибка", "Матрица A вырождена (определитель = 0), решение невозможно.");
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
void MainWindow::onLoginClicked()
{
    if (!loginWindow) {
        loginWindow = new LoginWindow(&dbManager, this);
        connect(loginWindow, &QDialog::finished, this, [this](int result) {
            Q_UNUSED(result);
            currentUserId = loginWindow->getUserId();
            updateLoginButton();
        });
    }

    loginWindow->show();
    loginWindow->raise();
    loginWindow->activateWindow();
}

void MainWindow::updateLoginButton()
{
    if (currentUserId != -1) {
        loginButton->setText("Аккаунт");
    } else {
        loginButton->setText("Войти");
    }
}

MainWindow::~MainWindow()
{
}
