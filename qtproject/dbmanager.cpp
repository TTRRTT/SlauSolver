#include "dbmanager.h"
#include <QSqlError>
#include <QDebug>

DBManager::DBManager(QObject *parent)
    : QObject(parent)
{
}

DBManager::~DBManager()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool DBManager::connectToDatabase(const QString &host, const QString &database,
                                  const QString &username, const QString &password, int port)
{
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(host);
    db.setPort(port);
    db.setDatabaseName(database);
    db.setUserName(username);
    db.setPassword(password);

    if (!db.open()) {
        qWarning() << "Не удалось подключиться к БД:" << db.lastError().text();
        return false;
    }

    qDebug() << "Подключено к БД успешно.";
    return true;
}

bool DBManager::registerUser(const QString &nickname, const QString &login,
                             const QString &email, const QString &password_hash)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (nickname, login, email, password_hash) VALUES (?, ?, ?, ?)");
    query.addBindValue(nickname);
    query.addBindValue(login);
    query.addBindValue(email);
    query.addBindValue(password_hash);

    if (!query.exec()) {
        qWarning() << "Ошибка регистрации пользователя:" << query.lastError().text();
        return false;
    }
    return true;
}

std::optional<User> DBManager::findUserByLogin(const QString &login)
{
    QSqlQuery query(db);
    query.prepare("SELECT id, nickname, login, email, password_hash FROM users WHERE login = ?");
    query.addBindValue(login);

    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }

    User user;
    user.id = query.value("id").toInt();
    user.nickname = query.value("nickname").toString();
    user.login = query.value("login").toString();
    user.email = query.value("email").toString();
    user.password_hash = query.value("password_hash").toString();

    return user;
}

std::optional<User> DBManager::findUserById(int id)
{
    QSqlQuery query(db);
    query.prepare("SELECT id, nickname, login, email, password_hash FROM users WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }

    User user;
    user.id = query.value("id").toInt();
    user.nickname = query.value("nickname").toString();
    user.login = query.value("login").toString();
    user.email = query.value("email").toString();
    user.password_hash = query.value("password_hash").toString();

    return user;
}

bool DBManager::logSolution(int user_id, const QString &matrix_content,
                            const QString &result_status, const QString &solution_output)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO matrix_solutions (user_id, matrix_content, result_status, solution_output) VALUES (?, ?, ?, ?)");
    query.addBindValue(user_id);
    query.addBindValue(matrix_content);
    query.addBindValue(result_status);
    query.addBindValue(solution_output);

    if (!query.exec()) {
        qWarning() << "Ошибка логирования решения:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DBManager::logSolutionAnonymous(const QString &matrix_content,
                                     const QString &result_status, const QString &solution_output)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO matrix_solutions (matrix_content, result_status, solution_output) VALUES (?, ?, ?)");
    query.addBindValue(matrix_content);
    query.addBindValue(result_status);
    query.addBindValue(solution_output);

    if (!query.exec()) {
        qWarning() << "Ошибка логирования анонимного решения:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DBManager::createSession(int user_id, const QString &token)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO sessions (user_id, session_token) VALUES (?, ?)");
    query.addBindValue(user_id);
    query.addBindValue(token);

    if (!query.exec()) {
        qWarning() << "Ошибка создания сессии:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DBManager::validateSession(const QString &token)
{
    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM sessions WHERE session_token = ? AND expires_at > NOW()");
    query.addBindValue(token);

    if (!query.exec()) {
        qWarning() << "Ошибка проверки сессии:" << query.lastError().text();
        return false;
    }

    return query.next();
}

void DBManager::cleanupExpiredSessions()
{
    QSqlQuery query(db);
    query.exec("DELETE FROM sessions WHERE expires_at < NOW()");

    if (query.lastError().isValid()) {
        qWarning() << "Ошибка очистки сессий:" << query.lastError().text();
    }
}
