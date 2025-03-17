#ifndef Marsaxod_h
#define Marsaxod_h
#include <Arduino.h>
#include <Servo.h>


#define L "L"
#define R "R"
#define x "x"
#define y "y"
#define CH1 "CH1"
#define CH2 "CH2"
#define CH5 "CH5"
#define SWC "switch"


//класс для работы с датчиком препятствия
class barrier{
	public:
		barrier(String tipe); //функция инициализации датчика, tipe - определяет тип датчика L если левый, R если правый
		bool getBarrier(); //функция передающяя наличие или отсутствие препятствия 
	private:
		String _tipe_;
};

barrier::barrier(String tipe){
	_tipe_ = tipe; // записываем тип датчика 
	if (tipe == "R"){
		pinMode(6, INPUT); // назначаем нужный порт
	}
	if (tipe == "L"){
		pinMode(7, INPUT); // назначаем нужный порт
	}
}

bool barrier::getBarrier(){
	if (_tipe_ == "R"){
		return !digitalRead(6); // возвращаем значение с датчика
	}
	if (_tipe_ == "L"){
		return !digitalRead(7); // возвращаем значение с датчика
	}
	return 0;
}



void brake(int col = 5, int time = 50, String dir="f"){ // !!! работает только с инициализованым двигателем
	// если dir задан как b  тоесть торможение при изначальном движении назад
	if (dir == "b"){
		// цикл для торможения
		for (int i = 0; i<col; i++){
			// включаем двигатели на максимум
			analogWrite(5, 255);
			analogWrite(11, 255);
			digitalWrite(A2, i%2==0); // при чётных значениях i будет идти движение, при нечётных - простой
			digitalWrite(A3, 0);
			digitalWrite(A0, i%2==0);// при чётных значениях i будет идти движение, при нечётных - простой
			digitalWrite(A1, 0);
			delay(time);// ждём
		}
	}
	// если dir задан как f  тоесть торможение при изначальном движении вперёд
	if (dir == "f"){
		// цикл для торможения
		for (int i = 0; i<col; i++){
			// включаем двигатели на максимум
			analogWrite(5, 255);
			analogWrite(11, 255);
			digitalWrite(A3, i%2==0);// при чётных значениях i будет идти движение, при нечётных - простой
			digitalWrite(A2, 0);
			digitalWrite(A1, i%2==0);// при чётных значениях i будет идти движение, при нечётных - простой
			digitalWrite(A0, 0);
			delay(time);// ждём
		}
	}
	// теперь выключаем все двигатели
    analogWrite(5, 0);
    analogWrite(11, 0);
    digitalWrite(A2, 0);
    digitalWrite(A3, 0);
    digitalWrite(A0, 0);
    digitalWrite(A1, 0);	
}



// класс для работы с приёмником
class receiver {
	public:
		receiver(String channel); // режим настройки приёмника, принимает название канала или название оси, тумблера (x, y, switch) 
		int getCH(int minLimit, int maxLimit, int defaultValue); // режим получения данных с приёмника
		//minLimit - значение при минимальном положении стика или тумблера
		//maxLimit - значение при максимальном положении стика или тумблера
		//defaultValue - значение когда стик или тумблер на нуле
	private:
		int _chen;
};

receiver::receiver(String channel){
	if (channel == "CH1" || channel == "x"){
		_chen = 2;
		pinMode(2, INPUT);
	}
	if (channel == "CH2" || channel == "y"){
		_chen = 3;
		pinMode(3, INPUT);
	}
	if (channel == "CH5" || channel == "switch"){
		_chen = 4;
		pinMode(4, INPUT);
	}
}

int receiver::getCH(int minLimit, int maxLimit, int defaultValue){
  // считываем сигналы от радиоприёмника
  // время ожидания импулься 30000мкс=30мс, что соответствует 33.3Гц, т.е. 50Гц с небольшим запасом
  int ch = pulseIn(_chen, HIGH, 30000);
  if (ch < 100) return defaultValue; // если значение слишком мало возвращаем стандартное значение
  // считается, что минимальная длительность импульса 1000мкс, а максимальная 2000мкс
  return map(ch, 1000, 2000, minLimit, maxLimit); // возвращаем значение
}



// класс для работы с мотороми
class motors {
	public:
		motors(String tipe); // настроить мотор, принимает L для подключения левого мотора или R для правого
		void setSpd(int spd); // задать скорость от -255 до 255
		void stopMotor(); // остановить двигатель
		
