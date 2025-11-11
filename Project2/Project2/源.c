
#include <stdio.h>
#include "raylib.h"
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define MAX_POPUP 100

Font gFont;

Font loadChineseFont();

/** 定义页面 */
enum PageId
{
	Start,
	Home,
};

/** 定义弹出窗 */
typedef struct Popup {
	Rectangle rect;			/*!矩形*/
	const char* title;		/*!标题*/
	const char* content;	/*!内容*/
	Color color;			/*!颜色*/
	bool active;			/*!是否激活*/
}Popup;

enum PageId pageId = Start;
Texture2D heartImg;

const char* contents[] = { "醴陵一中我们爱你","想念你了，萝莉一中","飞翔哥！！！","在遵道楼打牌被抓",
"明德至善，博通仰高"," 想吃醴陵炒粉 ","好怀念大家一起奋斗的场景","仙山蕴秀，渌水流长","咚咚咚，京腻要跑操哦" };

const Color colors[] = {
	{249, 251, 231,255},
	{240, 235, 248,255},
	{231, 247, 244,255},
	{252, 228, 236,255},
	{255, 240, 229,255},
};

bool drawDialog(Rectangle rect, const char* title, const char* content)
{
	//绘制标题栏 53
	DrawRectangle(rect.x, rect.y, rect.width, 53, CLITERAL(Color){243, 243, 243, 255});
	//绘制一条线
	DrawLine(rect.x, rect.y + 52, rect.x + rect.width, rect.y + 52, CLITERAL(Color){237, 237, 237, 255});
	DrawLine(rect.x, rect.y + 53, rect.x + rect.width, rect.y + 53, CLITERAL(Color){237, 237, 237, 255});
	//绘制窗口
	DrawRectangle(rect.x, rect.y + 53, rect.width, rect.height - 53, WHITE);

	//绘制按钮
	Rectangle btnRect = {
		rect.x + rect.width - 16 - 57,
		rect.y + rect.height - 20 - 36,
		57,36
	};
	Color color = { 0, 120, 212, 255 };
	//判断鼠标是否再按钮上
	if (CheckCollisionPointRec(GetMousePosition(), btnRect)) {
		color = CLITERAL(Color) { 0, 126, 223, 255 };
		//是否点击了按钮
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			return true;
		}
	}
	DrawRectangleRec(btnRect, color);
	//按钮文本
	DrawTextEx(gFont, "确定", CLITERAL(Vector2){btnRect.x + 13, btnRect.y + 10}, 16, 0, WHITE);

	//绘制标题文本
	//DrawText("I Love You!", rect.x + 30, rect.y, 16, BLACK);
	DrawTextEx(gFont, title, CLITERAL(Vector2){rect.x + 30, rect.y + (53 - 16) / 2}, 16, 0, BLACK);
	//绘制正文
	DrawTextEx(gFont, content, CLITERAL(Vector2){rect.x + 10, rect.y + 75}, 16, 0, BLACK);
	return false;
}

void popup_init(Popup* pthis)
{
	pthis->title = "回忆语录";
	pthis->content = contents[GetRandomValue(0, sizeof(contents) / sizeof(contents[0]) - 1)];
	pthis->color = colors[GetRandomValue(0, sizeof(colors) / sizeof(colors[0]) - 1)];
	int w = 230;
	int h = 60;
	pthis->rect = CLITERAL(Rectangle) {
		GetRandomValue(9, GetRenderWidth() - w - 9),
			GetRandomValue(9, GetRenderHeight() - h - 9),
			w, h
	};
	pthis->active = false;
}

