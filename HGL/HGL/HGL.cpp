#include "stdafx.h"
#include "HGL.h"
#include "HGLInnerFun.h"	//HGL内部功能函数


//本引擎所处状态
HGLState g_HGLState = eHGLNon;
//被渲染的窗口
SDL_Surface* g_RenderWnd = NULL;

//-----------------------------------------//

//透视投影信息
perspectiveInfo g_Perspective;

//视锥体（用于物体剔除、多边形裁剪）
frustum g_ViewFrustum;

//当前矩阵标志
HGLMatrixType g_currentMatrix;

//当前编辑中的物体
EditPrimitive g_CurrentPrimitive;

//渲染列表
renderList g_RenderList;

//标准顶点（新的顶点未指定法线、颜色等就延续该顶点的）
Vertex g_standardVertex;

//渲染模式等一些状态
HGLRenderState g_RenderState;



////////////
//初始化库//
////////////
bool HGL_Inital()
{
	int info= SDL_WasInit( SDL_INIT_VIDEO );

	if( info & SDL_INIT_VIDEO )
	{
		//已经初始化过了
		return false;
	}

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		//初始化失败
		return false;
	}

	//初始化成功，设置状态标志
	g_HGLState = eHGLInit;

	//一些全局变量的默认值
	g_currentMatrix = eHGLWorldMatrix;

	//初始化渲染列表
	hgl_InitRenderList( g_RenderList );

	//渲染状态初始化
	hgl_InitRenderState();

	return true;
}


//////////////////////////////////////
//创建一个渲染窗口，创建成功返回true//
//////////////////////////////////////
bool HGL_CreateWnd( const char* name, int wid, int hei, bool fullscreen )
{
	if( g_HGLState != eHGLInit )
	{
		//窗口已存在
		return false;
	}

	//窗口宽高传递给透视投影结构
	g_Perspective._screenWid = wid;
	g_Perspective._screenHei = hei;

	if( fullscreen )
	{
		g_RenderWnd = SDL_SetVideoMode( wid, hei, PIXCEL_DEPTH, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN );
	}else
	{
		g_RenderWnd = SDL_SetVideoMode( wid, hei, PIXCEL_DEPTH, SDL_HWSURFACE | SDL_DOUBLEBUF );
	}

	if( g_RenderWnd )
	{
		//创建成功，设置窗口名和标志
		SDL_WM_SetCaption( name, NULL );
		g_HGLState = eHGLWnd;
	}else
	{
		return false;
	}

	return true;
}


////////////////////////
//将缓冲内容显示到前台//
////////////////////////
void HGL_SwapBuffer()
{
	//根据找色模式选择不同的光栅化函数
	void (*Raster)( const triangle& trg );

	if( g_RenderState.m_FillMode == eHGLLineMode )	//线框模式
	{
		Raster = hgl_RasterLineTriangle;
	}else
	//实心模式
	{
		if( g_RenderState.m_shadeMode == HGL_FLAT )							//平坦着色
		{
			if( g_RenderState.m_TexMode == HGL_TexDisable )//纹理映射未开启
			{
				Raster = hgl_RasterTriangle;			//---【未完，待修改】
			}else
			{
				Raster = hgl_RasterTrangleTex;
			}
		}else if( g_RenderState.m_shadeMode == HGL_GOURAUD )				//高洛德着色
		{
			if( g_RenderState.m_TexMode == HGL_TexDisable )//纹理映射未开启
			{
				Raster = hgl_RasterGouraudTrangle;		//---【尚未实现】
			}else
			{
				Raster = hgl_RasterGouraudTrangleTex;
			}
		}
	}

	//开启纹理映射的话，贴图准备
	if( g_RenderState.m_TexMode == HGL_TexEnable )
	{
		if( g_RenderState.m_texture[HGL_Tex0] )
		{
			//贴图锁定失败，更换实心光栅化函数
			if( hgl_LockTexture( g_RenderState.m_texture[HGL_Tex0] ) == false )
			{
				Raster = hgl_RasterTriangle;
			}
		}
	}

	//将渲染列表里的东西画到屏幕上去
	triangle* facet = g_RenderList.triList->next;

	//遍历渲染列表来画三角形
	hgl_BeginDraw();
	for( int i(0); i < g_RenderList.triangleCount; i++ )
	{
		Raster( *facet );
		facet = facet->next;
	}
	hgl_EndDraw();

	//解除贴图锁定
	if( g_RenderState.m_texture[HGL_Tex0] )
	{
		hgl_UnlockTexture( g_RenderState.m_texture[HGL_Tex0] );
	}

	SDL_Flip( g_RenderWnd );
}