	private:
		String _tipe;
		int _col=20;
		int _time=50;
		int _spd;
};

void motors::stopMotor(){
	if (_tipe=="L"){
		// останавливаем вращение на необходимом двигателе
		digitalWrite(A0, 0);
		digitalWrite(A1, 0);
		analogWrite(11, 0);
	}
	if (_tipe=="R"){
		// останавливаем вращение на необходимом двигателе
		digitalWrite(A2, 0);
		digitalWrite(A3, 0);
		analogWrite(5, 0);
	}
	_spd=0;
}

motors::motors(String tipe){
	_tipe=tipe;
	if (tipe == "L"){// если выбран тип левых двигателей настраиваем необходимые порты
		pinMode(11, OUTPUT);
		pinMode(A0, OUTPUT);
		pinMode(A1, OUTPUT);
	}
	if (tipe == "R"){// если выбран тип правых двигателей настраиваем необходимые порты
		pinMode(5, OUTPUT);
		pinMode(A2, OUTPUT);
		pinMode(A3, OUTPUT);
	}
}
void motors::setSpd(int spd){
	_spd=spd;
	if (spd>20){// если скорость больше нуля (тоесть движение вперёд) и достаточная для начала вращения мотора 
		if (_tipe=="L"){// если выбран тип левых двигателей 
			digitalWrite(A0, 1);
			digitalWrite(A1, 0);
			analogWrite(11, spd);
		}
		if (_tipe=="R"){// если выбран тип правых двигателей 
			digitalWrite(A2, 1);
			digitalWrite(A3, 0);
			analogWrite(5, spd);
		}
	}
	else if (spd<=-20){// если скорость меньше нуля (тоесть движение назад) и достаточная для начала вращения мотора 
		if (_tipe=="L"){
			digitalWrite(A0, 0);
			digitalWrite(A1, 1);
			analogWrite(11, -1*spd);
		}
		if (_tipe=="R"){
			digitalWrite(A2, 0);
			digitalWrite(A3, 1);
			analogWrite(5, -1*spd);
		}
	}
	else {
		if (_tipe=="L"){
			// останавливаем вращение на необходимом двигателе
			digitalWrite(A0, 0);
			digitalWrite(A1, 0);
			analogWrite(11, 0);
		}
		if (_tipe=="R"){
			// останавливаем вращение на необходимом двигателе
			digitalWrite(A2, 0);
			digitalWrite(A3, 0);
			analogWrite(5, 0);
		}
	}
	
}



// класс для работы с сервоприводом датчика расстаяния
class servo {
	public:
		setServo();//функция инициализации серво привода
		void setAngle(int angle);//функция поворота привода на заданый угол
	private:
	Servo mys;
		
};

servo::setServo(){
	mys.attach(9);//настраиваем сервопривод
};

void servo::setAngle(int angle){
	mys.write(angle);//задаём угол на сервопривод
}



void sonarSetup(){//функция настройки датчика расстояния 
	pinMode(12, OUTPUT);
	pinMode(13, INPUT);
};

float getCorDistant(){//функция для получения точного расстояния с датчика
	float sum = 0;//задаём переменной, которая будет являтся средним арефметическим 50 показаний датчика, значение 0
	for (int i = 0; i<50; i++){
		digitalWrite(12, 0);// выключаем порт ответственный за пин trig датчика
		delay(1);// ждём 1 миллисекунду (вдруг датчик до этого был включен), чтобы избежать помех (защита от дурака)
		digitalWrite(12, 1);// посылаем высокий сигнал на trig
		delayMicroseconds(10);// в течении 10 микросекунд
		digitalWrite(12, 0);//затем отключаем порт
		float dist = (float)pulseIn(13, HIGH, 200000)/58.0; // вычисляем расстояние полученное при этом измерении
		sum = sum+(dist/50.0);//находем среднее арефметическое
	}
	return sum;//возвращаем +- точное расстояние с датчика
};

int getDistant(){// функция для получения значений с датчика напрямую
	digitalWrite(12, 0);// выключаем порт ответственный за пин trig датчика
	delay(1);// ждём 1 миллисекунду (вдруг датчик до этого был включен), чтобы избежать помех (защита от дурака)
	digitalWrite(12, 1);// посылаем высокий сигнал на trig
	delayMicroseconds(10);// в течении 10 микросекунд
	digitalWrite(12, 0);//затем отключаем порт
	int dist = pulseIn(13, HIGH, 200000)/58;// вычисляем расстояние полученное при этом измерении
	return dist;//возвращаем расстояние
}

#endif



