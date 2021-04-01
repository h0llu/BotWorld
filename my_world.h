#ifndef MY_WORLD_H
#define MY_WORLD_H

#include "bot.h"
#include <QWidget>
#include <vector>
#include <QPainter>
#include <QTimerEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class my_world; }
QT_END_NAMESPACE

class my_world : public QWidget
{
		Q_OBJECT

	public:
		my_world(QWidget *parent = nullptr);
		~my_world();

		std::vector<Bot*>& getWorld();

	protected:
		virtual void paintEvent(QPaintEvent *event) override;
		virtual void timerEvent(QTimerEvent *event) override;
		void paint(QPainter *painter); //рисует мир
		void makeMove(); //сделать ход каждого бота


private slots:
        void on_Kill_clicked();

        void on_pushButton_clicked();

private:
		void generateTheChosenOne(); //первый бот

		Ui::my_world *ui;
		std::vector <Bot*> world;
        const int WORLD_HEIGHT = 900;
        const int WORLD_WIDTH = 1800;
        const int RECT_SIZE = 6;
		const int COLS = WORLD_WIDTH / RECT_SIZE;
		const int ROWS = WORLD_HEIGHT / RECT_SIZE;

		friend class Bot;
};
#endif // MY_WORLD_H