//////////////////////////
//摧毁窗口(如果存在的话)//
//////////////////////////
void HGL_DestroyWnd()
{
	//如果窗口存在则摧毁之
	if( g_RenderWnd )
	{
		//销毁渲染列表
		hgl_DestroyRenderList( g_RenderList );

		//窗口存在
		SDL_FreeSurface( g_RenderWnd );
		
		//设置状态标志
		g_HGLState = eHGLInit;

	}
}


//////////////////
//清除缓冲区内容//
//////////////////
void HGL_ClearBuffer()
{
	SDL_FillRect( g_RenderWnd, NULL, 0 );

	//渲染列表要重置
	g_RenderList.curTriangle = g_RenderList.triList;
	g_RenderList.triangleCount = 0;

}


////////////	【未完】
//获取事件//
////////////
bool HGL_GetEvent( HGLEvt* evt )
{
	SDL_Event sdlEvt;

	if( SDL_PollEvent( &sdlEvt ) )
	{
		switch( sdlEvt.type )
		{
		case SDL_QUIT:									//窗口退出事件
			evt->m_evtType = eHGLQuit;
			break;
		case SDL_ACTIVEEVENT:							//窗口活动事件
			evt->m_evtType = eHGLWndActive;
			break;
		case SDL_KEYDOWN:								//键盘按下事件
			evt->m_evtType = eHGLKeyboardDown;
			break;
		case SDL_KEYUP:									//键盘抬起事件
			evt->m_evtType = eHGLKeyboardUp;
			break;
		case SDL_MOUSEMOTION:							//鼠标移动（没有该事件，只是用来跟踪鼠标位置）
			evt->m_mousePos._x = sdlEvt.motion.x;
			evt->m_mousePos._y = sdlEvt.motion.y;
			break;
		case SDL_MOUSEBUTTONDOWN:						//鼠标按下事件
			evt->m_evtType = eHGLMouseDown;
			;
			break;
		case SDL_MOUSEBUTTONUP:							//鼠标抬起事件
			evt->m_evtType = eHGLMouseUp;
			break;
		case SDL_SYSWMEVENT:							//系统事件
			evt->m_evtType = eHGLSystem;
			break;
		case SDL_USEREVENT:								//用户事件
			evt->m_evtType = eHGLUser;
			break;
		default:
			break;
		}

		return true;
	}

	return false;
}


///////////////////////////
//检测HGL常用键是否被按下//
///////////////////////////
bool HGL_KeyPress( HGLCommonKey key )
{
	Uint8 *keystate = SDL_GetKeyState( NULL );

	if( keystate[key] )
	{
		return true;
	}

	return false;
}


