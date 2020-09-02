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

  void restart();

  void initGiveUpInfoLabels();

  void initDizhuNongminLabels();

  void initBuhefaLabel();

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
  void displayCommonCards(const QList<int> &);

  void tellAPushedCards();

  void notifyAThatBHasJustPushedCards(const QList<int> &);

  void tellAGiveUp();

  void displayGiveUpInfo(int n);

  bool checkIfGameOver();

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
  int positionToPersonIndex_[3] = {1, 2, 0};

  QList<CardLabel *> cardLabels_;
  QList<CardLabel *> tableCardLabels_;

  void displayCards();
  void showTableOnSelfScreen(const QList<int> &);

  void sleep(int t) {
    QTime time;
    time.start();
    while (time.elapsed() < t)            //等待时间流逝50ms
      QCoreApplication::processEvents();  //不停地处理事件，让程序保持响应
  }

  QList<int> stringToIntArray(const QString &str) {
    QList<int> l;
    for (QString str : str.split(".")) {
      if (!str.isEmpty()) {
        l.append(str.toInt());
      }
    }
    return l;
  }
  QString intArrayToString(const QList<int> &list) {
    QString str;
    for (int i = 0; i < list.size(); ++i) {
      str.push_back(QString::number(list[i]));
      str.push_back('.');
    }
    return str;
  }

  QList<QLabel *> jiaoORbujiaoLabels_;
  QList<QLabel *> giveupInfoLabels_;
  QList<QLabel *> dizhuNongminLabels_;
  QList<QLabel *> commonCardLabels_;
  QLabel *buhefaLabel_;

  void showChuOrBuchuBtns();

  int lastPushCardPerson_ = 0;

  void showRestartOrExitBtnsOnSelfScreen();

  void showWinOrLoseInfo(bool);

  QList<int> cardsNum_;
  QList<QLabel *> cardsNumLabel_;

  void updateCardNumber(int n, int cardsSize) {
    cardsNum_[n] -= cardsSize;
    cardsNumLabel_[personIndexToPosition_[n]]->setText(
        QString::number(cardsNum_[n]));
  }

  // Components used when gameover
  QLabel *winOrLoseLabel_;
  QPushButton *restartBtn_;
  QPushButton *exitBtn_;

 public slots:
  void startRequesting();

  void socketReadDataFromA();
  void socketDisconnectedFromA();

 signals:
  // void thereAreThreePeople();
};
#endif  // MAINWINDOW_H
