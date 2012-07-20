#include "stdafx.h"
#include "HGL.h"
#include "HGLInnerFun.h"	//HGL�ڲ����ܺ���


//����������״̬
HGLState g_HGLState = eHGLNon;
//����Ⱦ�Ĵ���
SDL_Surface* g_RenderWnd = NULL;

//-----------------------------------------//

//͸��ͶӰ��Ϣ
perspectiveInfo g_Perspective;

//��׶�壨���������޳�������βü���
frustum g_ViewFrustum;

//��ǰ�����־
HGLMatrixType g_currentMatrix;

//��ǰ�༭�е�����
EditPrimitive g_CurrentPrimitive;

//��Ⱦ�б�
renderList g_RenderList;

//��׼���㣨�µĶ���δָ�����ߡ���ɫ�Ⱦ������ö���ģ�
Vertex g_standardVertex;

//��Ⱦģʽ��һЩ״̬
HGLRenderState g_RenderState;



////////////
//��ʼ����//
////////////
bool HGL_Inital()
{
	int info= SDL_WasInit( SDL_INIT_VIDEO );

	if( info & SDL_INIT_VIDEO )
	{
		//�Ѿ���ʼ������
		return false;
	}

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		//��ʼ��ʧ��
		return false;
	}

	//��ʼ���ɹ�������״̬��־
	g_HGLState = eHGLInit;

	//һЩȫ�ֱ�����Ĭ��ֵ
	g_currentMatrix = eHGLWorldMatrix;

	//��ʼ����Ⱦ�б�
	hgl_InitRenderList( g_RenderList );

	//��Ⱦ״̬��ʼ��
	hgl_InitRenderState();

	return true;
}


//////////////////////////////////////
//����һ����Ⱦ���ڣ������ɹ�����true//
//////////////////////////////////////
bool HGL_CreateWnd( const char* name, int wid, int hei, bool fullscreen )
{
	if( g_HGLState != eHGLInit )
	{
		//�����Ѵ���
		return false;
	}

	//���ڿ�ߴ��ݸ�͸��ͶӰ�ṹ
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
		//�����ɹ������ô������ͱ�־
		SDL_WM_SetCaption( name, NULL );
		g_HGLState = eHGLWnd;
	}else
	{
		return false;
	}

	return true;
}


////////////////////////
//������������ʾ��ǰ̨//
////////////////////////
void HGL_SwapBuffer()
{
	//������ɫģʽѡ��ͬ�Ĺ�դ������
	void (*Raster)( const triangle& trg );

	if( g_RenderState.m_FillMode == eHGLLineMode )	//�߿�ģʽ
	{
		Raster = hgl_RasterLineTriangle;
	}else
	//ʵ��ģʽ
	{
		if( g_RenderState.m_shadeMode == HGL_FLAT )							//ƽ̹��ɫ
		{
			if( g_RenderState.m_TexMode == HGL_TexDisable )//����ӳ��δ����
			{
				Raster = hgl_RasterTriangle;			//---��δ�꣬���޸ġ�
			}else
			{
				Raster = hgl_RasterTrangleTex;
			}
		}else if( g_RenderState.m_shadeMode == HGL_GOURAUD )				//�������ɫ
		{
			if( g_RenderState.m_TexMode == HGL_TexDisable )//����ӳ��δ����
			{
				Raster = hgl_RasterGouraudTrangle;		//---����δʵ�֡�
			}else
			{
				Raster = hgl_RasterGouraudTrangleTex;
			}
		}
	}

	//��������ӳ��Ļ�����ͼ׼��
	if( g_RenderState.m_TexMode == HGL_TexEnable )
	{
		if( g_RenderState.m_texture[HGL_Tex0] )
		{
			//��ͼ����ʧ�ܣ�����ʵ�Ĺ�դ������
			if( hgl_LockTexture( g_RenderState.m_texture[HGL_Tex0] ) == false )
			{
				Raster = hgl_RasterTriangle;
			}
		}
	}

	//����Ⱦ�б���Ķ���������Ļ��ȥ
	triangle* facet = g_RenderList.triList->next;

	//������Ⱦ�б�����������
	hgl_BeginDraw();
	for( int i(0); i < g_RenderList.triangleCount; i++ )
	{
		Raster( *facet );
		facet = facet->next;
	}
	hgl_EndDraw();

	//�����ͼ����
	if( g_RenderState.m_texture[HGL_Tex0] )
	{
		hgl_UnlockTexture( g_RenderState.m_texture[HGL_Tex0] );
	}

	SDL_Flip( g_RenderWnd );
}


