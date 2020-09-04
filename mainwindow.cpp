#include "mainwindow.h"

#include <QApplication>
#include <QByteArray>
#include <QLabel>
#include <QPushButton>
#include <QTime>

PlayerB::PlayerB(QWidget* parent) : QMainWindow(parent) {
  setFixedSize(1200, 800);

  // startListening button
  buttonStartRequesting = new QPushButton(this);
  buttonStartRequesting->setGeometry(376, 512, 448, 120);
  buttonStartRequesting->setFont(QFont("Helvetica", 28));
  buttonStartRequesting->setText("Start connecting...");
  connect(buttonStartRequesting, SIGNAL(clicked()), this,
          SLOT(startRequesting()));

  initGiveUpInfoLabels();

  initDizhuNongminLabels();

  initBuhefaLabel();
}

PlayerB::~PlayerB() {}

void PlayerB::restart() {
  delete winOrLoseLabel_;
  delete restartBtn_;
  delete exitBtn_;

  cardsOfB_.clear();
  comcards_.clear();
  for (auto* i : commonCardLabels_) delete i;
  commonCardLabels_.clear();
  cardsOnTable_.clear();
  landlord_ = 0;
  for (auto* i : cardLabels_) delete i;
  cardLabels_.clear();
  for (auto* i : tableCardLabels_) delete i;
  tableCardLabels_.clear();
  jiaoORbujiaoLabels_.clear();
  lastPushCardPerson_ = 0;
  cardsNum_.clear();
  for (auto* i : cardsNumLabel_) delete i;
  cardsNumLabel_.clear();

  for (auto* i : dizhuNongminLabels_) i->hide();
  for (auto* i : giveupInfoLabels_) i->hide();
}

void PlayerB::initGiveUpInfoLabels() {
  for (int i = 0; i < 3; ++i) {
    QLabel* giveupLabel = new QLabel(this);
    giveupLabel->setText("不出");
    giveupInfoLabels_.append(giveupLabel);
    giveupInfoLabels_[i]->hide();
  }

  giveupInfoLabels_[0]->setGeometry(580, 440, 40, 32);
  giveupInfoLabels_[1]->setGeometry(160, 200, 40, 32);
  giveupInfoLabels_[2]->setGeometry(960, 200, 40, 32);
}

void PlayerB::initDizhuNongminLabels() {
  for (int i = 0; i < 3; ++i) {
    QLabel* dizhuNongminLabel = new QLabel(this);
    dizhuNongminLabel->setText("身份不明");
    dizhuNongminLabels_.append(dizhuNongminLabel);
    dizhuNongminLabels_[i]->hide();
  }

  dizhuNongminLabels_[0]->setGeometry(580, 420, 40, 32);
  dizhuNongminLabels_[1]->setGeometry(160, 168, 40, 32);
  dizhuNongminLabels_[2]->setGeometry(960, 168, 40, 32);
}

void PlayerB::initBuhefaLabel() {
  buhefaLabel_ = new QLabel(this);
  buhefaLabel_->hide();
  buhefaLabel_->setText("不合法");
  buhefaLabel_->setGeometry(240, 500, 50, 50);
}

void PlayerB::startRequesting() {
  socketToA_ = new QTcpSocket();
  connect(socketToA_, &QTcpSocket::readyRead, this,
          &PlayerB::socketReadDataFromA);
  connect(socketToA_, &QTcpSocket::disconnected, this,
          &PlayerB::socketDisconnectedFromA);
  socketToA_->connectToHost("localhost", portA_);
}

