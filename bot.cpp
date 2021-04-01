#include "bot.h"
#include "my_world.h"
#include <ctime>

Bot::Bot() {}

Bot::~Bot() {
    delete[] mind;
}

void Bot::pointer_increase(int inc) { //увеличение указателя на текущую команду генома
	command_pointer += inc;
    if (command_pointer >= 64)
        command_pointer -= 64;
}

void Bot::fillVegeterian() { //заполнить геном командами фотосинтеза (команда 27)
	for (int i = 0; i < mind_size; i++) {
        mind[i] = 27;
	}
}

void Bot::live(my_world *_world) { //цикл, в котором будут выполняться команды из генома
    for (int i = 0; i < exit_condition; i++) { // exit_condition команд максимум

        if (energy >= 3)
            energy -= 3; //бот теряет энергию на каждом шаге
        else
            energy = 0;

        check_if_dead(); //проверить, не мертв ли бот (сколько энергии)

		//если бот мертвый, тогда он делает ход как умерший бот и завершает ход
		if (!is_alive) {
			live_organic(_world);
			return;
		}

        absorb_minerals(_world); //каждый ход поглощает минералы
		if (energy > 999)
			make_child(_world); //создать потомка, если энергии больше 999

        //если бот мертвый, тогда он делает ход как умерший бот и завершает ход
        if (!is_alive) {
            live_organic(_world);
            return;
        }
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//поглотить минералы
        if (mind[command_pointer] == 24) {
			energy_from_minerals();
			pointer_increase(1);
			return; //завершаем ход
		}

        //скушоть в относительном направлении (в зависимости от текущего direction бота)
        if (mind[command_pointer] == 25) {
            m_direction = (m_direction + mind[(command_pointer + 1) % mind_size]) % 8;
            pointer_increase(mind[(command_pointer + energy_from_meat(_world)) % mind_size]);
            return; //завершаем ход
        }

        //скушоть в абсолютном направлении (не зависит от текущего direction бота)
        if (mind[command_pointer] == 26) {
            m_direction = mind[(command_pointer + 1) % mind_size] % 8;
            pointer_increase(mind[(command_pointer + energy_from_meat(_world)) % mind_size]);
            return; //завершаем ход
        }

        //фотосинтез
        if (mind[command_pointer] == 27) {
            energy_from_sun(_world);
            pointer_increase(1);
            return; //завершаем ход
        }

        //относительное перемещение (относительно текущего direction бота)
        if (mind[command_pointer] == 28) {
            pointer_increase(mind[(command_pointer + move((m_direction + mind[(command_pointer + 1) % mind_size]) % 8, _world)) % mind_size]);
			return; //завершаем ход
		}

        //абсолютное перемещение (не зависит от текущего direction бота)
        if (mind[command_pointer] == 29) {
             pointer_increase(mind[(command_pointer + move(mind[(command_pointer + 1) % mind_size] % 8, _world)) % mind_size]);
             return; //завершаем ход
        }

        //относительно посмотреть (относительно текущего direction бота)
        if (mind[command_pointer] == 30) {
            pointer_increase(mind[(command_pointer + look((m_direction + mind[(command_pointer + 1) % mind_size]) % 8, _world)) % mind_size]);
			// +1 команда выполнена
		}

        //абсолютно посмотреть (не зависит от текущего direction бота)
        if (mind[command_pointer] == 31) {
            pointer_increase(mind[(command_pointer + look(mind[(command_pointer + 1) % mind_size] % 8, _world)) % mind_size]);
            // +1 команда выполнена
        }

        //абсолютно повернуть (не зависит от текущего direction бота)
        if (mind[command_pointer] == 32) {
            m_direction = mind[(command_pointer + 1) % mind_size] % 8;
			pointer_increase(2);
			// +1 команда выполнена
		}

        //относительно повернуть (относительно текущего direction бота)
        if (mind[command_pointer] == 33) {
            m_direction = (m_direction + mind[(command_pointer + 1) % mind_size]) % 8;
            pointer_increase(2);
            // +1 команда выполнена
        }

		//сколько у меня энергии?
        if (mind[command_pointer] == 34) {
            if (energy < mind[(command_pointer + 1) % mind_size] * 15) {
                pointer_increase(mind[(command_pointer + 2) % mind_size]);
			}
			else {
                pointer_increase(mind[(command_pointer + 3) % mind_size]);
			}
			// +1 команда выполнена
		}

		//сколько у меня минералов?
        if (mind[command_pointer] == 35) {
            if (minerals < mind[(command_pointer + 1) % mind_size] * 100 / mind_size) {
                pointer_increase(mind[(command_pointer + 2) % mind_size]);
			}
			else {
                pointer_increase(mind[(command_pointer + 3) % mind_size]);
			}
			// +1 команда выполнена
		}

        //какой уровень? (координата Y)
        if (mind[command_pointer] == 36) {
            //умножаем значение параметра на 1.5
            //получаем число от 0 до 95 (63 * 1.5)
            if (y < mind[(command_pointer + 1) % mind_size] * 1.5) {
                pointer_increase(mind[(command_pointer + 2) % mind_size]);
            }
            else {
                pointer_increase(mind[(command_pointer + 3) % mind_size]);
            }
        }

        //деление
        if (mind[command_pointer] == 37) {
            make_child(_world);
			pointer_increase(1);
			return; //завершаем ход
		}

		// увеличиваем счётчик, если такой команды нет
        if (mind[command_pointer] < 24 || mind[command_pointer] > 37) {
			pointer_increase(mind[command_pointer]);
			// +1 команда выполнена
		}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	}
}

