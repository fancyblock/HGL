#pragma once
#include "HGLDefine.h"

//��ʼ����
bool HGL_Inital();

//����һ����Ⱦ���ڣ������ɹ�����true
bool HGL_CreateWnd( const char* name, int wid, int hei, bool fullscreen = false );

//�ݻٴ���(������ڵĻ�)
void HGL_DestroyWnd();

//��ȡ�¼�
bool HGL_GetEvent( HGLEvt* evt );

//���HGL���ü��Ƿ񱻰���
bool HGL_KeyPress( HGLCommonKey key );

//������������ʾ��ǰ̨
void HGL_SwapBuffer();

//������������ݣ���Ⱦ�б����Ҳ������
void HGL_ClearBuffer();

//����͸��ͶӰ
void HGL_Perspective( float fovy, float aspect, float zNear, float zFar );

//���õ�ǰ��������
void HGL_MatrixMode( HGLMatrixType matrixMode );

//���õ�ǰ����Ϊ��λ����
void HGL_LoadIdentity();

//��ǰ�������һ��ƽ�ƾ�����ˣ�
void HGL_Translate( float x, float y, float z );

//��ǰ�������һ����ת������ˣ�
void HGL_Rotate( float angle, float x, float y, float z );

//��ǰ�������һ�����ž�����ˣ�
void HGL_Scale( float x, float y, float z );

//��ǰ�������һ�������������ŵľ�����ˣ�
void HGL_Scale( float x, float y, float z, float k );

//��ʼ�༭һ������
void HGL_Begin( HGL_Primitive& primitive, int count );

//�����༭һ������
void HGL_End();

//����һ������
void HGL_DestroyPrimitive( HGL_Primitive& primitive );

//ָ��һ������λ��
void HGL_Vertex( float x, float y, float z );

//ָ��һ��������ɫ
void HGL_Color( float r, float g, float b );

//ָ��һ�����㷨��
void HGL_Normal( float x, float y, float z );

//ָ��һ������UV
void HGL_Texcoord( float u, float v );

//����һ������
void HGL_DrawPrimitive( HGL_Primitive& primitive, HGLPrimitiveType type );

//�õ���ǰ��Ⱦ�б��еĶ��������
int HGL_GetFacetCount();

//���ö�����޳�ģʽ[��ʱ��Ϊ����]
void HGL_CullMode( HGLCullMode mode );

//���ö���ε����ģʽ
void HGL_FillMode( HGLFillMode mode );

//���ò���
void HGL_SetMatrial( const HGL_Matrial& matrial );

//���ò���
void HGL_ResetMatrial();

//����ĳ������
void HGL_SetLight( HGL_LightNum lightIndex, HGL_Light* lightInfo );

//���ü򵥹���
void HGL_SetSimpleLight( HGL_SimpleLightNum lightIndex, HGL_SimpleLight* lightInfo );

//����ĳЩ��Ⱦѡ��	��δ�꡿
void HGL_Enable( HGL_RenderOption option );

//�ر�ĳЩ��Ⱦѡ��	��δ�꡿
void HGL_Disable( HGL_RenderOption option );

//������ɫģʽ
void HGL_ShadeMode( HGL_ShadeModel sm );

//��������
bool HGL_LoadTexture( HGL_Texture& tex, char* fileName );

//������
void HGL_BindTexture( HGL_TextureLevel num, HGL_Texture* tex );