void PlayerB::socketReadDataFromA() {
  qDebug() << "B IS READING FROM BUFFER";
  QByteArray buffer = socketToA_->readAll();

  QString BCards = readFromBuffer(buffer, "cardsAssignedToB");
  if (!BCards.isEmpty()) {
    buttonStartRequesting->hide();
    cardsOfB_ = stringToIntArray(BCards);

    displayCards();
  }

  QString someOneHasDecidedWhetherToCampaign =
      readFromBuffer(buffer, "ifWantToCampaign");
  if (!someOneHasDecidedWhetherToCampaign.isEmpty()) {
    qDebug() << "ifWantToCampaign RECEIVED";
    int personIndex = someOneHasDecidedWhetherToCampaign.toInt() / 10;
    bool ifCampaign = someOneHasDecidedWhetherToCampaign.toInt() % 10;
    showOthersIfCampaignInfo(personIndexToPosition_[personIndex], ifCampaign);
  }

  QString doYouWantToCampaign = readFromBuffer(buffer, "doYouWantToCampaign");
  if (!doYouWantToCampaign.isEmpty()) {
    showDoYouWantToCampaignButtons();
  }

  QString commonCardsStr = readFromBuffer(buffer, "commonCards");
  if (!commonCardsStr.isEmpty()) {
    comcards_.clear();
    buttonStartRequesting->hide();
    for (QString str : commonCardsStr.split(".")) {
      if (!str.isEmpty()) {
        comcards_.append(str.toInt());
      }
    }
    displayCommonCards(comcards_);
  }

  QString theLandlordIs = readFromBuffer(buffer, "theLandlordIs");
  if (!theLandlordIs.isEmpty()) {
    setLandlord(theLandlordIs.toInt());
    for (int i = 0; i < 3; ++i) {
      dizhuNongminLabels_[personIndexToPosition_[i]]->setText(
          landlord_ == i ? "地主" : "农民");
      dizhuNongminLabels_[personIndexToPosition_[i]]->show();
    }
    for (auto plb : jiaoORbujiaoLabels_) {
      plb->hide();
    }
    if (landlord_ == 1) {
      lastPushCardPerson_ = 1;
      cardsOfB_.append(comcards_);
      std::sort(cardsOfB_.begin(), cardsOfB_.end());
      displayCards();
    }

    // Display card numbers
    for (int i = 0; i < 3; ++i) {
      cardsNum_.append(17);
      QLabel* numLabel = new QLabel(this);
      cardsNumLabel_.append(numLabel);
    }
    qDebug() << "landlord is" << landlord_;
    cardsNum_[landlord_] += 3;

    for (int i = 0; i < 3; ++i) {
      cardsNumLabel_[personIndexToPosition_[i]]->setText(
          QString::number(cardsNum_[i]));
    }
    cardsNumLabel_[0]->setGeometry(580, 500, 40, 20);
    cardsNumLabel_[1]->setGeometry(200, 100, 40, 20);
    cardsNumLabel_[2]->setGeometry(920, 100, 40, 20);
    cardsNumLabel_[0]->show();
    cardsNumLabel_[1]->show();
    cardsNumLabel_[2]->show();
  }

  QString someOneHasPushedCards =
      readFromBuffer(buffer, "someOneHasPushedCards");
  if (!someOneHasPushedCards.isEmpty()) {
    cardsOnTable_ = stringToIntArray(someOneHasPushedCards);
    lastPushCardPerson_ = cardsOnTable_.back();

    giveupInfoLabels_[personIndexToPosition_[lastPushCardPerson_]]->hide();

    updateCardNumber(lastPushCardPerson_, cardsOnTable_.size() - 1);
    cardsOnTable_.pop_back();

    showTableOnSelfScreen(cardsOnTable_);

    if (checkIfGameOver()) return;
  }

  QString chuOrBuchu = readFromBuffer(buffer, "chuOrBuchu");
  if (!chuOrBuchu.isEmpty()) {
    showChuOrBuchuBtns();
  }

  QString someOneHasJustGivenUp =
      readFromBuffer(buffer, "someOneHasJustGivenUp");
  if (!someOneHasJustGivenUp.isEmpty()) {
    displayGiveUpInfo(someOneHasJustGivenUp.toInt());
  }

  // QString landlordWins = readFromBuffer(buffer, "gameOver");
  // if (!landlordWins.isEmpty()) {
  //   showWinOrLoseInfo(landlordWins == "true");
  //   showRestartOrExitBtnsOnSelfScreen();
  // }
}
void PlayerB::socketDisconnectedFromA() {}

