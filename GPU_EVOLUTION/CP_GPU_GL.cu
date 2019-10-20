#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

#include <GL/gl.h> 
#include <GL/glut.h> 
#include <cuda_gl_interop.h> 
#include <GL/glext.h> 
#include <GL/glx.h> 

#define REFRESH_DELAY     10 //ms

#define GET_PROC_ADDRESS( str ) glXGetProcAddress( (const GLubyte *)str ) 

static void HandleError( cudaError_t err, const char *file,  int line ) { 
    if (err != cudaSuccess) { 
            printf( "%s in %s at line %d\n", cudaGetErrorString( err ),  file, line ); 
            exit( EXIT_FAILURE ); 
    } 
} 
#define HANDLE_ERROR( err ) (HandleError( err, __FILE__, __LINE__ )) 


PFNGLBINDBUFFERARBPROC    glBindBuffer     = NULL; 
PFNGLDELETEBUFFERSARBPROC glDeleteBuffers  = NULL; 
PFNGLGENBUFFERSARBPROC    glGenBuffers     = NULL; 
PFNGLBUFFERDATAARBPROC    glBufferData     = NULL; 


GLuint  bufferObj; 
cudaGraphicsResource *resource; 

// define la interface entre opengl y mi sistema //////
#include "CP_GPU.h"
#include "interfaz.h"

static void draw_func( void ) { 
  	
  	rescale();
  	change_pixels();
	
	glDrawPixels(DIM, DIM, GL_RGBA, GL_UNSIGNED_BYTE, 0); 
	glutSwapBuffers(); 
}

static void key_func( unsigned char key, int r, int l) { 
  switch (key) { 
  	case 27: 
        HANDLE_ERROR( cudaGraphicsUnregisterResource( resource ) ); 
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 ); 
        glDeleteBuffers( 1, &bufferObj ); 
        exit(0); 
        break;
    case 'T':
        if(x - XINC > 0) x -= XINC; 
	Sptr->SetX(x);        
	std::cout << "x = " << x << std::endl;
        break;    
    case 't':
        x+=XINC; 
	Sptr->SetX(x);        
	std::cout << "x = " << x << std::endl;
        break;    
    default:
        break;
  } 
} 


////////////////////////////////////////////////////////////////////////////////
//! Mouse event handlers
////////////////////////////////////////////////////////////////////////////////
// mouse controls
int mouse_old_x, mouse_old_y;
int mouse_buttons = 0;

float rotate_x = 0.0, rotate_y = 0.0;
float translate_z = -3.0;

void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        mouse_buttons |= 1<<button;
    }
    else if (state == GLUT_UP)
    {
        mouse_buttons = 0;
    }

    mouse_old_x = x;
    mouse_old_y = y;

    sqx=(x>0 && x<DIM)?(x):(sqx); sqy=(y>0 && y<DIM)?(y):(sqy);
    std::cout << sqx << " (clicks) " << sqy << " " << signo << std::endl;   
}



void timerEvent(int value)
{
    glutPostRedisplay();
    glutTimerFunc(REFRESH_DELAY, timerEvent,0);
}



int main(int argc, char **argv) { 

	int args = 1;
	// size of the system (default 128)
	int L = (argc > args) ? (atoi(argv[args])):(512);
	if (L%2 == 1 || L < 0) {
		std::cout << "ERROR: L must be even and positive" << std::endl;
		exit(1);
	}
	args++;

	// temperature (default 2.0)
	float x = (argc > args) ? (atof(argv[args])):(1.5);
	args++;

	
	// iterations (default 1000)
	int trun = (argc > args) ? (atoi(argv[args])):(1000);
	args++;

	// global seed (default 0)
	int seed = (argc > args) ? (atoi(argv[args])):(0);
	srand(seed);

	std::cout << "L = " << L << std::endl;
	std::cout << "x = " << x << std::endl;
	std::cout << "trun = " << trun << std::endl;
	std::cout << "seed = " << seed << std::endl;

  ////// declarar/inicializar sistema //////
  ContactProcess S(L, x, seed);

  Sptr=&S;
  inicializar_variables_globales_sistema();
  //////////////////////////////////////////

  cudaGLSetGLDevice( 0 ); 

  glutInit( &argc, argv ); 
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA ); 
  glutInitWindowSize( L, L ); 
  glutCreateWindow( "OpenGL: Contact Processes app" ); 

  glBindBuffer    = (PFNGLBINDBUFFERARBPROC)GET_PROC_ADDRESS("glBindBuffer"); 
  glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC)GET_PROC_ADDRESS("glDeleteBuffers"); 
  glGenBuffers    = (PFNGLGENBUFFERSARBPROC)GET_PROC_ADDRESS("glGenBuffers"); 
  glBufferData    = (PFNGLBUFFERDATAARBPROC)GET_PROC_ADDRESS("glBufferData"); 

  glGenBuffers( 1, &bufferObj ); 
  glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, bufferObj ); 
  glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, DIM * DIM * 4, NULL, GL_DYNAMIC_DRAW_ARB ); 


  cudaGraphicsGLRegisterBuffer( &resource, bufferObj, cudaGraphicsMapFlagsNone ); 

// set up GLUT and kick off main loop 
 glutKeyboardFunc( key_func ); 
 glutDisplayFunc( draw_func ); 
 glutMouseFunc(mouse);

  glutTimerFunc(REFRESH_DELAY, timerEvent,0);
  glutMainLoop(); 
}