////////////////
//设置透视投影//
////////////////
void HGL_Perspective( float fovy, float aspect, float zNear, float zFar )
{
	if( g_HGLState < eHGLWnd )
	{
		return;
	}

	//视野角度为角度制，合法区间为（0，180）
	if( fovy <= 0 || fovy >= 180.0f )
	{
		return;
	}

	//宽高比需大于等于0
	if( aspect <= 0 )
	{
		return;
	}

	//远近剪裁面需大于0
	if( zNear <= 0 || zFar <= 0 )
	{
		return;
	}

	//设置透视信息
	g_Perspective._fovy = fovy;
	g_Perspective._aspect = aspect;

	const double PI = 3.14159265f;
	//根据视野角度算出D值（使坐标转换后位于（-1，1）之间）
	float d = float( 1.0 / tan( (fovy * 0.5 * PI )/180.0 ) );

	//计算透视计算要用到的数
	g_Perspective.mXoffset = g_Perspective._screenWid * 0.5f + 0.5f;
	g_Perspective.mYoffset = g_Perspective._screenHei * 0.5f + 0.5f;
	g_Perspective.mXfactor = d * g_Perspective.mXoffset;
	g_Perspective.mYfactor = d * aspect * ( -g_Perspective.mYoffset );

	//透视投影设置完毕，可以开始绘制物体了
	g_HGLState = eHGLReady;

	//矩阵初始化
	Matrix44Identity( g_RenderState.m_WorldMatrix );
	Matrix44Identity( g_RenderState.m_CameraMatrix );

	//计算视椎体的6个面（用于剔除和剪裁）、
	//视椎的正面朝外，也就是用IsFront判断时，如果返回true则是表示在视椎体外看不见的
	float farXY = zFar / d;
	//视椎体上下和左右的视角是一样的，farXY即为在远剪裁面上的最边缘的点的X和Y值
	vertex3 origin = Vertex3( 0.0f, 0.0f, 0.0f );					//原点
	vertex3 topLeft = Vertex3( -farXY, farXY, zFar );				//视椎体最远左上点
	vertex3 topRight = Vertex3( farXY, farXY, zFar );				//视椎体最远右上点
	vertex3 botLeft = Vertex3( -farXY, -farXY, zFar );				//视椎体最远左下点
	vertex3 botRight = Vertex3( farXY, -farXY, zFar );				//视椎体最远右下点

	g_ViewFrustum._farZ = zFar;
	g_ViewFrustum._nearZ = zNear;
	g_ViewFrustum._left = GetPlane( origin, botLeft, topLeft );
	g_ViewFrustum._right = GetPlane( origin, topRight, botRight );
	g_ViewFrustum._top = GetPlane( origin, topLeft, topRight );
	g_ViewFrustum._bottom = GetPlane( origin, botRight, botLeft );

}


////////////////////
//设置当前矩阵类型//
////////////////////
void HGL_MatrixMode( HGLMatrixType matrixMode )
{
	g_currentMatrix = matrixMode;
}


//////////////////////////
//设置当前矩阵为单位矩阵//
//////////////////////////
void HGL_LoadIdentity()
{
	switch( g_currentMatrix )
	{
	case eHGLWorldMatrix:	//重置世界矩阵
		Matrix44Identity( g_RenderState.m_WorldMatrix );
		break;
	case eHGLCameraMatrix:	//重置Camera矩阵
		Matrix44Identity( g_RenderState.m_CameraMatrix );
		break;
	default:
		break;
	}

	g_RenderState.m_ModelView = Matrix44Multi( g_RenderState.m_WorldMatrix, g_RenderState.m_CameraMatrix );

}


////////////////////////////////////
//当前矩阵乘以一个平移矩阵（左乘）//
////////////////////////////////////
void HGL_Translate( float x, float y, float z )
{
	matrix44 translateMatrix;

	switch( g_currentMatrix )
	{
	case eHGLWorldMatrix:	//世界矩阵
		Matrix44Translate( translateMatrix, x, y, z );
		g_RenderState.m_WorldMatrix = Matrix44Multi( translateMatrix, g_RenderState.m_WorldMatrix );
		break;
	case eHGLCameraMatrix:	//Camera矩阵
		Matrix44Translate( translateMatrix, -x, -y, -z );
		g_RenderState.m_CameraMatrix = Matrix44Multi( g_RenderState.m_CameraMatrix, translateMatrix );
		break;
	default:
		break;
	}

	g_RenderState.m_ModelView = Matrix44Multi( g_RenderState.m_WorldMatrix, g_RenderState.m_CameraMatrix );

}


