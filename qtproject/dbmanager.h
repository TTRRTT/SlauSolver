#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>
#include <optional>

struct User {
    int id;
    QString nickname;
    QString login;
    QString email;
    QString password_hash;
};

struct MatrixSolution {
    int id;
    std::optional<int> user_id;
    QDateTime timestamp;
    QString matrix_content;
    QString result_status;
    QString solution_output;
};

class DBManager : public QObject
{
    Q_OBJECT

public:
    explicit DBManager(QObject *parent = nullptr);
    ~DBManager();

    bool connectToDatabase(const QString &host, const QString &database,
                           const QString &username, const QString &password, int port = 5432);

    bool registerUser(const QString &nickname, const QString &login,
                      const QString &email, const QString &password_hash);
    std::optional<User> findUserByLogin(const QString &login);
    std::optional<User> findUserById(int id);

    bool logSolution(int user_id, const QString &matrix_content,
                     const QString &result_status, const QString &solution_output);
    bool logSolutionAnonymous(const QString &matrix_content,
                              const QString &result_status, const QString &solution_output);

    bool createSession(int user_id, const QString &token);
    bool validateSession(const QString &token);
    void cleanupExpiredSessions();

private:
    QSqlDatabase db;
};

#endif // DBMANAGER_H
