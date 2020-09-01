#include "mainwindow.h"

#include <QByteArray>
#include <QLabel>
#include <QPushButton>
#include <QTime>

PlayerB::PlayerB(QWidget* parent) : QMainWindow(parent) {
  setFixedSize(1200, 800);

  // startListening button
  buttonStartListeningAndRequesting = new QPushButton(this);
  buttonStartListeningAndRequesting->setGeometry(376, 512, 448, 120);
  buttonStartListeningAndRequesting->setFont(QFont("Helvetica", 28));
  buttonStartListeningAndRequesting->setText("Start connecting...");
  connect(buttonStartListeningAndRequesting, SIGNAL(clicked()), this,
          SLOT(startListeningAndRequesting()));
}

PlayerB::~PlayerB() {}

void PlayerB::startListeningAndRequesting() {
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
    buttonStartListeningAndRequesting->hide();
    for (QString str : BCards.split(".")) {
      if (!str.isEmpty()) {
        cardsOfB_.append(str.toInt());
      }
    }
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

  // QString theLandlordIs = readFromBuffer(buffer, "theLandlordIs");
  // if (!theLandlordIs.isEmpty()) {
  //   setLandlord(theLandlordIs.toInt());
  // }
}
void PlayerB::socketDisconnectedFromA() {}

void PlayerB::showOthersIfCampaignInfo(int personPosition, bool ifCampaign) {
  if (personPosition == 0) {
    QLabel* jiaoORbujiao = new QLabel(this);
    jiaoORbujiao->setText(ifCampaign ? "叫" : "不叫");
    jiaoORbujiao->setGeometry(580, 400, 40, 32);
    jiaoORbujiao->show();
  } else if (personPosition == 1) {
    QLabel* jiaoORbujiao = new QLabel(this);
    jiaoORbujiao->setText(ifCampaign ? "叫" : "不叫");
    jiaoORbujiao->setGeometry(360, 200, 40, 32);
    jiaoORbujiao->show();
  } else if (personPosition == 2) {
    QLabel* jiaoORbujiao = new QLabel(this);
    jiaoORbujiao->setText(ifCampaign ? "叫" : "不叫");
    jiaoORbujiao->setGeometry(760, 200, 40, 32);
    jiaoORbujiao->show();
  }
}

void PlayerB::showDoYouWantToCampaignButtons() {
  QPushButton* yesBtn = new QPushButton(this);
  QPushButton* noBtn = new QPushButton(this);

  yesBtn->setText("Yes");
  noBtn->setText("No");
  yesBtn->setGeometry(120, 220, 50, 50);
  noBtn->setGeometry(180, 220, 50, 50);
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
  qDebug() << "Displaying cards!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
  for (int i = 0; i < cardsOfB_.size(); ++i) {
    QLabel* cardLabel = new QLabel(this);
    QPixmap cardImage =
        QPixmap("./assets/" + QString::number(cardsOfB_[i]) + ".png");
    cardImage = cardImage.scaled(QSize(330, 160), Qt::IgnoreAspectRatio,
                                 Qt::SmoothTransformation);
    cardLabel->setPixmap(cardImage);
    cardLabel->setGeometry(80 + 50 * i, 600, 360, 200);
    cardLabel->show();
  }
}