#pragma once
#include "HGLDefine.h"


///////////////////////////////////////////////
//这里的函数不对外使用HGL的用户开放，内部使用//
///////////////////////////////////////////////


//---------------
//基本绘图函数
//---------------


//#define BRESENHAM_LINE		//使用Bresenham画线算法【未完】
#define DDA_LINE				//使用DDA画线算法

//锁定缓存
void hgl_BeginDraw();

//解锁缓存
void hgl_EndDraw();

//写缓存（画点）
void hgl_DrawPixel( int x, int y, char r, char g, char b );

//裁减线（返回true代表整条线都被裁掉了）	【未完】
bool hgl_ClipLine( int& x1, int y1, int x2, int y2 );

//写缓存（画线）
void hgl_DrawLine( int x1, int y1, int x2, int y2, char r, char g, char b );



//---------------
//数学函数
//---------------


////----------向量类----------////

//3d向量设置
vertex3 Vertex3( float x, float y, float z );

//4d向量设置
vertex4 Vertex4( float x, float y, float z, float w );

//齐次坐标转换为3D向量
vertex3 Vertex4To3( const vertex4& vtx4 );

//3D向量转换为齐次坐标
vertex4 Vertex3To4( const vertex3& vtx3 );

//向量取模
float Vertex3Length( const vertex3& vtx3 );

//设为零向量
void Vertex3Zero( vertex3& vtx3 );

//变成单位向量
void Vertex3Normalize( vertex3& vtx3 );

//向量加法
vertex3 Vertex3Add( const vertex3& vtx31, const vertex3& vtx32 );

//向量减法
vertex3 Vertex3Sub( const vertex3& vtx31, const vertex3& vtx32 );

//两点间距
float Vertex3Distance( const vertex3& vtx31, const vertex3& vtx32 );

//向量取反
void Vertex3Negate( vertex3& vtx3 );

//标量乘
vertex3 Vertex3MutiScalar( const vertex3& vtx3, const float scalar );

//标量除
vertex3 Vertex3DivScalar( const vertex3& vtx3, const float scalar );

//向量点乘
float Vertex3DotProduct( const vertex3& vtx31, const vertex3& vtx32 );

//向量叉乘
vertex3 Vertex3CorssProduct( const vertex3& vtx31, const vertex3& vtx32 );


////----------平面类----------////

//从三个点生成平面
plane GetPlane( const vertex3& vtx1, const vertex3& vtx2, const vertex3& vtx3 );


////----------向量平面类----------////

//向量是否在平面前面
bool IsFront( const vertex3& vtx3, const plane& pln );

//球是否在平面前面（相交不算）
bool IsFront( const vertex3& vtx3, float radius, const plane& pln );


////----------矩阵类----------////

//44矩阵变成单位矩阵
void Matrix44Identity( matrix44& mat44 );

//44矩阵相乘
matrix44 Matrix44Multi( const matrix44& mat1, const matrix44& mat2 );

//生成一个平移矩阵
void Matrix44Translate( matrix44& mat44, float x, float y, float z );

//生成一个缩放矩阵
void Matrix44Scale( matrix44& mat44, float xScale, float yScale, float zScale );

//生成一个沿指定轴缩放的矩阵
void Matrix44ScaleAxis( matrix44& mat44, const vertex3& axis, float scale );

//生成一个绕X轴旋转矩阵（弧度制）
void Matrix44RotateX( matrix44& mat44, float angle );

//生成一个绕Y轴旋转矩阵（弧度制）
void Matrix44RotateY( matrix44& mat44, float angle );

//生成一个绕Z轴旋转矩阵（弧度制）
void Matrix44RotateZ( matrix44& mat44, float angle );

//生成一个绕任意轴旋转的矩阵（弧度制）
void Matrix44Rotate( matrix44& mat44, float ax, float ay, float az, float angle );


////----------向量矩阵类----------////

//齐次坐标乘矩阵44
vertex4 Vertex4MultiMatrix44( const vertex4& vtx4, const matrix44& mat44 );

//齐次坐标乘矩阵43
vertex3 Vertex4MultiMatrix43( const vertex4& vtx4, const matrix44& mat43 );

//坐标乘矩阵33（用于法线的变换，因为其只需要旋转不需要平移）
vertex3 Vertex3MultiMatrix33( const vertex3& vtx3, const matrix44& mat33 );

//坐标乘矩阵43（内部转换成齐次坐标再乘）
vertex3 Vertex3MultiMatrix43( const vertex3& vtx3, const matrix44& mat33 );



//-------------------
//其他函数
//-------------------

//计算投影坐标
void hgl_Projection( short* x, short *y, const perspectiveInfo& fun, const vertex3& vtx );

//物体剔除
bool hgl_CullPrimitive( const HGL_Primitive& primitive, const matrix44& transMatrix, const frustum& space );

//初始化渲染列表（引擎初始化时做）
void hgl_InitRenderList( renderList& rl );

//清除渲染列表
void hgl_DestroyRenderList( renderList& rl );

//背面剔除判断
bool hgl_CullBack( triangle* facet );

//光栅化一个线框三角形
void hgl_RasterLineTriangle( const triangle& facet );

//光栅化三角形（实心），使用第一个顶点的颜色作为三角形的颜色
void hgl_RasterTriangle( const triangle& facet );

//光栅化三角形（高洛德着色实心）
void hgl_RasterGouraudTrangle( const triangle& facet );

//光栅化三角形（带纹理）（平坦着色）
void hgl_RasterTrangleTex( const triangle& facet );

//光栅化三角形（带纹理）（高罗德着色）
void hgl_RasterGouraudTrangleTex( const triangle& facet );

//渲染状态初始化
void hgl_InitRenderState();

//光照计算	【未完】
void hgl_LightCalc( triangle* facet, HGLRenderState& state );

//简单光照计算		【未完】
void hgl_SimpleLightCalc( triangle* facet, HGLRenderState& state );

//锁定位图
bool hgl_LockTexture( HGL_Texture* tex );

//解锁位图
bool hgl_UnlockTexture( HGL_Texture* tex );

//颜色采样
void hgl_sampler( HGL_Texture* tex, float u, float v, unsigned char* r, unsigned char* g, unsigned char* b );