void Bot::live_organic(my_world *_world) {
	// бот мертвый - он падает вниз, если там пусто и остается на месте, если там занято
	if (look(4, _world) == 2) {
		int coordinate = getCoordinateFromDirection(4, _world);
        _world->world[coordinate] = this;
        _world->world[y * _world->COLS + x] = NULL;
		//переопределить координаты бота
		y++;
	}
}

int Bot::move(int direction, my_world *_world) { //двинуться в направлении
    int coordinateToGo = getCoordinateFromDirection(direction, _world);
	//бот должен сдвинуться в нужном направлении
    //метод возвращает 2 - пусто, 3 - стена, 4 - умерший бот, 5 - живой бот
    if (look(direction, _world) == 2) {
        _world->world[coordinateToGo] = this;
        _world->world[y * _world->COLS + x] = NULL;
		//переопределить координаты бота
		if (direction == 0 || direction == 1) {
			y -= 1;
			x += direction;
			if (x == _world->COLS) {
				x = 0;
			}
		}
		else if (direction == 7) {
			y -= 1;
			if (x == 0) {
				x = _world->COLS - 1;
			}
			else {
				x -= 1;
			}
		}
		else if (direction == 2) {
			if (x == _world->COLS - 1) {
				x = 0;
			}
			else {
				x += 1;
			}
		}
		else if (direction == 6) {
			if (x == 0) {
				x = _world->COLS - 1;
			}
			else {
				x -= 1;
			}
		}
		else if (direction == 3 || direction == 4 || direction == 5) {
			y += 1;
			x = x - direction + 4;
			if (x == _world->COLS) {
				x = 0;
			}
			else if (x == -1) {
				x = _world->COLS - 1;
			}
		}
	}
//    if (direction == 2 || direction == 6) {
//        //если по данному направлению мертвый бот, то мы смотрим, можно ли толкнуть его вбок (если там пусто) и перемещаемся на его клетку
//        if (look(direction, _world) == 4 && _world->world[coordinateToGo]->look(direction, _world) == 2) {
//            _world->world[coordinateToGo]->move(direction, _world);
//            _world->world[coordinateToGo] = this;
//            _world->world[y * _world->COLS + x] = NULL;
//            //переопределить координаты бота
//            if (direction == 0 || direction == 1) {
//                y -= 1;
//                x += direction;
//                if (x == _world->COLS) {
//                    x = 0;
//                }
//            }
//            else if (direction == 7) {
//                y -= 1;
//                if (x == 0) {
//                    x = _world->COLS - 1;
//                }
//                else {
//                    x -= 1;
//                }
//            }
//            else if (direction == 2) {
//                if (x == _world->COLS - 1) {
//                    x = 0;
//                }
//                else {
//                    x += 1;
//                }
//            }
//            else if (direction == 6) {
//                if (x == 0) {
//                    x = _world->COLS - 1;
//                }
//                else {
//                    x -= 1;
//                }
//            }
//            else if (direction == 3 || direction == 4 || direction == 5) {
//                y += 1;
//                x = x - direction + 4;
//                if (x == _world->COLS) {
//                    x = 0;
//                }
//                else if (x == -1) {
//                    x = _world->COLS - 1;
//                }
//            }
//        }
//    }
	return look(direction, _world);
}

