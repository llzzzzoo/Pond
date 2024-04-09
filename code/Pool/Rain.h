#define OK 1
#define ERROR -1
#define TRUE 2
#define FALSE -2
#define OVERFLOW 0

#define ROW 40 // 至少为6，建议ROW < COLUMUN - 20
#define COLUMN 80 // 至少为21

typedef int Status;
typedef char RainType;
typedef char WaterType;
typedef char* Thunder;
enum Color
{
	RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, PURPLE
};

typedef struct RainNode{
	RainType rainType = '|'; // 雨滴形状
	int x = 0; // x的坐标
	int y = 0; // y的坐标
	int offsetY = 0; // y的偏置量，用来处理风吹的情况
	int waterRingNum = 0; // 产生的水圈的数目
	Color color = RED; // 枚举类型，指明颜色
	int tag = 0; // 为0表明无雨，为1表明由于，为2表明斜的雨出界了
} *EveryRain;

typedef struct waterNode {
	WaterType w; // 水波的形状
	int move_times = 0; // 水波移动的偏移量
	int tag = 0; // 为0表明为正常的水，为其他的则表明为水波
} *Water;

typedef struct SqQueue {
	EveryRain elem;  // 存储空间的基址
	int front;        // 队头位标
	int rear;         // 队尾位标，指示队尾元素的下一位置
	int maxSize;      // 最大长度
} RainSqQueue;

typedef struct ScreenNode{
	RainSqQueue rain; // 存放存在的全部雨滴的循环队列
	RainType (*screenIndex)[COLUMN]; // 对应屏幕上的坐标，负责显示内容，数组指针
	Water water; // 屏幕下方的水
} Screen; // 屏幕

void InitScreen(Screen &S); // 初始化屏幕状态
Status	CreateRain(Screen& S, RainType type, int en_times, int offsetY); // 产生雨滴，随机选取rain数组的元素补充雨滴
Status dropRain(Screen &S, int wind_flag); // 让雨下落
void flush(Screen S); // 刷新一次，将新的内容预存到显示数组中，包括雨滴，打雷，风，水波
void PrintScreen(Screen S, RainNode* fallIntoWater, int last_index_rain); // 刷新一次屏幕
int TouchWater(Screen& S, RainNode *fallIntoWater); // 触水时会发生的一系列反应
void CreateWaterRing(Screen &S, RainNode e); // 产生水花
void CheckWaterRing(Water water, int op); // 在每次打印水之前，检查水波
void CreateThunder(Screen& S); // 产生雷电
Status EnQueue_Sq(RainSqQueue& Q, RainNode e); // 入队
Status DeQueue_Sq(RainSqQueue& Q); // 出队
Status QueueEmpty_Sq(RainSqQueue Q); // 给队列判空
void freeScreen(Screen& S); // 将开辟的空间free掉
int getRand(int min, int max); //产生随机数，左闭右闭区间