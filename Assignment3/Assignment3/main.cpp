
#define _CRT_SECURE_NO_WARNINGS

#include <vgl.h>
#include <InitShader.h>
#include "MyCube.h"
#include "MyUtil.h"
#include <vec.h>
#include <mat.h>
#include <vector>
#include <queue>
#define MAZE_FILE	"maze.txt"

#define PI 3.141592f

using namespace std;

MyCube cube;
GLuint program;

mat4 g_Mat = mat4(1.0f);
GLuint uMat;
GLuint uColor;

float wWidth = 1000;
float wHeight = 500;

vec3 cameraPos = vec3(0, 0, 0);
vec3 viewDirection = vec3(0, 0, -1);
vec3 goalPos = vec3(0, 0, 0);

int MazeSize;
char maze[255][255] = { 0 };

float cameraSpeed = 0.1;

float g_time = 0;

float rotationSpeed = 6.0f;

// 길 찾기
int dx[4] = { 0, 1, 0, -1 };
int dy[4] = { -1, 0, 1, 0 };
vector<pair<int, int>> path;
vector<vec3> directionPath;
int pathCnt = 0;
int dirCnt = 0;

bool isTracking = false;
float spacebarCoolTime = 1.0f;




struct Node 
{
	int x, y; // 배열 좌표
	int g, h; // g: 출발지에서 현재 노드까지의 비용, h: 현재 노드에서 목표 노드까지의 예상 비용
	Node* parent; // 부모 노드 포인터

	//f = g + h
	int getF() const { return g + h; }

	Node(int _x, int _y, int _g, int _h, Node* _parent = nullptr) 
	{
		x = _x;
		y = _y;
		g = _g;
		h = _h;
		parent = _parent;
	}

	// 우선순위 큐에서 f(n) 값이 작은 순으로 정렬하기 위해 < 연산자 오버로딩
	bool operator<(const Node& other) const 
	{
		return getF() > other.getF();
	}
};

// 맨해튼 거리로 h(n) 계산
int heuristic(int x1, int y1, int x2, int y2) 
{
	return abs(x1 - x2) + abs(y1 - y2);
}

// 경로 재구성
void createPath(Node* node) 
{
	while (node != nullptr) {
		path.push_back({ node->x, node->y });
		node = node->parent;
	}
	reverse(path.begin(), path.end());
}

// 현재 위치를 2차원 배열의 위치로 변환
pair<int, int> getArrayPosFromPosition(vec3 originPos)
{
	vec3 pos = vec3(originPos.x + MazeSize / 2.0, 0, originPos.z + MazeSize / 2.0);

	pair<int, int> arrayPos;
	arrayPos.first = pos.x;
	arrayPos.second = pos.z;

	return arrayPos;
}

inline vec3 getPositionFromIndex(int i, int j)
{
	float unit = 1;
	vec3 leftTopPosition = vec3(-MazeSize / 2.0 + unit / 2, 0, -MazeSize / 2.0 + unit / 2);
	vec3 xDir = vec3(1, 0, 0);
	vec3 zDir = vec3(0, 0, 1);
	return leftTopPosition + i * xDir + j * zDir;
}

// 빨간색으로 길을 그리고, 추후 Tracking에서 사용할 directionPath를 초기화
void DrawRoadAndSetNavi() 
{
	if (path.size() <= 1)
		return;
	
	for (int i = 0; i < path.size() - 1; i++)
	{
		vec3 start = getPositionFromIndex(path[i].first, path[i].second);
		vec3 end = getPositionFromIndex(path[i+1].first, path[i+1].second);
		

		// 방향을 나타내는 벡터, 현재 가는 방향으로 벡터 성분에 -1 or 1이 할당, 나머지는 0
		vec3 dir = end - start;

		// 경로 방향 벡터 초기화
		directionPath.push_back(dir);

		// 큐브를 아래로 내리고, 현재 진행 방향으로 원점을 절반 만큼 이동해야함
		vec3 offset = vec3(dir.x / 2, -0.5f, dir.z / 2);

		float xScaling = dir.x == 0 ? 1.0f : dir.x * 10;
		float zScaling = dir.z == 0 ? 1.0f : dir.z * 10;

		vec3 color = vec3(1,0,0);
		mat4 ModelMat = Translate(start + offset) * Scale(0.1f * xScaling, 0.1f, 0.1f * zScaling);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
		glUniform4f(uColor, color.x, color.y, color.z, 1);
		cube.Draw(program);
	}
}



