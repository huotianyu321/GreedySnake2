/*
日志：
1. 现存问题：两条蛇不能同时动
2. 解决两条蛇不能同时动的问题：不用peekmessage 用getkeystate
*/

#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <mmsystem.h>

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#pragma comment(lib, "winmm.lib")  // 库文件

// 参数
const int map_width = 900;
const int map_height = 900;
const int window_width = 1200;
const int window_height = 900;

// ===
// === 图片资源
// ===
class ImageRes {
private:
	IMAGE bg;
	// snake1
	IMAGE head_up_open_1;
	IMAGE head_down_open_1;
	IMAGE head_left_open_1;
	IMAGE head_right_open_1;
	IMAGE head_up_close_1;
	IMAGE head_down_close_1;
	IMAGE head_left_close_1;
	IMAGE head_right_close_1;
	IMAGE body_1;
	// snake2
	IMAGE head_up_open_2;
	IMAGE head_down_open_2;
	IMAGE head_left_open_2;
	IMAGE head_right_open_2;
	IMAGE head_up_close_2;
	IMAGE head_down_close_2;
	IMAGE head_left_close_2;
	IMAGE head_right_close_2;
	IMAGE body_2;
	// 掩码图
	IMAGE head_open_mask;
	IMAGE mask;
	IMAGE food;
	IMAGE blink_food;
	IMAGE menu;
	IMAGE arrow;

public:
	ImageRes();
	const IMAGE& getBg() const { return bg; }
	// snake1
	const IMAGE& getHeadUpOpen1() const { return head_up_open_1; }
	const IMAGE& getHeadDownOpen1() const { return head_down_open_1; }
	const IMAGE& getHeadLeftOpen1() const { return head_left_open_1; }
	const IMAGE& getHeadRightOpen1() const { return head_right_open_1; }
	const IMAGE& getHeadDownClose1() const { return head_down_close_1; }
	const IMAGE& getHeadLeftClose1() const { return head_left_close_1; }
	const IMAGE& getHeadRightClose1() const { return head_right_close_1; }
	const IMAGE& getHeadUpClose1() const { return head_up_close_1; }
	const IMAGE& getBody1() const { return body_1; }
	// snake2
	const IMAGE& getHeadUpOpen2() const { return head_up_open_2; }
	const IMAGE& getHeadDownOpen2() const { return head_down_open_2; }
	const IMAGE& getHeadLeftOpen2() const { return head_left_open_2; }
	const IMAGE& getHeadRightOpen2() const { return head_right_open_2; }
	const IMAGE& getHeadDownClose2() const { return head_down_close_2; }
	const IMAGE& getHeadLeftClose2() const { return head_left_close_2; }
	const IMAGE& getHeadRightClose2() const { return head_right_close_2; }
	const IMAGE& getHeadUpClose2() const { return head_up_close_2; }
	const IMAGE& getBody2() const { return body_2; }

	const IMAGE& getMask() const { return mask; }
	const IMAGE& getHeadOpenMask() const { return head_open_mask; }
	const IMAGE& getFood() const { return food; }
	const IMAGE& getBlinkFood() const { return blink_food; }
	const IMAGE& getMenu() const { return menu; }
	const IMAGE& getArrow() const { return arrow; }
};

// ===
// === 坐标
// ===
struct Location {
	int x;
	int y;
	Location(int loc_x, int loc_y) : x(loc_x), y(loc_y) {};
};

class Snake;
// ===
// === 食物
// ===
class Food {
private:
	friend class Snake;
	std::vector<Location> foods_loc;  // 每个食物的位置
	std::vector<int>
		foods_remained_times;  // 每个食物剩余的时间,每个食物存在一定次数的循环，少于一定次数后开始闪烁，这里设置food_lived_times个循环
	const IMAGE* normal_food;
	const IMAGE* blink_food;
	const IMAGE* mask;
	std::mutex food_mtx;
	int food_lived_times;

public:
	Food(const ImageRes& imgs, int food_times);
	void addFood(const Snake& snake1, const Snake& snake2);
	void putFood();
	void freshFoodTime_and_Remove();  // 刷新食物剩余时间并删除超时的食物
};