////////////////////////////////////
//当前矩阵乘以一个旋转矩阵（左乘）//
////////////////////////////////////
void HGL_Rotate( float angle, float x, float y, float z )
{
	matrix44 rotateMatrix;

	switch( g_currentMatrix )
	{
	case eHGLWorldMatrix:	//世界矩阵
		Matrix44Rotate( rotateMatrix, x, y, z, angle );
		g_RenderState.m_WorldMatrix = Matrix44Multi( rotateMatrix, g_RenderState.m_WorldMatrix );
		break;
	case eHGLCameraMatrix:	//Camera矩阵
		Matrix44Rotate( rotateMatrix, x, y, z, -angle );
		g_RenderState.m_CameraMatrix = Matrix44Multi( g_RenderState.m_CameraMatrix, rotateMatrix );
		break;
	default:
		break;
	}

	g_RenderState.m_ModelView = Matrix44Multi( g_RenderState.m_WorldMatrix, g_RenderState.m_CameraMatrix );

}


////////////////////////////////////
//当前矩阵乘以一个缩放矩阵（左乘）//	Camera矩阵的话右乘
////////////////////////////////////
void HGL_Scale( float x, float y, float z )
{
	//世界矩阵才能应用此变换
	matrix44 scaleMatrix;

	if( g_currentMatrix == eHGLWorldMatrix )
	{
		Matrix44Scale( scaleMatrix, x, y, z );
		g_RenderState.m_WorldMatrix = Matrix44Multi( scaleMatrix, g_RenderState.m_WorldMatrix );
	}

	g_RenderState.m_ModelView = Matrix44Multi( g_RenderState.m_WorldMatrix, g_RenderState.m_CameraMatrix );

}


//////////////////////////////////////////////
//当前矩阵乘以一个沿任意轴缩放的矩阵（左乘）//	Camera矩阵的话右乘
//////////////////////////////////////////////
void HGL_Scale( float x, float y, float z, float k )
{
	//世界矩阵才能应用此变换
	matrix44 scaleMatrix;

	if( g_currentMatrix == eHGLWorldMatrix )
	{
		Matrix44ScaleAxis( scaleMatrix, Vertex3( x, y, z ), k );
		g_RenderState.m_WorldMatrix = Matrix44Multi( scaleMatrix, g_RenderState.m_WorldMatrix );
	}

	g_RenderState.m_ModelView = Matrix44Multi( g_RenderState.m_WorldMatrix, g_RenderState.m_CameraMatrix );

}


////////////////////
//开始编辑一个物体//
////////////////////
void HGL_Begin( HGL_Primitive& primitive, int count )
{
	//参数2为顶点数
	HGL_DestroyPrimitive( primitive );

	primitive.vertexCount = count;
	primitive.buffer = new Vertex[count];

	g_CurrentPrimitive.exist = true;
	g_CurrentPrimitive.curPrimitive = &primitive;
	g_CurrentPrimitive.vertexIndex = 0;
	g_CurrentPrimitive.curPrimitive->radius = 0.0f;

}


////////////////////
//结束编辑一个物体//
////////////////////
void HGL_End()
{
	g_CurrentPrimitive.exist = false;
}


////////////////
//销毁一个物体//
////////////////
void HGL_DestroyPrimitive( HGL_Primitive& primitive )
{
	if( primitive.buffer )
	{
		delete[] primitive.buffer;
		primitive.buffer = NULL;

		primitive.vertexCount = 0;

	}
}


