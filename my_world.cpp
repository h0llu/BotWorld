#include "my_world.h"
#include "ui_my_world.h"
#include <QPainter>
#include <QThread>

my_world::my_world(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::my_world)
{
	ui->setupUi(this);
	generateTheChosenOne();
    srand(time(0));
    startTimer(20);
}

my_world::~my_world()
{
	delete ui;
}

void my_world::generateTheChosenOne() { //первый бот, а также основные характеристики мира
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {    // обнуление вектора
            world.push_back(NULL);
		}
	}
	// создаем первого бота
	Bot *bot = new Bot();
	// размещаем посередине поля
	bot->setX(COLS / 2 - 1);
    bot->setY(ROWS / 2 - 7);
	// геном бота полностью заполнен командами фотосинтеза
	bot->fillVegeterian();
    bot->energy = 900;
	// закидываем бота в нужную ячейку вектора со всеми ботами
	world[bot->getY() * COLS + bot->getX()] = bot;
}


void my_world::paintEvent(QPaintEvent *event) {
	Q_UNUSED(event);

    QPainter *painter = new QPainter(this);
    paint(painter);
}

void my_world::paint(QPainter *painter) { //отрисовка мира
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			if (world[i * COLS + j] == NULL) {
				painter->fillRect(j * RECT_SIZE, i * RECT_SIZE, RECT_SIZE, RECT_SIZE, QBrush(Qt::white));
			}
			else if (world[i * COLS + j]->is_alive){
				painter->fillRect(j * RECT_SIZE, i * RECT_SIZE, RECT_SIZE, RECT_SIZE, QBrush(QColor(world[i * COLS + j]->getRed(), world[i * COLS + j]->getGreen(), world[i * COLS + j]->getBlue())));
				painter->drawRect(j * RECT_SIZE, i * RECT_SIZE, RECT_SIZE, RECT_SIZE);
			}
			else {
				painter->fillRect(j * RECT_SIZE, i * RECT_SIZE, RECT_SIZE, RECT_SIZE, QBrush(Qt::yellow, Qt::SolidPattern));
				painter->drawRect(j * RECT_SIZE, i * RECT_SIZE, RECT_SIZE, RECT_SIZE);
			}
		}
	}
}

void my_world::makeMove() {
	for (int i = 0; i < ROWS * COLS; i++) {
        if (world[i] != NULL) {
            if (!world[i]->checked) {
                world[i]->checked = true;
                world[i]->live(this);
            }
		}
    }
    for (int i = 0; i < ROWS * COLS; i++) {
        if (world[i] != NULL)
            world[i]->checked = false;
    }
}

void my_world::timerEvent(QTimerEvent *e) {
	Q_UNUSED(e);

	makeMove();
    this->repaint();
}

void my_world::on_Kill_clicked()
{
    for (int i = 0; i < ROWS * COLS; i++) {
        if (world[i] != NULL)
            if (rand() < RAND_MAX / 2) {
                delete world[i];
                world[i] = NULL;
            }
    }
}

void my_world::on_pushButton_clicked()
{
    for (int i = 0; i < ROWS * COLS; i++) {
        if (world[i] != NULL) {
                int adr = rand() % world[i]->mind_size;
                int value = rand() % world[i]->mind_size;
                world[i]->mind[adr] = value;
        }
    }
}
