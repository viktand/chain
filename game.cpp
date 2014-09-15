#include "game.h"
#include <QtGui>      // подключаем модуль QtGui
#include <math.h>     // подключаем математическую библиотеку
#include <QTimer>
#include <QVector>
#include <endlivel.h>
#include <selectlivel.h>
#include <helpwnd.h>
#include <about.h>
#include <QSound>

//#include <QDebug> // qDebug() << "...";

GLuint	texture[60];         // текстуры
QTimer  *timer;
HelpWnd *hwnd;              // окно правил игры
About   *abv;               // окно "о"
int     couBolls=0;         // кол-во выданных шаров
int     maxBolls=100;       // максимальное кол-во шаров
int     lColor=100;         // цвет предыдущего выданного шара (чтобы не повторяться)
int     lColor1=100;        // цвет предпредыдущего выданного шара (чтобы не повторяться)
float   screenX,screenY;    // базовые размеры окна программы - виртуальный размер в котором
                            // расчитываеются все события и построения, а потом переносятся на реальные размеры
float   rScreenX,rScreenY;  // реальные размеры окна
bool    goStop=false;       // остановить движение вперед
int     rotat=-1;           // нажатый шар - вокруг него пойдет вращение
int     rotStep=0;          // шаг вращения шаров
int     d1=1,d2=-1;         // направление движения по У для вращаемых шаров
int     rx1,rx2,ry1,ry2;    // координаты конечных точек для вращения шаров
bool    bx1,bx2,by1,by2;    // разрешение для вращения по координатам
const   double pi=3.14159265359;
int     startX,startY;      // начальные координаты шаров в текущем уровне
int     stopX,stopY;        // конечная позиция шаров в текущем уровне
int     livN=0;             // номер текущего уровня
bool    rot=true;           // разрешение круть шары
int     chpCou=0;           // кол-во кристаллов
int     frkCou=0;           // количество развилок
int     colors=0;           // кол-во вариантов цветов при генерации шаров
int     dBolls=0;           // изменение числа шаров после очередного пересчета (бонусы и т.д.)
int     rstX,rstY;          // координаты кнопки перезапуска уровня
int     extX,extY;          // координаты кнопки выхода на стартовый экран
bool    rstPress=false;     // флаг нажатия на кнопку перезапуска
float   bSizeX=1;           // масштаб 1 - для окна 1000Х1000 пикс.
float   bSizeY=1;           // масштаб 1 - для окна 1000Х1000 пикс.
float   b1x,b1y,b2x,b2y,b3x,b3y; // положение кнопок стартового экрана
int     brx,bry,bex,bey;    // положение нопок перезапуска и выхода
int     tblX,tblY;          // координаты табло остатка шаров
QSound  *sound;             // фоновая мелодия
QSound  *bum;               // звук уничтожения шара
QSound  *rotory;            // звук вращения шаров
QSound  *chipKill;          // звук исчезновения кристалла
QSound  *four;              // звук бонуса за четыре шара
float   rotAl=0;            // угол поворота группы шаров (после нажатия)
int     srcBolls=0;         // количество шаров, еще не вышедших из ограниченных источников
bool    killy=true;         // разрешить снятие шаров за пустое вращение
bool    forChip=true;       // разрешить бонус за снятие кристалла
bool    addBoll=false;      // добавить шар. Ну там долго объяснять где и когда.
bool    noBtn=false;        // не обрабатывать нажатия мыши на игровом экране


EndLivel    *eWind;
SelectLivel *sl;

struct bolls{
        int     x;
        int     y;
        bool    life;
        int     color;
        int     dx;     // направление и скорость >0 - слева направо, <0 - наоборот
        int     dy;
        float   scl;    // коэф. уменьшения размера
        bool    decMax; // true - уменьшить общее кол-во шаров после удаления этого
        int     way;    // номер отрезка траектории, на котором находится шар
        bool    lider;  // true - шар первый на своем отрезке траектории
        bool    go;     // true - последняя проверка показала, что путь перед шаром сободен
        bool    xBoll;  // невидимый шар, который движется только если не все шары выданы на поле, т.е. тормоз для цепи
};
struct livs{
        int x1;
        int y1;
        int x2;
        int y2;
        int dx;
        int dy;
};
struct chips{
        int     color;
        int     x;
        int     y;
        bool    stop; // true - в этот кристалл уперлась колонна
        bool    kill; // true - этот кристалл надо удалить
        int     boll; // номер шара, упертого в кристалл
};

struct forks{
        int     type; // тип
        int     x;
        int     y;
        bool    dir; // состояние развилки
};

struct rotBolls{
        int     x;
        int     y;
        int     index;
        bool    rot;    // надо крутить
};

struct sources{
        int     x;
        int     y;
        int     capacity;
};

std::vector<bolls> bls;      // массив шаров
std::vector<livs> lvs;       // описание маршрута движения шаров
std::vector<chips> chp;      // массив кристаллов
std::vector<int> klBls;      // массив удаляемых шаров
std::vector<forks> frk;      // массив развилок текущего уровня. Развилка - невидимая точка на траектории, которая вносит какую-либо неоднозначность в движение шаров
std::vector<rotBolls> rBls;  // массив вращающихся шаров (исходное положение)
std::vector<rotBolls> rBls2; // массив вращающихся шаров (текущее положение)
std::vector<sources> src;    // массив источников

Game::Game(QWidget *parent)
    : QGLWidget(parent)
{
    setStartBtn();
    loadLivel(livN);
    screenX=screenY=1000;
    rScreenX=rScreenY=500;
    eWind=new EndLivel(this);
    connect(eWind,SIGNAL(goNow()),this,SLOT(nextLivel()));
    connect(eWind,SIGNAL(toHome()),this,SLOT(toHome()));
    sl=new SelectLivel(this);
    connect(sl,SIGNAL(select(int)),this,SLOT(selLivel(int)));
    connect(sl,SIGNAL(finished(int)),this,SLOT(enlBtn()));
    abv= new About(this);
    connect(abv,SIGNAL(finished(int)),this,SLOT(enlBtn()));
    hwnd=new HelpWnd(this);
    connect(hwnd,SIGNAL(clsw()),this,SLOT(toHome()));
    connect(hwnd,SIGNAL(finished(int)),this,SLOT(toHome()));
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerStep()));
    timer->start(20);
    sound=new QSound(QString(":/new/prefix1/sound"));
    sound->setLoops(QSound::Infinite);
    sound->play();
    bum=new QSound(QString(":/new/prefix1/bum"));
    rotory=new QSound(QString(":/new/prefix1/rotor"));
    chipKill=new QSound(QString(":/new/prefix1/chip"));
    four=new QSound(QString(":/new/prefix1/four"));
}

