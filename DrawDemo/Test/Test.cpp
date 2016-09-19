//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>  
#include <map>  
#include <vector>  
#include <string> 
#include <GL/glew.h>
//#define GLFW_DLL
#include <GLFW/glfw3.h>

#define MAXSIZE 10000
using namespace std;

const char* vertex_shader =
        "#version 400\n"
        "in vec3 vp;"
        "void main () {"
        "  gl_Position = vec4 (vp, 1.0);"
        "}";

const char* fragment_shader =
        "#version 400\n"
		"uniform vec4 c;"
		"in vec3 Color;"
        "out vec4 frag_colour;"
        "void main () {"
        //"  frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
		// "  frag_colour = vec4 (Color, 1.0);"
		 "  frag_colour = c;"
        "}";

GLfloat g_Vertex[MAXSIZE][9];
GLuint g_VAOs[MAXSIZE] = {0};
GLuint shader_programme[MAXSIZE];
vector<float> csv_data;


void GenTrianglePoints(GLuint row, GLuint col, GLuint index)
{
	float points[9] = {0.0f};

	float baseY = 0.495f;
	float stepX = 0.045f;
	float stepY = 0.09f;
	float baseX = -stepX * 11 + row * stepX;
	if(col % 2 == 0) //倒三角形
	{
		
		points[0] = baseX + col * stepX;
		points[1] = baseY - stepY * row;
		points[3] = points[0] + stepX * 2;
		points[4] = points[1];
		points[6] = points[0] + stepX;
		points[7] = points[1] - stepY;
	}
	else
	{
		points[0] = baseX + (col + 1) * stepX;
		points[1] = baseY - stepY * row;
		points[3] = points[0] - stepX;
		points[4] = points[1] - stepY;
		points[6] = points[0] + stepX;
		points[7] = points[1] - stepY;
	}
	memcpy(g_Vertex + row * row + col, points, sizeof(float) * 9);
}

void GenTriangle(GLuint index, float* points, float* colors = NULL)
{
	GLuint vbo[2] = {0};
	glGenBuffers(2, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof (float), points, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);  
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colors, GL_STATIC_DRAW);  

    glGenVertexArrays(1, &g_VAOs[index]);
    glBindVertexArray(g_VAOs[index]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	if(colors)
	{
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL );  
	}
}

void GenShader(GLuint count)
{
	GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs, 1, &vertex_shader, NULL);
    glCompileShader (vs);

    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &fragment_shader, NULL);
    glCompileShader (fs);

	for(GLuint i = 0; i < count * count; i++)
	{
		shader_programme[i] = glCreateProgram ();
		glAttachShader (shader_programme[i], fs);
		glAttachShader (shader_programme[i], vs);
		glLinkProgram (shader_programme[i]);
	}

	
}

//计算一个字符串中的某个子串的个数
int strstr_cnt(const char *string, const char *substring)
{  
	int i,j,k,count = 0;  
	for (i = 0; string[i]; i++)
	{  
		for (j = i, k = 0; (string[j] == substring[k] && (j < strlen(string))); j++,k++)
		{  
			if (!substring[k + 1])
			{  
				count++;  
			}  
		}  
	}  
	return count;  
}

int substring_index(const char *s1,const char *s2, int pos)
{  
	int i,j,k;  
	for( i = pos ; s1[i] ; i++ )
	{  
		for( j = i, k = 0 ; s1[j] == s2[k]; j++,k++ )
		{  
			if (!s2[k + 1])
			{  
				return i;  
			}  
		}  
	}  
	return -1;  
}

int ReplaceStr(char* sSrc, char* sMatchStr, char* sReplaceStr)
{
        int StringLen;
        char caNewString[64];
        char* FindPos;
        FindPos =(char *)strstr(sSrc, sMatchStr);
        if( (!FindPos) || (!sMatchStr) )
                return -1;

        while( FindPos )
        {
                memset(caNewString, 0, sizeof(caNewString));
                StringLen = FindPos - sSrc;
                strncpy(caNewString, sSrc, StringLen);
                strcat(caNewString, sReplaceStr);
                strcat(caNewString, FindPos + strlen(sMatchStr));
                strcpy(sSrc, caNewString);

                FindPos =(char *)strstr(sSrc, sMatchStr);
        }
        free(FindPos);
        return 0;
}

char *fgetcsvline( FILE *fhead) 
{  
	char *ret_stat;  
	char data_buf[1024];  
	string stringbuf;  
	ret_stat = fgets(data_buf, 1024, fhead);  
	if (ret_stat != NULL) 
	{  
		int count = strstr_cnt(data_buf,","); 
		int startpos = 0;
		while (count > 0)
		{  
			
			int pos = substring_index(data_buf,",", startpos); 
			stringbuf = (string)data_buf; 
			  
			string csv_buf;
			csv_buf = stringbuf.substr(startpos, pos - startpos);
			if(csv_buf.length() != 0)
			{
				float f = atof(csv_buf.c_str());
				csv_data.push_back(f);
			}
			startpos = pos + 1;
			count--;
		}  
	}  
	return ret_stat;   
}


int LoadCSV(char *path)  
{	
	if(path == NULL)
	{
		printf("Invalid path");
		return 0;
	}
	ReplaceStr(path, "\\","/");
    FILE  *fp_head;
    char  *ret_stat;
	
    fp_head = fopen(path, "rt");
	if(fp_head == NULL)
	{
		printf("Can't open the data file");
		return 0;
	}
	int lineCount = 0;
	ret_stat = fgetcsvline(fp_head);
	while (ret_stat != NULL) 
	{
		lineCount++;
		ret_stat = fgetcsvline(fp_head);
	}
	return lineCount;
}  

int main(int argc, char** argv)
{
    // start GL context and O/S window using the GLFW helper library
	if(argc != 2)
	{
		fprintf (stderr, "需要两个参数\n");
        //return 1;
	}
	
	
    if (!glfwInit ()) 
	{
        fprintf (stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }
	
    GLFWwindow* window = glfwCreateWindow (800, 600, "Hello Triangle", NULL, NULL);
    if (!window) {
        fprintf (stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
	
    glfwMakeContextCurrent (window);
                                  
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
	
    // get version info
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);
	
    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
   
	//int flag = LoadCSV(argv[1]);
	int lineCount = LoadCSV("C:/Users/Administrator/Desktop/Draw/test.csv");
	if(!lineCount)
		return 1;

	int index = 0;
	for(GLuint i = 0; i < lineCount; i++)
	{
		int triangleCount = 2 * (lineCount - i) - 1; 
		for(GLuint j = 0; j < triangleCount; j++)
		{
			GenTrianglePoints(i, j, index);
			GenTriangle(index, g_Vertex[index]);
			index++;
		}

	}

	GenShader(lineCount);
		
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // wipe the drawing surface clear
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        

		for(GLuint i = 0; i < lineCount * lineCount; i++)
		{
				glUseProgram (shader_programme[i]);
				glBindVertexArray (g_VAOs[i]);
				glDrawArrays (GL_TRIANGLES, 0, 3);
				GLfloat color = csv_data[i];
				GLfloat R = color / 1000000;
				GLfloat G = color / 10000;
				G = G - GLuint(G);
				GLfloat B = (color / 100);
				B = B - GLuint(B);
				GLuint color_Sh = glGetUniformLocation(shader_programme[i],"c");
				glUniform4f(color_Sh, R, G, B, 1.0f);

		}

		
        // update other events like input handling 
        glfwPollEvents ();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers (window);
    }

    // close GL context and any other GLFW resources
    glfwTerminate();

    return 0;
}