////////////////////
//指定一个顶点位置//
////////////////////
void HGL_Vertex( float x, float y, float z )
{
	//标志判断，保证只在HGL_Begin和HGL_End之间的调用才有效
	if( !g_CurrentPrimitive.exist )
	{
		return;
	}

	Vertex* pt;
	pt = &g_CurrentPrimitive.curPrimitive->buffer[g_CurrentPrimitive.vertexIndex];

	g_standardVertex._position._x = x;
	g_standardVertex._position._y = y;
	g_standardVertex._position._z = z;

	*pt = g_standardVertex;

	//计算该顶点到中心的距离
	float distance = Vertex3Length( pt->_position );

	//如果大于原来的物体半径则更新物体半径
	if( distance > g_CurrentPrimitive.curPrimitive->radius )
	{
		g_CurrentPrimitive.curPrimitive->radius = distance;
	}

	//防止溢出
	if( g_CurrentPrimitive.vertexIndex < g_CurrentPrimitive.curPrimitive->vertexCount - 1 )
	{
		g_CurrentPrimitive.vertexIndex++;
	}

}


////////////////////
//指定一个顶点颜色//
////////////////////
void HGL_Color( float r, float g, float b )
{
	//标志判断，保证只在HGL_Begin和HGL_End之间的调用才有效
	if( !g_CurrentPrimitive.exist )
	{
		return;
	}

	//防止越界
	if( r < 0 )
	{
		r = 0;
	}

	if( r > 1.0f )
	{
		r = 1.0f;
	}

	if( g < 0 )
	{
		g = 0;
	}

	if( g > 1.0f )
	{
		g = 1.0f;
	}

	if( b < 0 )
	{
		b = 0;
	}

	if( r > 1.0f )
	{
		r = 1.0f;
	}

	Vertex* pt;
	pt = &g_CurrentPrimitive.curPrimitive->buffer[g_CurrentPrimitive.vertexIndex];

	g_standardVertex._color._r = int( r * 255.0f );
	g_standardVertex._color._g = int( g * 255.0f );
	g_standardVertex._color._b = int( b * 255.0f );

	pt->_color = g_standardVertex._color;

}


////////////////////
//指定一个顶点法线//
////////////////////
void HGL_Normal( float x, float y, float z )
{
	//标志判断，保证只在HGL_Begin和HGL_End之间的调用才有效
	if( !g_CurrentPrimitive.exist )
	{
		return;
	}

	Vertex* pt;
	pt = &g_CurrentPrimitive.curPrimitive->buffer[g_CurrentPrimitive.vertexIndex];

	g_standardVertex._normal._x = x;
	g_standardVertex._normal._y = y;
	g_standardVertex._normal._z = z;

	pt->_normal = g_standardVertex._normal;

}


//////////////////
//指定一个顶点UV//
//////////////////
void HGL_Texcoord( float u, float v )
{
	//标志判断，保证只在HGL_Begin和HGL_End之间的调用才有效
	if( !g_CurrentPrimitive.exist )
	{
		return;
	}

	Vertex* pt;
	pt = &g_CurrentPrimitive.curPrimitive->buffer[g_CurrentPrimitive.vertexIndex];

	g_standardVertex._uv._x = u;
	g_standardVertex._uv._y = v;

	pt->_uv = g_standardVertex._uv;

}


