/*
��־��
1. �ִ����⣺�����߲���ͬʱ��
2. ��������߲���ͬʱ�������⣺����peekmessage ��getkeystate
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
#pragma comment(lib, "winmm.lib")  // ���ļ�

// ����
const int map_width = 900;
const int map_height = 900;
const int window_width = 1200;
const int window_height = 900;

// ===
// === ͼƬ��Դ
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
	// ����ͼ
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
// === ����
// ===
struct Location {
	int x;
	int y;
	Location(int loc_x, int loc_y) : x(loc_x), y(loc_y) {};
};

class Snake;
// ===
// === ʳ��
// ===
class Food {
private:
	friend class Snake;
	std::vector<Location> foods_loc;  // ÿ��ʳ���λ��
	std::vector<int>
		foods_remained_times;  // ÿ��ʳ��ʣ���ʱ��,ÿ��ʳ�����һ��������ѭ��������һ��������ʼ��˸����������food_lived_times��ѭ��
	const IMAGE* normal_food;
	const IMAGE* blink_food;
	const IMAGE* mask;
	std::mutex food_mtx;
	int food_lived_times;

public:
	Food(const ImageRes& imgs, int food_times);
	void addFood(const Snake& snake1, const Snake& snake2);
	void putFood();
	void freshFoodTime_and_Remove();  // ˢ��ʳ��ʣ��ʱ�䲢ɾ����ʱ��ʳ��
};

// ===
// === ��
// ===
class Snake {
private:
	friend class Food;

private:
	std::vector<Location> body_loc;  // ����ÿһ���ֵ����꣨ͼƬ���Ͻǣ�
	int speed = 25;                  // �ƶ��ٶȣ����ص㣩
	int moving_direction;            // 1��2��3��4��
	int play_big_head_times = 0;  // ��ʳ��ʱ��ͷ����ѭ������
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
	std::mutex snake_mtx;  // �����鹲������� �����߳������߳�֮�䣩
public:
	Snake(int snake_id, const ImageRes& imgs);
	void putSnake();  // ������
	void decreaseBigHeadTimes();
	void moveControl(int input_direction);  // �ƶ�����
	int moveCheck();
	bool hitSelfCheck();         // ������ײ���
	bool hitWallCheck() const;   // ײǽ���
	bool eatFood(Food& food);    // �Ƿ�Ե�ʳ��
	bool eatTail(Snake& snake);  // �Ե���һ���ߵ�β��
	int getBodyLen();            // ��ȡ������
	int getId() const { return id; }
};

// ===
// === Tools
// ===
// ��ͼ
void putAlpha(int picture_x, int picture_y, const IMAGE& pic,
	const IMAGE& mask);
// ����������
void setTextStyle();
// ��ʾ�÷�
void showScore(int id, int score);
// ˢ����Ļ
void freshScreen(const ImageRes& imgs, Snake& snake1, Snake& snake2,
	Food& food);
// ��ȡʱ��
long long getTime();
// �̵߳��ú���
void snakeControl(Snake& snake, Snake& another_snake, Food& food,
	bool& game_over, const int& loop_sleep_time);
// �˵�
void freshMenu(const ImageRes& imgs, const int arrow_y_loc);
int menu(const ImageRes& imgs);  // ������Ϸ�Ѷ�1��2��3 , 4 �˳�

int main() {
	std::srand(std::time(nullptr));
	initgraph(window_width, window_height);
	setTextStyle();
	// ������
	mciSendString(_T("open res//bg_music.mp3"), 0, 0, 0);
	mciSendString(_T("open res//eat.mp3"), 0, 0, 0);
	// ��������
	mciSendString(_T("play res//bg_music.mp3 repeat"), 0, 0, 0);
	// ��Ϸ��ѭ��
	while (true) {
		int food_times = 150;      // ʳ��ѭ��ѭ������
		int loop_sleep_time = 40;  // ѭ������ʱ�䣬ms
		int new_food_time = 3;
		ImageRes imgs;  // ����ͼƬ��Դ
		// �˵�
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

		// ��Ϸ������
		while (true) {
			freshScreen(imgs, snake1, snake2, food);
			// === ����ʳ��
			food.freshFoodTime_and_Remove();
			// ÿnew_food_time���һ����ʳ��
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
				while (true)  // ��ס���棬�ȴ�ESC��������
				{
					if (GetKeyState(VK_ESCAPE) & 0x8000) break;
				}
				break;  // �˳���Ϸ���浽���˵�
			}
			// ����cpuռ�ã������ƶ��ٶ�
			std::this_thread::sleep_for(std::chrono::milliseconds(loop_sleep_time));
		}
	}

	closegraph();
	// �ر�����
	mciSendString(_T("close res//bg_music.mp3"), 0, 0, 0);
	mciSendString(_T("close res//eat.mp3"), 0, 0, 0);
	return 0;
}

// ===
// === ���ߺ���
// ===
// ��ͼ
void putAlpha(int picture_x, int picture_y, const IMAGE& pic,
	const IMAGE& mask) {
	putimage(picture_x, picture_y, &mask, SRCPAINT);
	putimage(picture_x, picture_y, &pic, SRCAND);
}
// ��ȡ��ǰʱ��
long long getTime() {
	// ��ȡ��ǰʱ���
	const auto t = std::chrono::system_clock::now();
	// ����ʱ���Ӹ������Ρ��������𾭹���ʱ��ת��������Ϊ��λ��������
	return std::chrono::duration_cast<std::chrono::seconds>(t.time_since_epoch())
		.count();
}
// ˢ����Ļ
void freshScreen(const ImageRes& imgs, Snake& snake1, Snake& snake2,
	Food& food) {
	BeginBatchDraw();  // ��ͼ������������������ֹ����
	cleardevice();
	putimage(0, 0, &imgs.getBg());
	food.putFood();
	snake1.putSnake();
	showScore(snake1.getId(), snake1.getBodyLen() - 4);
	snake2.putSnake();
	showScore(snake2.getId(), snake2.getBodyLen() - 4);
	EndBatchDraw();
}
// �̵߳��ú���
void snakeControl(Snake& snake, Snake& another_snake, Food& food,
	bool& game_over, const int& loop_sleep_time) {
	while (true) {
		// ��鰴����Ϣ,�������ƶ�
		snake.moveControl(snake.moveCheck());

		// ��ײ���
		if (snake.hitWallCheck() || snake.hitSelfCheck() || game_over ||
			snake.getBodyLen() == 1) {
			game_over = true;
			break;
		}
		if (snake.eatFood(food) || snake.eatTail(another_snake))
			mciSendString(_T("play res//eat.mp3"), 0, 0, 0);
		// ����ͷ���Ŵ����ݼ�
		snake.decreaseBigHeadTimes();

		std::this_thread::sleep_for(std::chrono::milliseconds(loop_sleep_time));
	}
}
// �˵�
void freshMenu(const ImageRes& imgs, const int arrow_y_loc) {
	cleardevice();
	putimage(0, 0, &imgs.getBg());
	putimage(150, 50, &imgs.getMenu());
	putimage(150, arrow_y_loc, &imgs.getArrow());
}
int menu(const ImageRes& imgs)  // ������Ϸ�Ѷ�1��2��3 , 4 �˳�
{
	int choice = 1;
	int arrow_y_loc = 120;
	freshMenu(imgs, arrow_y_loc);
	while (true) {
		if (GetKeyState(VK_DOWN) & 0x8000 ||
			GetKeyState(0x53) & 0x8000)  // �����s���¼�
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
			GetKeyState(0x57) & 0x8000)  // ���»س���
		{
			return choice;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
// ����������
void setTextStyle() {
	settextstyle(30, 0, _T("����"));  // ��������
	settextcolor(BLACK);              // ������ɫ
	setbkmode(TRANSPARENT);           // ���ñ���ɫΪ͸��
}
// ��ʾ�÷�
void showScore(int id, int score) {
	TCHAR s1[10];
	_stprintf(s1, _T("%d�ŵ÷�"), id);
	TCHAR s2[10];
	_stprintf(s2, _T("%d ��"), score);
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
// === ͼƬ��Դ
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
// === ��
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
	// ������ƣ�ͼ�㸲��
	// ��������
	snake_mtx.lock();  // ���̻߳�ͼʱ���������߳��޸�����
	for (int i = body_loc.size() - 1; i > 0; --i) {
		putAlpha(body_loc[i].x, body_loc[i].y, *body, *mask);
	}
	// ������ͷ
	// û�г�ʳ��ʱ
	if (play_big_head_times == 0) {
		if (moving_direction == 1)  // ��
			putAlpha(body_loc[0].x, body_loc[0].y, *head_up_close, *mask);
		else if (moving_direction == 2)  // ��
			putAlpha(body_loc[0].x, body_loc[0].y, *head_down_close, *mask);
		else if (moving_direction == 3)  // ��
			putAlpha(body_loc[0].x, body_loc[0].y, *head_left_close, *mask);
		else if (moving_direction == 4)  // ��
			putAlpha(body_loc[0].x, body_loc[0].y, *head_right_close, *mask);
	}
	// �Ե�ʳ��ʱ
	else {
		if (moving_direction == 1)  // ��
			putAlpha(body_loc[0].x - 15, body_loc[0].y - 15, *head_up_open,
				*head_open_mask);
		else if (moving_direction == 2)  // ��
			putAlpha(body_loc[0].x - 15, body_loc[0].y - 15, *head_down_open,
				*head_open_mask);
		else if (moving_direction == 3)  // ��
			putAlpha(body_loc[0].x - 15, body_loc[0].y - 15, *head_left_open,
				*head_open_mask);
		else if (moving_direction == 4)  // ��
			putAlpha(body_loc[0].x - 15, body_loc[0].y - 15, *head_right_open,
				*head_open_mask);
	}
	snake_mtx.unlock();
}
void Snake::decreaseBigHeadTimes() {
	if (play_big_head_times != 0) --play_big_head_times;
}
void Snake::moveControl(int input_direction) {
	// ���������趨��������
	if (input_direction == 0) return;
	// ������뷴������������
	if ((input_direction == 1 && moving_direction == 2) ||
		(input_direction == 2 && moving_direction == 1) ||
		(input_direction == 3 && moving_direction == 4) ||
		(input_direction == 4 && moving_direction == 3))
		return;
	// ������봹ֱ�������ͬ���������˶�
	else {
		moving_direction = input_direction;
		snake_mtx.lock();  // ���߳��޸�����ʱ���������̻߳�ͼ
		// ������������
		for (int i = body_loc.size() - 1; i > 0; --i) {
			body_loc[i].x = body_loc[i - 1].x;
			body_loc[i].y = body_loc[i - 1].y;
		}
		// ������ͷ
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
	if (GetKeyState(0x57) & 0x8000 && id == 1)  // w������
		return 1;
	if (GetKeyState(0x53) & 0x8000 && id == 1)  // s������
		return 2;
	if (GetKeyState(0x41) & 0x8000 && id == 1)  // a������
		return 3;
	if (GetKeyState(0x44) & 0x8000 && id == 1)  // d������
		return 4;
	if (GetKeyState(VK_UP) & 0x8000 && id == 2)  // �ϼ�����
		return 1;
	if (GetKeyState(VK_DOWN) & 0x8000 && id == 2)  // �¼�����
		return 2;
	if (GetKeyState(VK_LEFT) & 0x8000 && id == 2)  // �������
		return 3;
	if (GetKeyState(VK_RIGHT) & 0x8000 && id == 2)  // �Ҽ�����
		return 4;
	return 0;  // �޹ذ������ɿ�����
}
bool Snake::hitSelfCheck() {
	int head_center_x = body_loc[0].x + 25;
	int head_center_y = body_loc[0].y + 25;
	int body_center_x;
	int body_center_y;
	int distance_x;
	int distance_y;
	snake_mtx.lock();  // ��������м��ʱ�������޸�����
	for (int i = 1; i < body_loc.size(); ++i) {
		body_center_x = body_loc[i].x + 25;
		body_center_y = body_loc[i].y + 25;
		distance_x = head_center_x - body_center_x;
		distance_y = head_center_y - body_center_y;
		if (distance_x * distance_x + distance_y * distance_y <=
			600)  // 625��pic_sizeһ���ƽ��
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
	// �Ե�ʳ���ɾ��ʳ��
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
			1250)  // 1250��pic_size�Խ���һ���ƽ��
		{
			if_eat = true;
			food.food_mtx
				.lock();  // ���߳�ɾ�����Ե���ʳ�ﲻ�������߳��е�ʳ�﷽���޸������С
			food.foods_loc.erase(food.foods_loc.begin() + i);
			food.foods_remained_times.erase(food.foods_remained_times.begin() + i);
			food.food_mtx.unlock();
			break;
		}
	}
	// ��������
	if (if_eat) {
		play_big_head_times = 6;  // ������ͷ����6��ѭ��
		// ��β���ӳ���
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
		snake_mtx.lock();  // ���߳���������ʱ���������̻߳�ͼ
		body_loc.emplace_back(new_x, new_y);
		snake_mtx.unlock();
		return true;
	}
	else
		return false;
}
bool Snake::eatTail(Snake& another_snake) {
	if (another_snake.getBodyLen() == 1)  // ���������ͷ
		return false;

	bool if_eat = false;
	// �Ե���β��ɾ����һ����β
	int head_center_x = body_loc[0].x + 25;
	int head_center_y = body_loc[0].y + 25;
	int tail_center_x =
		another_snake.body_loc[another_snake.body_loc.size() - 1].x + 25;
	int tail_center_y =
		another_snake.body_loc[another_snake.body_loc.size() - 1].y + 25;
	if (head_center_x == tail_center_x &&
		head_center_y == tail_center_y)  // ��ͷ����β������ȫ�غ�
	{
		if_eat = true;
		another_snake.snake_mtx
			.lock();  // ����βʱ�������Ե����������������������̻߳�ͼ
		another_snake.body_loc.pop_back();
		another_snake.snake_mtx.unlock();
	}

	// ��������
	if (if_eat) {
		play_big_head_times = 6;
		// ��β���ӳ���
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
		snake_mtx.lock();  // ���߳���������ʱ���������̻߳�ͼ
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
// === ʳ��
// ===
Food::Food(const ImageRes& imgs, int food_times)
	: food_lived_times(food_times) {
	normal_food = &imgs.getFood();
	blink_food = &imgs.getBlinkFood();
	mask = &imgs.getMask();
	// һ��ʼ���1��food
	while (true) {
		bool addable = true;
		int rand_x = std::rand() % 850;  // (map_width - pic_size)
		int rand_y = std::rand() % 850;  // (map_height - pic_size)
		int head_x = 200;                // ������ʱ��1ͷ�ĺ�������
		int head_y = 200;
		int distance_x = rand_x - head_x;
		int distance_y = rand_y - head_y;
		if (distance_x * distance_x + distance_y * distance_y <
			5000)  // 5000 �Խ���ƽ��
		{
			addable = false;
		}
		head_x = 700;  // ������ʱ��2ͷ�ĺ�������
		head_y = 200;
		distance_x = rand_x - head_x;
		distance_y = rand_y - head_y;
		if (distance_x * distance_x + distance_y * distance_y <
			5000)  // 5000 �Խ���ƽ��
		{
			addable = false;
		}
		for (const Location& loc : foods_loc)  // food��������food�غ�
		{
			if (rand_x == loc.x && rand_y == loc.y) {
				addable = false;
				break;
			}
		}
		if (addable) {
			food_mtx.lock();  // �����һ��ʳ��ʱ���������̳߳�ʳ��
			foods_loc.emplace_back(rand_x, rand_y);
			foods_remained_times.emplace_back(food_lived_times);
			food_mtx.unlock();
			break;
		}
	}
}
void Food::addFood(const Snake& snake1, const Snake& snake2) {
	// �ھ�������ͷһ������֮������µ�ʳ��
	while (true) {
		bool addable = true;
		int rand_x = std::rand() % 850;  // (map_width - pic_size)
		int rand_y = std::rand() % 850;  // (map_height - pic_size)
		int head_x = snake1.body_loc[0].x;
		int head_y = snake1.body_loc[0].y;
		int distance_x = rand_x - head_x;
		int distance_y = rand_y - head_y;
		if (distance_x * distance_x + distance_y * distance_y <
			5000)  // 5000 �Խ���ƽ��
		{
			addable = false;
		}
		head_x = snake2.body_loc[0].x;
		head_y = snake2.body_loc[0].y;
		distance_x = rand_x - head_x;
		distance_y = rand_y - head_y;
		if (distance_x * distance_x + distance_y * distance_y <
			5000)  // 5000 �Խ���ƽ��
		{
			addable = false;
		}
		for (const Location& loc : foods_loc)  // food��������food�غ�
		{
			if (rand_x == loc.x && rand_y == loc.y) {
				addable = false;
				break;
			}
		}
		if (addable) {
			food_mtx.lock();  // �����һ��ʳ��ʱ���������̳߳�ʳ��
			foods_loc.emplace_back(rand_x, rand_y);
			foods_remained_times.emplace_back(food_lived_times);
			food_mtx.unlock();
			break;
		}
	}
}
void Food::putFood() {
	// ��ʣ60��ѭ��ʱ��˸
	food_mtx.lock();  // ����ʳ��ʱ���������̳߳�ʳ��
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
			food_mtx.lock();  // ���߳�����ʳ��ʱ���������̳߳�ʳ��
			foods_loc.erase(foods_loc.begin() + i);
			foods_remained_times.erase(foods_remained_times.begin() + i);
			food_mtx.unlock();
			break;  // ÿ�μ��ֻɾ��һ��ʳ��
		}
	}
	for (int& x : foods_remained_times) {
		food_mtx.lock();
		--x;
		food_mtx.unlock();
	}
}
