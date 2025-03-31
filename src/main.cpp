#include "raylib.h"
#include <iostream>

#pragma region imgui
#include "imgui.h"
#include "rlImGui.h"
#include "imguiThemes.h"
#pragma endregion

const int WINDOWWIDTH = 1280;
const int WINDOWHEIGHT = 720;


struct Ball
{
	Vector2 ballPos;
	int speed_x, speed_y;
	int radius;

	void Draw();
	void Update(float deltaTime);
};


void Ball::Draw()
{
	DrawCircle(ballPos.x, ballPos.y, radius, WHITE);
}

void Ball::Update(float deltaTIme)
{
	ballPos.x += speed_x * deltaTIme;
	ballPos.y += speed_y * deltaTIme;


	//border collisions
	if ((ballPos.y + radius)>= WINDOWHEIGHT || (ballPos.y - radius) <= 0)
	{
		speed_y *= -1;
	}
	if ((ballPos.x + radius) >= WINDOWWIDTH || (ballPos.x - radius) <= 0)
	{
		speed_x *= -1;
	}

}

struct Paddle
{
	Vector2 paddlePos;
	int paddleWidth;
	int paddleHeight;
	int paddleSpeed;

	void Draw();
	void Update(float deltaTime);
	void CpuUpdate(float deltaTime, Ball& ball);
};

void Paddle::Draw()
{
	DrawRectangle(paddlePos.x, paddlePos.y, paddleWidth, paddleHeight, WHITE);
}

void Paddle::Update(float deltaTime)
{
	
	if (IsKeyDown(KEY_UP) && paddlePos.y >= 10)
	{
		paddlePos.y = paddlePos.y - paddleSpeed * deltaTime;
	}
	if (IsKeyDown(KEY_DOWN) && paddlePos.y <= (WINDOWHEIGHT - paddleHeight) - 10)
	{
		paddlePos.y = paddlePos.y + paddleSpeed * deltaTime;
	}
}

void Paddle::CpuUpdate(float deltaTime, Ball& ball)
{
	if (paddlePos.y + paddleHeight / 2 > ball.ballPos.y)
	{
		paddlePos.y = paddlePos.y - paddleSpeed * deltaTime;
	}
	if (paddlePos.y + paddleHeight / 2 < ball.ballPos.y)
	{
		paddlePos.y = paddlePos.y + paddleSpeed * deltaTime;
	}

	if (paddlePos.y <= 0)
	{
		paddlePos.y = 0;
	}
	if (paddlePos.y + paddleHeight >= WINDOWHEIGHT)
	{
		paddlePos.y = WINDOWHEIGHT - paddleHeight;
	}

}


int main(void)
{

	//SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(WINDOWWIDTH, WINDOWHEIGHT, "raylib [core] example - basic window");
	SetTargetFPS(60);

	int playerScore = 0;
	int cpuScore = 0;

	Ball ball{};
	ball.ballPos = {
		WINDOWWIDTH / 2.f,
		WINDOWHEIGHT / 2.f
	};
	ball.speed_x = 500;
	ball.speed_y = 500;
	ball.radius = 15;

	Paddle player{};
	player.paddlePos = {
		10,
		WINDOWHEIGHT / 2.f - 50
	};
	player.paddleWidth = 15;
	player.paddleHeight = 100;
	player.paddleSpeed = 700;

	Paddle cpu{};
	cpu.paddlePos = {
		WINDOWWIDTH - 25,
		WINDOWHEIGHT / 2.f - 50
	};
	cpu.paddleWidth = 15;
	cpu.paddleHeight = 100;
	cpu.paddleSpeed = 700;
#pragma region imgui
	rlImGuiSetup(true);

	//you can use whatever imgui theme you like!
	//ImGui::StyleColorsDark();
	//imguiThemes::yellow();
	//imguiThemes::gray();
	imguiThemes::green();
	//imguiThemes::red();
	//imguiThemes::embraceTheDarkness();


	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.FontGlobalScale = 2;

	ImGuiStyle &style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		//style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.5f;
		//style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;
	}

#pragma endregion


	
	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();

#pragma region Update

		ball.Update(deltaTime);
		player.Update(deltaTime);
		cpu.CpuUpdate(deltaTime, ball);



		bool playerCollided = CheckCollisionCircleRec(
			ball.ballPos,
			ball.radius,
			{
				player.paddlePos.x,
				player.paddlePos.y,
				(float)player.paddleWidth,
				(float)player.paddleHeight
			}
		);


		if (playerCollided)
		{
			ball.speed_x *= -1;
		}


		bool cpuCollided = CheckCollisionCircleRec(
			ball.ballPos,
			ball.radius,
			{
				cpu.paddlePos.x,
				cpu.paddlePos.y,
				(float)cpu.paddleWidth,
				(float)cpu.paddleHeight
			}
		);


		if (cpuCollided)
		{
			ball.speed_x *= -1;
		}

		if ((ball.ballPos.x + ball.radius) >= WINDOWWIDTH)
		{
			playerScore++;
		}
			
		if ((ball.ballPos.x - ball.radius) <= 0)
		{
			cpuScore++;
		}

#pragma endregion


#pragma region Drawing

		{
			BeginDrawing();
			ClearBackground(BLACK);


#pragma region imgui
			rlImGuiBegin();

			ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
			ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
			ImGui::PopStyleColor(2);
#pragma endregion
			

			DrawLine(WINDOWWIDTH / 2.f, 0, WINDOWWIDTH / 2.f, WINDOWHEIGHT, GREEN);
			//DrawRectangle(10, WINDOWHEIGHT/2.f - 50, 15, 100, WHITE);
			player.Draw();
			//DrawRectangle(WINDOWWIDTH - 25, WINDOWHEIGHT/2.f - 50, 15, 100, WHITE);
			cpu.Draw();
			ball.Draw();
			DrawText(TextFormat("%i", playerScore), WINDOWWIDTH / 4.f, 20, 100, WHITE);
			DrawText(TextFormat("%i", cpuScore), (WINDOWWIDTH - (WINDOWWIDTH / 4.f)), 20, 100, WHITE);
			


#pragma region imgui
			rlImGuiEnd();

			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
#pragma endregion

			EndDrawing();
		}

#pragma endregion

	}


#pragma region imgui
	rlImGuiShutdown();
#pragma endregion



	CloseWindow();

	return 0;
}