int Bot::look(int direction, my_world *_world) { //посмотреть в направлении
    //если бот идет наверх или вниз, определяем, есть ли там стена
	switch (direction) {
		case 0:
            if (y == 0)
                return 3; //стена
            break;
		case 1:
            if (y == 0)
                return 3; //стена
            break;
		case 7:
			if (y == 0)
                return 3; //стена
            break;
		case 3:
            if (y == _world->ROWS - 1)
                return 3; // стена
        break;
		case 4:
            if (y == _world->ROWS - 1)
                return 3; // стена
        break;
		case 5:
			if (y == _world->ROWS - 1)
				return 3; // стена
        break;
    }
    //определение координаты по направлению бота
    int coordinate = getCoordinateFromDirection(direction, _world);
    if (_world->world[coordinate] == NULL)
		return 2; // пусто

       if (!(_world->world[coordinate]->is_alive))
		return 4; // умерший бот

	else
		return 5; // живой бот
}

int Bot::getCoordinateFromDirection(int direction, my_world *_world) { //получить координату из направления
	int coordinate = -1;
	//определение координаты по направлению
	if (direction == 0 || direction == 1) {
        if (y == 0) {
            if (x + direction == _world->COLS)
                coordinate = y * _world->COLS;
            else
                coordinate = y * _world->COLS + x + direction;
        }
        else {
            if (x + direction == _world->COLS)
                coordinate = (y - 1) * _world->COLS;
            else
                coordinate = (y - 1) * _world->COLS + x + direction;
        }
	}
	else if (direction == 7) {
        if (y == 0) {
            if (x == 0)
                coordinate = y * _world->COLS + _world->COLS - 1;
            else
                coordinate = y * _world->COLS - 1 + x;
        }
        else {
            if (x == 0)
                coordinate = (y - 1) * _world->COLS + _world->COLS - 1;
            else
                coordinate = (y - 1) * _world->COLS - 1 + x;
        }
	}
	else if (direction == 2) {
		if (x == _world->COLS - 1)
			coordinate = y * _world->COLS;
		else
			coordinate = y * _world->COLS + 1 + x;
	}
	else if (direction == 6) {
		if (x == 0)
			coordinate = y * _world->COLS + _world->COLS - 1;
		else
			coordinate = y * _world->COLS - 1 + x;
	}
	else if (direction == 3 || direction == 4 || direction == 5) {
        if (y == _world->ROWS - 1) {
            if (x - direction + 4 == -1)
                coordinate = y * _world->COLS + _world->COLS - 1;
            else if (x - direction + 4 == _world->COLS)
                coordinate = y * _world->COLS;
            else
                coordinate = y * _world->COLS + x - direction + 4;
        }
        else {
            if (x - direction + 4 == -1)
                coordinate = (y + 1) * _world->COLS + _world->COLS - 1;
            else if (x - direction + 4 == _world->COLS)
                coordinate = (y + 1) * _world->COLS;
            else
                coordinate = (y + 1) * _world->COLS + x - direction + 4;
        }
	}
	return coordinate;
}
void Bot::check_if_dead() { //бот становится мертвым, если энергии меньше 0
	if (energy_min()) {
		is_alive = false;
	}
}

