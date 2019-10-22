#include <thrust/execution_policy.h>
#include <thrust/functional.h>

///////////////////////////////////////////////////////////
ContactProcess *Sptr;
int paso;

int sqx, sqy;
int signo;
float fieldmax, fieldmin;
int planoz;
float x;

#ifndef DIM
#define DIM	512
#endif

#define XINC 0.01


void rescale(){
  fieldmax=1.f;		
  fieldmin=-1.f;		
}

// functor para mapear buffer de opengl al del sistema y viceversa (overloaded)
struct mapear_cuda_ogl_op
{
    float max, min;
    mapear_cuda_ogl_op(float _min, float _max):max(_max),min(_min){};

    __device__
    uchar4 operator()(int v)
    {
      uchar4 u4;
      u4.x = u4.y = u4.z = u4.w = 0;
      u4.y = (v>0)?(255):(0);
      u4.x = (v>0)?(255):(0);
   
      return u4;
    }
    __device__
    float operator()(uchar4 u)
    {
      return (u.y*max/255.f)-(u.x*max/255.f);
    }
};


void mapear_ogl_cuda(int *d, uchar4 *ptr){
  float min=-3; float max=3;
  thrust::transform(thrust::device, ptr, ptr+DIM*DIM, d, mapear_cuda_ogl_op(min, max));  
}

void mapear_cuda_ogl(uchar4 *ptr,int *d){
   
    thrust::transform(thrust::device, d, d+DIM*DIM, ptr, mapear_cuda_ogl_op(fieldmin,fieldmax));  
}

// lo que pasa cada vez que opengl llama a draw
void change_pixels(){

  cudaGraphicsMapResources( 1, &resource, NULL ); 
  uchar4* devPtr; 
  size_t  size; 
  cudaGraphicsResourceGetMappedPointer( (void**)&devPtr, &size, resource); 

  Sptr->Dynamics(paso);

  mapear_cuda_ogl(devPtr,Sptr->dptr);
  cudaGraphicsUnmapResources( 1, &resource, NULL ); 

};

void inicializar_variables_globales_sistema()
{
  std::cout << "inicializando variables del sistema..."; 
  paso=1;
  signo=1;
  rescale();
  x=1.6;
  planoz = 0;		
  Sptr->SetX(x);		
  std::cout << "done" << std::endl;
}
////////////////////////////////////////////////