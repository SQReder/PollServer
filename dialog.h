#ifndef DIALOG_H
#define DIALOG_H

#include <QtCore>
#include <QDialog>

class QFile;

class PollsServer;

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    bool isCodeAlreadyUsed(QString category, QString code);

private:
    Ui::Dialog *ui;
    QSharedPointer<PollsServer> _serv;
    void addToLog(QString text, QColor color = Qt::darkGray);
    QMap<QString, QString> _votes; // code to filename
    QMap<QString, QVector<QString>> _usedCodes; // code to categories
    QFile _logFile;
    short _port;

    void startServer();
    void loadConfig();
    void initializeLogging();

public slots:
    void onAddUserToGui(QString name);
    void onRemoveUserFromGui(QString name);
    void onMessageToGui(QString message, QString from);
    void onAddLogToGui(QString string, QColor color);
    void onVoteUp(QString category, QString code, QString filename);

private slots:
    void on_pbStartStop_toggled(bool checked);
};

#endif // DIALOG_H