void Game::toHome()
{
    livN=0;
    noBtn=false;
    loadLivel(0);
}

void Game::enlBtn()
{
    noBtn=false;
}

bool Game::searchBolls(int index) // составить список шаров, примыкающих к index
{
    rBls.clear();
    rotBolls rb;
    bool b;
    std::vector<rotBolls> rr;
    float sx=1,sy=0;
    float dx=0,dy=0;
    for(float al=0;al<pi*1.99;al+=pi/38){  // делаем условный оборот вокруг шарика
        sx=bls[index].x+102*cos(al);
        sy=bls[index].y+102*sin(al);
        for(int i=0;i<int(bls.size());i++){
            if(i!=index){
                dx=bls[i].x-sx;
                dy=bls[i].y-sy;
                if(sqrt(dx*dx+dy*dy)<5){
                    b=true;
                    for(int j=0;j<int(rBls.size());j++)
                        if(i==rBls[j].index || bls[i].xBoll){b=false;break;}
                    if(b){
                        rb.x=bls[i].x;
                        rb.y=bls[i].y;
                        rb.index=i;
                        rb.rot=true;
                        rBls.push_back(rb);
                    }
                }
            }
        }

    }
    rr=rBls;
    rBls2=rBls;
    return (int(rBls.size())>1);
}

void Game::selLivel(int index)
{
    livN=index;
    loadLivel(livN);
}

void Game::nextLivel()
{
    noBtn=false;
    loadLivel(++livN);
}

void Game::setStartBtn()
{
    b1x=b2x=b3x=0;
    b1y=-0.1;
    b2y=-0.35;
    b3y=-0.6;
    brx=rstX; bry=rstY;
    bex=extX; bey=extY;
}

void Game::loadLivel(int index)
{
    if(index==0){return;}
    QString ln=":/new/prefix1/liv";
    QString str;
    ln.append(QString::number(index));
    int x1,x2,y1,y2,dx,dy;
    x1=x2=y1=y2=dx=dy=0;
    int step=0;
    int j=0;
    lvs.clear();
    chp.clear();
    frk.clear();
    src.clear();
    bls.clear();
    killy=true;
    forChip=true;
    chpCou=0;
    frkCou=0;
    maxBolls=100;
    couBolls=0;
    lColor=lColor1=100;
    dBolls=0;
    livN=0;
    //for(int i=0;i<maxBolls;i++)bls.push_back(bolls());
    QFile file(ln);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            ln=line.trimmed();
            if(line[0]=='#')continue;
            if(ln.left(4)=="liv:"){ln=ln.mid(4,ln.length()-5);livN=ln.toInt();continue;}
            if(ln=="noKill"){killy=false;continue;}
            if(ln=="noForChip"){forChip=false;continue;}
            if(ln.left(4)=="end:"){setEnd(ln);continue;}
            if(ln.left(4)=="fon:"){setTable(ln);continue;}
            if(ln.left(4)=="ch0:"){setChip(0,ln);continue;}
            if(ln.left(4)=="ch1:"){setChip(1,ln);continue;}
            if(ln.left(4)=="ch2:"){setChip(2,ln);continue;}
            if(ln.left(4)=="ch3:"){setChip(3,ln);continue;}
            if(ln.left(4)=="ch4:"){setChip(4,ln);continue;}
            if(ln.left(4)=="ch5:"){setChip(5,ln);continue;} // преграда из 2 шаров
            if(ln.left(4)=="ch6:"){setChip(6,ln);continue;} // преграда из 3 шаров
            if(ln.left(4)=="ch7:"){setChip(7,ln);continue;} // преграда из 4 шаров
            if(ln.left(4)=="ch8:"){setChip(8,ln);continue;} // преграда из 2 шаров сверху
            if(ln.left(6)=="bolls:"){ln=ln.mid(6,ln.length()-7);colors=ln.toInt();continue;}
            if(ln.left(4)=="max:"){ln=ln.mid(4,ln.length()-5);maxBolls=ln.toInt();continue;}
            if(ln.left(4)=="rst:"){rstButton(ln);continue;}
            if(ln.left(4)=="ext:"){extButton(ln);continue;}
            if(ln.left(4)=="tbl:"){setTbl(ln);continue;}
            if(ln.left(6)=="fork0:"){setFork(0,ln);continue;}
            if(ln.left(6)=="fork1:"){setFork(1,ln);continue;}
            if(ln.left(6)=="fork2:"){setFork(2,ln);continue;}
            if(ln.left(8)=="setBoll:"){setBoll(ln);continue;}
            if(ln.left(7)=="source:"){setSource(ln);continue;}
            if(ln[0]=='f'){
                str.clear();
                step=0;
                for(int i=2;i<ln.length();i++){
                    if(ln[i]==';'){
                        switch(step){
                        case 0:
                            x1=str.toInt();
                            str.clear();
                            step++;
                            break;
                        case 1:
                            y1=str.toInt();
                            str.clear();
                            step++;
                            break;
                        case 2:
                            x2=str.toInt();
                            str.clear();
                            step++;
                            break;
                        case 3:
                            y2=str.toInt();
                            str.clear();
                            step++;
                            break;
                        case 4:
                            dx=str.toInt();
                            str.clear();
                            step++;
                            break;
                        case 5:
                            dy=str.toInt();
                        }
                    }
                    else str.append(ln[i]);
                }
                lvs.push_back(livs());
                lvs[j].x1=x1;lvs[j].y1=y1;lvs[j].x2=x2;lvs[j].y2=y2;lvs[j].dx=dx;lvs[j].dy=dy;
                j++;

            }
        }
        if(livN==0){loadLivel(0);return;}
        setStartBtn();
        genBolls();
}

