#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QJsonDocument>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include <QtNetwork>

#include "cardlabel.h"
#include "cards.h"
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

  void setLandlord(int n) { landlord_ = n; }
  void displayCommonCards(QList<int>);

  void tellAPushedCards();

  void notifyAThatBHasJustPushedCards(QList<int>);

  void tellAGiveUp();

  void displayGiveUpInfo(int n);

 private:
  QPushButton *buttonStartRequesting;
  // 连接
  bool connectedToA_ = false;
  QTcpSocket *socketToA_ = nullptr;
  const int portA_ = 10080;

  QList<int> cardsOfB_;
  QList<int> comcards_;
  QList<int> cardsOnTable_;
  // 出牌顺序是...->A->B->C->A->...
  // 地主为A：0，B：1，C：2
  int landlord_ = 0;
  int personIndexToPosition_[3] = {2, 0, 1};

  QList<CardLabel *> cardLabels_;
  QList<CardLabel *> tableCardLabels_;

  void displayCards();
  void showTableOnSelfScreen(QList<int>);

  QList<int> stringToIntArray(QString str) {
    QList<int> l;
    for (QString str : str.split(".")) {
      if (!str.isEmpty()) {
        l.append(str.toInt());
      }
    }
    return l;
  }
  QString intArrayToString(QList<int> list) {
    QString str;
    for (int i = 0; i < list.size(); ++i) {
      str.push_back(QString::number(list[i]));
      str.push_back('.');
    }
    return str;
  }

  void showChuOrBuchuBtns();

  int lastPushCardPerson_ = 0;

 public slots:
  void startRequesting();

  void socketReadDataFromA();
  void socketDisconnectedFromA();

 signals:
  // void thereAreThreePeople();
};
#endif  // MAINWINDOW_H
