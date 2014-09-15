#ifndef GAME_H
#define GAME_H
#include <QtOpenGL>

class Game : public QGLWidget
{
    Q_OBJECT

public:
    Game(QWidget *parent = 0);
    ~Game();

protected:
    // метод для проведения инициализаций, связанных с OpenGL
    /*virtual*/ void initializeGL();
    // метод вызывается при изменении размеров окна виджета
    /*virtual*/ void resizeGL(int nWidth, int nHeight);
    // метод, чтобы заново перерисовать содержимое виджета
    /*virtual*/ void paintGL();
    // метод обработки событий мыши при отжатии клавиши мыши
    /*virtual*/ void mouseReleaseEvent(QMouseEvent* pe);
    // метод обработки события мыши при нажатии клавиши мыши
    /*virtual*/ void mousePressEvent(QMouseEvent* pe);

private:
    void loadLivel(int index);              // загрузить уровень № index

    void drawWorld();                       // нарисовать мир
    void circle(float r, int s);            // нарисовать круг c шагом s градусов
    void circleTex(float r, int s);         // нарисовать круг c шагом s градусов и текущей текстурой
    void oval(float rx, float ry, int s);   // нарисовать овал c шагом s градусов и радиусами rx ry
    void ovalTex(float rx, float ry, int s);// нарисовать овал c шагом s градусов и радиусами rx ry и текстурой
    void rectangle(float w, float h);       // нарисовать прямоугольник шининой w, высотой h залитый основным цветом
    void rectangle2(float w, float h);      // нарисовать прямоугольник шининой w, высотой h без заливки
    void rectangleTex(float w, float h);    // нарисовать прямоугольник шининой w, высотой h с текстурой
    void chip0(float rx);                   // нарисовать кристалл типа 0 размера px (зеленый)
    void chip1(float rx);                   // нарисовать кристалл типа 1 размера px (голубой)
    void chip2(float rx);                   // нарисовать кристалл типа 2 размера px (красный)
    void chip3(float rx);                   // нарисовать кристалл типа 3 размера px (лиловый)
    void chip4(float rx);                   // нарисовать кристалл типа 4 размера px (желтый)
    void paintExit();                       // нарисовать выход
    void twoBolls(float rx);                // нарисовать два шара рядом снизу (преграда)
    void twoBollsUp(float rx);              // нарисовать два шара рядом сверху (преграда)
    void threeBolls(float rx);              // нарисовать три шара рядом (преграда)
    void fourBolls(float rx);               // нарисовать четыре шара рядом (преграда)
    void paintChips();                      // нарисовать кристаллы
    void paintBolls();                      // нарисовать шары
    void paintTable();                      // нарисовать стол
    void rstButton(QString ln);             // извлечь координаты кнопки перезапуска уровня
    void extButton(QString ln);             // извлечь координаты кнопки выхода на стартовый экран
    void paintRst();                        // нарисовать кнопку перезапуска уровня
    void showRest();                        // нарисовать табло с остатком шаров
    void drawBtns();                        // нарисовать кнопки стартового экрана
    void setStartBtn();                     // задать положение кнопок стартового экрана
    void setTbl(QString ln);                // задать положение табло остатка шаров
    void setEnd(QString ln);                // задать положение точки выхода с уровня
    int  frkDX(int i);                      // получить dx для развилки № i
    int  frkDY(int i);                      // получить dy для развилки № i
    bool lider(int index);                  // true - шар № index первый на своем фрагменте траектории
    int  colorBoll(int index);              // получить случайный цвет в диапазоне index
    void setBoll(QString ln);               // установить шар на поле с параметрами "ln"
    void setSource(QString ln);             // установить источник на поле с параметрами "ln"
    void genBolls();                        // генерация новых шаров (если возможно)
    void setTable(QString ln);              // установить картинку фона уровня



    void loadTextures();                    // создать тектуры
    bool startLiv();                        // выход шаров true - шар был выдан
    void forward();                         // расчет движения вперед
    bool goForward(int index);              // true - необходимо сдвинуться вперед для шара index
    void killBolls();                       // удалить шары из списка
    bool intervalBolls(int index);          // true - шар index не соприкасается с другими
    void rotor();                           // совершить один шаг вращения шаров при нажатии
    void killThree();                       // найти и убрать три и более подряд шаров
    void setChip(int index, QString ln);    // установить кристалл типа index, координаты в строке ln
    void setFork(int index, QString ln);    // установить развилку типа index, координаты в строке ln
    void killChip();                        // удалить кристаллы
    bool searchBolls(int index);            // составить список шаров, контактирующих с index по кругу против часовой стрелки. False - нет таких или только 1



private slots:
    void timerStep();                       // событие таймера - построение нового кадра
    void nextLivel();                       // переход на следующий уровень
    void selLivel(int index);               // выбор уровня по сигналу из окна выбора
    void toHome();                          // выход на начальный экран
    void enlBtn();                          // просто разрешить кнопки на экране
};

#endif // GAME_H