void Game::setTable(QString ln) // установка заднего фона - загрузка текстуры
{
    ln=ln.mid(4,ln.length()-5);
    QString s=":/new/prefix1/";
    if(ln.indexOf(".")>-1){ // из файла
        s.clear();
    }
    s.append(ln);
    texture[24]=bindTexture(QPixmap(s), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void Game::setSource(QString ln)
{
    int       step=0;
    QString   str;
    sources   b;
    str.clear();
    for(int i=7;i<ln.length();i++){
        if(ln[i]==';'){
            switch(step){
            case 0:
                b.x=str.toInt();
                str.clear();
                step++;
                break;
            case 1:
                b.y=str.toInt();
                str.clear();
                step++;
                break;
            case 2:
                b.capacity=str.toInt();
                str.clear();
                step++;
                break;
               }
        }
        else str.append(ln[i]);
    }
    src.push_back(b);
}

void Game::setBoll(QString ln)
{
    int step=0;
    QString str;
    bolls   b;
    str.clear();
    for(int i=8;i<ln.length();i++){
        if(ln[i]==';'){
            switch(step){
            case 0:
                b.x=str.toInt();
                str.clear();
                step++;
                break;
            case 1:
                b.y=str.toInt();
                str.clear();
                step++;
                break;
            case 2:
                b.color=str.toInt();
                str.clear();
                step++;
                break;
               }
        }
        else str.append(ln[i]);
    }
    b.xBoll=(b.color==-1);
    b.life=true;
    b.scl=1;
    b.decMax=false;
    b.lider=false;
    b.way=-1;
    bls.push_back(b);
    couBolls++;
}

void Game::setChip(int index, QString ln)
{
    int step=0;
    QString str;
    chp.push_back(chips());
    chpCou++;
    chp[chpCou-1].color=index;
    str.clear();
    for(int i=4;i<ln.length();i++){
        if(ln[i]==';'){
            switch(step){
            case 0:
                chp[chpCou-1].x=str.toInt();
                str.clear();
                step++;
                break;
            case 1:
                chp[chpCou-1].y=str.toInt();
               }
        }
        else str.append(ln[i]);
    }
    chp[chpCou-1].stop=false;
    chp[chpCou-1].kill=false;
}

void Game::setFork(int index, QString ln)
{
    int step=0;
    QString str;
    forks fr;
    fr.type=index;
    str.clear();
    for(int i=6;i<ln.length();i++){
        if(ln[i]==';'){
            switch(step){
            case 0:
                fr.x=str.toInt();
                str.clear();
                step++;
                break;
            case 1:
                fr.y=str.toInt();
               }
        }
        else str.append(ln[i]);
    }
    fr.dir=true;
    frk.push_back(fr);
}

void Game::rstButton(QString ln)
{
    int step=0;
    QString str;
    str.clear();
    for(int i=4;i<ln.length();i++){
        if(ln[i]==';'){
            switch(step){
            case 0:
                rstX=str.toInt();
                str.clear();
                step++;
                break;
            case 1:
                rstY=str.toInt();
               }
        }
        else str.append(ln[i]);
    }
}

void Game::setEnd(QString ln)
{
    int step=0;
    QString str;
    str.clear();
    for(int i=4;i<ln.length();i++){
        if(ln[i]==';'){
            switch(step){
            case 0:
                stopX=str.toInt();
                str.clear();
                step++;
                break;
            case 1:
                stopY=str.toInt();
               }
        }
        else str.append(ln[i]);
    }
}

void Game::setTbl(QString ln)
{
    int step=0;
    QString str;
    str.clear();
    for(int i=4;i<ln.length();i++){
        if(ln[i]==';'){
            switch(step){
            case 0:
                tblX=str.toInt();
                str.clear();
                step++;
                break;
            case 1:
                tblY=str.toInt();
               }
        }
        else str.append(ln[i]);
    }
}

void Game::extButton(QString ln)
{
    int step=0;
    QString str;
    str.clear();
    for(int i=4;i<ln.length();i++){
        if(ln[i]==';'){
            switch(step){
            case 0:
                extX=str.toInt();
                str.clear();
                step++;
                break;
            case 1:
                extY=str.toInt();
               }
        }
        else str.append(ln[i]);
    }
}

void Game::forward()
{
    genBolls();
    for(int i=0;i<int(bls.size());i++){
        if(!rot && (i>=rotat-1 && i<=rotat+2))continue;
        if(goForward(i)){
            bls[i].x=bls[i].x+bls[i].dx;
            bls[i].y=bls[i].y+bls[i].dy;
        }
        bls[i].lider=lider(i);
        if(!bls[i].lider){
            if(goForward(i)){        // двойная скорость, если это не первый шар, т.е. еще один шаг по траектории
                bls[i].x=bls[i].x+bls[i].dx;
                bls[i].y=bls[i].y+bls[i].dy;
            }
        }
     }
}

bool Game::lider(int index)
{
    if(bls[index].xBoll)return true;
    for(int i=0;i<int(bls.size())-1;i++){
        if(bls[i].way!=bls[index].way)continue;
        if(bls[index].dx>0 && bls[i].x>bls[index].x)return false;
        if(bls[index].dx<0 && bls[i].x<bls[index].x)return false;
        if(bls[index].dy>0 && bls[i].y>bls[index].y)return false;
        if(bls[index].dy<0 && bls[i].y<bls[index].y)return false;
    }
    return true;
}

bool Game::intervalBolls(int index) // поиск возможности сдвинуть шар (интервала перед ним)
{
    if(index==-1)return true;
    int x1,y1;
    if(bls[index].xBoll)return true;
    bls[index].go=false;
    // проверка на упертость в кристалл
    for(int i=0;i<chpCou;i++){
            x1=chp[i].x-bls[index].x;
            y1=chp[i].y-bls[index].y;
            if(sqrt(x1*x1+y1*y1)<100){
                chp[i].stop=true;
                chp[i].boll=index;
                return false;
            }
        }
    // проверка на упертость в зону вращения
    if(!rot){
        x1=bls[rotat].x-(bls[index].x+bls[index].dx);
        y1=bls[rotat].y-(bls[index].y+bls[index].dy);
        if(sqrt(x1*x1+y1*y1)<202)return false;
    }

    // проверка на упертость в другие шары
    for(int i=0;i<int(bls.size());i++){
        if(i!=index){
            x1=bls[i].x-(bls[index].x+bls[index].dx);
            y1=bls[i].y-(bls[index].y+bls[index].dy);
            if(sqrt(x1*x1+y1*y1)<100){
                //if(bls[i].xBoll)bls[i].back=i;
                return false;
            }

        }
    }
    bls[index].go=true;
    return true;
}

bool Game::goForward(int index)
{
    double l1,l2,l3;
    int    x1,y1,dx,dy;
    bls[index].dx=0;
    bls[index].dy=0;
    if(bls[index].xBoll){  // проверка для Х-шаров
        if(int(bls.size())==maxBolls)return false;
        for(int i=0;i<int(chp.size());i++){
            if(bls[index].x==chp[i].x &&
               bls[index].y==chp[i].y) return false;
        }

    }
    // проверка по развилкам
    if(int(frk.size())>0)
    for(int i=0;i<int(frk.size());i++){
        if(frk[i].x==bls[index].x && frk[i].y==bls[index].y){
            bls[index].dx=frkDX(i);
            bls[index].dy=frkDY(i);
            break;
        }
    }
    // проверка по отрезкам траектории
    for(int i=0;i<int(lvs.size());i++){
        x1=lvs[i].x1-lvs[i].x2;
        y1=lvs[i].y1-lvs[i].y2;
        l1=sqrt(x1*x1+y1*y1);  // длина фрагмента
        x1=lvs[i].x1-bls[index].x;
        y1=lvs[i].y1-bls[index].y;
        l2=sqrt(x1*x1+y1*y1);  // длина 1
        x1=lvs[i].x2-bls[index].x;
        y1=lvs[i].y2-bls[index].y;
        l3=sqrt(x1*x1+y1*y1);  // длина 2
        dx=lvs[i].dx;
        dy=lvs[i].dy;

        if(l2+l3-l1<0.01){
            bls[index].dx=dx;
            bls[index].dy=dy;
            bls[index].way=i;
            break;

        }
    }
    if(bls[index].x==stopX && bls[index].y==stopY && !bls[index].xBoll){
        noBtn=true;
        eWind->show();
        return false;
    }
    return intervalBolls(index);
}

int Game::frkDX(int i) // шаг с развилки по X
{
    switch(frk[i].type){
        case 0:
            frk[i].dir=!frk[i].dir;
            if(frk[i].dir)return 5; else return-5;
            break;
        case 1:
            frk[i].dir=!frk[i].dir;
            if(frk[i].dir)return 5; else return 0;
            break;
        case 2:
            frk[i].dir=!frk[i].dir;
            if(frk[i].dir)return -5; else return 0;
            break;
    }
    return 0;
}

int Game::frkDY(int i) // шаг с развилки по Y
{
    switch(frk[i].type){
        case 0:
            return i-i;
            break;
        case (1 || 2):
            if(frk[i].dir)return 0; else return -10;
            break;
    }
    return 0;
}

void doRoto() // один шаг вращения шаров
{
    float sx,sy,dx,dy;
    float al=0;
    sx=rBls.size();
    while(al<pi*1.99){  // делаем условный оборот вокруг шарика
        sx=bls[rotat].x+102*cos(al);
        sy=bls[rotat].y+102*sin(al);
        for(int i=0;i<int(rBls.size());i++){
            if(rBls2[i].rot){
                dx=bls[rBls[i].index].x-sx;
                dy=bls[rBls[i].index].y-sy;
                if(sqrt(dx*dx+dy*dy)<5){
                      al+=pi/38;
                      sx=bls[rotat].x+100*cos(al);
                      sy=bls[rotat].y+100*sin(al);
                      bls[rBls[i].index].x=sx;
                      bls[rBls[i].index].y=sy;
                      continue;
                }
            }

        }
    al+=pi/38;
    }
}


void Game::rotor()
{
    float dx,dy;
    int j;
    doRoto();
    doRoto();
    for(int i=0;i<int(rBls.size());i++){ // проверка на конец вращения
        if(i==int(rBls.size())-1)j=0;else j=i+1;
        dx=bls[rBls[i].index].x-rBls[j].x;
        dy=bls[rBls[i].index].y-rBls[j].y;
        if(sqrt(dx*dx+dy*dy)<12){
            rBls2[i].rot=false;
            bls[rBls[i].index].x=rBls[j].x;
            bls[rBls[i].index].y=rBls[j].y;
        }
    }

    // проверка на завершение вращения
    rot=true;
    for(int i=0;i<int(rBls2.size());i++){
        if(rBls2[i].rot){
            rot=false;
            return;
        }
    }
    if(rot){
        int  c=bls[rBls[0].index].color;
        bool f=false;
        for(int i=1;i<int(rBls.size());i++){
            if(c!=bls[rBls[i].index].color) f=true;
        }
        if(killy){
            if(f)dBolls--;
        } else {
            if(int(bls.size())<maxBolls) addBoll=true;
           }
        }

}

void Game::genBolls()
{
    srcBolls=0;  // всего шаров в ограниченных источниках (не выданных)
    for(int i=0;i<int(src.size());i++){
        if(src[i].capacity>0)srcBolls+=src[i].capacity;
    }
    for(int i=0;i<int(src.size());i++){
        if(src[i].capacity<0){
            if(maxBolls==int(bls.size())+srcBolls)continue;
        }
        if(src[i].capacity==0 && !addBoll)continue;
        startX=src[i].x;
        startY=src[i].y;
        if(startLiv()){
            if(addBoll && src[i].capacity==0)addBoll=false;
            if(src[i].capacity>0)src[i].capacity--;
        }
    }
}

bool Game::startLiv() // генерация новых шаров
{

    if(int(bls.size())==maxBolls)return false;
    int ax,ay;
    if(int(bls.size())>0){ // проверка на свободное место для генерации
        for(int i=bls.size()-1;i>=0;i--){
            ax=bls[i].x-startX;
            ay=bls[i].y-startY;
            if(sqrt(ax*ax+ay*ay)<=99) return false;
        }
    }
    // проверка на упертость в зону вращения
    if(!rot){
        ax=bls[rotat].x-startX;
        ay=bls[rotat].y-startY;
        if(sqrt(ax*ax+ay*ay)<300)return false;
    }
    bolls b;
    b.x=startX;
    b.y=startY;
    b.life=true;
    b.go=false;
    do{
        b.color=colorBoll(colors);
      }while(b.color==lColor && b.color==lColor1);
    lColor1=lColor;
    lColor=b.color;
    b.dx=0;
    b.dy=0;
    b.scl=1;
    b.decMax=false;
    b.lider=false;
    b.way=-1;      // нет отрезка траектории
    b.xBoll=false; // обычный шар
    bls.push_back(b);
    couBolls++;
    return true;
}

int Game::colorBoll(int index)
{
    int c=rand()%index;
    if(c<5)return c;
    else
        switch(c){
            case 5:
                c=12; break;
            case 6:
                c=49; break;
            case 7:
                c=50; break;
            case 8:
                c=51; break;
            case 9:
                c=52; break;
            default:
                c=0; break;
        }
    return c;
}

/*virtual*/void Game::mousePressEvent(QMouseEvent* pe) // нажатие клавиши мыши
{
    if(noBtn)return;
    int mx=(-rScreenX/2+pe->x())*bSizeX;
    int my=(rScreenY/2-pe->y())*bSizeY;
    if(livN==0){
        if(abs(mx)<500 && abs(my+100)<100){b1x+=0.01;b1y-=0.01;return;}
        if(abs(mx)<500 && abs(my+350)<100){b2x+=0.01;b2y-=0.01;return;}
        if(abs(mx)<500 && abs(my+600)<100){b3x+=0.01;b3y-=0.01;return;}
    }

    if(abs(mx-rstX)<150 && abs(my-rstY)<50){ // кнопка перезапуска
        brx+=5;
        bry-=5;
    }

    if(abs(mx-extX)<150 && abs(my-extY)<50){ // кнопка выхода с уровня
        bex+=5;
        bey-=5;
    }
}

/*virtual*/void Game::mouseReleaseEvent(QMouseEvent* pe) // отжатие клавиши мыши
{
   if(!rot || noBtn)return;
   setStartBtn();
   int mx=(-rScreenX/2+pe->x())*bSizeX;
   int my=(rScreenY/2-pe->y())*bSizeY;
   if(livN==0){
       setStartBtn();
       if(abs(mx)<500 && abs(my+100)<100){noBtn=true;sl->show();return;} // кнопка start начального экрана
       if(abs(mx)<500 && abs(my+350)<100){noBtn=true;hwnd->show();return;} // кнопка help начального экрана
       if(abs(mx)<500 && abs(my+600)<100){noBtn=true;abv->show();return;} // кнопка about начального экрана

   }
   if(livN==0)return;
   if(abs(mx-rstX)<150 && abs(my-rstY)<50){ // кнопка перезапуска
       loadLivel(livN);
       return;
   }
   if(abs(mx-extX)<150 && abs(my-extY)<50){ // кнопка выхода с уровня
       livN=0;
       loadLivel(0);
       return;
   }
   int bol=-1; // номер шара, на который нажали
   for (int i=0; i<couBolls; i++){
       if(abs(bls[i].x-mx)<50 && abs(bls[i].y-my)<50) bol=i; // нажатие на шар
   }
   if(!bls[bol].life)return;

   if(searchBolls(bol)){
       for(int i=0;i<int(rBls.size());i++){
           if(!bls[rBls[i].index].life)return; // не крутить, если рядом "умирающий" шар
       }
       rot=false;
       rotat=bol;
       rBls2=rBls;
       rotory->play();
   }
   return;


   if(!bls[bol+1].life || !bls[bol+1].life)return;
   if(intervalBolls(bol) || intervalBolls(bol+1))return;
   rot=false;
   rotat=bol;
   rx1=bls[bol+1].x;
   rx2=bls[bol-1].x;
   ry1=bls[bol+1].y;
   ry2=bls[bol-1].y;
   d1=1,d2=-1;
   bx1=bx2=by1=by2=true;
   rotory->play();
}

Game::~Game()
{
}

void Game::initializeGL()
{
    // цвет для очистки буфера изображения - будет просто фон окна
    qglClearColor(Qt::black);
    glEnable(GL_DEPTH_TEST);  // устанавливает режим проверки глубины пикселей
    glShadeModel(GL_SMOOTH);    // отключает режим сглаживания цветов


    glEnableClientState(GL_VERTEX_ARRAY); // активизация массива вершин
    glEnableClientState(GL_COLOR_ARRAY);  // активизация массива цветов вершин

    loadTextures();

    // установить режим двумерных текстур
    //getTextureArray(); // определить массив текстурных координат вершин
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);// активизируем массив текстурных координат

    // поддержка прозрачности текстур
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5f);

    setAutoBufferSwap(true);


}