void Bot::make_child(my_world *_world) { //деление бота
	energy -= 150; //150 единиц энергии на создание копии
	//если у бота не хватило энергии, тогда смерть
	if (energy <= 0) {
		is_alive = false;
		return;
	}
	int dir = -1; //направление, по которому свободна клетка
	//проверим, окружен ли бот, и найдем свободное место
	for (int i = 0; i < 8; i++) {
		if (look(i, _world) == 2) {
			dir = i;
            if (rand() < RAND_MAX / 8)
                break;
		}
	}
    //если бот окружен, то он умирает (в данной версии у бота есть ШАНС умереть от того, что он окружен)
	if (dir == -1) {
        if (rand() < RAND_MAX / 2) {
            energy = 0;
            is_alive = false;
        }
		return;
	}
	//если все хорошо, тогда создаем потомка
	Bot *new_bot = new Bot();
	//направление случайное
    new_bot->m_direction = rand() % 8;
	//цвет как у предка
	new_bot->colorB = colorB;
	new_bot->colorR = colorR;
	new_bot->colorG = colorG;
	//определяем координаты
	// - - - - - - - - - - - - - - - - - - - - - - -
	if (dir == 0 || dir == 1) {
		new_bot->y = y - 1;
		new_bot->x = x + dir;
		if (x == _world->COLS - 1) {
			new_bot->x = 0;
		}
	}
	else if (dir == 7) {
		new_bot->y = y - 1;
		if (x == 0) {
			new_bot->x = _world->COLS - 1;
		}
		else {
            new_bot->x = x - 1;
		}
	}
	else if (dir == 2) {
		new_bot->y = y;
		if (x == _world->COLS - 1) {
			new_bot->x = 0;
		}
		else {
			new_bot->x = x + 1;
		}
	}
	else if (dir == 6) {
		new_bot->y = y;
		if (x == 0) {
			new_bot->x = _world->COLS - 1;
		}
		else {
			new_bot->x = x - 1;
		}
	}
	else if (dir == 3 || dir == 4 || dir == 5) {
		new_bot->y = y + 1;
		new_bot->x = x - dir + 4;
		if (x == _world->COLS - 1 && dir == 3) {
            new_bot->x = 0;
		}
		else if (x == 0 && dir == 5) {
            new_bot->x = _world->COLS - 1;
		}
	}
	// - - - - - - - - - - - - - - - - - - - - - - -

	//копируем геном предка
	for (int i = 0; i < mind_size; i++) {
		new_bot->mind[i] = mind[i];
	}

    //но есть шанс мутировать (33%)
    if (rand() < RAND_MAX / 3) {
		int adr = rand() % mind_size; //случайная ячейка генома
		int value = rand() % mind_size; //случайное значение ячейки
		new_bot->mind[adr] = value;
	}

	//отбираем у предка половину энергии и минералов
	new_bot->energy = energy / 2;
	energy /= 2;
	new_bot->minerals = minerals / 2;
	minerals /= 2;

    _world->world[new_bot->y * _world->COLS + new_bot->x] = new_bot;
}

void Bot::energy_from_sun(my_world *_world) { //бот получает энергию от солнца
    if (y < _world->ROWS / 16)
		energy += 11;
    else if (y < 2 *_world->ROWS / 16)
		energy += 10;
    else if (y < 3 *_world->ROWS / 16)
		energy += 9;
    else if (y < 4 *_world->ROWS / 16)
		energy += 8;
    else if (y < 5 *_world->ROWS / 16)
		energy += 7;
    else if (y < 6 *_world->ROWS / 16)
		energy += 6;
    else if (y < 7 *_world->ROWS / 16)
		energy += 5;
    else if (y < 8 *_world->ROWS / 16)
		energy += 4;
    else if (y < 9 *_world->ROWS / 16)
		energy += 3;
    else if (y < 10 *_world->ROWS / 16)
		energy += 2;
    else if (y < 11 *_world->ROWS / 16)
		energy += 1;
	greenize();
}

