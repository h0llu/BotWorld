#include "my_world.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	my_world w;
	w.show();
	return a.exec();
}