// A* 알고리즘 구현
void aStar() 
{
	path.clear();
	directionPath.clear();

	pair<int, int> start = getArrayPosFromPosition(cameraPos);
	pair<int, int> goal = getArrayPosFromPosition(goalPos);
	int rowSize = MazeSize;
	int colSize = MazeSize;

	// Open List를 위한 우선순위 큐와 Closed List를 위한 2차원 배열
	priority_queue<Node> openList;
	vector<vector<bool>> closedList(rowSize, vector<bool>(colSize, false));

	// 시작 노드 초기화
	Node* startNode = new Node(start.first, start.second, 0, heuristic(start.first, start.second, goal.first, goal.second));
	openList.push(*startNode);

	while (!openList.empty()) {
		// 우선순위 큐에서 f 값이 가장 작은 노드를 꺼냄
		Node current = openList.top();
		openList.pop();

		// 목표 지점에 도달했으면 경로를 반환
		if (current.x == goal.first && current.y == goal.second) 
		{
			createPath(&current);
			return;
		}

		// 현재 노드를 Closed List에 추가
		closedList[current.x][current.y] = true;

		// 인접한 노드들을 탐색
		for (int i = 0; i < 4; i++) {
			int nx = current.x + dx[i];
			int ny = current.y + dy[i];

			// 미로의 범위를 벗어나거나 벽이거나 이미 탐색된 노드인 경우 무시
			if (nx < 0 || ny < 0 || nx >= rowSize || ny >= colSize || maze[nx][ny] == '*' || closedList[nx][ny])
				continue;

			// g(n)과 h(n) 계산
			int gCost = current.g + 1;
			int hCost = heuristic(nx, ny, goal.first, goal.second);

			// 새로운 노드 생성 및 추가
			Node* neighborNode = new Node(nx, ny, gCost, hCost, new Node(current));
			openList.push(*neighborNode);
		}
	}
	return;
}


void LoadMaze()
{
	FILE* file = fopen(MAZE_FILE, "r");
	char buf[255];
	fgets(buf, 255, file);
	sscanf(buf, "%d", &MazeSize);
	for (int j = 0; j < MazeSize; j++)
	{
		fgets(buf, 255, file);
		for (int i = 0; i < MazeSize; i++)
		{
			maze[i][j] = buf[i];
			if (maze[i][j] == 'C')				// Setup Camera Position
				cameraPos = getPositionFromIndex(i, j);
			if (maze[i][j] == 'G')				// Setup Goal Position
				goalPos = getPositionFromIndex(i, j);
		}
	}
	fclose(file);
}

void DrawMaze(bool isRightScreen)
{
	for (int j = 0; j < MazeSize; j++)
		for (int i = 0; i < MazeSize; i++)
		{
			if (maze[i][j] == '*')
			{
				vec3 color = vec3(i / (float)MazeSize, j / (float)MazeSize, 1);
				mat4 ModelMat = Translate(getPositionFromIndex(i, j));
				glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
				glUniform4f(uColor, color.x, color.y, color.z, 1);
				cube.Draw(program);
			}
			else if (maze[i][j] == 'r')
			{
				vec3 color = vec3(1,0,0);
				mat4 ModelMat = Translate(getPositionFromIndex(i, j));
				glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
				glUniform4f(uColor, color.x, color.y, color.z, 1);
				cube.Draw(program);

				if (isRightScreen)
				{
					maze[i][j] = '*';
				}
				
			}
		}
			
}

void myInit()
{
	LoadMaze();
	cube.Init();
	program = InitShader("vshader.glsl", "fshader.glsl");

}

void DrawGrid()
{
	float n = 40;
	float w = MazeSize;
	float h = MazeSize;

	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(0, -0.5, -h / 2 + h / n * i) * Scale(w, 0.02, 0.02);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(-w / 2 + w / n * i, -0.5, 0) * Scale(0.02, 0.02, h);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
}