//////////////////////////
//�ݻٴ���(������ڵĻ�)//
//////////////////////////
void HGL_DestroyWnd()
{
	//������ڴ�����ݻ�֮
	if( g_RenderWnd )
	{
		//������Ⱦ�б�
		hgl_DestroyRenderList( g_RenderList );

		//���ڴ���
		SDL_FreeSurface( g_RenderWnd );
		
		//����״̬��־
		g_HGLState = eHGLInit;

	}
}


//////////////////
//�������������//
//////////////////
void HGL_ClearBuffer()
{
	SDL_FillRect( g_RenderWnd, NULL, 0 );

	//��Ⱦ�б�Ҫ����
	g_RenderList.curTriangle = g_RenderList.triList;
	g_RenderList.triangleCount = 0;

}


////////////	��δ�꡿
//��ȡ�¼�//
////////////
bool HGL_GetEvent( HGLEvt* evt )
{
	SDL_Event sdlEvt;

	if( SDL_PollEvent( &sdlEvt ) )
	{
		switch( sdlEvt.type )
		{
		case SDL_QUIT:									//�����˳��¼�
			evt->m_evtType = eHGLQuit;
			break;
		case SDL_ACTIVEEVENT:							//���ڻ�¼�
			evt->m_evtType = eHGLWndActive;
			break;
		case SDL_KEYDOWN:								//���̰����¼�
			evt->m_evtType = eHGLKeyboardDown;
			break;
		case SDL_KEYUP:									//����̧���¼�
			evt->m_evtType = eHGLKeyboardUp;
			break;
		case SDL_MOUSEMOTION:							//����ƶ���û�и��¼���ֻ�������������λ�ã�
			evt->m_mousePos._x = sdlEvt.motion.x;
			evt->m_mousePos._y = sdlEvt.motion.y;
			break;
		case SDL_MOUSEBUTTONDOWN:						//��갴���¼�
			evt->m_evtType = eHGLMouseDown;
			;
			break;
		case SDL_MOUSEBUTTONUP:							//���̧���¼�
			evt->m_evtType = eHGLMouseUp;
			break;
		case SDL_SYSWMEVENT:							//ϵͳ�¼�
			evt->m_evtType = eHGLSystem;
			break;
		case SDL_USEREVENT:								//�û��¼�
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
//���HGL���ü��Ƿ񱻰���//
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
//����͸��ͶӰ//
////////////////
void HGL_Perspective( float fovy, float aspect, float zNear, float zFar )
{
	if( g_HGLState < eHGLWnd )
	{
		return;
	}

	//��Ұ�Ƕ�Ϊ�Ƕ��ƣ��Ϸ�����Ϊ��0��180��
	if( fovy <= 0 || fovy >= 180.0f )
	{
		return;
	}

	//��߱�����ڵ���0
	if( aspect <= 0 )
	{
		return;
	}

	//Զ�������������0
	if( zNear <= 0 || zFar <= 0 )
	{
		return;
	}

	//����͸����Ϣ
	g_Perspective._fovy = fovy;
	g_Perspective._aspect = aspect;

	const double PI = 3.14159265f;
	//������Ұ�Ƕ����Dֵ��ʹ����ת����λ�ڣ�-1��1��֮�䣩
	float d = float( 1.0 / tan( (fovy * 0.5 * PI )/180.0 ) );

	//����͸�Ӽ���Ҫ�õ�����
	g_Perspective.mXoffset = g_Perspective._screenWid * 0.5f + 0.5f;
	g_Perspective.mYoffset = g_Perspective._screenHei * 0.5f + 0.5f;
	g_Perspective.mXfactor = d * g_Perspective.mXoffset;
	g_Perspective.mYfactor = d * aspect * ( -g_Perspective.mYoffset );

	//͸��ͶӰ������ϣ����Կ�ʼ����������
	g_HGLState = eHGLReady;

	//�����ʼ��
	Matrix44Identity( g_RenderState.m_WorldMatrix );
	Matrix44Identity( g_RenderState.m_CameraMatrix );

	//������׵���6���棨�����޳��ͼ��ã���
	//��׵�����泯�⣬Ҳ������IsFront�ж�ʱ���������true���Ǳ�ʾ����׵���⿴������
	float farXY = zFar / d;
	//��׵�����º����ҵ��ӽ���һ���ģ�farXY��Ϊ��Զ�������ϵ����Ե�ĵ��X��Yֵ
	vertex3 origin = Vertex3( 0.0f, 0.0f, 0.0f );					//ԭ��
	vertex3 topLeft = Vertex3( -farXY, farXY, zFar );				//��׵����Զ���ϵ�
	vertex3 topRight = Vertex3( farXY, farXY, zFar );				//��׵����Զ���ϵ�
	vertex3 botLeft = Vertex3( -farXY, -farXY, zFar );				//��׵����Զ���µ�
	vertex3 botRight = Vertex3( farXY, -farXY, zFar );				//��׵����Զ���µ�

	g_ViewFrustum._farZ = zFar;
	g_ViewFrustum._nearZ = zNear;
	g_ViewFrustum._left = GetPlane( origin, botLeft, topLeft );
	g_ViewFrustum._right = GetPlane( origin, topRight, botRight );
	g_ViewFrustum._top = GetPlane( origin, topLeft, topRight );
	g_ViewFrustum._bottom = GetPlane( origin, botRight, botLeft );

}


////////////////////
//���õ�ǰ��������//
////////////////////
void HGL_MatrixMode( HGLMatrixType matrixMode )
{
	g_currentMatrix = matrixMode;
}


//////////////////////////
//���õ�ǰ����Ϊ��λ����//
//////////////////////////
void HGL_LoadIdentity()
{
	switch( g_currentMatrix )
	{
	case eHGLWorldMatrix:	//�����������
		Matrix44Identity( g_RenderState.m_WorldMatrix );
		break;
	case eHGLCameraMatrix:	//����Camera����
		Matrix44Identity( g_RenderState.m_CameraMatrix );
		break;
	default:
		break;
	}

	g_RenderState.m_ModelView = Matrix44Multi( g_RenderState.m_WorldMatrix, g_RenderState.m_CameraMatrix );

}


////////////////////////////////////
//��ǰ�������һ��ƽ�ƾ�����ˣ�//
////////////////////////////////////
void HGL_Translate( float x, float y, float z )
{
	matrix44 translateMatrix;

	switch( g_currentMatrix )
	{
	case eHGLWorldMatrix:	//�������
		Matrix44Translate( translateMatrix, x, y, z );
		g_RenderState.m_WorldMatrix = Matrix44Multi( translateMatrix, g_RenderState.m_WorldMatrix );
		break;
	case eHGLCameraMatrix:	//Camera����
		Matrix44Translate( translateMatrix, -x, -y, -z );
		g_RenderState.m_CameraMatrix = Matrix44Multi( g_RenderState.m_CameraMatrix, translateMatrix );
		break;
	default:
		break;
	}

	g_RenderState.m_ModelView = Matrix44Multi( g_RenderState.m_WorldMatrix, g_RenderState.m_CameraMatrix );

}


////////////////////////////////////
//��ǰ�������һ����ת������ˣ�//
////////////////////////////////////
void HGL_Rotate( float angle, float x, float y, float z )
{
	matrix44 rotateMatrix;

	switch( g_currentMatrix )
	{
	case eHGLWorldMatrix:	//�������
		Matrix44Rotate( rotateMatrix, x, y, z, angle );
		g_RenderState.m_WorldMatrix = Matrix44Multi( rotateMatrix, g_RenderState.m_WorldMatrix );
		break;
	case eHGLCameraMatrix:	//Camera����
		Matrix44Rotate( rotateMatrix, x, y, z, -angle );
		g_RenderState.m_CameraMatrix = Matrix44Multi( g_RenderState.m_CameraMatrix, rotateMatrix );
		break;
	default:
		break;
	}

	g_RenderState.m_ModelView = Matrix44Multi( g_RenderState.m_WorldMatrix, g_RenderState.m_CameraMatrix );

}


////////////////////////////////////
//��ǰ�������һ�����ž�����ˣ�//	Camera����Ļ��ҳ�
////////////////////////////////////
void HGL_Scale( float x, float y, float z )
{
	//����������Ӧ�ô˱任
	matrix44 scaleMatrix;

	if( g_currentMatrix == eHGLWorldMatrix )
	{
		Matrix44Scale( scaleMatrix, x, y, z );
		g_RenderState.m_WorldMatrix = Matrix44Multi( scaleMatrix, g_RenderState.m_WorldMatrix );
	}

	g_RenderState.m_ModelView = Matrix44Multi( g_RenderState.m_WorldMatrix, g_RenderState.m_CameraMatrix );

}


//////////////////////////////////////////////
//��ǰ�������һ�������������ŵľ�����ˣ�//	Camera����Ļ��ҳ�
//////////////////////////////////////////////
void HGL_Scale( float x, float y, float z, float k )
{
	//����������Ӧ�ô˱任
	matrix44 scaleMatrix;

	if( g_currentMatrix == eHGLWorldMatrix )
	{
		Matrix44ScaleAxis( scaleMatrix, Vertex3( x, y, z ), k );
		g_RenderState.m_WorldMatrix = Matrix44Multi( scaleMatrix, g_RenderState.m_WorldMatrix );
	}

	g_RenderState.m_ModelView = Matrix44Multi( g_RenderState.m_WorldMatrix, g_RenderState.m_CameraMatrix );

}


////////////////////
//��ʼ�༭һ������//
////////////////////
void HGL_Begin( HGL_Primitive& primitive, int count )
{
	//����2Ϊ������
	HGL_DestroyPrimitive( primitive );

	primitive.vertexCount = count;
	primitive.buffer = new Vertex[count];

	g_CurrentPrimitive.exist = true;
	g_CurrentPrimitive.curPrimitive = &primitive;
	g_CurrentPrimitive.vertexIndex = 0;
	g_CurrentPrimitive.curPrimitive->radius = 0.0f;

}


////////////////////
//�����༭һ������//
////////////////////
void HGL_End()
{
	g_CurrentPrimitive.exist = false;
}


////////////////
//����һ������//
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
//ָ��һ������λ��//
////////////////////
void HGL_Vertex( float x, float y, float z )
{
	//��־�жϣ���ֻ֤��HGL_Begin��HGL_End֮��ĵ��ò���Ч
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

	//����ö��㵽���ĵľ���
	float distance = Vertex3Length( pt->_position );

	//�������ԭ��������뾶���������뾶
	if( distance > g_CurrentPrimitive.curPrimitive->radius )
	{
		g_CurrentPrimitive.curPrimitive->radius = distance;
	}

	//��ֹ���
	if( g_CurrentPrimitive.vertexIndex < g_CurrentPrimitive.curPrimitive->vertexCount - 1 )
	{
		g_CurrentPrimitive.vertexIndex++;
	}

}


////////////////////
//ָ��һ��������ɫ//
////////////////////
void HGL_Color( float r, float g, float b )
{
	//��־�жϣ���ֻ֤��HGL_Begin��HGL_End֮��ĵ��ò���Ч
	if( !g_CurrentPrimitive.exist )
	{
		return;
	}

	//��ֹԽ��
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
//ָ��һ�����㷨��//
////////////////////
void HGL_Normal( float x, float y, float z )
{
	//��־�жϣ���ֻ֤��HGL_Begin��HGL_End֮��ĵ��ò���Ч
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
//ָ��һ������UV//
//////////////////
void HGL_Texcoord( float u, float v )
{
	//��־�жϣ���ֻ֤��HGL_Begin��HGL_End֮��ĵ��ò���Ч
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
//����һ������//
////////////////
void HGL_DrawPrimitive( HGL_Primitive& primitive, HGLPrimitiveType type )
{
	//��������屻�޳������ٽ�����ȥ
	if( hgl_CullPrimitive( primitive, g_RenderState.m_ModelView, g_ViewFrustum ) )
	{
		return;
	}

	int i;
	short x, y;
	vertex3 vtx;
	int vertexCount;

	////�������Σ�������Ⱦ�б�ͳһ��Ⱦ��
	//----------------------------------
	if( type == eHGLPrimitiveTriangle )
	{
		//��������������ΪС�ڵ���ԭ������������3�ı���
		vertexCount = primitive.vertexCount;
		vertexCount -= vertexCount % 3;

		//����任
		for( i = 0; i < vertexCount; i += 3 )
		{
			//��Ⱦ�б���������ڵ�
			if( !g_RenderList.curTriangle->next )
			{
				g_RenderList.curTriangle->next = new triangle();
				g_RenderList.curTriangle->next->next = NULL;
			}

			//ģ���ڵ������μ�����Ⱦ�б�
			g_RenderList.curTriangle->next->_vtx[0] = primitive.buffer[i];
			g_RenderList.curTriangle->next->_vtx[1] = primitive.buffer[i+1];
			g_RenderList.curTriangle->next->_vtx[2] = primitive.buffer[i+2];

			//����任
			g_RenderList.curTriangle->next->_vtx[0]._position = Vertex3MultiMatrix43( g_RenderList.curTriangle->next->_vtx[0]._position, g_RenderState.m_ModelView );
			g_RenderList.curTriangle->next->_vtx[1]._position = Vertex3MultiMatrix43( g_RenderList.curTriangle->next->_vtx[1]._position, g_RenderState.m_ModelView );
			g_RenderList.curTriangle->next->_vtx[2]._position = Vertex3MultiMatrix43( g_RenderList.curTriangle->next->_vtx[2]._position, g_RenderState.m_ModelView );

			//�����������޳�
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

			//ƽ̹��ɫģʽ�Ļ�ֻҪ�任һ�����ߣ���������Ϊ�淨��
			if( g_RenderState.m_shadeMode == HGL_FLAT )
			{
				//ʹ�������εĵ�һ������ķ�����Ϊ�淨��
				g_RenderList.curTriangle->next->_normal = Vertex3MultiMatrix33( g_RenderList.curTriangle->next->_vtx[0]._normal, g_RenderState.m_ModelView );

			}else
			//����Ͷ�3������ķ��߶����б任
			{
				g_RenderList.curTriangle->next->_vtx[0]._normal = Vertex3MultiMatrix33( g_RenderList.curTriangle->next->_vtx[0]._normal, g_RenderState.m_ModelView );
				g_RenderList.curTriangle->next->_vtx[1]._normal = Vertex3MultiMatrix33( g_RenderList.curTriangle->next->_vtx[1]._normal, g_RenderState.m_ModelView );
				g_RenderList.curTriangle->next->_vtx[2]._normal = Vertex3MultiMatrix33( g_RenderList.curTriangle->next->_vtx[2]._normal, g_RenderState.m_ModelView );

			}

			//ʹ�������εĵ�һ���������ɫ��Ϊ����ɫ
			g_RenderList.curTriangle->next->_color = g_RenderList.curTriangle->next->_vtx[0]._color;

			//���ռ��㣨�������ߵı任����������տ����Ž���
			if( g_RenderState.m_Lighting )
			{
				hgl_LightCalc( g_RenderList.curTriangle->next, g_RenderState );
			}else
			//�򵥹��ռ���
			if( g_RenderState.m_SimpleLighting )
			{
				hgl_SimpleLightCalc( g_RenderList.curTriangle->next, g_RenderState );
			}else
			//û�й���
			{
				//����Ĺ���ǿ�ȶ���Ϊ1
				g_RenderList.curTriangle->next->_vtx[0]._power = 1.0f;
				g_RenderList.curTriangle->next->_vtx[1]._power = 1.0f;
				g_RenderList.curTriangle->next->_vtx[2]._power = 1.0f;
			}

			//͸�Ӽ���
			hgl_Projection( &g_RenderList.curTriangle->next->_vtx[0]._pixel._x,
							&g_RenderList.curTriangle->next->_vtx[0]._pixel._y,
							g_Perspective, g_RenderList.curTriangle->next->_vtx[0]._position );
			hgl_Projection( &g_RenderList.curTriangle->next->_vtx[1]._pixel._x,
							&g_RenderList.curTriangle->next->_vtx[1]._pixel._y,
							g_Perspective, g_RenderList.curTriangle->next->_vtx[1]._position );
			hgl_Projection( &g_RenderList.curTriangle->next->_vtx[2]._pixel._x,
							&g_RenderList.curTriangle->next->_vtx[2]._pixel._y,
							g_Perspective, g_RenderList.curTriangle->next->_vtx[2]._position );

			//һ�������δ�����ɣ���Ⱦ�б����
			g_RenderList.curTriangle = g_RenderList.curTriangle->next;
			g_RenderList.triangleCount++;
		}

	}else
	////����ģʽ
	//----------
	if( type == eHGLPrimitiveLine )
	{
		//��ֹ�����������������Ϊż��ʱ����һ��
		if( primitive.vertexCount % 2 == 0 )
		{
			vertexCount = primitive.vertexCount;
		}else
		{
			vertexCount = primitive.vertexCount - 1;
		}

		//����任
		for( i = 0; i < vertexCount; i++ )
		{
			//����任
			vtx = Vertex3MultiMatrix43( primitive.buffer[i]._position, g_RenderState.m_ModelView );
			//ͶӰ�任
			hgl_Projection( &primitive.buffer[i]._pixel._x, &primitive.buffer[i]._pixel._y, g_Perspective, vtx );
		}

		//����
		hgl_BeginDraw();
		for( i = 0; i < vertexCount; i += 2 )
		{
			//����
			hgl_DrawLine( primitive.buffer[i]._pixel._x, primitive.buffer[i]._pixel._y,
				primitive.buffer[i+1]._pixel._x, primitive.buffer[i+1]._pixel._y, 
				primitive.buffer[i]._color._r, primitive.buffer[i]._color._g, 
				primitive.buffer[i]._color._b );
		}
		hgl_EndDraw();

	}else
	////����ģʽ
	//----------
	if( type == eHGLPrimitivePoint )
	{
		hgl_BeginDraw();
		for( i = 0; i < primitive.vertexCount; i++ )
		{
			//����任
			vtx = Vertex3MultiMatrix43( primitive.buffer[i]._position, g_RenderState.m_ModelView );
			//ͶӰ�任
			hgl_Projection( &x, &y, g_Perspective, vtx );
			//����
			hgl_DrawPixel( x, y, primitive.buffer[i]._color._r, primitive.buffer[i]._color._g, 
									primitive.buffer[i]._color._b );
		}
		hgl_EndDraw();
	}

}


//////////////////////////////////
//�õ���ǰ��Ⱦ�б��еĶ��������//
//////////////////////////////////
int HGL_GetFacetCount()
{
	return g_RenderList.triangleCount;
}


//////////////////////
//���ö�����޳�ģʽ//
//////////////////////
void HGL_CullMode( HGLCullMode mode )
{
	g_RenderState.m_CullMode = mode;
}


////////////////////////
//���ö���ε����ģʽ//
////////////////////////
void HGL_FillMode( HGLFillMode mode )
{
	g_RenderState.m_FillMode = mode;
}


////////////
//���ò���//
////////////
void HGL_SetMatrial( const HGL_Matrial& matrial )
{
	g_RenderState.m_matrial = matrial;
}


////////////
//���ò���//
////////////
void HGL_ResetMatrial()
{
	g_RenderState.m_matrial.attri = 0;

	//�����ⷴ�����
	g_RenderState.m_matrial.ambient._r = (char)255;
	g_RenderState.m_matrial.ambient._g = (char)255;
	g_RenderState.m_matrial.ambient._b = (char)255;
	g_RenderState.m_matrial.ambient._a = (char)255;
	//����ⷴ�����
	g_RenderState.m_matrial.diffuse._r = (char)255;
	g_RenderState.m_matrial.diffuse._g = (char)255;
	g_RenderState.m_matrial.diffuse._b = (char)255;
	g_RenderState.m_matrial.diffuse._a = (char)255;
	//�߹ⷴ�����
	g_RenderState.m_matrial.specular._r = 0;
	g_RenderState.m_matrial.specular._g = 0;
	g_RenderState.m_matrial.specular._b = 0;
	g_RenderState.m_matrial.specular._a = 0;
	//�Է������
	g_RenderState.m_matrial.emissive._r = 0;
	g_RenderState.m_matrial.emissive._g = 0;
	g_RenderState.m_matrial.emissive._b = 0;
	g_RenderState.m_matrial.emissive._a = 0;

}


////////////////
//����ĳ������//
////////////////
void HGL_SetLight( HGL_LightNum lightIndex, HGL_Light* lightInfo )
{
	//��������ȷ��
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
//���ü򵥹���//
////////////////
void HGL_SetSimpleLight( HGL_SimpleLightNum lightIndex, HGL_SimpleLight* lightInfo )
{
	//��������ȷ��
	if( lightIndex < 0 || lightIndex > HGL_MaxSimpleLight )		//�򵥹������ֻ��3����Դ
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
//����ĳЩ��Ⱦѡ��//
////////////////////
void HGL_Enable( HGL_RenderOption option )
{
	switch( option )
	{
		//����
	case HGL_LIGHT:
		g_RenderState.m_Lighting = true;
		g_RenderState.m_SimpleLighting = false;
		break;
		//�򵥹��գ���û����ɫ��ֻ��ǿ�ȣ���Դ����ֻ��ƽ�й�͵��Դ��
	case HGL_SIMPLELIGHT:
		g_RenderState.m_Lighting = false;
		g_RenderState.m_SimpleLighting = true;
		break;
		//����ӳ��
	case HGL_TEXTURE:
		g_RenderState.m_TexMode = HGL_TexEnable;
		break;
	default:
		break;
	}
}


////////////////////
//�ر�ĳЩ��Ⱦѡ��//
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
//������ɫģʽ//
////////////////
void HGL_ShadeMode( HGL_ShadeModel sm )
{
	g_RenderState.m_shadeMode = sm;
}


////////////
//��������//
////////////
bool HGL_CreateTexture( HGL_Texture& tex, int wid, int hei )
{
	return true;
}


////////////
//��������//
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

	//����ʧ��
	if( tex._data == NULL )
	{
		return false;
	}

	tex._wid = tex._data->w;
	tex._hei = tex._data->h;

	return true;
}


////////////
//������//
////////////
void HGL_BindTexture( HGL_TextureLevel num, HGL_Texture* tex )
{
	g_RenderState.m_texture[num] = tex;
}