void PlayerB::showOthersIfCampaignInfo(int personPosition, bool ifCampaign) {
  QLabel* jiaoORbujiao = new QLabel(this);
  jiaoORbujiaoLabels_.append(jiaoORbujiao);
  jiaoORbujiao->setText(ifCampaign ? "叫" : "不叫");
  if (personPosition == 0) {
    jiaoORbujiao->setGeometry(580, 400, 40, 32);
  } else if (personPosition == 1) {
    jiaoORbujiao->setGeometry(160, 200, 40, 32);
  } else if (personPosition == 2) {
    jiaoORbujiao->setGeometry(960, 200, 40, 32);
  }
  jiaoORbujiao->show();
}

void PlayerB::showDoYouWantToCampaignButtons() {
  QPushButton* yesBtn = new QPushButton(this);
  QPushButton* noBtn = new QPushButton(this);

  yesBtn->setText("叫地主");
  noBtn->setText("不叫");
  yesBtn->setGeometry(120, 500, 50, 50);
  noBtn->setGeometry(180, 500, 50, 50);
  yesBtn->show();
  noBtn->show();
  // doYouWantToCampaign->addButton(yesBtn);
  // doYouWantToCampaign->addButton(noBtn);

  connect(yesBtn, SIGNAL(clicked()), yesBtn, SLOT(hide()));
  connect(yesBtn, SIGNAL(clicked()), noBtn, SLOT(hide()));
  connect(noBtn, SIGNAL(clicked()), yesBtn, SLOT(hide()));
  connect(noBtn, SIGNAL(clicked()), noBtn, SLOT(hide()));

  connect(yesBtn, &QPushButton::clicked,
          [=]() { castToA("doYouWantToCampaign", "true"); });
  connect(noBtn, &QPushButton::clicked,
          [=]() { castToA("doYouWantToCampaign", "false"); });
}

void PlayerB::displayCards() {
  // 先删除之前的卡
  for (QLabel* item : cardLabels_) {
    delete item;
  }
  cardLabels_.clear();

  // SHOW
  for (int i = 0; i < cardsOfB_.size(); ++i) {
    CardLabel* cardLabel = new CardLabel(this);
    cardLabels_.append(cardLabel);
    QPixmap cardImage =
        QPixmap("./assets/" + QString::number(cardsOfB_[i]) + ".png");
    cardImage = cardImage.scaled(QSize(330, 160), Qt::IgnoreAspectRatio,
                                 Qt::SmoothTransformation);
    cardLabel->setPixmap(cardImage);
    cardLabel->setGeometry(40 + 50 * i, 600, 360, 200);
    cardLabel->show();
  }
}

void PlayerB::displayCommonCards(const QList<int>& commonCards) {
  for (auto* i : commonCardLabels_) delete i;
  commonCardLabels_.clear();
  for (int i = 0; i < commonCards.size(); ++i) {
    QLabel* cardLabel = new QLabel(this);
    commonCardLabels_.append(cardLabel);
    QPixmap cardImage =
        QPixmap("./assets/" + QString::number(commonCards[i]) + ".png");
    cardImage = cardImage.scaled(QSize(330, 160), Qt::IgnoreAspectRatio,
                                 Qt::SmoothTransformation);
    cardLabel->setPixmap(cardImage);
    cardLabel->setGeometry(480 + 50 * i, 30, 360, 200);
    cardLabel->show();
  }
}

void PlayerB::showTableOnSelfScreen(const QList<int>&) {
  // 先删除之前的卡
  for (QLabel* item : tableCardLabels_) {
    delete item;
  }
  tableCardLabels_.clear();

  // SHOW
  for (int i = 0; i < cardsOnTable_.size(); ++i) {
    CardLabel* cardLabel = new CardLabel(this);
    tableCardLabels_.append(cardLabel);
    QPixmap cardImage =
        QPixmap("./assets/" + QString::number(cardsOnTable_[i]) + ".png");
    cardImage = cardImage.scaled(QSize(330, 160), Qt::IgnoreAspectRatio,
                                 Qt::SmoothTransformation);
    cardLabel->setPixmap(cardImage);
    cardLabel->setGeometry(500 + 50 * i, 250, 360, 200);
    cardLabel->show();
  }
}