void Game::resizeGL(int nWidth, int nHeight)
{
    rScreenX=nWidth;
    rScreenY=nHeight;
    if(nWidth==nHeight){
        bSizeX=screenX*2/float(nWidth);
        bSizeY=screenY*2/float(nHeight);
    }
    if(nWidth<nHeight){
        bSizeX=screenX*2/float(nWidth);
        bSizeY=(screenY*2/float(nHeight))*(float(nHeight)/float(nWidth));
    }
    if(nWidth>nHeight){
        bSizeX=(screenX*2/float(nWidth))*(float(nWidth)/float(nHeight));
        bSizeY=screenY*2/float(nHeight);
    }
    glMatrixMode(GL_PROJECTION); // устанавливает текущей проекционную матрицу
    glLoadIdentity();            // присваивает проекционной матрице единичную матрицу


    GLfloat ratio=(GLfloat)nHeight/(GLfloat)nWidth;

    // мировое окно
    if (nWidth>=nHeight)
       // параметры видимости ортогональной проекции
       glOrtho(-1.0/ratio, 1.0/ratio, -1.0, 1.0, -1.0, 1.0);
    else
       glOrtho(-1.0, 1.0, -1.0*ratio, 1.0*ratio, -1.0, 1.0);
    // плоскости отсечения (левая, правая, верхняя, нижняя, передняя, задняя)

    // поле просмотра
    glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);

}

