#pragma once
#include "HGLDefine.h"


///////////////////////////////////////////////
//����ĺ���������ʹ��HGL���û����ţ��ڲ�ʹ��//
///////////////////////////////////////////////


//---------------
//������ͼ����
//---------------


//#define BRESENHAM_LINE		//ʹ��Bresenham�����㷨��δ�꡿
#define DDA_LINE				//ʹ��DDA�����㷨

//��������
void hgl_BeginDraw();

//��������
void hgl_EndDraw();

//д���棨���㣩
void hgl_DrawPixel( int x, int y, char r, char g, char b );

//�ü��ߣ�����true���������߶����õ��ˣ�	��δ�꡿
bool hgl_ClipLine( int& x1, int y1, int x2, int y2 );

//д���棨���ߣ�
void hgl_DrawLine( int x1, int y1, int x2, int y2, char r, char g, char b );



//---------------
//��ѧ����
//---------------


////----------������----------////

//3d��������
vertex3 Vertex3( float x, float y, float z );

//4d��������
vertex4 Vertex4( float x, float y, float z, float w );

//�������ת��Ϊ3D����
vertex3 Vertex4To3( const vertex4& vtx4 );

//3D����ת��Ϊ�������
vertex4 Vertex3To4( const vertex3& vtx3 );

//����ȡģ
float Vertex3Length( const vertex3& vtx3 );

//��Ϊ������
void Vertex3Zero( vertex3& vtx3 );

//��ɵ�λ����
void Vertex3Normalize( vertex3& vtx3 );

//�����ӷ�
vertex3 Vertex3Add( const vertex3& vtx31, const vertex3& vtx32 );

//��������
vertex3 Vertex3Sub( const vertex3& vtx31, const vertex3& vtx32 );

//������
float Vertex3Distance( const vertex3& vtx31, const vertex3& vtx32 );

//����ȡ��
void Vertex3Negate( vertex3& vtx3 );

//������
vertex3 Vertex3MutiScalar( const vertex3& vtx3, const float scalar );

//������
vertex3 Vertex3DivScalar( const vertex3& vtx3, const float scalar );

//�������
float Vertex3DotProduct( const vertex3& vtx31, const vertex3& vtx32 );

//�������
vertex3 Vertex3CorssProduct( const vertex3& vtx31, const vertex3& vtx32 );


////----------ƽ����----------////

//������������ƽ��
plane GetPlane( const vertex3& vtx1, const vertex3& vtx2, const vertex3& vtx3 );


////----------����ƽ����----------////

//�����Ƿ���ƽ��ǰ��
bool IsFront( const vertex3& vtx3, const plane& pln );

//���Ƿ���ƽ��ǰ�棨�ཻ���㣩
bool IsFront( const vertex3& vtx3, float radius, const plane& pln );


////----------������----------////

//44�����ɵ�λ����
void Matrix44Identity( matrix44& mat44 );

//44�������
matrix44 Matrix44Multi( const matrix44& mat1, const matrix44& mat2 );

//����һ��ƽ�ƾ���
void Matrix44Translate( matrix44& mat44, float x, float y, float z );

//����һ�����ž���
void Matrix44Scale( matrix44& mat44, float xScale, float yScale, float zScale );

//����һ����ָ�������ŵľ���
void Matrix44ScaleAxis( matrix44& mat44, const vertex3& axis, float scale );

//����һ����X����ת���󣨻����ƣ�
void Matrix44RotateX( matrix44& mat44, float angle );

//����һ����Y����ת���󣨻����ƣ�
void Matrix44RotateY( matrix44& mat44, float angle );

//����һ����Z����ת���󣨻����ƣ�
void Matrix44RotateZ( matrix44& mat44, float angle );

//����һ������������ת�ľ��󣨻����ƣ�
void Matrix44Rotate( matrix44& mat44, float ax, float ay, float az, float angle );


////----------����������----------////

//�������˾���44
vertex4 Vertex4MultiMatrix44( const vertex4& vtx4, const matrix44& mat44 );

//�������˾���43
vertex3 Vertex4MultiMatrix43( const vertex4& vtx4, const matrix44& mat43 );

//����˾���33�����ڷ��ߵı任����Ϊ��ֻ��Ҫ��ת����Ҫƽ�ƣ�
vertex3 Vertex3MultiMatrix33( const vertex3& vtx3, const matrix44& mat33 );

//����˾���43���ڲ�ת������������ٳˣ�
vertex3 Vertex3MultiMatrix43( const vertex3& vtx3, const matrix44& mat33 );



//-------------------
//��������
//-------------------

//����ͶӰ����
void hgl_Projection( short* x, short *y, const perspectiveInfo& fun, const vertex3& vtx );

//�����޳�
bool hgl_CullPrimitive( const HGL_Primitive& primitive, const matrix44& transMatrix, const frustum& space );

//��ʼ����Ⱦ�б������ʼ��ʱ����
void hgl_InitRenderList( renderList& rl );

//�����Ⱦ�б�
void hgl_DestroyRenderList( renderList& rl );

//�����޳��ж�
bool hgl_CullBack( triangle* facet );

//��դ��һ���߿�������
void hgl_RasterLineTriangle( const triangle& facet );

//��դ�������Σ�ʵ�ģ���ʹ�õ�һ���������ɫ��Ϊ�����ε���ɫ
void hgl_RasterTriangle( const triangle& facet );

//��դ�������Σ��������ɫʵ�ģ�
void hgl_RasterGouraudTrangle( const triangle& facet );

//��դ�������Σ���������ƽ̹��ɫ��
void hgl_RasterTrangleTex( const triangle& facet );

//��դ�������Σ������������޵���ɫ��
void hgl_RasterGouraudTrangleTex( const triangle& facet );

//��Ⱦ״̬��ʼ��
void hgl_InitRenderState();

//���ռ���	��δ�꡿
void hgl_LightCalc( triangle* facet, HGLRenderState& state );

//�򵥹��ռ���		��δ�꡿
void hgl_SimpleLightCalc( triangle* facet, HGLRenderState& state );

//����λͼ
bool hgl_LockTexture( HGL_Texture* tex );

//����λͼ
bool hgl_UnlockTexture( HGL_Texture* tex );

//��ɫ����
void hgl_sampler( HGL_Texture* tex, float u, float v, unsigned char* r, unsigned char* g, unsigned char* b );