int Bot::energy_from_meat(my_world *_world) { //получение энергии от поедания + само поедание
    int meat_coordinate = getCoordinateFromDirection(m_direction, _world);
    if (look(m_direction, _world) == 2)
		return 2; //пусто
    else if (look(m_direction, _world) == 3)
		return 3; //стена
	rednize();
	//если в клетке умерший бот
    if (!(_world->world[meat_coordinate]->get_is_alive())) {
        delete _world->world[meat_coordinate];
        _world->world[meat_coordinate] = NULL;
		//100 единиц энергии за умершего бота
		energy += 100;
		return 4;
	}
	//если в клетке живой бот
	else {
        //если у жертвы больше энергии, чем у напавшего (жертва жирнее, больше, выше, что угодно)
        //тогда хищник получает четверть энергии жертвы (оторвал кусок и убежал), а жертва не умирает
        if (_world->world[meat_coordinate]->energy > energy) {
            energy += _world->world[meat_coordinate]->energy / 4;
            _world->world[meat_coordinate]->energy /= 4;
            return 5;
        }
		//бот получает половину энергии того, кого съел
        energy += _world->world[meat_coordinate]->energy / 2;
        delete _world->world[meat_coordinate];
        _world->world[meat_coordinate] = NULL;
		return 5;
	}
}

void Bot::energy_from_minerals() { //бот получает энергию из минералов, 4 единицы энергии за каждый минерал
    if (minerals > 0)
        bluenize();
    if (minerals > 100) { //максимум 100 минералов за ход
		minerals -= 100;
        energy += 200;
	}
	else {
        energy += 2 * minerals;
		minerals = 0;
	}
}

void Bot::absorb_minerals(my_world *_world) { //бот поглощает минералы
    if (y > (_world->ROWS / 2) && y < (_world->ROWS / 2 + _world->ROWS / 6))
		minerals += 1;
    else if (y > (_world->ROWS / 2 + _world->ROWS / 6) && y < (_world->ROWS / 2 + 2 * _world->ROWS / 6))
		minerals += 2;
    else if (y > _world->ROWS / 2 + 2 * _world->ROWS / 6)
		minerals += 3;
}

bool Bot::energy_max() { //достаточно энергии для деления
    return energy > 999;
}

bool Bot::energy_min() { // энергии <= 0 - смерть
    return energy <= 0;
}

void Bot::bluenize() { //бот синеет от поедания минералов
    if (colorG - 64 >= 0)
        colorG -= 64;
	else
		colorG = 0;


    if (colorR - 64 >= 0)
        colorR -= 64;
	else
		colorR = 0;


    if (colorB <= 191)
        colorB += 64;
	else
		colorB = 255;
}

void Bot::greenize() { //бот зеленеет от фотосинтеза
    if (colorB - 64 >= 0)
        colorB -= 64;
	else
		colorB = 0;


    if (colorR - 64 >= 0)
        colorR -= 64;
	else
		colorR = 0;


    if (colorG <= 191)
        colorG += 64;
	else
		colorG = 255;
}

void Bot::rednize() { //бот краснеет от поедания мяса
    if (colorG - 64 >= 0)
        colorG -= 64;
	else
		colorG = 0;


    if (colorB - 64 >= 0)
        colorB -= 64;
	else
		colorB = 0;


    if (colorR <= 191)
        colorR += 64;
	else
		colorR = 255;
}

int Bot::getRed() {
	return colorR;
}

int Bot::getBlue() {
	return colorB;
}

int Bot::getGreen() {
	return colorG;
}

bool Bot::get_is_alive() { //геттер для поля is_alive для относительных команд
	return is_alive;
}

void Bot::setX(int x) {
	this->x = x;
}

int Bot::getX() {
	return x;
}

void Bot::setY(int y) {
	this->y = y;
}

int Bot::getY() {
	return y;
}