void Game::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if(livN==0){
        drawWorld();
        return;
    }

    dBolls=0;
    if(!rot) rotor();
    forward();
    if(rot) killThree();


    drawWorld(); // нарисовать фигуру

    if(dBolls==-1 && maxBolls==couBolls){
        bls[couBolls-1].life=false;
        bls[couBolls-1].decMax=true;
    }
    if(dBolls==-1 && maxBolls>couBolls){
        maxBolls--;
    }
    if(dBolls>0)maxBolls+=dBolls;
    dBolls=0;
    if(maxBolls<couBolls)couBolls=maxBolls;
}

void Game::killThree()
{
    // поиск одноцветных групп >2 шаров
    float sx=1,sy=0;
    float dx=0,dy=0;
    bool  f=false;
    std::vector<int> b;
    for(int i=0;i<couBolls;i++){
        if(!bls[i].life)continue;
        b.clear();
        b.push_back(i);
        for(float al=0;al<pi*1.99;al+=pi/18){  // делаем условный оборот вокруг шарика
            sx=bls[i].x+101*cos(al);
            sy=bls[i].y+101*sin(al);
            for(int j=0;j<int(bls.size());j++){
                if(i!=j){
                    dx=bls[j].x-sx;
                    dy=bls[j].y-sy;
                    if(sqrt(dx*dx+dy*dy)<12 && bls[i].color==bls[j].color)b.push_back(j);
                }
            }
        }
        if(int(b.size())>2){ //проверка на группу 4 или 5
            for(int j=0;j<3;j++){
                for(float al=0;al<pi*1.99;al+=pi/64){  // делаем условный оборот вокруг шарика
                    sx=bls[b[j]].x+101*cos(al);
                    sy=bls[b[j]].y+101*sin(al);
                    for(int l=0;l<int(bls.size());l++){
                        if(i!=l && l!=b[j]){
                            dx=bls[l].x-sx;
                            dy=bls[l].y-sy;
                            if(sqrt(dx*dx+dy*dy)<12 && bls[l].color==bls[b[j]].color)b.push_back(l);
                        }
                    }
                }
            }
        }
        // Исключить повторы
        if(int(b.size())>1){
            std::vector<int> b2;
            bool f;
            b2.push_back(b[0]);
            for(int k=0;k<int(b.size());k++){
                f=true;
                for(int m=0;m<int(b2.size());m++){
                    if(b[k]==b2[m]){f=false;break;}
                }
                if(f)b2.push_back(b[k]);
            }
            b.clear();
            b=b2;
        }
        if(int(b.size())>2){
            dBolls=0;
            addBoll=false;
            for(int j=0;j<int(b.size());j++) bls[b[j]].life=false;
            if(int(b.size())>3){ // бонус за >3
                f=true;
                for(int d=0;d<couBolls;d++)if(bls[d].color==bls[b[0]].color)bls[d].life=false;
            }
        }
        // проверка на кристаллы
        if(int(b.size())>1){
            for(int j=0;j<int(b.size());j++){
                for(float al=0;al<pi*1.99;al+=pi/38){  // делаем условный оборот вокруг шарика
                    sx=bls[b[j]].x+103*cos(al);
                    sy=bls[b[j]].y+103*sin(al);
                    for(int l=0;l<int(chp.size());l++){
                            dx=chp[l].x-sx;
                            dy=chp[l].y-sy;
                            if(sqrt(dx*dx+dy*dy)<12){
                                if((chp[l].color==bls[b[j]].color && chp[l].color<5) ||
                                        chp[l].color==5 || chp[l].color==8){
                                            chp[l].kill=true;
                                            bls[b[0]].life=false;
                                            //bls[b[1]].life=false;
                                            continue;
                                }
                                if(chp[l].color==6 && int(b.size())>2){chp[l].kill=true; continue;}
                                if(chp[l].color==7 && int(b.size())>3){chp[l].kill=true; continue;}

                            }
                        }
                    }
                }



            }
    }
    // итог
    killBolls();
    killChip();
    if(f)four->play();
}