void drawCamera()
{
	float cameraSize = 0.5;

	// viewDirection으로 부터 y축 기준 회전 각도 계산
	float angle = atan2(viewDirection.x, viewDirection.z);

	mat4 ModelMat = Translate(cameraPos) * RotateY(angle * (180.0f / PI)) * Scale(vec3(cameraSize));
	glUseProgram(program);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	ModelMat = Translate(cameraPos + viewDirection * cameraSize / 2) * RotateY(angle * (180.0f / PI)) * Scale(vec3(cameraSize / 2));
	glUseProgram(program);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);
}

void drawGoal()
{
	glUseProgram(program);
	float GoalSize = 0.7;

	mat4 ModelMat = Translate(goalPos) * RotateY(g_time * 3) * Scale(vec3(GoalSize));
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 0, 0, 0);
	cube.Draw(program);

	ModelMat = Translate(goalPos) * RotateY(g_time * 3 + 45) * Scale(vec3(GoalSize));
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 0, 0, 0);
	cube.Draw(program);
}


void drawScene(bool bDrawCamera = true)
{
	glUseProgram(program);
	uMat = glGetUniformLocation(program, "uMat");
	uColor = glGetUniformLocation(program, "uColor");

	DrawGrid();
	DrawMaze(bDrawCamera);
	drawGoal();
	DrawRoadAndSetNavi();

	if (bDrawCamera)
		drawCamera();

}

void display()
{
	glEnable(GL_DEPTH_TEST);

	float vWidth = wWidth / 2;
	float vHeight = wHeight;

	// LEFT SCREEN : View From Camera (Perspective Projection)
	glViewport(0, 0, vWidth, vHeight);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	float h = 4;
	float aspectRatio = vWidth / vHeight;
	float w = aspectRatio * h;
	mat4 ViewMat = myLookAt(cameraPos, cameraPos + viewDirection, vec3(0, 1, 0));
	mat4 ProjMat = myPerspective(45, aspectRatio, 0.01, 20);

	g_Mat = ProjMat * ViewMat;
	drawScene(false);							// drawing scene except the camera


	// RIGHT SCREEN : View from above (Orthographic parallel projection)
	glViewport(vWidth, 0, vWidth, vHeight);
	h = MazeSize;
	w = aspectRatio * h;
	ViewMat = myLookAt(vec3(0, 5, 0), vec3(0, 0, 0), vec3(0, 0, -1));
	ProjMat = myOrtho(-w / 2, w / 2, -h / 2, h / 2, 0, 20);

	g_Mat = ProjMat * ViewMat;
	drawScene(true);


	glutSwapBuffers();
}

bool isCollide(vec3 pos)
{
	pair<int,int> arrayPos = getArrayPosFromPosition(pos);
	if (maze[arrayPos.first][arrayPos.second] == '*')
	{
		maze[arrayPos.first][arrayPos.second] = 'r';
		return true;
	}
		
	return false;
}

bool isCompleteMove(vec3 nextPos)
{
	// 오차 범위
	float tolerance = 0.1f;

	// 현재 위치와 목표 위치 사이 거리
	float distance = length(nextPos - cameraPos);

	return distance < tolerance;
}

bool isCompleteTurn(vec3 nextDir)
{
	// 오차 범위
	float tolerance = 3 * PI / 180;

	// 현재 방향과 목표 방향 사이 각도
	float angleDiff = acos(dot(normalize(viewDirection), normalize(nextDir)));

	return angleDiff < tolerance;
}


