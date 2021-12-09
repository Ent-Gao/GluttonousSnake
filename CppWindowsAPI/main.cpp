#include <array>
#include <conio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <windows.h>
#include <winuser.h>
using namespace std;

//控制台高度与宽度，单位为字符，注意字符宽度：字符高度=1：2,实际两个字符当作一个单元格
const int mapx = 80;
const int mapy = 24;
const int mapSize = mapx * mapy;
const int indexSize = mapSize / 2;
const int halfPerimeter = mapx + mapy;

//**************************************
//Snake 类
class Snake
{
private:
    int direction;
    int tail;
    int length;
    int addLength;
    int accelerator;
    array<int, indexSize> body;

public:
    Snake();
    ~Snake();
    int setDirection(int diret);
    int addAddLength(int addAddlength);
    int growMove();
    int move();
    const array<int, indexSize> &getSnake() const;
    int getTail() const;
    int getLength() const;
    int getDirection() const;
    int getAccelerator() const;
};

Snake::Snake()
{
    this->direction = 2;
    this->tail = 0;
    this->length = 5;
    this->body.at(0) = indexSize + 10;
    this->body.at(1) = indexSize + 12;
    this->body.at(2) = indexSize + 14;
    this->body.at(3) = indexSize + 16;
    this->body.at(4) = indexSize + 18;
    this->accelerator = 1;
    this->addLength = 0;
}

Snake::~Snake()
{
}

int Snake::setDirection(int direction)
{
    this->direction = direction;
    return 0;
}

int Snake::addAddLength(int addAddLength)
{
    this->addLength += addAddLength;
    return 0;
}

int Snake::growMove()
{
    int adjust, xAdjust, yAdjust, previousHeadIndex, currentHeadIndex;

    for (int i = 0; i < this->accelerator; i++)
    {
        currentHeadIndex = (this->tail + this->length + i) % (indexSize);
        previousHeadIndex = (this->tail + this->length + i - 1) % (indexSize);
        this->body.at(currentHeadIndex) = this->body.at(previousHeadIndex) + this->direction;
        xAdjust = (this->direction > 0) ? (-mapx) : mapx;
        xAdjust = ((((this->body.at(currentHeadIndex) % mapx) - (this->body.at(previousHeadIndex) % mapx)) == this->direction) && this->body.at(currentHeadIndex) >= 0) ? 0 : xAdjust;
        yAdjust = (this->direction > 0) ? (-mapSize) : (mapSize);
        yAdjust = (this->body.at(currentHeadIndex) >= 0 && this->body.at(currentHeadIndex) < mapSize) ? 0 : yAdjust;
        adjust = (this->direction / mapx) == 0 ? xAdjust : yAdjust;
        this->body.at(currentHeadIndex) = (this->body.at(currentHeadIndex) + adjust) % (mapSize);
    }
    int isAdd = (this->addLength > this->accelerator) ? this->accelerator : this->addLength;
    this->length = (this->length + isAdd >= indexSize) ? indexSize : (this->length + isAdd);
    this->addLength -= isAdd;
    this->tail = (this->tail + this->accelerator - isAdd) % (indexSize);
    return 0;
}

const array<int, indexSize> &Snake::getSnake() const
{
    const array<int, indexSize> &a = this->body;
    return a;
}

int Snake::getTail() const
{
    return this->tail;
}

int Snake::getLength() const
{
    return this->length;
}

int Snake::getDirection() const
{
    return this->direction;
}

int Snake::getAccelerator() const
{
    return this->accelerator;
}

//Food类
class Food
{
private:
    int position;
    int num;

public:
    Food();
    ~Food();
    int spawn();
    int setPosition(int position);
    int getPosition() const;
    int getNum() const;
};

Food::Food()
{
    this->num = 0;
    this->position = -mapSize;
}

Food::~Food()
{
}

int Food::spawn()
{
    this->num += 1;
    return 0;
}

int Food::setPosition(int position)
{
    this->position = position;
    return 0;
}

int Food::getPosition() const
{
    return this->position;
}

int Food::getNum() const
{
    return this->num;
}

//Bonus类
class Bonus : public Food
{
private:
    int life;

public:
    Bonus();
    ~Bonus();
    int fade();
    int setLife(int life);
    int getLife() const;
};

Bonus::Bonus()
{
    this->life = 0;
    this->setPosition(mapSize + mapx * 2 - 24);
}

Bonus::~Bonus()
{
}

