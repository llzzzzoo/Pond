#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "Rain.h"


void InitScreen(Screen& S) {
    S.rain.elem = (EveryRain)malloc(sizeof(RainNode) * ROW * COLUMN + 1); // 难死了这几个初始化
    S.rain.front = 0;
    S.rain.rear = 0;
    S.rain.maxSize = ROW * COLUMN + 1;
    S.screenIndex = (RainType(*)[COLUMN])malloc(sizeof(RainType) * ROW * COLUMN); // 1维数组*COLUMN
    S.water = (Water)malloc(sizeof(waterNode) * COLUMN);
    if (S.rain.elem == NULL || S.screenIndex == NULL || S.water == NULL) return;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            S.screenIndex[i][j] = 32;
        }
    }
  
    for (int i = 0; i < COLUMN; i++) {
        S.water[i].w = '~';
        S.water[i].move_times = 0;
        S.water[i].tag = 0;
    }
}

void freeScreen(Screen& S) {
    // free(S.rain.elem);S.rain.elem = NULL；// 太妙了，此处不能free
    // 因为结构体内存对齐S地址=S.rain的地址=S.rain.elem的地址，这里free就会把整个S给free了
    // 但毕竟都快写完了，而且S局部变量，离开循环就没了，问题不大，下次小心吧
    free(S.screenIndex); // 指向数组的指针，直接释放即可
    free(S.water);
    S.screenIndex = NULL;
    S.water = NULL;
}

// 产生雨滴，随机选取rain数组的元素补充雨滴
Status	CreateRain(Screen& S, RainType type, int en_times, int offsetY) {
    // 随机选取下标，随机产生波纹数目，随机产生颜色
    if (NULL == S.rain.elem)return ERROR;
    // 随机选取坐标
    for (int j = 0; j < COLUMN; j++) {
        int n = getRand(1, 80) % en_times;
        if (0 == n) { // 被选中
            RainNode e;
            e.rainType = type; // 不同情况，雨滴不同
            e.x = 0;
            e.y = j; // 对应第一行
            e.offsetY = offsetY; // 对应第一行
            e.waterRingNum = getRand(1, 3);
            e.color = (Color)getRand(0, 6); // 此处强制类型转换
            e.tag = 1;
            EnQueue_Sq(S.rain, e); // 入队
        }
    }
    return OK;
}

/*
    让雨滴下落
*/
Status dropRain(Screen& S, int wind_flag) {
    if (NULL == S.rain.elem)return ERROR;
    RainSqQueue Q = S.rain;
    // 遍历队列，然后让他们的y坐标下移
    for (int j = Q.front; j != Q.rear; j = (j + 1) % Q.maxSize) {
        Q.elem[j].x++;
        // 此处的wind_flag的设置是为了后期补充其他功能时更方便
        if (wind_flag == 1) {
            // 偏移量大于0，且y大于等于0，才减1，也就是说，y最小为-1
            if (Q.elem[j].offsetY > 0 && Q.elem[j].y >= 0) {
                Q.elem[j].y--;
                Q.elem[j].offsetY--;
            }
            // 此处有一个极难发现的bug，那就是执行完上面的if之后，y变为-1，同时接触水面，但是tag未修改，导致产生了water[-1]
            if (Q.elem[j].y < 0) {
                Q.elem[j].tag = 2;
            }
        }
    }
    return OK;
}

/*
    刷新一次，将新的内容预存到显示数组中，包括雨滴，打雷，风，水波
*/
void flush(Screen S) {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            S.screenIndex[i][j] = 32;
        }
    }
    RainSqQueue Q = S.rain;
    for (int j = Q.front; j != Q.rear; j=(j+1)%Q.maxSize) {
         int x = Q.elem[j].x;
         int y = Q.elem[j].y;
         // 处理y为-1的情况
         y < 0 ? S.screenIndex[x][0] = 32 : S.screenIndex[x][y] = Q.elem[j].rainType;
    }

}


/*
    打印出屏幕当前所有内容
*/
void PrintScreen(Screen S, RainNode* fallIntoWater, int last_index_rain) {
    // 打印屏幕内容
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            printf("%c", S.screenIndex[i][j]);
        }
        printf("\n");
    }
 
    // 打印水
    CheckWaterRing(S.water, 1);
    for (int i = 0; i < COLUMN; i++) {
        printf("%c", S.water[i].w);
    }
    printf("\n");
    CheckWaterRing(S.water, -1); // 还原水滴

    // 打印雨滴信息
    if (fallIntoWater == NULL) return;
    printf("从左至右，下面为落入水里的雨滴的信息:\n");
    for (int i = 0; i < last_index_rain; i++) {
        printf("第%d滴雨的颜色为: ", i + 1);
        switch (fallIntoWater[i].color)
        {
        case RED:
            printf("RED\n");
            break;
        case ORANGE:
            printf("ORANGE\n");
            break;
        case YELLOW:
            printf("YELLOW\n");
            break;
        case GREEN:
            printf("GREEN\n");
            break;
        case CYAN:
            printf("CYAN\n");
            break;
        case BLUE:
            printf("BLUE\n");
            break;
        case PURPLE:
            printf("PURPLE\n");
            break;
        default:
            break;
        }
    }
}