void idle()
{
	g_time += 1;

	if (isTracking)
	{
		pair<int, int> nextPath = path[pathCnt];
		vec3 nextPos = getPositionFromIndex(nextPath.first, nextPath.second);
		vec3 nextDir = directionPath[dirCnt];

		if (isCompleteMove(nextPos) && isCompleteTurn(nextDir))
		{
			// 목표 위치면 Tracking 종료
			if (pathCnt == path.size() - 1)
			{
				isTracking = false;
				return;
			}
			pathCnt++;
			dirCnt++;
		}
		else
		{
			if (!isCompleteTurn(nextDir))	// 1. viewDirection이 nextDir이 될 때 까지 회전
			{
				float currentAngle = atan2(viewDirection.x, viewDirection.z);
				float targetAngle = atan2(nextDir.x, nextDir.z);

				// 회전할 각도 계산
				float angleDiff = targetAngle - currentAngle;

				// 각도 차이를 -PI ~ PI 범위로 조정
				if (angleDiff > PI) angleDiff -= 2 * PI;
				if (angleDiff < -PI) angleDiff += 2 * PI;

				// 회전 방향 결정
				if (angleDiff > 0) 
				{
					// 시계 반대 방향으로 회전
					mat4 rotationMat = RotateY(rotationSpeed);
					vec4 rotatedVec = rotationMat * vec4(viewDirection, 0.0f);
					viewDirection = normalize(vec3(rotatedVec.x, rotatedVec.y, rotatedVec.z));
				}
				else 
				{
					// 시계 방향으로 회전
					mat4 rotationMat = RotateY(-rotationSpeed);
					vec4 rotatedVec = rotationMat * vec4(viewDirection, 0.0f);
					viewDirection = normalize(vec3(rotatedVec.x, rotatedVec.y, rotatedVec.z));
				}
			}
			else if (!isCompleteMove(nextPos))	// 2. 1을 만족시켰다면, cameraPos가 nextPos가 될 때 까지 이동
			{
				vec3 nextPos = cameraPos + cameraSpeed * nextDir;
				if (!isCollide(nextPos))
					cameraPos = nextPos;
			}
		}
	}
	else
	{
		if ((GetAsyncKeyState('A') & 0x8000) == 0x8000)
		{
			float angle = atan2(viewDirection.x, viewDirection.z); // y축을 기준으로 회전 각도 계산 (라디안)
			cout << angle << "\n";

			mat4 rotationMat = RotateY(rotationSpeed);
			vec4 rotatedVec = rotationMat * vec4(viewDirection, 0.0f);
			viewDirection = vec3(rotatedVec.x, rotatedVec.y, rotatedVec.z);
			viewDirection = normalize(viewDirection); // 정규화하여 단위 벡터 유지
		}
		if ((GetAsyncKeyState('D') & 0x8000) == 0x8000)
		{
			float angle = atan2(viewDirection.x, viewDirection.z); // y축을 기준으로 회전 각도 계산 (라디안)

			mat4 rotationMat = RotateY(-rotationSpeed);
			vec4 rotatedVec = rotationMat * vec4(viewDirection, 0.0f);
			viewDirection = vec3(rotatedVec.x, rotatedVec.y, rotatedVec.z);
			viewDirection = normalize(viewDirection); // 정규화하여 단위 벡터 유지
		}
		if ((GetAsyncKeyState('W') & 0x8000) == 0x8000)	
		{
			vec3 nextPos = cameraPos + cameraSpeed * viewDirection;
			if (!isCollide(nextPos))
				cameraPos = nextPos;
		}
		if ((GetAsyncKeyState('S') & 0x8000) == 0x8000)	
		{
			vec3 nextPos = cameraPos + cameraSpeed * (-viewDirection);
			if (!isCollide(nextPos))
				cameraPos = nextPos;
		}
	}

	Sleep(16);											// for vSync
	glutPostRedisplay();
}

void reshape(int wx, int wy)
{
	printf("%d %d \n", wx, wy);
	wWidth = wx;
	wHeight = wy;
	glutPostRedisplay();
}

void keyboardFunc(unsigned char key, int x, int y) {
	
	if (key == 'q' && !isTracking)
	{
		aStar();
	}
	if (key == ' ') 
	{ 
		pathCnt = 1;
		dirCnt = 0;
		if (isTracking)
		{
			isTracking = false;
		}
		else if (path.size() >= 2)
		{
			isTracking = true;

			pair<int, int> startPath = path.front();
			vec3 startPos = getPositionFromIndex(startPath.first, startPath.second);
			vec3 startDir = directionPath.front();
			
			cameraPos = startPos;
			viewDirection = startDir;
		}
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(wWidth, wHeight);

	glutCreateWindow("Homework3 (Maze Navigator)");

	glewExperimental = true;
	glewInit();

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboardFunc);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}