int Bonus::fade()
{
    int lifeBar = 2 * 10 * this->life / (halfPerimeter);
    char del = 127;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx - 22, mapy + 1});
    cout << ": ";
    for (int i = 0; i < lifeBar; i++)
    {
        cout << "★";
    }
    for (int i = 0; i < 10 - lifeBar; i++)
    {
        cout << "  ";
    }
    if (this->life == 1)
    {
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {static_cast<short>(this->getPosition() % mapx), static_cast<short>(this->getPosition() / mapx)});
        cout << del << del;
        this->setPosition(mapSize + mapx * 2 - 24);
    }
    /*
    if (this->life)
    {
        Beep(600, 200);
    }
    */
    //播放声音会导致卡顿
    this->life = (this->life == 0) ? 0 : (this->life - 1);
    return 0;
}

int Bonus::setLife(int life)
{
    this->life = life;
    return 0;
}

int Bonus::getLife() const
{
    return this->life;
}

int decideTurn(int direction, int directionPre, int &turn)
{
    switch (direction - directionPre)
    {
    case (-mapx - 2):
        turn = 4;
        break;
    case (-mapx + 2):
        turn = 3;
        break;
    case (mapx + 2):
        turn = 2;
        break;
    case (mapx - 2):
        turn = 1;
        break;
    default:
        turn = 0;
        break;
    }
    return 0;
}

int drawSnake(const Snake &s, int turn)
{
    char del = 127;
    int length = s.getLength();
    int accelerator = s.getAccelerator();
    int previousTail = (s.getTail() - accelerator + indexSize) % indexSize;
    int previousHead = (s.getTail() + length - 1 - accelerator + indexSize) % indexSize;
    const array<int, indexSize> &sCurrent = s.getSnake();

    for (int i = 0; i < accelerator; i++)
    {
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {static_cast<short>(sCurrent.at((previousTail + i) % indexSize) % mapx), static_cast<short>(sCurrent.at((previousTail + i) % indexSize) / mapx)});
        cout << del << del;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {static_cast<short>(sCurrent.at(s.getTail()) % mapx), static_cast<short>(sCurrent.at(s.getTail()) / mapx)});
    cout << "■";
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {static_cast<short>(sCurrent.at((previousHead) % indexSize) % mapx), static_cast<short>(sCurrent.at((previousHead) % indexSize) / mapx)});
    switch (turn)
    {
    case 0:
        cout << "■";
        break;
    case 1:
        cout << "◣";
        break;
    case 2:
        cout << "◢";
        break;
    case 3:
        cout << "◥";
        break;
    case 4:
        cout << "◤";
        break;
    default:
        cout << "■";
        break;
    }
    for (int i = 1; i < accelerator; i++)
    {
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {static_cast<short>(sCurrent.at((previousHead + i) % indexSize) % mapx), static_cast<short>(sCurrent.at((previousHead + i) % indexSize) / mapx)});
        cout << "■";
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {static_cast<short>(sCurrent.at((s.getTail() + length - 1) % indexSize) % mapx), static_cast<short>(sCurrent.at((s.getTail() + length - 1) % indexSize) / mapx)});
    cout << "■";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return 0;
}

int drawFoodAndBonus(int position, int type, int color)
{
    string symbol;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {static_cast<short>(position % mapx), static_cast<short>(position / mapx)});
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    symbol = (type == 1) ? "■" : "★";
    cout << symbol;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return 0;
}

int collisionDetection(const Snake &s, const Food &f, const Bonus &b)
{
    int indexTail = s.getTail();
    int length = s.getLength();
    int accelerator = s.getAccelerator();
    const array<int, indexSize> &snake = s.getSnake();
    int foodPosition = f.getPosition();
    int bonusPosition = b.getPosition();
    int resultTemp = 1;
    int result = 0;
    for (int i = 0; (i < accelerator) && (result == 0); i++)
    {
        resultTemp = snake.at((indexTail + length - i - 1) % indexSize) - foodPosition;
        result = (resultTemp == 0) ? 1 : 0;
    }
    for (int i = 0; (i < accelerator) && (result == 0); i++)
    {
        resultTemp = snake.at((indexTail + length - i - 1) % indexSize) - bonusPosition;
        result = (resultTemp == 0) ? 2 : 0;
    }
    for (int i = indexTail; (i < indexTail + length - 4) && (result == 0); i++)
    {
        resultTemp = snake.at((indexTail + length - 1) % indexSize) - snake.at(i % indexSize);
        result = (resultTemp == 0) ? -1 : 0;
    }
    return result;
}