void popup_draw(Popup* pthis)
{
	if (!pthis->active)
		return;
	//绘制标题栏
	Rectangle r = pthis->rect;
	r.height = 25;
	DrawRectangleRec(r, pthis->color);
	DrawTextEx(gFont, pthis->title, CLITERAL(Vector2){r.x + 40, r.y + 6}, 13, 0, BLACK);

	//绘制正文栏
	r.y += 25;
	r.height = pthis->rect.height - r.height;
	DrawRectangleRec(r, ColorBrightness(pthis->color, 0.5));
	int tx = pthis->rect.x + (pthis->rect.width - MeasureText(pthis->content, 16)) / 2;
	int ty = r.y + (r.height - 16) / 2;
	DrawTextEx(gFont, pthis->content, CLITERAL(Vector2){tx - 20, ty}, 16, 0, BLACK);

	DrawTexture(heartImg, pthis->rect.x + 3, pthis->rect.y - 3, WHITE);

	//绘制阴影
	Color color = { 199, 199, 199, 255 };
	for (int i = 0; i < 9; i++) {
		DrawRectangleRoundedLines(CLITERAL(Rectangle) { pthis->rect.x - i, pthis->rect.y - i, pthis->rect.width + i * 2, pthis->rect.height + i * 2 }
		, 0.5, 0, color);
		//修改颜色
		color.a = 255 - 255 / 9.f * i;
		color.r = color.g = color.b = 199 + (255 - 199) / 9.f * i;
	}

}

int main(int argc, char* argv[])
{
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	//初始化图形窗口
	InitWindow(1080, 740, "萝莉一中回忆录");
	//设置帧率 60 120 FPS
	SetTargetFPS(60);
	//打开音频设备
	InitAudioDevice();

	gFont = loadChineseFont();
	//加载图片
	heartImg = LoadTexture("Resource/images/xiaohui.png");
	//加载音乐
	Music mus = LoadMusicStream("Resource/music/xiaoge.mp3");
	//播放音乐
	PlayMusicStream(mus);

	//定义弹出窗数组
	Popup popups[MAX_POPUP];
	//初始化弹出窗
	for (int i = 0; i < MAX_POPUP; i++) {
		popup_init(popups + i);
	}

	//帧数
	int frameCount = 0;

	//游戏、程序有主循环
	while (!WindowShouldClose())
	{
		BeginDrawing();	//开始绘图
		if (pageId == Start) {
			ClearBackground(CLITERAL(Color) { 141, 144, 148, 255 }); //用指定的颜色来清屏

			Rectangle rect = { 0,0,520,154 };
			rect.x = (GetRenderWidth() - rect.width) / 2;
			rect.y = (GetRenderHeight() - rect.height) / 2;
			if (drawDialog(rect, "醴陵一中回忆录", "你将回溯到高中生活，确定要继续吗？")) {
				//TraceLog(LOG_INFO, "button clicked");
				pageId = Home;
			}
		}
		else {
			ClearBackground(CLITERAL(Color) { 236, 240, 246, 255 }); //用指定的颜色来清屏
			//绘制弹出窗
			for (int i = 0; i < MAX_POPUP; i++) {
				popup_draw(popups + i);
			}
			//一段时间出现一个弹出窗
			if (frameCount % 20 == 0) {
				for (int i = 0; i < MAX_POPUP; i++) {
					if (popups[i].active == false) {
						popups[i].active = true;
						break;
					}
				}
			}
			//更新音乐流
			UpdateMusicStream(mus);
		}

		EndDrawing();	//结束绘图
		frameCount++;
	}

	UnloadMusicStream(mus);
	UnloadTexture(heartImg);
	//关闭音频设备
	CloseAudioDevice();
	//关闭窗口
	CloseWindow();
	return 0;
}


Font loadChineseFont()
{

	//打开字体
	int fileSize = 0;
	unsigned char* fontData = LoadFileData("C:/Windows/Fonts/simhei.ttf", &fileSize);
	if (!fontData) {
		TraceLog(LOG_WARNING, "Failed to load font file");
		return CLITERAL(Font) { 0 };
	}

	//加载中文文件
	int chineseSize = 0;
	unsigned char* chineseData = LoadFileData("chinese.txt", &chineseSize);
	if (!chineseData) {
		TraceLog(LOG_WARNING, "Failed to load chinese file");
		return CLITERAL(Font) { 0 };
	}

	//加载马点
	int codepointsCount = chineseSize;
	int* codepoints = LoadCodepoints(chineseData, &codepointsCount);
	if (!codepoints) {
		TraceLog(LOG_WARNING, "Failed to load codepoints");
		return CLITERAL(Font) { 0 };
	}

	//从内存加载字体
	Font font = LoadFontFromMemory(".ttf", fontData, fileSize, 26, codepoints, codepointsCount);
	if (!IsFontValid(font)) {
		TraceLog(LOG_WARNING, "Failed to load font from memory");
		return CLITERAL(Font) { 0 };
	}
	return font;
}
