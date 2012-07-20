// 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "HGL.h"
#include "HGLx.h"

unsigned long g_time;
int g_FrameCount = 0;


int _tmain(int argc, _TCHAR* argv[])
{
	HGL_Inital();
	HGL_CreateWnd( "HGL_Test", 400, 300 );
	HGL_Perspective( 90.0f, 400.0f/300.0f, 0.1f, 100.0f );
	HGL_CullMode( eHGLFrontCull );	//正面剔除
	HGL_FillMode( eHGLSolidMode );

	//使用简单光照
	HGL_Enable( HGL_SIMPLELIGHT );
	HGL_SimpleLight light;
	//设置一个简单光的方向光
	light._power = 0.66f;
	light._type = eHGLLightDirectional;
	light._direction._x = 0;
	light._direction._y = 0;
	light._direction._z = 1.0f;
	HGL_SetSimpleLight( HGL_SimpleLightDiffuse1, &light );

	//创建一个尖头立方体
	HGL_Primitive cube;
	HGL_Begin( cube, 36 );//36个顶点
	HGL_Color( 0.8f, 0.4f, 0.1f );
	//前
	HGL_Normal( 0, 0, -1 );
	HGL_Texcoord( 0, 0 );
	HGL_Vertex( -1, 1, -1 );
	HGL_Texcoord( 1.0f, 0 );
	HGL_Vertex( 1, 1, -1 );
	HGL_Texcoord( 1.0f, 1.0f );
	HGL_Vertex( 1, -1, -1 );
	HGL_Texcoord( 1.0f, 1.0f );
	HGL_Vertex( 1, -1, -1 );
	HGL_Texcoord( 0, 1.0f );
	HGL_Vertex( -1, -1, -1 );
	HGL_Texcoord( 0, 0 );
	HGL_Vertex( -1, 1, -1 );
	//左
	HGL_Normal( -1, 0, 0 );
	HGL_Texcoord( 0, 0 );
	HGL_Vertex( -1, 1, 1 );
	HGL_Texcoord( 1.0f, 0 );
	HGL_Vertex( -1, 1, -1 );
	HGL_Texcoord( 1.0f, 1.0f );
	HGL_Vertex( -1, -1, -1 );
	HGL_Texcoord( 1.0f, 1.0f );
	HGL_Vertex( -1, -1, -1 );
	HGL_Texcoord( 0, 1.0f );
	HGL_Vertex( -1, -1, 1 );
	HGL_Texcoord( 0, 0 );
	HGL_Vertex( -1, 1, 1 );
	//后
	HGL_Normal( 0, 0, 1 );
	HGL_Texcoord( 0, 0 );
	HGL_Vertex( 1, 1, 1 );
	HGL_Texcoord( 1.0f, 0 );
	HGL_Vertex( -1, 1, 1 );
	HGL_Texcoord( 1.0f, 1.0f );
	HGL_Vertex( -1, -1, 1 );
	HGL_Texcoord( 1.0f, 1.0f );
	HGL_Vertex( -1, -1, 1 );
	HGL_Texcoord( 0, 1.0f );
	HGL_Vertex( 1, -1, 1 );
	HGL_Texcoord( 0, 0 );
	HGL_Vertex( 1, 1, 1 );
	//右
	HGL_Normal( 1, 0, 0 );
	HGL_Texcoord( 0, 0 );
	HGL_Vertex( 1, 1, -1 );
	HGL_Texcoord( 1.0f, 0 );
	HGL_Vertex( 1, 1, 1 );
	HGL_Texcoord( 1.0f, 1.0f );
	HGL_Vertex( 1, -1, 1 );
	HGL_Texcoord( 1.0f, 1.0f );
	HGL_Vertex( 1, -1, 1 );
	HGL_Texcoord( 0, 1.0f );
	HGL_Vertex( 1, -1, -1 );
	HGL_Texcoord( 0, 0 );
	HGL_Vertex( 1, 1, -1 );
	//下
	HGL_Normal( 0, -1, 0 );
	HGL_Texcoord( 0, 0 );
	HGL_Vertex( -1, -1, -1 );
	HGL_Texcoord( 1.0f, 0 );
	HGL_Vertex( 1, -1, -1 );
	HGL_Texcoord( 1.0f, 1.0f );
	HGL_Vertex( 1, -1, 1 );
	HGL_Texcoord( 1.0f, 1.0f );
	HGL_Vertex( 1, -1, 1 );
	HGL_Texcoord( 0, 1.0f );
	HGL_Vertex( -1, -1, 1 );
	HGL_Texcoord( 0, 0 );
	HGL_Vertex( -1, -1, -1 );
	//上
	HGL_Normal( 0, 1, 0 );
	HGL_Texcoord( 0, 0 );
	HGL_Vertex( -1, 1, 1 );
	HGL_Texcoord( 1.0f, 0 );
	HGL_Vertex( 1, 1, 1 );
	HGL_Texcoord( 1.0f, 1.0f );
	HGL_Vertex( 1, 1, -1 );
	HGL_Texcoord( 1.0f, 1.0f );
	HGL_Vertex( 1, 1, -1 );
	HGL_Texcoord( 0, 1.0f );
	HGL_Vertex( -1, 1, -1 );
	HGL_Texcoord( 0, 0 );
	HGL_Vertex( -1, 1, 1 );
	HGL_End();

	float angle = 0;

	//启用纹理映射
	HGL_Enable( HGL_TEXTURE );
	HGL_Texture tex1;
	HGL_Texture tex2;
	HGL_LoadTexture( tex1, "kid.bmp" );
	HGL_LoadTexture( tex2, "nobita.bmp" );
	HGL_BindTexture( HGL_Tex0, &tex1 );

//	HGL_ShadeMode( HGL_ShadeModel::HGL_GOURAUD );

	//
	HGLEvt evt;
	bool running = true;

	float pitch = 0.0f;
	float yaw = 0.0f;

	while( running )
	{
		angle += 0.001f;

		if( HGL_GetEvent( &evt ) )
		{
			//事件处理
			if( evt.m_evtType == eHGLQuit )
			{
				running = false;
			}
		}else
		{
			//空闲时渲染
			HGL_ClearBuffer();
			HGL_MatrixMode( eHGLWorldMatrix );
				HGL_LoadIdentity();
				HGL_Translate( 0,0,10 );
				HGL_Rotate( angle, 0.4f, 1.0f, 0 );
				HGL_DrawPrimitive( cube, eHGLPrimitiveTriangle );
			HGL_SwapBuffer();

			if( HGL_KeyPress( HGLCommonKey::eHGL_SPACE ) )
			{
				HGL_BindTexture( HGL_Tex0, &tex2 );
			}else
			{
				HGL_BindTexture( HGL_Tex0, &tex1 );
			}

			HGL_MatrixMode( eHGLCameraMatrix );
			HGL_LoadIdentity();
			HGL_Rotate( yaw, 0.0f, 1.0f, 0.0f );
			HGL_Rotate( pitch, 1.0f, 0, 0 );

			if( HGL_KeyPress( eHGL_RIGHT ) )
			{
				yaw += 0.0001f;
			}
			if( HGL_KeyPress( eHGL_LEFT) )
			{
				yaw -= 0.0001f;
			}
			if( HGL_KeyPress( eHGL_UP) )
			{
				pitch += 0.0001f;
			}
			if( HGL_KeyPress( eHGL_DOWN) )
			{
				pitch -= 0.0001f;
			}

			//统计帧数等
			g_FrameCount++;
			long t = (long)time(0);
			if( t - g_time >= 5 )
			{
				printf( "FPS: %.2f\n", (float)g_FrameCount / 5.0f );
				printf( "Triangle Count: %d\n", HGL_GetFacetCount() );
				g_FrameCount = 0;
				g_time = t;
			}
			if( HGL_KeyPress( eHGL_ESC ) )
			{
				running = false;
			}
		}
	}

	HGL_DestroyWnd();
	return 0;
}