int spawnDetection(int position, int forbiddenPosition, const Snake &s)
{
    int result = 0;
    int indexTail = s.getTail();
    int length = s.getLength();
    const array<int, indexSize> &snake = s.getSnake();
    for (int i = indexTail; (i < indexTail + length) && (result == 0); i++)
    {
        result = ((snake.at(i % indexSize) - position) == 0) ? 1 : 0;
    }
    result = (position == forbiddenPosition) ? 1 : result;
    return result;
}

int main()
{
    //设置名字
    SetConsoleTitle("贪吃蛇(感觉更像毛毛虫==）");
    //设置行列数
    char setXYCommand[30] = "mode con lines=321 cols=000";
    setXYCommand[15] = 48 + (mapy + 2) / 100;
    setXYCommand[16] = 48 + ((mapy + 2) / 10) % 10;
    setXYCommand[17] = 48 + ((mapy + 2) % 10);
    setXYCommand[24] = 48 + mapx / 100;
    setXYCommand[25] = 48 + (mapx / 10) % 10;
    setXYCommand[26] = 48 + (mapx % 10);
    system(setXYCommand);
    //隐藏光标
    CONSOLE_CURSOR_INFO cursor_info = {1, 0};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
    //禁止鼠标输入
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hInput, &mode);
    mode &= ~ENABLE_QUICK_EDIT_MODE;
    mode |= ENABLE_MOUSE_INPUT;
    SetConsoleMode(hInput, mode | ENABLE_MOUSE_INPUT);

    int score = 0;
    int victory = 0; //判断是否胜利（达到最长长度）
    int i = 0;       //控制主界面（i = 0）、游戏界面（i = 1）、切换难度设置（i = 2）、玩法介绍（i = 3）、结束界面（i = 5）、退出（i = 4）。
    int iTem = 1;
    int difficultyMode = 4; //1：简单 4：普通 9：困难
    while (true)
    {
        if (i == 0) //主界面
        {
            system("cls");
            //画logo
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 8, indexSize / mapx - 5});
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
            cout << "■";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
            cout << "■■■■■■■■◣";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 + 10, indexSize / mapx - 4});
            cout << "■";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 14, indexSize / mapx - 3});
            cout << "◢■■■■■■■■■■■◤";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 14, indexSize / mapx - 2});
            cout << "■";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 14, indexSize / mapx - 1});
            cout << "◥■■■■■■■■■";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            switch (difficultyMode)
            {
            case 1:
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 3, indexSize / mapx});
                cout << "★";
                break;
            case 4:
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx});
                cout << "★★";
                break;
            case 9:
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 5, indexSize / mapx});
                cout << "★★★";
                break;
            default:
                break;
            }
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 15, mapy + 1});
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            cout << "换行键来切换  空格键来确定";
            //画选项
            switch (iTem)
            {
            case 1:
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx + 2});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 244);
                cout << "开始游戏";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx + 3});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                cout << "选择难度";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx + 4});
                cout << "玩法介绍";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx + 5});
                cout << "退出";
                break;
            case 2:

                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx + 3});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 244);
                cout << "选择难度";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx + 2});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                cout << "开始游戏";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx + 4});
                cout << "玩法介绍";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx + 5});
                cout << "退出";
                break;
            case 3:
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx + 2});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                cout << "开始游戏";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx + 3});
                cout << "选择难度";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx + 5});
                cout << "退出";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx + 4});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 244);
                cout << "玩法介绍";
                break;
            case 4:
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx + 2});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                cout << "开始游戏";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx + 3});
                cout << "选择难度";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx + 4});
                cout << "玩法介绍";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx + 5});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 244);
                cout << "退出";
                break;
            default:
                break;
            }
            //处理选择按键
            int k = _kbhit();
            if (k)
            {
                char c = _getch();
                switch (c)
                {
                case '\r': //Enter键
                    iTem = (iTem) % 4 + 1;
                    Beep(700, 150);
                    break;
                case ' ': //Space键
                    i = iTem;
                    iTem = 1;
                    Beep(700, 150);
                    break;
                default:
                    break;
                }
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); //防止sleep时背景变化
            Sleep(100);
        }
        else if (i == 2) //切换难度界面
        {
            system("cls");
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 22, mapy + 1});
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            cout << "换行键来切换  空格键来确定  Esc放弃更改";
            //画选项
            switch (iTem)
            {
            case 1:
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx - 2});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 244);
                cout << "简单";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                cout << "普通";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx + 2});
                cout << "困难";
                break;
            case 2:
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx - 2});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                cout << "简单";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 244);
                cout << "普通";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx + 2});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                cout << "困难";
                break;
            case 3:
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx - 2});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                cout << "简单";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                cout << "普通";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx + 2});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 244);
                cout << "困难";
                break;
            default:
                break;
            }
            //处理按键
            int k = _kbhit();
            if (k)
            {
                char c = _getch();
                switch (c)
                {
                case '\r': //Enter键
                    iTem = (iTem) % 3 + 1;
                    Beep(700, 150);
                    break;
                case ' ': //Space键
                    difficultyMode = iTem * iTem;
                    iTem = 2;
                    i = 0;
                    Beep(700, 150);
                    break;
                case 27: //Esc键
                    i = 0;
                    iTem = 2;
                    Beep(700, 150);
                    break;
                default:
                    break;
                }
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); //防止sleep时背景变化
            Sleep(100);
        }
        else if (i == 3) //玩法介绍界面
        {
            system("cls");
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 8, mapy + 1});
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            cout << "按换行键返回";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx - 4});
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 36);
            cout << "玩法介绍";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 9, indexSize / mapx - 2});
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
            cout << "按W键或I键向上";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 9, indexSize / mapx - 1});
            cout << "按S键或K键向下";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 9, indexSize / mapx});
            cout << "按A键或J键向上";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 9, indexSize / mapx + 1});
            cout << "按D键或L键向上";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 16, indexSize / mapx + 2});
            cout << "按SPACE键暂停，按其他键继续";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 12, indexSize / mapx + 3});
            cout << "按Esc键中断游戏并退出";
            int k = _kbhit();
            if (k)
            {
                char c = _getch();
                if (c == '\r')
                {
                    i = 0;
                    iTem = 3;
                    Beep(700, 150);
                }
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); //防止sleep时背景变化
            Sleep(100);
        }
        else if (i == 5) //结束时的选择界面
        {
            system("cls");
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 6, indexSize / mapx - 1});
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 100);
            cout << "游戏得分";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 4, indexSize / mapx});
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
            cout << score;
            if (victory)
            {
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 5, indexSize / mapx + 1});
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                cout << "胜利！";
            }
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 26, mapy + 1});
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            cout << "空格键重新开始  换行键返回主界面  Esc键退出游戏";
            //按键控制
            int k = _kbhit();
            if (k)
            {
                char c = _getch();
                switch (c)
                {
                case '\r':     //Enter键
                    score = 0; //清空当局分数
                    victory = 0;
                    i = 0;
                    Beep(700, 150);
                    break;
                case ' ':      //Space键
                    score = 0; //清空当局分数
                    victory = 0;
                    i = 1;
                    Beep(700, 150);
                    break;
                case 27: //Esc键
                    i = 4;
                    Beep(700, 150);
                    break;
                default:
                    break;
                }
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); //防止sleep时背景变化
            Sleep(100);
        }
        else if (i == 4) //退出
        {
            return 0;
        }
        else if (i == 1) //游戏运行
        {
            system("cls");
            Snake s;
            Food f;
            Bonus b;
            srand((unsigned int)time(NULL)); //食物生成种子
            f.spawn();
            f.setPosition((rand() % indexSize) * 2);
            while (spawnDetection(f.getPosition(), b.getPosition(), s))
            {
                f.setPosition((rand() % indexSize) * 2);
            }
            //画初始蛇
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {10, indexSize / mapx});
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
            cout << "■■■■";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
            cout << "■";
            _getch();
            //画分割线
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {0, mapy});
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            for (int i = 0; i < mapx / 2; i++)
            {
                cout << "—";
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            for (int i = 0; i <= difficultyMode / 4; i++)
            {
                cout << "★";
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            cout << "当前得分:";
            //游戏循环
            int gameCtl = 1; //控制游戏循环
            int turn = 0;
            while (gameCtl)
            {

                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {16, mapy + 1});
                cout << score;
                drawFoodAndBonus(f.getPosition(), 1, 3); //画食物
                drawFoodAndBonus(b.getPosition(), 2, 4); //画bonus
                drawSnake(s, turn);
                turn = 0;
                //读取键盘控制，改变方向
                int k = _kbhit();
                char c;
                int direction = 0;
                int directionPre = 0;
                if (k)
                {
                    c = _getch();
                IHateDoingThis:
                    switch (c)
                    {
                    case ' ':
                        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 13, mapy + 1});
                        cout << "暂停，按其他键继续...";
                        c = _getch();
                        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 13, mapy + 1});
                        cout << "                     ";
                        goto IHateDoingThis; //为了让暂停后可以立刻转弯，而不是先走一格再转
                        break;
                    case 27:
                        i = 4;
                        gameCtl = 0;
                        Beep(700, 150);
                        break;
                    case 'w':
                        directionPre = s.getDirection();
                        s.setDirection((mapx) == (directionPre) ? mapx : -mapx);
                        direction = s.getDirection();
                        decideTurn(direction, directionPre, turn);
                        break;
                    case 'i':
                        directionPre = s.getDirection();
                        s.setDirection((mapx) == (directionPre) ? mapx : -mapx);
                        direction = s.getDirection();
                        decideTurn(direction, directionPre, turn);
                        break;
                    case 'a':
                        directionPre = s.getDirection();
                        s.setDirection((2) == (directionPre) ? 2 : -2);
                        direction = s.getDirection();
                        decideTurn(direction, directionPre, turn);
                        break;
                    case 'j':
                        directionPre = s.getDirection();
                        s.setDirection((2) == (directionPre) ? 2 : -2);
                        direction = s.getDirection();
                        decideTurn(direction, directionPre, turn);
                        break;
                    case 's':
                        directionPre = s.getDirection();
                        s.setDirection((-mapx) == (directionPre) ? -mapx : mapx);
                        direction = s.getDirection();
                        decideTurn(direction, directionPre, turn);
                        break;
                    case 'k':
                        directionPre = s.getDirection();
                        s.setDirection((-mapx) == (directionPre) ? -mapx : mapx);
                        direction = s.getDirection();
                        decideTurn(direction, directionPre, turn);
                        break;
                    case 'd':
                        directionPre = s.getDirection();
                        s.setDirection((-2) == (directionPre) ? -2 : 2);
                        direction = s.getDirection();
                        decideTurn(direction, directionPre, turn);
                        break;
                    case 'l':
                        directionPre = s.getDirection();
                        s.setDirection((-2) == (directionPre) ? -2 : 2);
                        direction = s.getDirection();
                        decideTurn(direction, directionPre, turn);
                        break;
                    default:
                        break;
                    }
                }

                b.fade();
                int result = collisionDetection(s, f, b);
                int num = f.getNum();
                switch (result)
                {
                case -1:
                    Beep(600, 250);
                    Beep(450, 150);
                    Beep(300, 450);
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 14, mapy + 1});
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                    cout << "游戏结束，请按任意键...";
                    _getch();
                    i = 5;
                    gameCtl = 0;
                    break;
                case 0:
                    s.growMove();
                    break;
                case 1:
                    //Beep(500, 200);
                    s.addAddLength(1);
                    s.growMove();
                    score += 10;
                    f.spawn();
                    f.setPosition((rand() % indexSize) * 2);
                    while (spawnDetection(f.getPosition(), b.getPosition(), s))
                    {
                        f.setPosition((rand() % indexSize) * 2);
                    }
                    if ((num != 0) && ((num % 5) == 0))
                    {
                        b.setLife(mapx / 2 + mapy / 2);
                        b.setPosition((rand() % indexSize) * 2);
                        while (spawnDetection(b.getPosition(), f.getPosition(), s))
                        {
                            b.setPosition((rand() % indexSize) * 2);
                        }
                    }
                    break;
                case 2:
                    Beep(800, 200);
                    Beep(1000, 100);
                    s.growMove();
                    score += 100 * b.getLife() / (halfPerimeter);
                    b.setLife(0);
                    b.setPosition(mapSize + mapx * 2 - 24);
                    break;
                default:
                    break;
                }
                if (s.getLength() == indexSize) //达到最长长度，游戏胜利
                {
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {mapx / 2 - 14, mapy + 1});
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                    cout << "游戏胜利，请按任意键...";
                    _getch();
                    i = 5;
                    gameCtl = 0;
                    victory = 1;
                }
                Sleep(400 / difficultyMode);
            } //游戏内的while
        }     //进游戏界面的判断if
    }         //最外的while
    return 0;
}