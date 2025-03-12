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
	vec2 points[NUM_POINTS];		// gpu���� ������ ���Ŀ� cpu���� �����͸� ������� �ʿ䰡 ���� -> ���÷� ����
	for (int i = 0; i < NUM_POINTS; i++)
	{
		points[i].x = (rand() % 1000) / 500.0f - 1;
		points[i].y = (rand() % 1000) / 500.0f - 1;
	}
	
	// 2. Send Data to GPU
	
	// (1) generate vertex array and bind the array
	// - array�� �ش��ϴ� �޸𸮸� gpu���� �����Ҵ�
	// - �� �����Ҵ� �� �޸𸮿� ���� �ڵ�(GLuint vao) �� ���� � �ڵ��� ����� ���� Binding��
	// - �ڵ�(���α׷� �� �޸𸮿� ������ �� �ִ� Ƽ������ ����) ==> GLuint vao   (�ü���� �ڵ��� ��� ������ ��)
	GLuint vao;		// OpenGL���� unsigned int�� ��Ī�ϴ� ���ϵ� ���� (��⸶�� bit �� �� �ٸ� �� �ֱ� ������)
	glGenVertexArrays(1, &vao);  // �� ��? , � �ڵ鿡?
	glBindVertexArray(vao);		// "���� �� �ڵ鿡 ���� �����Ұž�"
	


	// (2) generate vertex buffer and bind the buffer
	// - �ڵ鸵�ϰ��ִ� �޸𸮿� buffer�� ���� �� ���ε�
	GLuint vbo;
	glGenBuffers(1, &vbo);	// �� ��?, � �ڵ鿡?
	glBindBuffer(GL_ARRAY_BUFFER, vbo);	// � ����?, ������?
	


	// (3) copy my data to the buffer
	// cpu���� ������ �����͸� GPU�� �Ҵ��� ���ۿ� ����
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW); // 
	
	

	// 3. Load Shaders
	// Vertex Shader �������� ȭ�� ���� ��ġ�� ����ִ� Vertex Shader�� ���� �ۼ��ؾ��� -> ī�޶�
	// Fragment Shader ���� ������ ĥ�Ұǵ�? -> ����
	GLuint program;
	program = InitShader("vshader.glsl", "fshader.glsl");	// ���̴��� gpu���� ������ gpu�� ������ �� �ε� -> �ڵ� ����
	glUseProgram(program);
	
	
	// 4. connect Data with Shader
	// �����Ϳ� ���̴��� ���� - � �����Ͱ� � ���̴��� ��� ����Ǵµ�? �� ���� (���̴��� �����͸� ��� ��������?)
	// ex (�� �� �ΰ��� ��� �ϳ��� ������ �ؼ�����)
	int vPosition = 0;
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	// "vPosition ��� ������ float�� �ش��ϴ� ������ 2���� �����ؼ� ������ ��"


}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glDrawArrays(GL_POINTS, 0, NUM_POINTS);		// "�Ʊ� ������ �����͸� ������ �׷���!" (������ ���ε��� Array�� �������) --> �ֽ� ���
	
	glFlush();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Hello GL");

	// �ֽ� Open GL�� ����Ϸ��� ���� �� ���� �ʿ�
	glewExperimental = true;
	glewInit();

	

	init();	// �����͸� ����� �����ϴ� Init�� glewInit() �� �Ŀ� ���
	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}