// ===
// === 蛇
// ===
class Snake {
private:
	friend class Food;

private:
	std::vector<Location> body_loc;  // 蛇身每一部分的坐标（图片左上角）
	int speed = 25;                  // 移动速度（像素点）
	int moving_direction;            // 1上2下3左4右
	int play_big_head_times = 0;  // 吃食物时大头播放循环次数
	int id;
	const IMAGE* head_up_open;
	const IMAGE* head_down_open;
	const IMAGE* head_left_open;
	const IMAGE* head_right_open;
	const IMAGE* head_up_close;
	const IMAGE* head_down_close;
	const IMAGE* head_left_close;
	const IMAGE* head_right_close;
	const IMAGE* body;
	const IMAGE* head_open_mask;
	const IMAGE* mask;
	std::mutex snake_mtx;  // 蛇数组共享变量锁 （主线程与蛇线程之间）
public:
	Snake(int snake_id, const ImageRes& imgs);
	void putSnake();  // 绘制蛇
	void decreaseBigHeadTimes();
	void moveControl(int input_direction);  // 移动控制
	int moveCheck();
	bool hitSelfCheck();         // 自身碰撞检测
	bool hitWallCheck() const;   // 撞墙检测
	bool eatFood(Food& food);    // 是否吃到食物
	bool eatTail(Snake& snake);  // 吃掉另一条蛇的尾巴
	int getBodyLen();            // 获取蛇身长度
	int getId() const { return id; }
};

// ===
// === Tools
// ===
// 绘图
void putAlpha(int picture_x, int picture_y, const IMAGE& pic,
	const IMAGE& mask);
// 设置字体风格
void setTextStyle();
// 显示得分
void showScore(int id, int score);
// 刷新屏幕
void freshScreen(const ImageRes& imgs, Snake& snake1, Snake& snake2,
	Food& food);
// 获取时间
long long getTime();
// 线程调用函数
void snakeControl(Snake& snake, Snake& another_snake, Food& food,
	bool& game_over, const int& loop_sleep_time);
// 菜单
void freshMenu(const ImageRes& imgs, const int arrow_y_loc);
int menu(const ImageRes& imgs);  // 返回游戏难度1，2，3 , 4 退出

int main() {
	std::srand(std::time(nullptr));
	initgraph(window_width, window_height);
	setTextStyle();
	// 打开音乐
	mciSendString(_T("open res//bg_music.mp3"), 0, 0, 0);
	mciSendString(_T("open res//eat.mp3"), 0, 0, 0);
	// 播放音乐
	mciSendString(_T("play res//bg_music.mp3 repeat"), 0, 0, 0);
	// 游戏主循环
	while (true) {
		int food_times = 150;      // 食物循环循环次数
		int loop_sleep_time = 40;  // 循环待机时间，ms
		int new_food_time = 3;
		ImageRes imgs;  // 加载图片资源
		// 菜单
		int hardness = menu(imgs);
		if (hardness == 1) {
			food_times = 150;
			loop_sleep_time = 40;
			new_food_time = 3;
		}
		else if (hardness == 2) {
			food_times = 100;
			loop_sleep_time = 20;
			new_food_time = 2;
		}
		else if (hardness == 3) {
			food_times = 50;
			loop_sleep_time = 10;
			new_food_time = 1;
		}
		else if (hardness == 4)
			break;

		Snake snake1(1, imgs);
		Snake snake2(2, imgs);
		Food food(imgs, food_times);
		long long game_start_time, game_end_time;
		bool game_over = false;
		game_start_time = getTime();
		std::thread snake_thread_1(snakeControl, std::ref(snake1), std::ref(snake2),
			std::ref(food), std::ref(game_over),
			std::cref(loop_sleep_time));
		std::thread snake_thread_2(snakeControl, std::ref(snake2), std::ref(snake1),
			std::ref(food), std::ref(game_over),
			std::cref(loop_sleep_time));

		// 游戏运行中
		while (true) {
			freshScreen(imgs, snake1, snake2, food);
			// === 处理食物
			food.freshFoodTime_and_Remove();
			// 每new_food_time添加一个新食物
			game_end_time = getTime();
			if (game_end_time - game_start_time >= new_food_time) {
				game_start_time = game_end_time;
				food.addFood(snake1, snake2);
			}

			if (game_over) {
				// freshScreen(imgs, snake1, snake2, food, snake1_play_eat_action_count,
				// snake2_play_eat_action_count);
				snake_thread_1.join();
				snake_thread_2.join();
				while (true)  // 卡住画面，等待ESC按键按下
				{
					if (GetKeyState(VK_ESCAPE) & 0x8000) break;
				}
				break;  // 退出游戏画面到主菜单
			}
			// 降低cpu占用，控制移动速度
			std::this_thread::sleep_for(std::chrono::milliseconds(loop_sleep_time));
		}
	}

	closegraph();
	// 关闭音乐
	mciSendString(_T("close res//bg_music.mp3"), 0, 0, 0);
	mciSendString(_T("close res//eat.mp3"), 0, 0, 0);
	return 0;
}