void Game::killBolls()
{
    std::vector<bolls> b;
    for(int i=0;i<couBolls;i++)
        if(!bls[i].life)
            bls[i].scl-=0.1;
    for(int i=0;i<couBolls;i++)
        if(bls[i].scl>0.3)b.push_back(bls[i]);
        else if(bls[i].decMax)maxBolls--;
    if(couBolls==int(b.size()))return;
    bls.clear();
    bls=b;
    couBolls=b.size();
    bum->play();

}

void Game::killChip()
{
    for(int i=0;i<chpCou;i++)
        if(chp[i].kill){
            if(chp[i].color<5 && forChip)dBolls+=5;
            if(chpCou==1){
                chp.clear();
                chpCou=0;
                chipKill->play();
                return;
            }
            if(i<chpCou-1)
            for(int j=i; j<chpCou-1; j++){
                chp[j]=chp[j+1];
            }
            chpCou--;
            chp.pop_back();
            chipKill->play();
       }
}

void Game::timerStep()
{
    paintGL();
    updateGL();
}

void Game::twoBolls(float rx)
{
    glBindTexture(GL_TEXTURE_2D, texture[52]);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(-rx/2,-rx/2,0);
    circleTex(rx/2,5);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(rx/2,-rx/2,0);
    circleTex(rx/2,5);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void Game::twoBollsUp(float rx)
{
    glBindTexture(GL_TEXTURE_2D, texture[52]);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(rx/2,rx/2,0);
    circleTex(rx/2,5);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-rx/2,rx/2,0);
    circleTex(rx/2,5);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void Game::threeBolls(float rx)
{
    glBindTexture(GL_TEXTURE_2D, texture[52]);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(0,rx/2,0);
    circleTex(rx/2,5);
    glPopMatrix();
    twoBolls(rx);
    glDisable(GL_TEXTURE_2D);

}

void Game::fourBolls(float rx)
{
    twoBolls(rx);
    glPushMatrix();
    glTranslatef(0,rx,0);
    twoBolls(rx);
    glPopMatrix();
}

void Game::drawWorld() // нарисовать картинку   ******************************************************
{
    if(livN==0){
        drawBtns();     // кнопки начального экрана
    }
    else {
        paintChips();   // кристаллы
        paintBolls();   // шары
        paintExit();    // выход
        paintRst();     // кнопка перезапуска и кнопка выхода
        showRest();     // табло остатка шаров
    }
    paintTable();   // стол
    glFinish();
    //swapBuffers();                           // ****************************************************
}

void Game::drawBtns()
{
    glColor3f(0,0,0);
    // start
    glBindTexture(GL_TEXTURE_2D, texture[44]);
    glPushMatrix();
    glTranslatef(b1x,b1y,0);
    rectangle2(1,0.2);
    rectangleTex(1,0.2);
    glPopMatrix();
    // help
    glBindTexture(GL_TEXTURE_2D, texture[45]);
    glPushMatrix();
    glTranslatef(b2x,b2y,0);
    rectangle2(1,0.2);
    rectangleTex(1,0.2);
    glPopMatrix();
    // about
    glBindTexture(GL_TEXTURE_2D, texture[46]);
    glPushMatrix();
    glTranslatef(b3x,b3y,0);
    rectangle2(1,0.2);
    rectangleTex(1,0.2);
    glPopMatrix();
}

void Game::paintRst()
{  
    // restart
    glBindTexture(GL_TEXTURE_2D, texture[13]);
    glPushMatrix();
    glTranslatef(float(brx)/1000.0,float(bry)/1000.0,0);
    rectangle2(0.3,0.1);
    rectangleTex(0.3,0.1);
    glPopMatrix();
    // exit
    glBindTexture(GL_TEXTURE_2D, texture[47]);
    glPushMatrix();
    glTranslatef(float(bex)/1000.0,float(bey)/1000.0,0);
    rectangle2(0.3,0.1);
    rectangleTex(0.3,0.1);
    glPopMatrix();
}

void Game::paintTable()
{
    if(livN==0)glBindTexture(GL_TEXTURE_2D, texture[5]);
    else glBindTexture(GL_TEXTURE_2D, texture[24]);
    rectangleTex(2,2);
}

void Game::paintBolls()
{
    for(int i=0;i<couBolls;i++){
        if(bls[i].xBoll)continue;
        glBindTexture(GL_TEXTURE_2D, texture[bls[i].color]);
        glPushMatrix();
        glTranslatef(float(bls[i].x)/1000.0,float(bls[i].y)/1000.0,0);
        circleTex(0.05*bls[i].scl,5);
        glPopMatrix();
    }
}

void Game::paintChips()
{
    if(!chp.empty()){
    for(int i=0;i<chpCou;i++){
        switch(chp[i].color){
            case 0:
                glPushMatrix();
                glTranslatef(float(chp[i].x)/1000.0,float(chp[i].y)/1000.0,0);
                chip0(0.045);
                glPopMatrix();
                break;
            case 1:
                glPushMatrix();
                glTranslatef(float(chp[i].x)/1000.0,float(chp[i].y)/1000.0,0);
                chip1(0.045);
                glPopMatrix();
                break;
            case 2:
                glPushMatrix();
                glTranslatef(float(chp[i].x)/1000.0,float(chp[i].y)/1000.0,0);
                chip2(0.045);
                glPopMatrix();
                break;
            case 3:
                glPushMatrix();
                glTranslatef(float(chp[i].x)/1000.0,float(chp[i].y)/1000.0,0);
                chip3(0.045);
                glPopMatrix();
                break;
            case 4:
                glPushMatrix();
                glTranslatef(float(chp[i].x)/1000.0,float(chp[i].y)/1000.0,0);
                chip4(0.045);
                glPopMatrix();
                break;
            case 5:
                glPushMatrix();
                glTranslatef(float(chp[i].x)/1000.0,float(chp[i].y)/1000.0,0);
                twoBolls(0.045);
                glPopMatrix();
                break;
            case 6:
                glPushMatrix();
                glTranslatef(float(chp[i].x)/1000.0,float(chp[i].y)/1000.0,0);
                threeBolls(0.045);
                glPopMatrix();
                break;
            case 7:
                glPushMatrix();
                glTranslatef(float(chp[i].x)/1000.0,float(chp[i].y)/1000.0,0);
                fourBolls(0.045);
                glPopMatrix();
                break;
            case 8:
                glPushMatrix();
                glTranslatef(float(chp[i].x)/1000.0,float(chp[i].y)/1000.0,0);
                twoBollsUp(0.045);
                glPopMatrix();
                break;
        }
      }
    }
}

void Game::paintExit()
{
    glPushMatrix();
    glTranslatef(stopX/1000.0,stopY/1000.0,0);
    glBindTexture(GL_TEXTURE_2D, texture[11]);
    glEnable(GL_TEXTURE_2D);
    circleTex(0.05,5);
    //rectangleTex(0.1,0.1);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void Game::showRest() // табло
{
    float x1=-0.07,x2=0.07;
    float y1=0.05,y2=-0.05;
    int j;
    glPushMatrix();
    glTranslatef(float(tblX)/1000.0,float(tblY)/1000.0,0);
    j=maxBolls-int(bls.size());
           if(j<10){
                glBindTexture(GL_TEXTURE_2D, texture[14+j]);
                glEnable(GL_TEXTURE_2D);
                glBegin(GL_QUADS);
                    glTexCoord2d(0,0);
                    glVertex2f(x1+0.05,y2);
                    glTexCoord2d(1,0);
                    glVertex2f(x2-0.05,y2);
                    glTexCoord2d(1,1);
                    glVertex2f(x2-0.05,y1);
                    glTexCoord2d(0,1);
                    glVertex2f(x1+0.05,y1);
                glEnd();
                glDisable(GL_TEXTURE_2D);

             }
           if(j<99 && j>9){
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texture[14+j/10]);
                glBegin(GL_QUADS);
                    glTexCoord2d(0,0);
                    glVertex2f(x1+0.03,y2);
                    glTexCoord2d(1,0);
                    glVertex2f(0,y2);
                    glTexCoord2d(1,1);
                    glVertex2f(0,y1);
                    glTexCoord2d(0,1);
                    glVertex2f(x1+0.03,y1);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, texture[14+j-10*(j/10)]);
                glBegin(GL_QUADS);
                    glTexCoord2d(0,0);
                    glVertex2f(0,y2);
                    glTexCoord2d(1,0);
                    glVertex2f(x2-0.03,y2);
                    glTexCoord2d(1,1);
                    glVertex2f(x2-0.03,y1);
                    glTexCoord2d(0,1);
                    glVertex2f(0,y1);
                glEnd();
                glDisable(GL_TEXTURE_2D);

             }
    glPopMatrix();
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[48]);
    glTranslatef(float(tblX)/1000.0,float(tblY)/1000.0,0);
    rectangleTex(0.15,0.15);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();


}

