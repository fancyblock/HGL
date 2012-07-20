#pragma once
#include "HGLDefine.h"

//初始化库
bool HGL_Inital();

//创建一个渲染窗口，创建成功返回true
bool HGL_CreateWnd( const char* name, int wid, int hei, bool fullscreen = false );

//摧毁窗口(如果存在的话)
void HGL_DestroyWnd();

//获取事件
bool HGL_GetEvent( HGLEvt* evt );

//检测HGL常用键是否被按下
bool HGL_KeyPress( HGLCommonKey key );

//将缓冲内容显示到前台
void HGL_SwapBuffer();

//清除缓冲区内容，渲染列表清除也在这里
void HGL_ClearBuffer();

//设置透视投影
void HGL_Perspective( float fovy, float aspect, float zNear, float zFar );

//设置当前矩阵类型
void HGL_MatrixMode( HGLMatrixType matrixMode );

//设置当前矩阵为单位矩阵
void HGL_LoadIdentity();

//当前矩阵乘以一个平移矩阵（左乘）
void HGL_Translate( float x, float y, float z );

//当前矩阵乘以一个旋转矩阵（左乘）
void HGL_Rotate( float angle, float x, float y, float z );

//当前矩阵乘以一个缩放矩阵（左乘）
void HGL_Scale( float x, float y, float z );

//当前矩阵乘以一个沿任意轴缩放的矩阵（左乘）
void HGL_Scale( float x, float y, float z, float k );

//开始编辑一个物体
void HGL_Begin( HGL_Primitive& primitive, int count );

//结束编辑一个物体
void HGL_End();

//销毁一个物体
void HGL_DestroyPrimitive( HGL_Primitive& primitive );

//指定一个顶点位置
void HGL_Vertex( float x, float y, float z );

//指定一个顶点颜色
void HGL_Color( float r, float g, float b );

//指定一个顶点法线
void HGL_Normal( float x, float y, float z );

//指定一个顶点UV
void HGL_Texcoord( float u, float v );

//绘制一个物体
void HGL_DrawPrimitive( HGL_Primitive& primitive, HGLPrimitiveType type );

//得到当前渲染列表中的多边形数量
int HGL_GetFacetCount();

//设置多边形剔除模式[逆时针为正面]
void HGL_CullMode( HGLCullMode mode );

//设置多边形的填充模式
void HGL_FillMode( HGLFillMode mode );

//设置材质
void HGL_SetMatrial( const HGL_Matrial& matrial );

//重置材质
void HGL_ResetMatrial();

//设置某个光照
void HGL_SetLight( HGL_LightNum lightIndex, HGL_Light* lightInfo );

//设置简单光照
void HGL_SetSimpleLight( HGL_SimpleLightNum lightIndex, HGL_SimpleLight* lightInfo );

//开启某些渲染选项	【未完】
void HGL_Enable( HGL_RenderOption option );

//关闭某些渲染选项	【未完】
void HGL_Disable( HGL_RenderOption option );

//设置着色模式
void HGL_ShadeMode( HGL_ShadeModel sm );

//加载纹理
bool HGL_LoadTexture( HGL_Texture& tex, char* fileName );

//绑定纹理
void HGL_BindTexture( HGL_TextureLevel num, HGL_Texture* tex );