void PlayerB::showChuOrBuchuBtns() {
  qDebug() << "B chu or buchu";
  QPushButton* chuBtn = new QPushButton(this);
  QPushButton* buchuBtn = new QPushButton(this);

  chuBtn->setText("出牌");
  buchuBtn->setText("不出");
  chuBtn->setGeometry(120, 500, 50, 50);
  buchuBtn->setGeometry(180, 500, 50, 50);
  chuBtn->show();
  buchuBtn->show();

  buchuBtn->setDisabled(lastPushCardPerson_ == 1);
  if (lastPushCardPerson_ == 1) {
    cardsOnTable_ = {};
  }
  // btn connect
  connect(chuBtn, &QPushButton::clicked, [=]() {
    // lastPushCardPerson_ = 1;
    // cardsOnTable_ = {1};  //
    // showTableOnSelfScreen(cardsOnTable_);

    QList<int> cardsToPush;
    for (int i = 0; i < cardsOfB_.size(); ++i) {
      if (cardLabels_[i]->isMoved()) {
        cardsToPush.append(cardsOfB_[i]);
      }
    }

    // CHECK TODO

    qDebug() << "CHECKING...1";
    if (CARDS_CMP(cardsToPush, cardsOnTable_)) {
      qDebug() << "CHECKING...2";
      giveupInfoLabels_[personIndexToPosition_[1]]->hide();
      buhefaLabel_->hide();
      updateCardNumber(1, cardsToPush.size());

      chuBtn->hide();
      buchuBtn->hide();

      // B不再展示出掉的牌
      for (int i = 0; i < cardsToPush.size(); ++i) {
        cardsOfB_.removeOne(cardsToPush[i]);
      }
      displayCards();

      notifyAThatBHasJustPushedCards(cardsToPush);
      lastPushCardPerson_ = 1;
      cardsOnTable_ = cardsToPush;
      showTableOnSelfScreen(cardsOnTable_);

      if (checkIfGameOver()) return;
    } else {
      buhefaLabel_->show();
    }
  });
  connect(buchuBtn, &QPushButton::clicked, [=]() {
    displayGiveUpInfo(1);
    buhefaLabel_->hide();
    chuBtn->hide();
    buchuBtn->hide();
    sleep(50);
    tellAGiveUp();
  });
}

void PlayerB::notifyAThatBHasJustPushedCards(const QList<int>& BPushedCards) {
  castToA("BHasPushedCards", intArrayToString(BPushedCards));
}

void PlayerB::tellAGiveUp() { castToA("BHasGivenUp", "info"); }

void PlayerB::displayGiveUpInfo(int n) {
  giveupInfoLabels_[personIndexToPosition_[n]]->show();
}

void PlayerB::showRestartOrExitBtnsOnSelfScreen() {
  restartBtn_ = new QPushButton(this);
  exitBtn_ = new QPushButton(this);

  restartBtn_->setText("重新开始");
  exitBtn_->setText("退出");
  restartBtn_->setGeometry(970, 500, 50, 50);
  exitBtn_->setGeometry(1030, 500, 50, 50);
  restartBtn_->show();
  exitBtn_->show();

  connect(exitBtn_, SIGNAL(clicked()), QApplication::instance(), SLOT(quit()));

  connect(restartBtn_, &QPushButton::clicked, [=]() {
    restart();
    sleep(50);
    castToA("restart", "info");
  });
}

void PlayerB::showWinOrLoseInfo(bool win) {
  winOrLoseLabel_ = new QLabel(this);
  winOrLoseLabel_->setText(win ? "YOU WIN!!!" : "YOU LOSE!!!");
  winOrLoseLabel_->setGeometry(200, 50, 800, 400);
  winOrLoseLabel_->setFont(QFont("Helvetica", 48));
  winOrLoseLabel_->show();
}

bool PlayerB::checkIfGameOver() {
  for (int i = 0; i < cardsNum_.size(); ++i) {
    if (cardsNum_[i] == 0) {
      showWinOrLoseInfo((landlord_ == i) == (landlord_ == 1));
      showRestartOrExitBtnsOnSelfScreen();
      return true;
    }
  }
  return false;
}