void Game::circle(float r, int s) //круг
{
    float rd=pi/180;
    float a1,a2;
    for(int i=0; i<360; i=i+s){
        a1=i*rd;
        a2=(i+s)*rd;
        glBegin(GL_TRIANGLES);
            glVertex2f(r*cos(a1),r*sin(a1));
            glVertex2f(r*cos(a2),r*sin(a2));
            glVertex2f(0,0);
        glEnd();
    }
}

void Game::chip0(float rx)
{
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_POLYGON);
        glTexCoord2d(0.5,1);
        glVertex2f(0,rx);
        glTexCoord2d(1,0.75);
        glVertex2f(rx,rx/2);
        glTexCoord2d(1,0.25);
        glVertex2f(rx,-rx/2);
        glTexCoord2d(0.5,0);
        glVertex2f(0,-rx);
        glTexCoord2d(0,0.25);
        glVertex2f(-rx,-rx/2);
        glTexCoord2d(0,0.75);
        glVertex2f(-rx,rx/2);
     glEnd();
     glDisable(GL_TEXTURE_2D);
}

void Game::chip1(float rx)
{
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_POLYGON);
        glTexCoord2d(0.5,1);
        glVertex2f(0,rx);
        glTexCoord2d(1,1);
        glVertex2f(rx,rx);
        glTexCoord2d(1,0.5);
        glVertex2f(rx,0);
        glTexCoord2d(0.5,0);
        glVertex2f(0,-rx);
        glTexCoord2d(0,0);
        glVertex2f(-rx,-rx);
        glTexCoord2d(0,0.5);
        glVertex2f(-rx,0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Game::chip2(float rx)
{
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_POLYGON);
        glTexCoord2d(0.333,1);
        glVertex2f(-rx/3,rx);
        glTexCoord2d(0.666,1);
        glVertex2f(rx/3,rx);
        glTexCoord2d(1,0.666);
        glVertex2f(rx,rx/3);
        glTexCoord2d(1,0.333);
        glVertex2f(rx,-rx/3);
        glTexCoord2d(0.666,0);
        glVertex2f(rx/3,-rx);
        glTexCoord2d(0.333,0);
        glVertex2f(-rx/3,-rx);
        glTexCoord2d(0,0.333);
        glVertex2f(-rx,-rx/3);
        glTexCoord2d(0,0.666);
        glVertex2f(-rx,rx/3);
     glEnd();
     glDisable(GL_TEXTURE_2D);
}

