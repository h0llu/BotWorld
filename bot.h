#ifndef BOT_H
#define BOT_H
class my_world;
//реализация ботов, далее каждый бот записывается в вектор (что-то типа вектора объекта класса World)
//для отрисовки в мире
class Bot {
	public:
		Bot();
        ~Bot();

		void pointer_increase(int inc); //увеличение значения указателя на текущую ячейку генома
		void live(my_world *_world); //цикл, в котором будут выполняться команды из генома
		void live_organic(my_world *_world); //поведение мертвых ботов
        void energy_from_sun(my_world *_world); //бот получает энергию от солнца
        void absorb_minerals(my_world *_world); //бот поглощает минералы каждый ход в зависимости от высоты
		void energy_from_minerals(); //бот получает энергию из минералов
		int energy_from_meat(my_world *_world); //получение энергии от поедания + само поедание
		void make_child(my_world *_world); //деление бота, если энергии больше 999
		void check_if_dead(); //бот становится мертвым, если энергии меньше 0
		bool energy_max(); //достаточно энергии для деления
		bool energy_min(); //энергии меньше 0
		bool get_is_alive(); //геттер для параметра is_alive для относительных команд
		int move(int direction, my_world *_world); //двинуться в направлении direction
		int look(int direction,  my_world *_world); //посмотреть в направлении direction

		void setX(int x); // сеттер для координаты
		int getX(); //геттер для координаты
		void setY(int y); // сеттер для координаты
		int getY(); //геттер для координаты

		int getCoordinateFromDirection(int direction, my_world *_world); //получить координату из направления

		void bluenize(); //немного посинеть, поглотил минералы
		void greenize(); //немного позеленеть, фотосинтез
		void rednize(); //немного покраснеть, съел кого-то

		//геттеры для цветов бота
		int getRed();
		int getBlue();
		int getGreen();

		void fillVegeterian(); // геном первого бота заполняется командами фотосинтеза

	private:

        bool checked = false; //проверка: сделал ли бот ход в данный момент
        //(бот может подвинуться на клетку ниже, тогда мир дойдет до этой клетки и бот сделает еще один ход)

        const int mind_size = 64; //количество ячеек в геноме
		int* mind = new int[mind_size]; //геном бота
		int x = -1, y = -1; //координаты бота в мире
		int colorG = 255, colorB = 0, colorR = 0; //цвет бота: зеленый - фотосинтез, красный - хищник, синий - минералы
		int energy = 5; //здоровье бота от 0 до 999
		int minerals = 0; //количество минералов у бота
		int command_pointer = 0; //указатель на текущую команду в геноме
        const int exit_condition = 10; //условие выхода из цикла: совершено n команд
        int m_direction = 0; //направлние бота от 0 до 7: 0 - вверх, 1 - вверх вправо и тд.
		bool is_alive = true; //true - жив, false - мертв

		friend class my_world;
};

#endif // BOT_H

//+2 - пусто, +3 - стена, +4 - органика, +5 - бот
