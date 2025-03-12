
#include <vgl.h>
#include <math.h>
#include <InitShader.h>

struct vec2
{
	float x, y;
};
const int NUM_POINTS = 5000;

void init()
{
	// Generate Data in CPU
	vec2 points[NUM_POINTS];
	for (int i = 0; i < NUM_POINTS; i++)
	{
		points[i].x = (rand() % 1000) / 500.0f - 1;
		points[i].y = (rand() % 1000) / 500.0f - 1;
	}


	// 1. Generate Vertex Array
	GLuint vao;
	glGenVertexArrays(1, &vao);

	// 2. Bind the Vertex Array
	glBindVertexArray(vao);

	// 3. Generate a Vertex Buffer Object
	GLuint vbo;
	glGenBuffers(1, &vbo);

	// 4. Bind the Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, vbo);


	// 5. Copy data to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);



	// Load Shaders
	GLuint program;
	program = InitShader("vshader.glsl", "fshader.glsl");	// 셰이더를 gpu에 보내서 gpu가 컴파일 후 로드 -> 핸들 반환
	glUseProgram(program);


	// Connect data with shader
	// 데이터와 셰이더의 매핑 방법을 기술 
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_POINTS, 0, NUM_POINTS);		// "아까 전송한 데이터를 점으로 그려줘!" (마지막 바인딩된 Array를 대상으로) 

	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Hello GL");

	glewExperimental = true;
	glewInit();

	init();	// 데이터를 만들고 전송하는 Init은 glewInit() 이 후에 사용해야함
	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}


