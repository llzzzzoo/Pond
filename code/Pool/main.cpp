#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "Rain.h"

int main() {
	while (true)
	{
		Screen S;
		InitScreen(S);
		char user_en; // 代表接收用户输入的回车
		RainNode fallIntoWater[COLUMN]; // 用来存储落入水中雨滴的信息
		int last_index = 0; // 落入水中的雨滴的最后一个元素的下标
		int en_times = 0; // 对应用户的输入，实现随机输入改变，雨滴密度改变
		int den = 0; // 改变雨滴密度
		int code = 0;
		int c = 0; // 清空输入缓存
		RainType type = 0;
		int offsetY = 0;
		int wind_flag = 0; // 为0时表明无风，为1时表明有风
		int aa = 0;
		do {
			code = 0;
			wind_flag = 0;
			system("cls");
			printf("=====请欣赏今晚奇丽的“池塘夜雨”=====\n");
			printf("|----- 1: 随风潜入夜,润物细无声 -----|\n");
			printf("|----- 2: 雷声千嶂落,雨色万峰来 -----|\n");
			printf("|----- 3: 离开 ----------------------|\n");
			printf("======================================");
			printf("\n请输入对应指令：");
			scanf("%d", &code);
			while ((c = getchar()) != '\n' && c != EOF); // 清空输入缓存
		}while (code < 1 || code > 3);

		switch (code)//注意下面都是函数的声明格式 具体输入都要根据自己调整 
		{
			case 1:
				type = '|';
				offsetY = 0;
				wind_flag = 0;
				break;
			case 2:
				type = '/';
				offsetY = COLUMN / 2 - 1;
				wind_flag = 1;
				break;
			case 3:
				return 1;
		}
		system("cls");
		do {
			en_times++;
			if (en_times <= getRand(ROW * 2, ROW * 3)) {
				en_times <= getRand(ROW, ROW * 2 - 1) ? den = COLUMN / 5 : den = COLUMN / 20;
			} else {
				en_times = 0;
			}
			CreateRain(S, type, den, offsetY);
			flush(S); // 将flush和打雷分开，打雷同时雨滴也要往下走
			if (wind_flag == 1) CreateThunder(S);
			PrintScreen(S, fallIntoWater, last_index);
			last_index = 0;
			printf("\n输入ENTER继续，0+ENTER离开(为达到最好效果，请放大窗口，持续按压ENTER键)");
			user_en = getchar();
			dropRain(S, wind_flag);
			last_index = TouchWater(S, fallIntoWater);
			system("cls");
		} while (10 == user_en);
		freeScreen(S);
	}
	return 0;
}