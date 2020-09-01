#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QJsonDocument>
#include <QList>
#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include <QtNetwork>

class PlayerB : public QMainWindow {
  Q_OBJECT

 public:
  PlayerB(QWidget *parent = nullptr);
  ~PlayerB();

  template <typename QBA, typename Str>
  QString readFromBuffer(QBA buffer, Str key) {
    qDebug() << "buffer:" << buffer;
    // 转化为Json
    QJsonDocument document = QJsonDocument::fromJson(buffer);

    qDebug() << "document:" << document;

    QJsonObject rootObj = document.object();

    qDebug() << "rootObj:" << rootObj;

    return rootObj.value(key).toString();
  }

  void showOthersIfCampaignInfo(int personIndex, bool ifCampaign);

  void showDoYouWantToCampaignButtons();

  void castToA(QString key, QString value) {
    QJsonObject jsonObject;
    jsonObject.insert(key, value);

    QJsonDocument jsonDocument;
    jsonDocument.setObject(jsonObject);
    QByteArray dataArray = jsonDocument.toJson();

    socketToA_->write(dataArray);
  }

 private:
  QPushButton *buttonStartListeningAndRequesting;
  // 连接
  bool connectedToA_ = false;
  QTcpSocket *socketToA_ = nullptr;
  const int portA_ = 10080;

  QList<int> cardsOfB_;

  // 出牌顺序是...->A->B->C->A->...
  // 地主为A：0，B：1，C：2
  int landlord_ = 0;
  int personIndexToPosition_[3] = {2, 0, 1};

  void displayCards();

 public slots:
  void startListeningAndRequesting();

  void socketReadDataFromA();
  void socketDisconnectedFromA();

 signals:
  // void thereAreThreePeople();
};
#endif  // MAINWINDOW_H