/*
    触碰到水时，会抹去存在，并且产生水花
*/
int TouchWater(Screen &S, RainNode* fallIntoWater) {
    // 遍历最后一行位置的雨滴，当drop发生后，产生水花，并抹掉它们在坐标和循环数组的存在
    // 抹去存在，直接除去循环数组里面的就好了，因为我的flush操作会刷新一遍显示数组
    // 从队头往后遍历COLUMN次(一行最多COLUMN个)，当x坐标最后一行的，就把tag置为0，并且出栈
    int the_last_rain = 0;
    int pre_front = 0; // 记录加1前的front值
    for (int i = 0; i < COLUMN; i++) {
        if (S.rain.elem[S.rain.front].x == ROW) {
            RainNode e = S.rain.elem[S.rain.front];
            pre_front = S.rain.front;
            DeQueue_Sq(S.rain); // 删除这些落水的，此处的出栈需要对S，不能对局部变量
            // 当tag为1时才产生水波，如果为2，即出界，不产生水波
            if (e.tag == 1) {
                CreateWaterRing(S, e);// 蹦出水花，此处传过去的参数也要是S，不能是局部变量
            }
            S.rain.elem[pre_front].tag = 0;
            fallIntoWater[the_last_rain++] = e;// 检查落水雨滴的颜色
        }
    }
    return the_last_rain; // 返回数组的最后一个下标
}

// 产生水波
void CreateWaterRing(Screen &S, RainNode e) {
    // 考虑到边界条件
    int y = e.y; // e下落时的y坐标
    if (y < 0) return; // 勇敢if，不怕溢出
    S.water[y].move_times = S.water[y].tag = e.waterRingNum;
}

/*
    在每次打印水之前，检查水波
    @param op 为1时负责检查随波，为-1时负责还原水波
*/
void CheckWaterRing(Water water, int op) {
    // 注意边界条件
    for (int i = 0; i < COLUMN; i++) {
        if (op == 1) {
            if (water[i].tag != 0) {
                int offsetLeft = i - (water[i].move_times - water[i].tag + 1);
                int offsetRight = i + (water[i].move_times - water[i].tag + 1);
                if (offsetLeft >= 0) {
                    water[offsetLeft].w = '(';
                }
                if (offsetRight <= COLUMN - 1) {
                    water[offsetRight].w = ')';
                }
                water[i].tag--; // 减一次，下次就会偏移更远
            }
        } else if(op == -1) {
            if (water[i].tag == 0) {
                water[i].w = '~';
                water[i].move_times = 0;
            }
        }
    }
}

// 创造闪电
void CreateThunder(Screen& S) {
    // 设定雷电的单位长度
    int len = ROW / 8;
    // 从边界往后遍历，随机选取点产生雷电
    for (int i = len + 1; i < COLUMN; i = i + len + 2) {
        int num = getRand(1, 10);// 1 / 5的概率
        if (num % 5 == 0) {
            // 产生雷电
            int indexX1 = 0; // x坐标
            int indexY1 = i;
            int indexX2 = 0;
            int indexY2 = 0;
            int indexX3 = 0;
            int indexY3 = 0;
            for (int offset = 0; offset < len; offset++) {
                S.screenIndex[indexX1++][indexY1 - offset] = '/';
                indexX2 = indexX1 - 1;
                indexY2 = indexY1 - offset;
            }
            for (int offset = 0; offset < len; offset++) {
                S.screenIndex[indexX2][indexY2 + offset + 1] = '_';
                indexX3 = indexX2;
                indexY3 = indexY2 + offset + 1;
            }
            for (int offset = 0; offset < len; offset++) {
                S.screenIndex[++indexX3][indexY3 - offset] = '/';
            }
        }
    }
}

Status EnQueue_Sq(RainSqQueue& Q, RainNode e) {
    // 若当前队列不满，在队列的尾元素之后，插入元素 e 为新的队列尾元素
    if (Q.front == (Q.rear + 1) % Q.maxSize) return ERROR;
    Q.elem[Q.rear] = e;
    Q.rear = (Q.rear + 1) % Q.maxSize;
    return OK;
}

Status DeQueue_Sq(RainSqQueue& Q) {
    // 若队列不空，则删除队列Q中的头元素
    if (TRUE == QueueEmpty_Sq(Q)) return ERROR; // 队空报错
    Q.front = (Q.front + 1) % Q.maxSize; // Q.front循环加1
    return OK;
}

Status QueueEmpty_Sq(RainSqQueue Q) { // 判断队列Q是否空，若空则返回TRUE，否则FALSE
    return Q.front == Q.rear ? TRUE : FALSE;
}

// 左闭右闭区间，产生随机数
int getRand(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}