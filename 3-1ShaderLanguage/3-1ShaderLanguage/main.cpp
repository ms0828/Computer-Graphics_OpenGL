#include <vgl.h>
#include <math.h>
#include <InitShader.h>

struct vec2 
{
	float x, y;
};

const int NUM_POINTS = 500;

void init()
{
	// 1. Generate Data in CPU
	vec2 points[NUM_POINTS];		// gpu에게 전송한 이후에 cpu에서 데이터를 들고있을 필요가 없음 -> 로컬로 변경
	for (int i = 0; i < NUM_POINTS; i++)
	{
		points[i].x = (rand() % 1000) / 500.0f - 1;
		points[i].y = (rand() % 1000) / 500.0f - 1;
	}
	
	// 2. Send Data to GPU
	
	// (1) generate vertex array and bind the array
	// - array에 해당하는 메모리를 gpu에게 동적할당
	// - 각 동적할당 된 메모리에 대한 핸들(GLuint vao) 중 현재 어떤 핸들을 사용할 건지 Binding함
	// - 핸들(프로그램 밖 메모리에 접근할 수 있는 티켓으로 이해) ==> GLuint vao   (운영체제가 핸들을 양수 값으로 줌)
	GLuint vao;		// OpenGL에서 unsigned int를 지칭하는 통일된 변수 (기기마다 bit 수 가 다를 수 있기 때문에)
	glGenVertexArrays(1, &vao);  // 몇 개? , 어떤 핸들에?
	glBindVertexArray(vao);		// "지금 이 핸들에 대해 설정할거야"
	


	// (2) generate vertex buffer and bind the buffer
	// - 핸들링하고있는 메모리에 buffer를 생성 후 바인딩
	GLuint vbo;
	glGenBuffers(1, &vbo);	// 몇 개?, 어떤 핸들에?
	glBindBuffer(GL_ARRAY_BUFFER, vbo);	// 어떤 종류?, 무엇을?
	


	// (3) copy my data to the buffer
	// cpu에서 생성한 데이터를 GPU에 할당한 버퍼에 복사
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW); // 
	
	

	// 3. Load Shaders
	// Vertex Shader 데이터의 화면 상의 위치를 잡아주는 Vertex Shader를 직접 작성해야함 -> 카메라
	// Fragment Shader 무슨 색으로 칠할건데? -> 조명
	GLuint program;
	program = InitShader("vshader.glsl", "fshader.glsl");	// 셰이더를 gpu에게 보내서 gpu가 컴파일 후 로드 -> 핸들 리턴
	glUseProgram(program);
	
	
	// 4. connect Data with Shader
	// 데이터와 셰이더를 연결 - 어떤 데이터가 어떤 셰이더랑 어떻게 연결되는데? 를 정의 (셰이더가 데이터를 어떻게 읽을껀데?)
	// ex (앞 에 두개씩 끊어서 하나의 점으로 해석해줘)
	int vPosition = 0;
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	// "vPosition 라는 변수는 float에 해당하는 변수는 2개씩 연속해서 읽으면 돼"


}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glDrawArrays(GL_POINTS, 0, NUM_POINTS);		// "아까 전송한 데이터를 점으로 그려줘!" (마지막 바인딩된 Array를 대상으로) --> 최신 방법
	
	glFlush();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Hello GL");

	// 최신 Open GL을 사용하려면 다음 두 줄이 필요
	glewExperimental = true;
	glewInit();

	

	init();	// 데이터를 만들고 전송하는 Init은 glewInit() 이 후에 사용
	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}