////////////////
//绘制一个物体//
////////////////
void HGL_DrawPrimitive( HGL_Primitive& primitive, HGLPrimitiveType type )
{
	//如果该物体被剔除，则不再进行下去
	if( hgl_CullPrimitive( primitive, g_RenderState.m_ModelView, g_ViewFrustum ) )
	{
		return;
	}

	int i;
	short x, y;
	vertex3 vtx;
	int vertexCount;

	////画三角形（加入渲染列表统一渲染）
	//----------------------------------
	if( type == eHGLPrimitiveTriangle )
	{
		//调整顶点数量，为小于等于原顶点数的最大的3的倍数
		vertexCount = primitive.vertexCount;
		vertexCount -= vertexCount % 3;

		//顶点变换
		for( i = 0; i < vertexCount; i += 3 )
		{
			//渲染列表不够，扩充节点
			if( !g_RenderList.curTriangle->next )
			{
				g_RenderList.curTriangle->next = new triangle();
				g_RenderList.curTriangle->next->next = NULL;
			}

			//模型内的三角形加入渲染列表
			g_RenderList.curTriangle->next->_vtx[0] = primitive.buffer[i];
			g_RenderList.curTriangle->next->_vtx[1] = primitive.buffer[i+1];
			g_RenderList.curTriangle->next->_vtx[2] = primitive.buffer[i+2];

			//坐标变换
			g_RenderList.curTriangle->next->_vtx[0]._position = Vertex3MultiMatrix43( g_RenderList.curTriangle->next->_vtx[0]._position, g_RenderState.m_ModelView );
			g_RenderList.curTriangle->next->_vtx[1]._position = Vertex3MultiMatrix43( g_RenderList.curTriangle->next->_vtx[1]._position, g_RenderState.m_ModelView );
			g_RenderList.curTriangle->next->_vtx[2]._position = Vertex3MultiMatrix43( g_RenderList.curTriangle->next->_vtx[2]._position, g_RenderState.m_ModelView );

			//这里做背面剔除
			if( g_RenderState.m_CullMode )
			{
				if( hgl_CullBack( g_RenderList.curTriangle->next ) )
				{
					if( g_RenderState.m_CullMode == eHGLBackCull )
					{
						continue;
					}
				}else
				{
					if( g_RenderState.m_CullMode == eHGLFrontCull )
					{
						continue;
					}
				}
			}

			//平坦着色模式的话只要变换一个法线，并将其作为面法线
			if( g_RenderState.m_shadeMode == HGL_FLAT )
			{
				//使用三角形的第一个顶点的法线作为面法线
				g_RenderList.curTriangle->next->_normal = Vertex3MultiMatrix33( g_RenderList.curTriangle->next->_vtx[0]._normal, g_RenderState.m_ModelView );

			}else
			//否则就对3个顶点的法线都进行变换
			{
				g_RenderList.curTriangle->next->_vtx[0]._normal = Vertex3MultiMatrix33( g_RenderList.curTriangle->next->_vtx[0]._normal, g_RenderState.m_ModelView );
				g_RenderList.curTriangle->next->_vtx[1]._normal = Vertex3MultiMatrix33( g_RenderList.curTriangle->next->_vtx[1]._normal, g_RenderState.m_ModelView );
				g_RenderList.curTriangle->next->_vtx[2]._normal = Vertex3MultiMatrix33( g_RenderList.curTriangle->next->_vtx[2]._normal, g_RenderState.m_ModelView );

			}

			//使用三角形的第一个顶点的颜色作为面颜色
			g_RenderList.curTriangle->next->_color = g_RenderList.curTriangle->next->_vtx[0]._color;

			//光照计算（包含法线的变换），如果光照开启才进行
			if( g_RenderState.m_Lighting )
			{
				hgl_LightCalc( g_RenderList.curTriangle->next, g_RenderState );
			}else
			//简单光照计算
			if( g_RenderState.m_SimpleLighting )
			{
				hgl_SimpleLightCalc( g_RenderList.curTriangle->next, g_RenderState );
			}else
			//没有光照
			{
				//顶点的光照强度都设为1
				g_RenderList.curTriangle->next->_vtx[0]._power = 1.0f;
				g_RenderList.curTriangle->next->_vtx[1]._power = 1.0f;
				g_RenderList.curTriangle->next->_vtx[2]._power = 1.0f;
			}

			//透视计算
			hgl_Projection( &g_RenderList.curTriangle->next->_vtx[0]._pixel._x,
							&g_RenderList.curTriangle->next->_vtx[0]._pixel._y,
							g_Perspective, g_RenderList.curTriangle->next->_vtx[0]._position );
			hgl_Projection( &g_RenderList.curTriangle->next->_vtx[1]._pixel._x,
							&g_RenderList.curTriangle->next->_vtx[1]._pixel._y,
							g_Perspective, g_RenderList.curTriangle->next->_vtx[1]._position );
			hgl_Projection( &g_RenderList.curTriangle->next->_vtx[2]._pixel._x,
							&g_RenderList.curTriangle->next->_vtx[2]._pixel._y,
							g_Perspective, g_RenderList.curTriangle->next->_vtx[2]._position );

			//一个三角形处理完成，渲染列表更新
			g_RenderList.curTriangle = g_RenderList.curTriangle->next;
			g_RenderList.triangleCount++;
		}

	}else
	////画线模式
	//----------
	if( type == eHGLPrimitiveLine )
	{
		//防止读溢出，顶点数量不为偶数时减掉一个
		if( primitive.vertexCount % 2 == 0 )
		{
			vertexCount = primitive.vertexCount;
		}else
		{
			vertexCount = primitive.vertexCount - 1;
		}

		//顶点变换
		for( i = 0; i < vertexCount; i++ )
		{
			//坐标变换
			vtx = Vertex3MultiMatrix43( primitive.buffer[i]._position, g_RenderState.m_ModelView );
			//投影变换
			hgl_Projection( &primitive.buffer[i]._pixel._x, &primitive.buffer[i]._pixel._y, g_Perspective, vtx );
		}

		//绘制
		hgl_BeginDraw();
		for( i = 0; i < vertexCount; i += 2 )
		{
			//画线
			hgl_DrawLine( primitive.buffer[i]._pixel._x, primitive.buffer[i]._pixel._y,
				primitive.buffer[i+1]._pixel._x, primitive.buffer[i+1]._pixel._y, 
				primitive.buffer[i]._color._r, primitive.buffer[i]._color._g, 
				primitive.buffer[i]._color._b );
		}
		hgl_EndDraw();

	}else
	////画点模式
	//----------
	if( type == eHGLPrimitivePoint )
	{
		hgl_BeginDraw();
		for( i = 0; i < primitive.vertexCount; i++ )
		{
			//坐标变换
			vtx = Vertex3MultiMatrix43( primitive.buffer[i]._position, g_RenderState.m_ModelView );
			//投影变换
			hgl_Projection( &x, &y, g_Perspective, vtx );
			//绘制
			hgl_DrawPixel( x, y, primitive.buffer[i]._color._r, primitive.buffer[i]._color._g, 
									primitive.buffer[i]._color._b );
		}
		hgl_EndDraw();
	}

}


