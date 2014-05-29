#include <QTime>
#include <QDateTime>

#include "dialog.h"
#include "ui_dialog.h"
#include "pollsserver.h"
#include "configmanager.h"

QTextStream log();

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    loadConfig();
//    startServer();
    initializeLogging();
    _serv= QSharedPointer<PollsServer>(nullptr);
}


Dialog::~Dialog()
{
    _logFile.close();
    delete ui;
}


void Dialog::startServer() {

    if (!_serv.isNull())
        _serv.clear();
    _serv = QSharedPointer<PollsServer>(new PollsServer(this));
    connect(_serv.data(), SIGNAL(addLogToGui(QString,QColor)), this, SLOT(onAddLogToGui(QString,QColor)));

    auto ip = ui->leHost->text();
    auto port = ui->lePort->text();

    QHostAddress addr;
    if (!addr.setAddress(ip))
    {
        addToLog(" invalid address " + ip, Qt::darkRed);
        return;
    }

    if (_serv->doStartServer(addr, port.toInt()))
    {
        addToLog(" server started at " + ip + ":" + port, Qt::darkGreen);
        ui->pbStartStop->setText("Stop server");
    }
    else
    {
        addToLog(" server not started at " + ip + ":" + port, Qt::darkRed);
        ui->pbStartStop->setChecked(true);
    }
}


void Dialog::loadConfig()
{
    ConfigManager::Instance()->Load("server.config");
    ui->leHost->setText(ConfigManager::Get("ip"));
    ui->lePort->setText(ConfigManager::Get("port"));
}


void Dialog::initializeLogging()
{
    QString timestamp;
    timestamp.setNum(QDateTime::currentDateTimeUtc().toTime_t());
    _logFile.setFileName("votes-" + timestamp + ".txt");
    _logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream log(&_logFile);
    QString message("Votes logs in votes-%1.txt");

    addToLog(message.arg(timestamp));
}


void Dialog::onAddUserToGui(QString name)
{
    ui->lwClients->addItem(name);
    addToLog(QTime::currentTime().toString()+" "+name+" has joined", Qt::darkGreen);
}


void Dialog::onRemoveUserFromGui(QString name)
{
    for (int i = 0; i < ui->lwClients->count(); ++i)
        if (ui->lwClients->item(i)->text() == name)
        {
            ui->lwClients->takeItem(i);
            addToLog(QTime::currentTime().toString()+" "+name+" has left", Qt::darkGreen);
            break;
        }
}


void Dialog::onMessageToGui(QString message, QString from)
{
    addToLog(QTime::currentTime().toString()+" message from "+from+": "+message);
}


void Dialog::onAddLogToGui(QString string, QColor color)
{
    addToLog(string, color);
}


void Dialog::addToLog(QString text, QColor color)
{
    ui->lwLog->addItem(QTime::currentTime().toString()+" "+text);

    int lastItemIndex = ui->lwLog->count() - 1;
    ui->lwLog->item(lastItemIndex)->setTextColor(color);
    ui->lwLog->scrollToBottom();
}


void Dialog::on_pbStartStop_toggled(bool checked)
{
    if (!checked) {
        addToLog(" server stopped at " + _serv->serverAddress().toString() +
                 ":" + QString::number(_serv->serverPort()), Qt::darkGreen);
        _serv->close();
        ui->pbStartStop->setText("Start server");
    }
    else
    {
        this->startServer();
    }
}


void Dialog::onVoteUp(QString category, QString code, QString filename) {
    _votes.insert(code, filename);

    if(!_usedCodes.contains(code))
        _usedCodes.insert(code, QVector<QString>());

    if (!_usedCodes[code].contains(category))
        _usedCodes[code].push_back(category);

    onAddLogToGui("Vote " + category + " " + code + " " + filename, Qt::yellow);

    QTextStream log(&_logFile);
    log << code << " " << filename << "\n";
    _logFile.flush();
}


bool Dialog::isCodeAlreadyUsed(QString category, QString code) {
    bool haveKey = _usedCodes.contains(code);
    if (!haveKey)
        return false;

    bool voteCategory = _usedCodes[code].contains(category);
    if (!voteCategory)
        return false;

    return true;
}
