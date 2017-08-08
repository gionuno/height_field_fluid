#include <GL/glew.h>
#include <GL/gl.h>

#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "frame.hpp"

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#include <cmath>

using namespace std;

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int winx = 640;
int winy = 640;

GLFWwindow * window = nullptr;

int init_gl_window()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(winx,winy,"Height Field Fluid",nullptr,nullptr);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewInit();

    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    return 0;
}

int main(int argc,char ** argv)
{
    srand(time(nullptr));
    if(init_gl_window()<0) return -1;

    shader copy;
    copy.load_file(GL_VERTEX_SHADER,"vert.glsl");
    copy.load_file(GL_FRAGMENT_SHADER,"copy.glsl");
    copy.create();

    //Shaders for BFECC
    shader advect;
    advect.load_file(GL_VERTEX_SHADER,"vert.glsl");
    advect.load_file(GL_FRAGMENT_SHADER,"advect.glsl");
    advect.create();

    shader correct;
    correct.load_file(GL_VERTEX_SHADER,"vert.glsl");
    correct.load_file(GL_FRAGMENT_SHADER,"correct.glsl");
    correct.create();

    shader integrate_h;
    integrate_h.load_file(GL_VERTEX_SHADER,"vert.glsl");
    integrate_h.load_file(GL_FRAGMENT_SHADER,"integrate_h.glsl");
    integrate_h.create();

    shader integrate_u;
    integrate_u.load_file(GL_VERTEX_SHADER,"vert.glsl");
    integrate_u.load_file(GL_FRAGMENT_SHADER,"integrate_u.glsl");
    integrate_u.create();

    shader show;
    show.load_file(GL_VERTEX_SHADER,"vert.glsl");
    show.load_file(GL_FRAGMENT_SHADER,"show.glsl");
    show.create();

    int N = 128;
    int M = 128;

    float * h_ = new float [3*N*M];
    float * s_ = new float [3*N*M];

    float * u_ = new float [3*N*M];

    for(int i=0;i<N;i++)
    for(int j=0;j<M;j++)
    {
        float x_ = (i-0.5*N)*2./N;
        float y_ = (j-0.5*M)*2./M;
        float f_  = y_-0.25*sin(2.0*M_PI*x_);
        float g_  = y_-0.5*cos(2.0*M_PI*x_);
        float n_ = x_*x_+y_*y_;
        s_[3*N*j+3*i]   = sqrt((x_+0.8)*(x_+0.8)+y_*y_) < 0.017625 ? 10. :0.0;
        h_[3*N*j+3*i]   = 0.0;
        h_[3*N*j+3*i+1] = 100.*(x_<0.0)*(x_*x_)-100.0*exp(-100.0*f_*f_)-100.*exp(-100.*g_*g_)+rand()*20./RAND_MAX-10.0;
        h_[3*N*j+3*i+2] = 0.0;
        u_[3*N*j+3*i  ] = 0.0;
        u_[3*N*j+3*i+1] = 0.0;
        u_[3*N*j+3*i+2] = 0.0;
    }

    float hmin =   1e10;
    float hmax =  -1e10;
    float umax =  0.0;
    for(int i=0;i<N;i++)
    for(int j=0;j<M;j++)
    {
        float aux = h_[3*N*j+3*i+1]+h_[3*N*j+3*i];
        hmin = hmin > aux ? aux : hmin;
        hmax = hmax < aux ? aux : hmax;
    }
    hmax -= hmin;
    umax  = 0.5*hmax;
    for(int i=0;i<N;i++)
    for(int j=0;j<M;j++)
    {
        h_[3*N*j+3*i]   /= hmax;
        h_[3*N*j+3*i+1] -= hmin;
        h_[3*N*j+3*i+1] /= hmax;
    }
    frame h(N,M);
    frame u(N,M);
    frame s(N,M);

    glBindTexture(GL_TEXTURE_2D,h.ctex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB32F,N,M,0,GL_RGB,GL_FLOAT,h_);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D,0);

    glBindTexture(GL_TEXTURE_2D,u.ctex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB32F,N,M,0,GL_RGB,GL_FLOAT,u_);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D,0);

    glBindTexture(GL_TEXTURE_2D,s.ctex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB32F,N,M,0,GL_RGB,GL_FLOAT,s_);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D,0);


    double dt =  5e-2;
    double dx =  1e0;
    double  g =  9.8;
    screen screen_;
    int t=1;
    while (!glfwWindowShouldClose(window))
    {
        frame u_c(N,M);
        u_c.begin();
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            copy.begin();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D,u.ctex);
                glUniform1i(copy("S"),0);
                screen_.draw();
            copy.end();
        u_c.end();
        frame u_a(N,M);
        u_a.begin();
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            advect.begin();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D,u_c.ctex);
                glUniform1i(advect("u"),0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D,u.ctex);
                glUniform1i(advect("S"),1);
                glUniform1f(advect("dt"),dt);
                glUniform1f(advect("umax"),umax);
                screen_.draw();
            advect.end();
        u_a.end();
        frame u_b(N,M);
        u_b.begin();
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            advect.begin();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D,u_c.ctex);
                glUniform1i(advect("u"),0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D,u_a.ctex);
                glUniform1i(advect("S"),1);
                glUniform1f(advect("dt"),-dt);
                glUniform1f(advect("umax"),umax);
                screen_.draw();
            advect.end();
        u_b.end();
        u_a.begin();
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            correct.begin();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D,u.ctex);
                glUniform1i(correct("P0"),0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D,u_b.ctex);
                glUniform1i(correct("P1"),1);
                glUniform1f(correct("umax"),umax);
                screen_.draw();
            correct.end();
        u_a.end();
        u.begin();
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            advect.begin();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D,u_c.ctex);
                glUniform1i(advect("u"),0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D,u_a.ctex);
                glUniform1i(advect("S"),1);
                glUniform1f(advect("dt"),dt);
                glUniform1f(advect("umax"),umax);
                screen_.draw();
            advect.end();
        u.end();

        frame h_c(N,M);
        h_c.begin();
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            copy.begin();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D,h.ctex);
                glUniform1i(copy("S"),0);
                screen_.draw();
            copy.end();
        h_c.end();
        h.begin();
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            integrate_h.begin();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D,h_c.ctex);
                glUniform1i(integrate_h("h"),0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D,u.ctex);
                glUniform1i(integrate_h("u"),1);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D,s.ctex);
                glUniform1i(integrate_h("s"),2);
                glUniform1f(integrate_h("dt"),dt);
                glUniform1f(integrate_h("dx"),dx);
                glUniform1f(integrate_h("hmax"),hmax);
                glUniform1f(integrate_h("umax"),umax);
                screen_.draw();
            integrate_h.end();
        h.end();

        u_c.begin();
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            copy.begin();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D,u.ctex);
                glUniform1i(copy("S"),0);
                screen_.draw();
            copy.end();
        u_c.end();

        u.begin();
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            integrate_u.begin();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D,h.ctex);
                glUniform1i(integrate_u("h"),0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D,u_c.ctex);
                glUniform1i(integrate_u("u"),1);
                glUniform1f(integrate_u("dt"),dt);
                glUniform1f(integrate_u("dx"),dx);
                glUniform1f(integrate_u("g"),g);
                glUniform1f(integrate_u("hmax"),hmax);
                glUniform1f(integrate_u("umax"),umax);
                screen_.draw();
            integrate_u.end();
        u.end();

        cout << t << endl;

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, winx,winy);
        show.begin();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,h.ctex);
            glUniform1i(show("h"),0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D,u.ctex);
            glUniform1i(show("u"),1);

            glUniform1f(show("hmax"),hmax);
            glUniform1f(show("umax"),umax);

            screen_.draw();
        show.end();

        glfwSwapBuffers(window);
        glfwPollEvents();
        //max_P += 1.0;
        t++;
    }

    delete [] h_;
    delete [] u_;
    delete [] s_;

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