//////////////////////////////////
//得到当前渲染列表中的多边形数量//
//////////////////////////////////
int HGL_GetFacetCount()
{
	return g_RenderList.triangleCount;
}


//////////////////////
//设置多边形剔除模式//
//////////////////////
void HGL_CullMode( HGLCullMode mode )
{
	g_RenderState.m_CullMode = mode;
}


////////////////////////
//设置多边形的填充模式//
////////////////////////
void HGL_FillMode( HGLFillMode mode )
{
	g_RenderState.m_FillMode = mode;
}


////////////
//设置材质//
////////////
void HGL_SetMatrial( const HGL_Matrial& matrial )
{
	g_RenderState.m_matrial = matrial;
}


////////////
//重置材质//
////////////
void HGL_ResetMatrial()
{
	g_RenderState.m_matrial.attri = 0;

	//环境光反射参数
	g_RenderState.m_matrial.ambient._r = (char)255;
	g_RenderState.m_matrial.ambient._g = (char)255;
	g_RenderState.m_matrial.ambient._b = (char)255;
	g_RenderState.m_matrial.ambient._a = (char)255;
	//漫射光反射参数
	g_RenderState.m_matrial.diffuse._r = (char)255;
	g_RenderState.m_matrial.diffuse._g = (char)255;
	g_RenderState.m_matrial.diffuse._b = (char)255;
	g_RenderState.m_matrial.diffuse._a = (char)255;
	//高光反射参数
	g_RenderState.m_matrial.specular._r = 0;
	g_RenderState.m_matrial.specular._g = 0;
	g_RenderState.m_matrial.specular._b = 0;
	g_RenderState.m_matrial.specular._a = 0;
	//自发光参数
	g_RenderState.m_matrial.emissive._r = 0;
	g_RenderState.m_matrial.emissive._g = 0;
	g_RenderState.m_matrial.emissive._b = 0;
	g_RenderState.m_matrial.emissive._a = 0;

}


