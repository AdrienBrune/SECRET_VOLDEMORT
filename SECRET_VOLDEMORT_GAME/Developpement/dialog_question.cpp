#include "dialog_question.h"
#include "ui_dialog_question.h"

Dialog_Question::Dialog_Question(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Question)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);

    connect(ui->buttonYes, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->buttonNo, SIGNAL(clicked()), this, SLOT(reject()));
}

Dialog_Question::~Dialog_Question()
{
    delete ui;
}

void Dialog_Question::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.setOpacity(0.96);
    painter.setBrush(QColor(19,19,19));
    painter.drawRoundedRect(QRect(0, 0, width(), height()), 5, 5, Qt::SizeMode::RelativeSize);
}