// ===
// === 工具函数
// ===
// 绘图
void putAlpha(int picture_x, int picture_y, const IMAGE& pic,
	const IMAGE& mask) {
	putimage(picture_x, picture_y, &mask, SRCPAINT);
	putimage(picture_x, picture_y, &pic, SRCAND);
}
// 获取当前时间
long long getTime() {
	// 获取当前时间点
	const auto t = std::chrono::system_clock::now();
	// 将上时间点从格林威治。。。。起经过的时间转换成以秒为单位，并计数
	return std::chrono::duration_cast<std::chrono::seconds>(t.time_since_epoch())
		.count();
}
// 刷新屏幕
void freshScreen(const ImageRes& imgs, Snake& snake1, Snake& snake2,
	Food& food) {
	BeginBatchDraw();  // 将图像先贴到缓冲区，防止闪屏
	cleardevice();
	putimage(0, 0, &imgs.getBg());
	food.putFood();
	snake1.putSnake();
	showScore(snake1.getId(), snake1.getBodyLen() - 4);
	snake2.putSnake();
	showScore(snake2.getId(), snake2.getBodyLen() - 4);
	EndBatchDraw();
}
// 线程调用函数
void snakeControl(Snake& snake, Snake& another_snake, Food& food,
	bool& game_over, const int& loop_sleep_time) {
	while (true) {
		// 检查按键消息,控制蛇移动
		snake.moveControl(snake.moveCheck());

		// 碰撞检测
		if (snake.hitWallCheck() || snake.hitSelfCheck() || game_over ||
			snake.getBodyLen() == 1) {
			game_over = true;
			break;
		}
		if (snake.eatFood(food) || snake.eatTail(another_snake))
			mciSendString(_T("play res//eat.mp3"), 0, 0, 0);
		// 大蛇头播放次数递减
		snake.decreaseBigHeadTimes();

		std::this_thread::sleep_for(std::chrono::milliseconds(loop_sleep_time));
	}
}
// 菜单
void freshMenu(const ImageRes& imgs, const int arrow_y_loc) {
	cleardevice();
	putimage(0, 0, &imgs.getBg());
	putimage(150, 50, &imgs.getMenu());
	putimage(150, arrow_y_loc, &imgs.getArrow());
}
int menu(const ImageRes& imgs)  // 返回游戏难度1，2，3 , 4 退出
{
	int choice = 1;
	int arrow_y_loc = 120;
	freshMenu(imgs, arrow_y_loc);
	while (true) {
		if (GetKeyState(VK_DOWN) & 0x8000 ||
			GetKeyState(0x53) & 0x8000)  // 如果按s或下键
		{
			if (arrow_y_loc == 720) {
				choice = 1;
				arrow_y_loc = 120;
			}
			else {
				arrow_y_loc += 200;
				++choice;
			}
			freshMenu(imgs, arrow_y_loc);
		}
		else if (GetKeyState(VK_UP) & 0x8000 || GetKeyState(0x57) & 0x8000) {
			if (arrow_y_loc == 120) {
				choice = 4;
				arrow_y_loc = 720;
			}
			else {
				arrow_y_loc -= 200;
				--choice;
			}
			freshMenu(imgs, arrow_y_loc);
		}
		else if (GetKeyState(VK_RETURN) & 0x8000 ||
			GetKeyState(0x57) & 0x8000)  // 按下回车键
		{
			return choice;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
// 设置字体风格
void setTextStyle() {
	settextstyle(30, 0, _T("隶书"));  // 设置字体
	settextcolor(BLACK);              // 文字颜色
	setbkmode(TRANSPARENT);           // 设置背景色为透明
}
// 显示得分
void showScore(int id, int score) {
	TCHAR s1[10];
	_stprintf(s1, _T("%d号得分"), id);
	TCHAR s2[10];
	_stprintf(s2, _T("%d 分"), score);
	if (id == 1) {
		outtextxy(950, 400, s1);
		outtextxy(990, 440, s2);
	}
	else if (id == 2) {
		outtextxy(950, 480, s1);
		outtextxy(990, 520, s2);
	}
}
// ===
// === 图片资源
// ===
ImageRes::ImageRes() {
	loadimage(&bg, _T("res\\bg.png"));

	// snake1
	loadimage(&head_left_open_1, _T("res\\head_left_open_1.png"));
	loadimage(&head_right_open_1, _T("res\\head_right_open_1.png"));
	loadimage(&head_down_open_1, _T("res\\head_down_open_1.png"));
	loadimage(&head_up_open_1, _T("res\\head_up_open_1.png"));
	loadimage(&head_left_close_1, _T("res\\head_left_close_1.png"));
	loadimage(&head_right_close_1, _T("res\\head_right_close_1.png"));
	loadimage(&head_down_close_1, _T("res\\head_down_close_1.png"));
	loadimage(&head_up_close_1, _T("res\\head_up_close_1.png"));
	loadimage(&body_1, _T("res\\body_1.png"));

	// snake2
	loadimage(&head_left_open_2, _T("res\\head_left_open_2.png"));
	loadimage(&head_right_open_2, _T("res\\head_right_open_2.png"));
	loadimage(&head_down_open_2, _T("res\\head_down_open_2.png"));
	loadimage(&head_up_open_2, _T("res\\head_up_open_2.png"));
	loadimage(&head_left_close_2, _T("res\\head_left_close_2.png"));
	loadimage(&head_right_close_2, _T("res\\head_right_close_2.png"));
	loadimage(&head_down_close_2, _T("res\\head_down_close_2.png"));
	loadimage(&head_up_close_2, _T("res\\head_up_close_2.png"));
	loadimage(&body_2, _T("res\\body_2.png"));

	loadimage(&head_open_mask, _T("res\\head_open_mask.png"));
	loadimage(&mask, _T("res\\mask.png"));
	loadimage(&food, _T("res\\food.png"));
	loadimage(&blink_food, _T("res\\blink_food.png"));
	loadimage(&arrow, _T("res\\arrow.png"));
	loadimage(&menu, _T("res\\menu.png"));
}

// ===
// === 蛇
// ===
Snake::Snake(int snake_id, const ImageRes& imgs) : id(snake_id) {
	head_open_mask = &imgs.getHeadOpenMask();
	mask = &imgs.getMask();
	if (id == 1) {
		moving_direction = 4;
		head_up_open = &imgs.getHeadUpOpen1();
		head_down_open = &imgs.getHeadDownOpen1();
		head_left_open = &imgs.getHeadLeftOpen1();
		head_right_open = &imgs.getHeadRightOpen1();
		head_up_close = &imgs.getHeadUpClose1();
		head_down_close = &imgs.getHeadDownClose1();
		head_left_close = &imgs.getHeadLeftClose1();
		head_right_close = &imgs.getHeadRightClose1();
		body = &imgs.getBody1();

		body_loc.emplace_back(200, 200);
		body_loc.emplace_back(175, 200);
		body_loc.emplace_back(150, 200);
		body_loc.emplace_back(125, 200);
	}
	else if (id == 2) {
		moving_direction = 3;
		head_up_open = &imgs.getHeadUpOpen2();
		head_down_open = &imgs.getHeadDownOpen2();
		head_left_open = &imgs.getHeadLeftOpen2();
		head_right_open = &imgs.getHeadRightOpen2();
		head_up_close = &imgs.getHeadUpClose2();
		head_down_close = &imgs.getHeadDownClose2();
		head_left_close = &imgs.getHeadLeftClose2();
		head_right_close = &imgs.getHeadRightClose2();
		body = &imgs.getBody2();

		body_loc.emplace_back(700, 200);
		body_loc.emplace_back(725, 200);
		body_loc.emplace_back(750, 200);
		body_loc.emplace_back(775, 200);
	}
}
void Snake::putSnake() {
	// 倒序绘制，图层覆盖
	// 绘制蛇身
	snake_mtx.lock();  // 主线程绘图时不允许蛇线程修改数组
	for (int i = body_loc.size() - 1; i > 0; --i) {
		putAlpha(body_loc[i].x, body_loc[i].y, *body, *mask);
	}
	// 绘制蛇头
	// 没有吃食物时
	if (play_big_head_times == 0) {
		if (moving_direction == 1)  // 上
			putAlpha(body_loc[0].x, body_loc[0].y, *head_up_close, *mask);
		else if (moving_direction == 2)  // 下
			putAlpha(body_loc[0].x, body_loc[0].y, *head_down_close, *mask);
		else if (moving_direction == 3)  // 左
			putAlpha(body_loc[0].x, body_loc[0].y, *head_left_close, *mask);
		else if (moving_direction == 4)  // 右
			putAlpha(body_loc[0].x, body_loc[0].y, *head_right_close, *mask);
	}
	// 吃到食物时
	else {
		if (moving_direction == 1)  // 上
			putAlpha(body_loc[0].x - 15, body_loc[0].y - 15, *head_up_open,
				*head_open_mask);
		else if (moving_direction == 2)  // 下
			putAlpha(body_loc[0].x - 15, body_loc[0].y - 15, *head_down_open,
				*head_open_mask);
		else if (moving_direction == 3)  // 左
			putAlpha(body_loc[0].x - 15, body_loc[0].y - 15, *head_left_open,
				*head_open_mask);
		else if (moving_direction == 4)  // 右
			putAlpha(body_loc[0].x - 15, body_loc[0].y - 15, *head_right_open,
				*head_open_mask);
	}
	snake_mtx.unlock();
}
void Snake::decreaseBigHeadTimes() {
	if (play_big_head_times != 0) --play_big_head_times;
}
void Snake::moveControl(int input_direction) {
	// 无输入或非设定按键输入
	if (input_direction == 0) return;
	// 如果输入反方向，跳出函数
	if ((input_direction == 1 && moving_direction == 2) ||
		(input_direction == 2 && moving_direction == 1) ||
		(input_direction == 3 && moving_direction == 4) ||
		(input_direction == 4 && moving_direction == 3))
		return;
	// 如果输入垂直方向或相同方向则处理运动
	else {
		moving_direction = input_direction;
		snake_mtx.lock();  // 蛇线程修改坐标时不允许主线程绘图
		// 更新蛇身坐标
		for (int i = body_loc.size() - 1; i > 0; --i) {
			body_loc[i].x = body_loc[i - 1].x;
			body_loc[i].y = body_loc[i - 1].y;
		}
		// 更新蛇头
		if (moving_direction == 1)
			body_loc[0].y -= speed;
		else if (moving_direction == 2)
			body_loc[0].y += speed;
		else if (moving_direction == 3)
			body_loc[0].x -= speed;
		else if (moving_direction == 4)
			body_loc[0].x += speed;
		snake_mtx.unlock();
	}
}
int Snake::moveCheck() {
	if (GetKeyState(0x57) & 0x8000 && id == 1)  // w键按下
		return 1;
	if (GetKeyState(0x53) & 0x8000 && id == 1)  // s键按下
		return 2;
	if (GetKeyState(0x41) & 0x8000 && id == 1)  // a键按下
		return 3;
	if (GetKeyState(0x44) & 0x8000 && id == 1)  // d键按下
		return 4;
	if (GetKeyState(VK_UP) & 0x8000 && id == 2)  // 上键按下
		return 1;
	if (GetKeyState(VK_DOWN) & 0x8000 && id == 2)  // 下键按下
		return 2;
	if (GetKeyState(VK_LEFT) & 0x8000 && id == 2)  // 左键按下
		return 3;
	if (GetKeyState(VK_RIGHT) & 0x8000 && id == 2)  // 右键按下
		return 4;
	return 0;  // 无关按键或松开按键
}
bool Snake::hitSelfCheck() {
	int head_center_x = body_loc[0].x + 25;
	int head_center_y = body_loc[0].y + 25;
	int body_center_x;
	int body_center_y;
	int distance_x;
	int distance_y;
	snake_mtx.lock();  // 对自身进行检查时不允许修改数组
	for (int i = 1; i < body_loc.size(); ++i) {
		body_center_x = body_loc[i].x + 25;
		body_center_y = body_loc[i].y + 25;
		distance_x = head_center_x - body_center_x;
		distance_y = head_center_y - body_center_y;
		if (distance_x * distance_x + distance_y * distance_y <=
			600)  // 625是pic_size一半的平方
		{
			snake_mtx.unlock();
			return true;
		}
	}
	snake_mtx.unlock();
	return false;
}
bool Snake::hitWallCheck() const {
	return (body_loc[0].x <= 0 || body_loc[0].x >= map_width - 50 ||
		body_loc[0].y <= 0 || body_loc[0].y >= map_height - 50);
}
bool Snake::eatFood(Food& food) {
	bool if_eat = false;
	// 吃到食物后删除食物
	int head_center_x = body_loc[0].x + 25;
	int head_center_y = body_loc[0].y + 25;
	int food_center_x;
	int food_center_y;
	int distance_x;
	int distance_y;
	for (int i = 0; i < food.foods_loc.size(); ++i) {
		food_center_x = food.foods_loc[i].x + 25;
		food_center_y = food.foods_loc[i].y + 25;
		distance_x = head_center_x - food_center_x;
		distance_y = head_center_y - food_center_y;
		if (distance_x * distance_x + distance_y * distance_y <=
			1250)  // 1250是pic_size对角线一半的平方
		{
			if_eat = true;
			food.food_mtx
				.lock();  // 蛇线程删除被吃掉的食物不允许主线程中的食物方法修改数组大小
			food.foods_loc.erase(food.foods_loc.begin() + i);
			food.foods_remained_times.erase(food.foods_remained_times.begin() + i);
			food.food_mtx.unlock();
			break;
		}
	}
	// 增加蛇身
	if (if_eat) {
		play_big_head_times = 6;  // 将大蛇头播放6个循环
		// 蛇尾增加长度
		int new_x, new_y;
		int last_node_x = body_loc[body_loc.size() - 1].x;
		int last_node_y = body_loc[body_loc.size() - 1].y;
		int penultimate_node_x = body_loc[body_loc.size() - 2].x;
		int penultimate_node_y = body_loc[body_loc.size() - 2].y;
		if (last_node_x == penultimate_node_x) {
			new_x = last_node_x;
			if (last_node_y < penultimate_node_y)
				new_y = last_node_y - 25;
			else
				new_y = last_node_y + 25;
		}
		else if (last_node_y == penultimate_node_y) {
			new_y = last_node_y;
			if (last_node_x < penultimate_node_x)
				new_x = last_node_x - 25;
			else
				new_x = last_node_x + 25;
		}
		snake_mtx.lock();  // 蛇线程增加蛇身时不允许主线程绘图
		body_loc.emplace_back(new_x, new_y);
		snake_mtx.unlock();
		return true;
	}
	else
		return false;
}
bool Snake::eatTail(Snake& another_snake) {
	if (another_snake.getBodyLen() == 1)  // 不允许吃蛇头
		return false;

	bool if_eat = false;
	// 吃到蛇尾后删除另一条蛇尾
	int head_center_x = body_loc[0].x + 25;
	int head_center_y = body_loc[0].y + 25;
	int tail_center_x =
		another_snake.body_loc[another_snake.body_loc.size() - 1].x + 25;
	int tail_center_y =
		another_snake.body_loc[another_snake.body_loc.size() - 1].y + 25;
	if (head_center_x == tail_center_x &&
		head_center_y == tail_center_y)  // 蛇头吃蛇尾必须完全重合
	{
		if_eat = true;
		another_snake.snake_mtx
			.lock();  // 吃蛇尾时不允许被吃的蛇自身增长，不允许主线程绘图
		another_snake.body_loc.pop_back();
		another_snake.snake_mtx.unlock();
	}

	// 增加蛇身
	if (if_eat) {
		play_big_head_times = 6;
		// 蛇尾增加长度
		int new_x, new_y;
		int last_node_x = body_loc[body_loc.size() - 1].x;
		int last_node_y = body_loc[body_loc.size() - 1].y;
		int penultimate_node_x = body_loc[body_loc.size() - 2].x;
		int penultimate_node_y = body_loc[body_loc.size() - 2].y;
		if (last_node_x == penultimate_node_x) {
			new_x = last_node_x;
			if (last_node_y < penultimate_node_y)
				new_y = last_node_y - 25;
			else
				new_y = last_node_y + 25;
		}
		else if (last_node_y == penultimate_node_y) {
			new_y = last_node_y;
			if (last_node_x < penultimate_node_x)
				new_x = last_node_x - 25;
			else
				new_x = last_node_x + 25;
		}
		snake_mtx.lock();  // 蛇线程增加蛇身时不允许主线程绘图
		body_loc.emplace_back(new_x, new_y);
		snake_mtx.unlock();
		return true;
	}
	else
		return false;
}
int Snake::getBodyLen() {
	int len = 0;
	snake_mtx.lock();
	len = body_loc.size();
	snake_mtx.unlock();
	return len;
}

// ===
// === 食物
// ===
Food::Food(const ImageRes& imgs, int food_times)
	: food_lived_times(food_times) {
	normal_food = &imgs.getFood();
	blink_food = &imgs.getBlinkFood();
	mask = &imgs.getMask();
	// 一开始添加1个food
	while (true) {
		bool addable = true;
		int rand_x = std::rand() % 850;  // (map_width - pic_size)
		int rand_y = std::rand() % 850;  // (map_height - pic_size)
		int head_x = 200;                // 构造蛇时蛇1头的横纵坐标
		int head_y = 200;
		int distance_x = rand_x - head_x;
		int distance_y = rand_y - head_y;
		if (distance_x * distance_x + distance_y * distance_y <
			5000)  // 5000 对角线平方
		{
			addable = false;
		}
		head_x = 700;  // 构造蛇时蛇2头的横纵坐标
		head_y = 200;
		distance_x = rand_x - head_x;
		distance_y = rand_y - head_y;
		if (distance_x * distance_x + distance_y * distance_y <
			5000)  // 5000 对角线平方
		{
			addable = false;
		}
		for (const Location& loc : foods_loc)  // food不与已有food重合
		{
			if (rand_x == loc.x && rand_y == loc.y) {
				addable = false;
				break;
			}
		}
		if (addable) {
			food_mtx.lock();  // 构造第一个食物时不允许蛇线程吃食物
			foods_loc.emplace_back(rand_x, rand_y);
			foods_remained_times.emplace_back(food_lived_times);
			food_mtx.unlock();
			break;
		}
	}
}
void Food::addFood(const Snake& snake1, const Snake& snake2) {
	// 在距两个蛇头一定距离之外产生新的食物
	while (true) {
		bool addable = true;
		int rand_x = std::rand() % 850;  // (map_width - pic_size)
		int rand_y = std::rand() % 850;  // (map_height - pic_size)
		int head_x = snake1.body_loc[0].x;
		int head_y = snake1.body_loc[0].y;
		int distance_x = rand_x - head_x;
		int distance_y = rand_y - head_y;
		if (distance_x * distance_x + distance_y * distance_y <
			5000)  // 5000 对角线平方
		{
			addable = false;
		}
		head_x = snake2.body_loc[0].x;
		head_y = snake2.body_loc[0].y;
		distance_x = rand_x - head_x;
		distance_y = rand_y - head_y;
		if (distance_x * distance_x + distance_y * distance_y <
			5000)  // 5000 对角线平方
		{
			addable = false;
		}
		for (const Location& loc : foods_loc)  // food不与已有food重合
		{
			if (rand_x == loc.x && rand_y == loc.y) {
				addable = false;
				break;
			}
		}
		if (addable) {
			food_mtx.lock();  // 构造第一个食物时不允许蛇线程吃食物
			foods_loc.emplace_back(rand_x, rand_y);
			foods_remained_times.emplace_back(food_lived_times);
			food_mtx.unlock();
			break;
		}
	}
}
void Food::putFood() {
	// 还剩60次循环时闪烁
	food_mtx.lock();  // 绘制食物时不允许蛇线程吃食物
	for (int i = 0; i < foods_loc.size(); ++i) {
		if (foods_remained_times[i] > 60)
			putAlpha(foods_loc[i].x, foods_loc[i].y, *normal_food, *mask);
		else if (foods_remained_times[i] > 50)
			putAlpha(foods_loc[i].x, foods_loc[i].y, *blink_food, *mask);
		else if (foods_remained_times[i] > 40)
			putAlpha(foods_loc[i].x, foods_loc[i].y, *normal_food, *mask);
		else if (foods_remained_times[i] > 30)
			putAlpha(foods_loc[i].x, foods_loc[i].y, *blink_food, *mask);
		else if (foods_remained_times[i] > 20)
			putAlpha(foods_loc[i].x, foods_loc[i].y, *normal_food, *mask);
		else if (foods_remained_times[i] > 10)
			putAlpha(foods_loc[i].x, foods_loc[i].y, *blink_food, *mask);
		else
			putAlpha(foods_loc[i].x, foods_loc[i].y, *normal_food, *mask);
	}
	food_mtx.unlock();
}
void Food::freshFoodTime_and_Remove() {
	for (int i = 0; i < foods_remained_times.size(); ++i) {
		if (foods_remained_times[i] == 0) {
			food_mtx.lock();  // 主线程清理食物时不允许蛇线程吃食物
			foods_loc.erase(foods_loc.begin() + i);
			foods_remained_times.erase(foods_remained_times.begin() + i);
			food_mtx.unlock();
			break;  // 每次检查只删除一个食物
		}
	}
	for (int& x : foods_remained_times) {
		food_mtx.lock();
		--x;
		food_mtx.unlock();
	}
}