////////////////
//设置某个光照//
////////////////
void HGL_SetLight( HGL_LightNum lightIndex, HGL_Light* lightInfo )
{
	//检查参数正确性
	if( lightIndex < 0 || lightIndex >= HGL_MaxLight )
	{
		return;
	}

	if( lightInfo )
	{
		g_RenderState.m_lightEnable[lightIndex] = true;
		g_RenderState.m_lights[lightIndex] = *lightInfo;
	}else
	{
		g_RenderState.m_lightEnable[lightIndex] = false;
	}

}


////////////////
//设置简单光照//
////////////////
void HGL_SetSimpleLight( HGL_SimpleLightNum lightIndex, HGL_SimpleLight* lightInfo )
{
	//检查参数正确性
	if( lightIndex < 0 || lightIndex > HGL_MaxSimpleLight )		//简单光照最多只设3个光源
	{
		return;
	}

	if( lightInfo )
	{
		g_RenderState.m_SimpleLightEnable[lightIndex] = true;
		g_RenderState.m_SimpleLights[lightIndex] = *lightInfo;
	}else
	{
		g_RenderState.m_SimpleLightEnable[lightIndex] = false;
	}
}


////////////////////
//开启某些渲染选项//
////////////////////
void HGL_Enable( HGL_RenderOption option )
{
	switch( option )
	{
		//光照
	case HGL_LIGHT:
		g_RenderState.m_Lighting = true;
		g_RenderState.m_SimpleLighting = false;
		break;
		//简单光照（光没有颜色，只有强度，光源类型只有平行光和点光源）
	case HGL_SIMPLELIGHT:
		g_RenderState.m_Lighting = false;
		g_RenderState.m_SimpleLighting = true;
		break;
		//纹理映射
	case HGL_TEXTURE:
		g_RenderState.m_TexMode = HGL_TexEnable;
		break;
	default:
		break;
	}
}


////////////////////
//关闭某些渲染选项//
////////////////////
void HGL_Disable( HGL_RenderOption option )
{
	switch( option )
	{
	case HGL_LIGHT:
		g_RenderState.m_Lighting = false;
		break;
	case HGL_SIMPLELIGHT:
		g_RenderState.m_SimpleLighting = false;
		break;
	case HGL_TEXTURE:
		g_RenderState.m_TexMode = HGL_TexDisable;
		break;
	default:
		break;
	}
}


////////////////
//设置着色模式//
////////////////
void HGL_ShadeMode( HGL_ShadeModel sm )
{
	g_RenderState.m_shadeMode = sm;
}


////////////
//创建纹理//
////////////
bool HGL_CreateTexture( HGL_Texture& tex, int wid, int hei )
{
	return true;
}


////////////
//加载纹理//
////////////
bool HGL_LoadTexture( HGL_Texture& tex, char* fileName )
{
	if( tex._locked )
	{
		return false;
	}

	if( tex._data )
	{
		SDL_FreeSurface( tex._data );
	}

	tex._data = SDL_LoadBMP( fileName );

	//加载失败
	if( tex._data == NULL )
	{
		return false;
	}

	tex._wid = tex._data->w;
	tex._hei = tex._data->h;

	return true;
}


////////////
//绑定纹理//
////////////
void HGL_BindTexture( HGL_TextureLevel num, HGL_Texture* tex )
{
	g_RenderState.m_texture[num] = tex;
}
