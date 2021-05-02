#ifndef DIALOG_QUESTION_H
#define DIALOG_QUESTION_H

#include <QDialog>
#include <QPainter>

namespace Ui {
class Dialog_Question;
}

class Dialog_Question : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Question(QWidget *parent = nullptr);
    ~Dialog_Question();

protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::Dialog_Question *ui;
};

#endif // DIALOG_QUESTION_H