void Game::chip3(float rx)
{
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_POLYGON);
        glTexCoord2d(0.333,1);
        glVertex2f(-rx/3,rx);
        glTexCoord2d(0.666,1);
        glVertex2f(rx/3,rx);
        glTexCoord2d(1,0.666);
        glVertex2f(rx,rx/3);
        glTexCoord2d(1,0.333);
        glVertex2f(rx,-rx/3);
        glTexCoord2d(0.666,0);
        glVertex2f(rx/3,-rx);
        glTexCoord2d(0.333,0);
        glVertex2f(-rx/3,-rx);
        glTexCoord2d(0,0.333);
        glVertex2f(-rx,-rx/3);
        glTexCoord2d(0,0.666);
        glVertex2f(-rx,rx/3);
     glEnd();
     glDisable(GL_TEXTURE_2D);
}

void Game::chip4(float rx)
{
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glTexCoord2d(0,0.5);
        glVertex2f(-rx,0);
        glTexCoord2d(0.5,1);
        glVertex2f(0,rx);
        glTexCoord2d(1,0.5);
        glVertex2f(rx,0);
        glTexCoord2d(0.5,0);
        glVertex2f(0,-rx);
     glEnd();
     glDisable(GL_TEXTURE_2D);
}

void Game::circleTex(float r, int s) //круг с текстурой
{
    float rd=pi/180;
    float a1,a2;
    glEnable(GL_TEXTURE_2D);
    for(int i=0; i<360; i=i+s){
        a1=i*rd;
        a2=(i+s)*rd;
        glBegin(GL_TRIANGLES);
            glTexCoord2f(cos(a1)/2+0.5,sin(a1)/2+0.5);
            glVertex2f(r*cos(a1),r*sin(a1));
            glTexCoord2f(cos(a2)/2+0.5,sin(a2)/2+0.5);
            glVertex2f(r*cos(a2),r*sin(a2));
            glTexCoord2f(0.5,0.5);
            glVertex2f(0,0);
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void Game::oval(float rx,float ry,int s) //овал
{
    float rd=pi/180;
    float a1,a2;
    for(int i=0; i<360; i=i+s){
        a1=i*rd;
        a2=(i+s)*rd;
        glBegin(GL_TRIANGLES);
            glVertex2f(rx*cos(a1),ry*sin(a1));
            glVertex2f(rx*cos(a2),ry*sin(a2));
            glVertex2f(0,0);
        glEnd();
    }
}

void Game::ovalTex(float rx,float ry,int s) //овал
{
    float rd=pi/180;
    float a1,a2;
    glEnable(GL_TEXTURE_2D);
    for(int i=0; i<360; i=i+s){
        a1=i*rd;
        a2=(i+s)*rd;
        glBegin(GL_TRIANGLES);
            glTexCoord2f(cos(a1)/2+0.5,sin(a1)/2+0.5);
            glVertex2f(rx*cos(a1),ry*sin(a1));
            glTexCoord2f(cos(a2)/2+0.5,sin(a2)/2+0.5);
            glVertex2f(rx*cos(a2),ry*sin(a2));
            glTexCoord2f(0.5,0.5);
            glVertex2f(0,0);
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}
void Game::rectangle(float w, float h) // прямоугольник
{
    if(w<0 || h<0)return;
    glBegin(GL_QUADS);
        glVertex2f(w/2,-h/2);
        glVertex2f(-w/2,-h/2);
        glVertex2f(-w/2,h/2);
        glVertex2f(w/2,h/2);
    glEnd();
}

void Game::rectangle2(float w, float h) // прямоугольник
{
    if(w<0 || h<0)return;
    glBegin(GL_LINE_LOOP);
        glVertex2f(w/2,-h/2);
        glVertex2f(-w/2,-h/2);
        glVertex2f(-w/2,h/2);
        glVertex2f(w/2,h/2);
    glEnd();
}
void Game::rectangleTex(float w, float h) // прямоугольник с текстурой
{
    if(w<0 || h<0)return;
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glTexCoord2f(1,0);
        glVertex2f(w/2,-h/2);
        glTexCoord2f(0,0);
        glVertex2f(-w/2,-h/2);
        glTexCoord2f(0,1);
        glVertex2f(-w/2,h/2);
        glTexCoord2f(1,1);
        glVertex2f(w/2,h/2);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Game::loadTextures()
{
    texture[0]=bindTexture(QPixmap(QString(":/new/prefix1/greenboll")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[1]=bindTexture(QPixmap(QString(":/new/prefix1/blueboll")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[2]=bindTexture(QPixmap(QString(":/new/prefix1/redboll")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[3]=bindTexture(QPixmap(QString(":/new/prefix1/lilacboll")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[4]=bindTexture(QPixmap(QString(":/new/prefix1/yellowboll")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[5]=bindTexture(QPixmap(QString(":/new/prefix1/startScreen")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[6]=bindTexture(QPixmap(QString(":/new/prefix1/greenchip")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[7]=bindTexture(QPixmap(QString(":/new/prefix1/redchip")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    texture[8]=bindTexture(QPixmap(QString(":/new/prefix1/bluechip")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);  
    
    texture[9]=bindTexture(QPixmap(QString(":/new/prefix1/yellowchip")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[10]=bindTexture(QPixmap(QString(":/new/prefix1/lilacchip")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[11]=bindTexture(QPixmap(QString(":/new/prefix1/exit")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[12]=bindTexture(QPixmap(QString(":/new/prefix1/gboll")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[13]=bindTexture(QPixmap(QString(":/new/prefix1/btnRestart")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // цифры
    QString s;
    for(int i=0;i<10;i++){
        s=":/new/prefix1/c"+QString::number(i);
        texture[14+i]=bindTexture(QPixmap(s), GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    }

    texture[44]=bindTexture(QPixmap(QString(":/new/prefix1/btnStart")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);


    texture[45]=bindTexture(QPixmap(QString(":/new/prefix1/btnHelp")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[46]=bindTexture(QPixmap(QString(":/new/prefix1/btnAbout")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[47]=bindTexture(QPixmap(QString(":/new/prefix1/btnExit")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[48]=bindTexture(QPixmap(QString(":/new/prefix1/ico")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[49]=bindTexture(QPixmap(QString(":/new/prefix1/grayboll")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[50]=bindTexture(QPixmap(QString(":/new/prefix1/orgboll")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[51]=bindTexture(QPixmap(QString(":/new/prefix1/org2boll")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    texture[52]=bindTexture(QPixmap(QString(":/new/prefix1/blue2boll")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

}
