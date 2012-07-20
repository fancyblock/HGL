#include "stdafx.h"
#include "HGLInnerFun.h"

//����Ⱦ�Ĵ���
extern SDL_Surface* g_RenderWnd;

//��Ⱦ״̬
extern HGLRenderState g_RenderState;


//��������
void hgl_BeginDraw()
{
	SDL_LockSurface( g_RenderWnd );
}

//��������
void hgl_EndDraw()
{
	SDL_UnlockSurface( g_RenderWnd );
}

//д���棨���㣩
void hgl_DrawPixel( int x, int y, char r, char g, char b )
{
	if( x < 0 || y < 0 || x >= g_RenderWnd->w || y >= g_RenderWnd->h )
	{
		return;
	}

	char* pos =	(char*)g_RenderWnd->pixels + ( g_RenderWnd->pitch * y ) + x * 3;

	pos[0] = b;
	pos[1] = g;
	pos[2] = r;

}

//�ü��ߣ�����true���������߶����õ��ˣ�	��δ�꡿
bool hgl_ClipLine( int& x1, int y1, int x2, int y2 )
{
	int wid = g_RenderWnd->w - 1;
	int hei = g_RenderWnd->h - 1;

	//��������λ�ö����
#define NOCLIP  0x0000
#define N_CLIP  0x0008
#define S_CLIP  0x0004
#define E_CLIP  0x0002
#define W_CLIP  0x0001

#define NE_CLIP N_CLIP|E_CLIP
#define SE_CLIP S_CLIP|E_CLIP
#define NW_CLIP N_CLIP|W_CLIP
#define SW_CLIP S_CLIP|W_CLIP

	int tx1 = x1;
	int	ty1 = y1;
	int tx2 = x2;
	int ty2 = y2;

	int p1State = NOCLIP; 
	int p2State = NOCLIP;

	//�жϵ��λ��
	if( y1 < 0 )
	{
		p1State |= N_CLIP;
	}else if( y1 > hei )
	{
		p1State |= S_CLIP;
	}

	if( x1 < 0 )
	{
		p1State |= W_CLIP;
	}else if( x1 > wid )
	{
		p1State |= E_CLIP;
	}

	if( y2 < 0 )
	{
		p2State |= N_CLIP;
	}else if( y2 > hei )
	{
		p2State |= S_CLIP;
	}

	if ( x2 < 0 )
	{
		p2State |= W_CLIP;
	}else if( x2 > wid )
	{
		p2State |= E_CLIP;
	}

	//���߶�ֱ���޳���������
	//�����������������λ��һ���������������㹹�ɵ��߲���������
	if( p1State & p2State )
	{
		return true;
	}

	//����ȫ��λ�ڿռ��У�����Ҫ�ü�
	if( p1State == 0 && p2State == 0 )
	{
		return false;
	}

	//��������λ�ý�������
	switch( p1State )
	{
	case N_CLIP:
		ty1 = 0;
		;
		break;
	case S_CLIP:
		break;
	case W_CLIP:
		break;
	case E_CLIP:
		break;
	case NE_CLIP:
		break;
	case SE_CLIP:
		break;
	case NW_CLIP: 
		break;
	case SW_CLIP:
		break;
	default:
		break;
	}
	//��һ����Ĳü�
	switch( p2State )
	{
	case N_CLIP:
		break;
	case S_CLIP:
		break;
	case W_CLIP:
		break;
	case E_CLIP:
		break;
	case NE_CLIP:
		break;
	case SE_CLIP:
		break;
	case NW_CLIP: 
		break;
	case SW_CLIP:
		break;
	default:
		break;
	}

	x1 = tx1;
	y1 = ty1;
	x2 = tx2;
	y2 = ty2;

	return false;
}

//д���棨���ߣ�	��δ�꡿
void hgl_DrawLine( int x1, int y1, int x2, int y2, char r, char g, char b )
{
	//�������ü������߲���
	if( hgl_ClipLine( x1, y1, x2, y2) )
	{
		return;
	}

	//DDA�㷨
#ifdef DDA_LINE
	int temp;
	char* buffer;

	if( x1 == x2 )				//��ֱ��
	{
		//��֤Y1С�ڵ���Y2
		if( y2 < y1 )
		{
			temp = y2;
			y2 = y1;
			y1 = temp;
		}
		//�ü�
		if( y1 < 0 )
		{
			y1 = 0;
		}
		if( y2 >= g_RenderWnd->h )
		{
			y2 = g_RenderWnd->h - 1;
		}
		//��ʼ�����ַ
		buffer = (char*)g_RenderWnd->pixels + ( g_RenderWnd->pitch * y1 + x1*3 );
		for( int i = y1; i <= y2; i++ )
		{
			buffer[0] = b;
			buffer[1] = g;
			buffer[2] = r;
			buffer += g_RenderWnd->pitch;
		}
	}else if( y1 == y2 )		//ˮƽ��
	{
		//��֤X2���ڵ���X1
		if( x2 < x1 )
		{
			temp = x2;
			x2 = x1;
			x1 = temp;
		}
		//�ü�
		if( x1 < 0 )
		{
			x1 = 0;
		}
		if( x2 >= g_RenderWnd->w )
		{
			x2 = g_RenderWnd->w -1 ;
		}
		//��ʼ�����ַ
		buffer = (char*)g_RenderWnd->pixels + ( g_RenderWnd->pitch * y1 + x1*3 );
		for( int i = x1; i <= x2; i ++ )
		{
			buffer[0] = b;
			buffer[1] = g;
			buffer[2] = r;
			buffer += 3;
		}
	}else						//б��
	{
		float k = float( y2 - y1 ) / float( x2 - x1 );
		float absk = abs(k);
		//���б��С��1����֤x1С�ڵ���x2�����б�ʴ���1����֤y1С�ڵ���y2
		if( ( absk < 1.0f && x2 < x1 ) || ( absk >= 1.0f && y2 < y1 ) )
		{
			x1 = x1 ^ x2;
			x2 = x1 ^ x2;
			x1 = x1 ^ x2;
			y1 = y1 ^ y2;
			y2 = y1 ^ y2;
			y1 = y1 ^ y2;
		}
		//б�ʾ���ֵС��1����
		if( absk < 1.0f )
		{
			float ypos = static_cast<float>(y1);
			for( int i = x1; i <= x2; i++ )
			{
				hgl_DrawPixel( i, int(ypos + 0.5f), r, g, b );
				ypos += k;
			}
		}else
		//б�ʾ���ֵ����1����
		{
			float xpos = static_cast<float>(x1);
			k = 1.0f / k;
			for( int i = y1; i <= y2; i++ )
			{
				hgl_DrawPixel( int(xpos + 0.5f), i, r, g, b );
				xpos += k;
			}
		}
	}
#endif

	//Bresenham�㷨��δ�꡿
#ifdef BRESENHAM_LINE

#endif

}


//---------------
//��ѧ����
//---------------

//3d��������
vertex3 Vertex3( float x, float y, float z )
{
	vertex3 result;

	result._x = x;
	result._y = y;
	result._z = z;

	return result;
}

//4d��������
vertex4 Vertex4( float x, float y, float z, float w )
{
	vertex4 result;

	result._x = x;
	result._y = y;
	result._z = z;
	result._w = w;

	return result;
}

//�������ת��Ϊ3D����
vertex3 Vertex4To3( const vertex4& vtx4 )
{
	vertex3 vtx3;

	vtx3._x = vtx4._x / vtx4._w;
	vtx3._y = vtx4._y / vtx4._w;
	vtx3._z = vtx4._z / vtx4._w;

	return vtx3;
}

//3D����ת��Ϊ�������
vertex4 Vertex3To4( const vertex3& vtx3 )
{
	vertex4 vtx4;

	vtx4._x = vtx3._x;
	vtx4._y = vtx3._y;
	vtx4._z = vtx3._z;
	vtx4._w = 1.0f;

	return vtx4;
}

//����ȡģ
float Vertex3Length( const vertex3& vtx3 )
{
	return sqrt( vtx3._x * vtx3._x + vtx3._y * vtx3._y + vtx3._z * vtx3._z );
}

//��Ϊ������
void Vertex3Zero( vertex3& vtx3 )
{
	vtx3._x = 0.0f;
	vtx3._y = 0.0f;
	vtx3._z = 0.0f;
}

//��ɵ�λ����
void Vertex3Normalize( vertex3& vtx3 )
{
	float mag = Vertex3Length( vtx3 );

	vtx3._x /= mag;
	vtx3._y /= mag;
	vtx3._z /= mag;
}

//�����ӷ�
vertex3 Vertex3Add( const vertex3& vtx31, const vertex3& vtx32 )
{
	vertex3 result;

	result._x = vtx31._x + vtx32._x;
	result._y = vtx31._y + vtx32._y;
	result._z = vtx31._z + vtx32._z;

	return result;
}

//��������
vertex3 Vertex3Sub( const vertex3& vtx31, const vertex3& vtx32 )
{
	vertex3 result;

	result._x = vtx31._x - vtx32._x;
	result._y = vtx31._y - vtx32._y;
	result._z = vtx31._z - vtx32._z;

	return result;
}

//������
float Vertex3Distance( const vertex3& vtx31, const vertex3& vtx32 )
{
	return Vertex3Length( Vertex3Sub( vtx31 , vtx32 ) );
}

//����ȡ��
void Vertex3Negate( vertex3& vtx3 )
{
	vtx3._x = -vtx3._x;
	vtx3._y = -vtx3._y;
	vtx3._z = -vtx3._z;
}

//������
vertex3 Vertex3MutiScalar( const vertex3& vtx3, const float scalar )
{
	vertex3 result;

	result._x = vtx3._x * scalar;
	result._y = vtx3._y * scalar;
	result._z = vtx3._z * scalar;

	return result;
}

//������
vertex3 Vertex3DivScalar( const vertex3& vtx3, const float scalar )
{
	vertex3 result;

	result._x = vtx3._x / scalar;
	result._y = vtx3._y / scalar;
	result._z = vtx3._z / scalar;

	return result;
}

//�������
float Vertex3DotProduct( const vertex3& vtx31, const vertex3& vtx32 )
{
	return vtx31._x * vtx32._x + vtx31._y * vtx32._y + vtx31._z * vtx32._z;
}

//�������
vertex3 Vertex3CorssProduct( const vertex3& vtx31, const vertex3& vtx32 )
{
	vertex3 result;

	result._x = vtx31._y * vtx32._z - vtx31._z * vtx32._y;
	result._y = vtx31._z * vtx32._x - vtx31._x * vtx32._z;
	result._z = vtx31._x * vtx32._y - vtx31._y * vtx32._x;

	return result;
}

//�����Ƿ���ƽ��ǰ��
bool IsFront( const vertex3& vtx3, const plane& pln )
{
	float distance = Vertex3DotProduct( vtx3, pln._normal );

	if( distance - pln._distance > 0 )
	{
		return true;
	}

	return false;
}

//���Ƿ���ƽ��ǰ�棨�ཻ���㣩
bool IsFront( const vertex3& vtx3, float radius, const plane& pln )
{
	float distance = Vertex3DotProduct( vtx3, pln._normal );

	if( distance - pln._distance > radius )
	{
		return true;
	}

	return false;
}

//������������ƽ��
plane GetPlane( const vertex3& vtx1, const vertex3& vtx2, const vertex3& vtx3 )
{
	plane result;

	vertex3 vc1 = Vertex3Sub( vtx2, vtx1 );
	vertex3 vc2 = Vertex3Sub( vtx3, vtx2 );
	result._normal = Vertex3CorssProduct( vc1, vc2 );
	Vertex3Normalize( result._normal );						//������λ��

	result._distance = Vertex3DotProduct( result._normal, vtx1 );

	return result;
}

//44�����ɵ�λ����
void Matrix44Identity( matrix44& mat44 )
{
	memset( mat44._m, 0, sizeof( float ) * 16 );

	mat44._v1._x = 1.0f;
	mat44._v2._y = 1.0f;
	mat44._v3._z = 1.0f;
	mat44._v4._w = 1.0f;
}

//44�������
matrix44 Matrix44Multi( const matrix44& mat1, const matrix44& mat2 )
{
	matrix44 result;

	int i,j,k;
	float temp;

	for( i = 0; i < 4; i++ )
	{
		for( j = 0; j < 4; j++ )
		{
			temp = 0.0f;
			for( k = 0; k < 4; k++ )
			{
				temp += mat1._vv[i]._m[k] * mat2._vv[k]._m[j];
			}

			result._vv[i]._m[j] = temp;
		}
	}

	return result;
}

//����һ��ƽ�ƾ���
void Matrix44Translate( matrix44& mat44, float x, float y, float z )
{
	Matrix44Identity( mat44 );

	mat44._v4._x = x;
	mat44._v4._y = y;
	mat44._v4._z = z;

}

//����һ�����ž���
void Matrix44Scale( matrix44& mat44, float xScale, float yScale, float zScale )
{
	Matrix44Identity( mat44 );

	mat44._v1._x = xScale;
	mat44._v2._y = yScale;
	mat44._v3._z = zScale;

}

//����һ����ָ�������ŵľ���
void Matrix44ScaleAxis( matrix44& mat44, const vertex3& axis, float scale )
{
	Matrix44Identity( mat44 );

	float kone = scale - 1;
	float xy = axis._x * axis._y;
	float xz = axis._x * axis._z;
	float yz = axis._y * axis._z;

	mat44._v1 = Vertex4( 1 + kone * axis._x*axis._x, kone * xy, kone * xz, 0 );
	mat44._v2 = Vertex4( mat44._v1._y, 1 + kone * axis._y*axis._y, kone * yz, 0 );
	mat44._v3 = Vertex4( mat44._v1._z, mat44._v2._z, 1 + kone * axis._z*axis._z, 0 );

}

//����һ����X����ת���󣨻����ƣ�
void Matrix44RotateX( matrix44& mat44, float angle )
{
	Matrix44Identity( mat44 );

	float sinx = sin( angle );
	float cosx = cos( angle );

	mat44._v2._y = cosx;
	mat44._v2._z = sinx;
	mat44._v3._y = -sinx;
	mat44._v3._z = cosx;

}

//����һ����Y����ת���󣨻����ƣ�
void Matrix44RotateY( matrix44& mat44, float angle )
{
	Matrix44Identity( mat44 );

	float sinx= sin( angle );
	float cosx= cos( angle );

	mat44._v1._x = cosx;
	mat44._v1._z = -sinx;
	mat44._v3._x = sinx;
	mat44._v3._z = cosx;

}

//����һ����Z����ת���󣨻����ƣ�
void Matrix44RotateZ( matrix44& mat44, float angle )
{
	Matrix44Identity( mat44 );

	float sinx= sin( angle );
	float cosx= cos( angle );

	mat44._v1._x = cosx;
	mat44._v1._y = sinx;
	mat44._v2._x = -sinx;
	mat44._v2._y = cosx;

}

//����һ������������ת�ľ��󣨻����ƣ�
void Matrix44Rotate( matrix44& mat44, float ax, float ay, float az, float angle )
{
	Matrix44Identity( mat44 );

	float sinx = sin( angle );
	float cosx = cos( angle );
	float cosx2 = 1.0f - cosx;

	float len = sqrt( ax * ax + ay * ay + az * az );

	float x = ax / len;
	float y = ay / len;
	float z = az / len;

	mat44._v1._x = x*x * cosx2 + cosx;
	mat44._v1._y = x*y * cosx2 + z * sinx;
	mat44._v1._z = x*z * cosx2 - y * sinx;

	mat44._v2._x = x*y * cosx2 - z * sinx;
	mat44._v2._y = y*y * cosx2 + cosx;
	mat44._v2._z = y*z * cosx2 + x * sinx;

	mat44._v3._x = x*z * cosx2 + y * sinx;
	mat44._v3._y = y*z * cosx2 - x * sinx;
	mat44._v3._z = z*z * cosx2 + cosx;

}

//�������˾���44
vertex4 Vertex4MultiMatrix44( const vertex4& vtx4, const matrix44& mat44 )
{
	vertex4 result;

	int i,j;
	float temp;

	for( i= 0; i< 4; i++ )
	{
		temp= 0.0f;
		for( j= 0; j< 4; j++ )
		{
			temp+= vtx4._m[j] * mat44._vv[j]._m[i];
		}
		result._m[i]= temp;
	}

	return result;
}

//�������˾���43
vertex3 Vertex4MultiMatrix43( const vertex4& vtx4, const matrix44& mat43 )
{
	vertex3 result;

	int i,j;
	float temp;

	for( i= 0; i< 3; i++ )
	{
		temp= 0.0f;
		for( j= 0; j< 4; j++ )
		{
			temp+= vtx4._m[j] * mat43._vv[j]._m[i];
		}
		result._m[i]= temp;
	}

	return result;
}

//����˾���43���ڲ�ת������������ٳˣ�
vertex3 Vertex3MultiMatrix43( const vertex3& vtx3, const matrix44& mat33 )
{
	vertex3 result;

	int i,j;
	float temp;

	for( i= 0; i< 3; i++ )
	{
		temp= 0.0f;
		for( j= 0; j< 3; j++ )
		{
			temp+= vtx3._m[j] * mat33._vv[j]._m[i];
		}
		result._m[i]= temp;
	}

	result._x += mat33._v4._x;
	result._y += mat33._v4._y;
	result._z += mat33._v4._z;

	return result;
}

//����˾���33
vertex3 Vertex3MultiMatrix33( const vertex3& vtx3, const matrix44& mat33 )
{
	vertex3 result;

	int i,j;
	float temp;

	for( i= 0; i< 3; i++ )
	{
		temp= 0.0f;
		for( j= 0; j< 3; j++ )
		{
			temp+= vtx3._m[j] * mat33._vv[j]._m[i];
		}
		result._m[i]= temp;
	}

	return result;
}

//����ͶӰ����
void hgl_Projection( short* x, short *y, const perspectiveInfo& fun, const vertex3& vtx )
{
	*x = short( fun.mXfactor * vtx._x / vtx._z + fun.mXoffset );
	*y = short( fun.mYfactor * vtx._y / vtx._z + fun.mYoffset );
}

//�����޳�
bool hgl_CullPrimitive( const HGL_Primitive& primitive, const matrix44& transMatrix, const frustum& space )
{
	vertex3 obj = Vertex3( transMatrix._v4._x, transMatrix._v4._y, transMatrix._v4._z );

	//�����Ƿ񳬳�Զ��������
	//ע��ȽϽ�������ʱֻҪ�����Χ����һ�㳬����ȫ���޳��������ڲ������õ�����·�ֹ���ֿ��µĴ���͸�ӷ���
	if( obj._z - primitive.radius <= space._nearZ || 
		obj._z - primitive.radius >= space._farZ )
	{
		return true;
	}

	//��׵�����Ҽ������޳�
	if( IsFront( obj, primitive.radius, space._left ) || 
		IsFront( obj, primitive.radius, space._right ) )
	{
		return true;
	}

	//��׵�����¼������޳�
	if( IsFront( obj, primitive.radius, space._top ) || 
		IsFront( obj, primitive.radius, space._bottom ) )
	{
		return true;
	}

	return false;
}

//��ʼ����Ⱦ�б������ʼ��ʱ����
void hgl_InitRenderList( renderList& rl )
{
	rl.triangleCount = 0;
	rl.triList = new triangle();
	rl.curTriangle = rl.triList;

	rl.triList->next = NULL;
}

//�����Ⱦ�б�
void hgl_DestroyRenderList( renderList& rl )
{
	while( rl.triList )
	{
		rl.curTriangle = rl.triList;
		rl.triList = rl.triList->next;

		delete rl.curTriangle;
	}
}

//�����޳��ж�
bool hgl_CullBack( triangle* facet )
{
	Vertex* vtxs = facet->_vtx;
	vertex3 facetDir;

	//�������ε��淨��
	facetDir = Vertex3CorssProduct( Vertex3Sub( vtxs[2]._position, vtxs[1]._position ), Vertex3Sub( vtxs[1]._position, vtxs[0]._position ) );

	if( Vertex3DotProduct( facetDir, vtxs[0]._position ) > 0 )
	{
		return true;
	}

	return false;
}

//��դ��һ���߿�������
void hgl_RasterLineTriangle( const triangle& facet )
{
	//��������
	hgl_DrawLine( facet._vtx[0]._pixel._x, facet._vtx[0]._pixel._y,
		facet._vtx[1]._pixel._x, facet._vtx[1]._pixel._y,
		facet._vtx[0]._color._r, facet._vtx[0]._color._g, facet._vtx[0]._color._b );
	hgl_DrawLine( facet._vtx[0]._pixel._x, facet._vtx[0]._pixel._y,
		facet._vtx[2]._pixel._x, facet._vtx[2]._pixel._y,
		facet._vtx[0]._color._r, facet._vtx[0]._color._g, facet._vtx[0]._color._b );
	hgl_DrawLine( facet._vtx[2]._pixel._x, facet._vtx[2]._pixel._y,
		facet._vtx[1]._pixel._x, facet._vtx[1]._pixel._y,
		facet._vtx[0]._color._r, facet._vtx[0]._color._g, facet._vtx[0]._color._b );

}

//��դ��һ���������Σ�ƽ�������Σ���ʵ�ģ�
void hgl_DrawTopTrangle( int x1, int y1, int x2, int y2, int x3, int y3, char r, char g, char b )
{
	float dxLeft;
	float dxRight;
	float left;
	float right;
	float height;

	int tmpx;
	int tmpy;
	int iLeft;
	int iRight;

	char* buffer;
	char* bufferX;

	//ȷ���ڶ�����λ�ڵ�����������
	if( x3 < x2 )
	{
		tmpx = x3;
		x3 = x2;
		x2 = tmpx;
	}

	height = float(y3 - y1);
	dxLeft = float(x2 - x1) / height;
	dxRight = float(x3 - x1) / height;

	left = float(x1);
	right = left;

	//����Yֵ�ü�
	if( y1 < 0 )
	{
		left += float(-y1) * dxLeft;
		right += float(-y1) * dxRight;
		
		y1 = 0;
	}
	if( y3 >= g_RenderWnd->h )
	{
		y3 = g_RenderWnd->h - 1;
	}

	buffer = (char*)g_RenderWnd->pixels + (y1 * g_RenderWnd->pitch);

	//Xֵ�ü��ж�
	int wid = g_RenderWnd->w;
	if( x1 >= 0 && x1 < wid && x2 >= 0 && x2 < wid && x3 >= 0 && x3 < wid )
	{
		for( tmpy = y1; tmpy <= y3; tmpy++ )
		{
			iLeft = (unsigned int)left;
			iRight = (unsigned int)right;
			bufferX = buffer + iLeft * 3;
			for( tmpx = iLeft; tmpx <= iRight; tmpx++ )
			{
				bufferX[0] = b;
				bufferX[1] = g;
				bufferX[2] = r;
				bufferX += 3;
			}

			left += dxLeft;
			right += dxRight;

			buffer += g_RenderWnd->pitch;
		}
	}else
	//Xֵ��Ҫ�ü�
	{
		for( tmpy = y1; tmpy <= y3; tmpy++, buffer += g_RenderWnd->pitch )
		{
			iLeft = (unsigned int)left;
			iRight = (unsigned int)right;

			left += dxLeft;
			right += dxRight;

			//������Ļ���
			if( iLeft < 0 )
			{
				iLeft = 0;

				if( iRight < 0 )
				{
					continue;
				}
			}
			//������Ļ�ұ�
			if( iRight >= g_RenderWnd->w )
			{
				iRight = g_RenderWnd->w - 1;

				if( iLeft >= g_RenderWnd->w )
				{
					continue;
				}
			}

			bufferX = buffer + iLeft * 3;
			for( tmpx = iLeft; tmpx <= iRight; tmpx++ )
			{
				bufferX[0] = b;
				bufferX[1] = g;
				bufferX[2] = r;
				bufferX += 3;
			}

		}
	}

}

//��դ��һ���������Σ�ƽ�������Σ���ʵ�ģ�
void hgl_DrawDownTrangle( int x1, int y1, int x2, int y2, int x3, int y3, char r, char g, char b )
{
	float dxLeft;
	float dxRight;
	float left;
	float right;
	float height;

	int tmpx;
	int tmpy;
	int iLeft;
	int iRight;

	char* buffer;
	char* bufferX;

	//ȷ����һ����λ�ڵڶ���������
	if( x2 < x1 )
	{
		tmpx = x2;
		x2 = x1;
		x1 = tmpx;
	}

	height = float(y3 - y1);
	dxLeft = float(x3 - x1) / height;
	dxRight = float(x3 - x2) / height;

	left = float(x1);
	right = float(x2);

	//����Yֵ�ü�
	if( y1 < 0 )
	{
		left += float(-y1) * dxLeft;
		right += float(-y1) * dxRight;

		y1 = 0;
	}
	if( y3 >= g_RenderWnd->h )
	{
		y3 = g_RenderWnd->h - 1;
	}

	buffer = (char*)g_RenderWnd->pixels + (y1 * g_RenderWnd->pitch);

	//Xֵ�ü��ж�
	int wid = g_RenderWnd->w;
	if( x1 >= 0 && x1 < wid && x2 >= 0 && x2 < wid && x3 >= 0 && x3 < wid )
	{
		for( tmpy = y1; tmpy <= y3; tmpy++ )
		{
			iLeft = (unsigned int)left;
			iRight = (unsigned int)right;
			bufferX = buffer + iLeft * 3;
			for( tmpx = iLeft; tmpx <= iRight; tmpx++ )
			{
				bufferX[0] = b;
				bufferX[1] = g;
				bufferX[2] = r;
				bufferX += 3;
			}

			left += dxLeft;
			right += dxRight;

			buffer += g_RenderWnd->pitch;
		}
	}else
	//Xֵ��Ҫ�ü�
	{
		for( tmpy = y1; tmpy <= y3; tmpy++, buffer += g_RenderWnd->pitch )
		{
			iLeft = (unsigned int)left;
			iRight = (unsigned int)right;

			left += dxLeft;
			right += dxRight;

			//������Ļ���
			if( iLeft < 0 )
			{
				iLeft = 0;

				if( iRight < 0 )
				{
					continue;
				}
			}
			//������Ļ�ұ�
			if( iRight >= g_RenderWnd->w )
			{
				iRight = g_RenderWnd->w - 1;

				if( iLeft >= g_RenderWnd->w )
				{
					continue;
				}
			}

			bufferX = buffer + iLeft * 3;
			for( tmpx = iLeft; tmpx <= iRight; tmpx++ )
			{
				bufferX[0] = b;
				bufferX[1] = g;
				bufferX[2] = r;
				bufferX += 3;
			}

		}
	}

}

//��դ�������Σ�ʵ�ģ�
void hgl_RasterTriangle( const triangle& facet )
{
	int x1 = facet._vtx[0]._pixel._x;
	int y1 = facet._vtx[0]._pixel._y;
	int x2 = facet._vtx[1]._pixel._x;
	int y2 = facet._vtx[1]._pixel._y;
	int x3 = facet._vtx[2]._pixel._x;
	int y3 = facet._vtx[2]._pixel._y;

	char r = facet._color._r;
	char g = facet._color._g;
	char b = facet._color._b;

	//�˻������β�����Ⱦ
	if( (x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3) )
	{
		return;
	}

	int tmpx;
	int tmpy;
	//����Yֵ������Y1-Y3��С����
	if( y1 > y2 )
	{
		tmpx = x1;
		tmpy = y1;
		x1 = x2;
		y1 = y2;
		x2 = tmpx;
		y2 = tmpy;
	}
	if( y2 > y3 )
	{
		tmpx = x2;
		tmpy = y2;
		x2 = x3;
		y2 = y3;
		x3 = tmpx;
		y3 = tmpy;
	}
	if( y1 > y2 )
	{
		tmpx = x1;
		tmpy = y1;
		x1 = x2;
		y1 = y2;
		x2 = tmpx;
		y2 = tmpy;
	}

	//�޳���������Ļ��Χ�ڵ�������
	int wid = g_RenderWnd->w - 1;
	int hei = g_RenderWnd->h - 1;
	if( y3 < 0 || y1 > hei || ( x1 < 0 && x2 < 0 && x3 < 0 ) || ( x1 > wid && x2 > wid && x3 > wid ) )
	{
		return;
	}

	//�и��ƽ�������κ�ƽ��������
	int newx;
	//ƽ��������
	if( y1 == y2 )
	{
		hgl_DrawDownTrangle( x1, y1, x2, y2, x3, y3, r, g, b );
	}else
	//ƽ��������
	if( y2 == y3 )
	{
		hgl_DrawTopTrangle( x1, y1, x2, y2, x3, y3, r, g, b );
	}else
	//��Ҫ���
	{
		newx = x1 + (int)(0.5+(float)(y2-y1)*(float)(x3-x1)/(float)(y3-y1));

		hgl_DrawTopTrangle( x1, y1, x2, y2, newx, y2, r, g, b );
		hgl_DrawDownTrangle( x2, y2, newx, y2, x3, y3, r, g, b );
	}

}

//��դ�������Σ�ʵ�ģ����������ɫʵ�ģ�	��δ�꡿
void hgl_RasterGouraudTrangle( const triangle& facet )
{
}

//��դ��ƽ�������Σ���������ƽ̹��ɫ��
void hgl_DrawDownTrangleTexFlat( const Vertex* vtx1, const Vertex* vtx2, const Vertex* vtx3 )
{
	const Vertex* pt1 = vtx1;
	const Vertex* pt2 = vtx2;
	const Vertex* pt3 = vtx3;

	//ȷ����һ�����ڵڶ��������
	if( pt1->_pixel._x > pt2->_pixel._x )
	{
		const Vertex* tmp = pt1;
		pt1 = pt2;
		pt2 = tmp;
	}

	char* buffer;
	char* bufferX;

	float lightPower = pt1->_power;		//����ǿ��
	unsigned char r,g,b;				//��ɫ����

	int yBegin = pt1->_pixel._y;		//Y������ʼ��
	int yEnd = pt3->_pixel._y;			//Y���������

	float left = float( pt1->_pixel._x );
	float right = float( pt2->_pixel._x );

	float leftU = pt1->_uv._x;
	float leftV = pt1->_uv._y;
	float rightU = pt2->_uv._x;
	float rightV = pt2->_uv._y;

	//���꼰UV������ֵ
	float height = float( pt3->_pixel._y - pt1->_pixel._y );
	float leftInc = float( pt3->_pixel._x - pt1->_pixel._x ) / height;
	float rightInc = float( pt3->_pixel._x - pt2->_pixel._x ) / height;
	float leftUinc = ( pt3->_uv._x - pt1->_uv._x ) / height;
	float leftVinc = ( pt3->_uv._y - pt1->_uv._y ) / height;
	float rightUinc = ( pt3->_uv._x - pt2->_uv._x ) / height;
	float rightVinc = ( pt3->_uv._y - pt2->_uv._y ) / height;

	if( yBegin < 0 )
	{
		left += ( -yBegin ) * leftInc;
		right += ( -yBegin ) * rightInc;
		leftU += ( -yBegin ) * leftUinc;
		leftV += ( -yBegin ) * leftVinc;
		rightU += ( -yBegin ) * rightUinc;
		rightV += ( -yBegin ) * rightVinc;

		yBegin = 0;
	}
	if( yEnd >= g_RenderWnd->h )
	{
		yEnd = g_RenderWnd->h - 1;
	}

	buffer = (char*)g_RenderWnd->pixels + g_RenderWnd->pitch * yBegin;

	int i;
	int j;
	for( i = yBegin; i <= yEnd; i++ )
	{
		int leftPix = int( left );
		int rightPix = int( right );

		//UV��ֵ
		float lineU = leftU;
		float lineV = leftV;
		float lineUinc = ( rightU - leftU ) / float( rightPix - leftPix );
		float lineVinc = ( rightV - leftV ) / float( rightPix - leftPix );

		if( leftPix < 0 )
		{
			lineU += ( -leftPix ) * lineUinc;
			lineV += ( -leftPix ) * lineVinc;
			leftPix = 0;
		}
		if( rightPix >= g_RenderWnd->w )
		{
			rightPix = g_RenderWnd->w - 1;
		}

		bufferX = buffer + leftPix * 3;

		//��դ����
		for( j = leftPix; j <= rightPix; j++ )
		{
			hgl_sampler( g_RenderState.m_texture[HGL_Tex0], lineU, lineV, &r, &g, &b );

			bufferX[0] = unsigned char( b * lightPower );
			bufferX[1] = unsigned char( g * lightPower );
			bufferX[2] = unsigned char( r * lightPower );

			lineU += lineUinc;
			lineV += lineVinc;
			bufferX += 3;
		}

		left += leftInc;
		right += rightInc;

		leftU += leftUinc;
		leftV += leftVinc;
		rightU += rightUinc;
		rightV += rightVinc;

		buffer += g_RenderWnd->pitch;
	}

}

//��դ��ƽ�������Σ���������ƽ̹��ɫ��
void hgl_DrawTopTrangleTexFlat( const Vertex* vtx1, const Vertex* vtx2, const Vertex* vtx3 )
{
	const Vertex* pt1 = vtx1;
	const Vertex* pt2 = vtx2;
	const Vertex* pt3 = vtx3;

	//ȷ���ڶ������ڵ����������
	if( pt2->_pixel._x > pt3->_pixel._x )
	{
		const Vertex* tmp = pt2;
		pt2 = pt3;
		pt3 = tmp;
	}

	char* buffer;
	char* bufferX;

	float lightPower = pt1->_power;		//����ǿ��
	unsigned char r,g,b;				//��ɫ����

	int yBegin = pt1->_pixel._y;		//Y������ʼ��
	int yEnd = pt3->_pixel._y;			//Y���������

	float left = float( pt1->_pixel._x );
	float right = left;

	float leftU = pt1->_uv._x;
	float leftV = pt1->_uv._y;
	float rightU = leftU;
	float rightV = leftV;

	//���꼰UV������ֵ
	float height = float( pt3->_pixel._y - pt1->_pixel._y );
	float leftInc = float( pt2->_pixel._x - pt1->_pixel._x ) / height;
	float rightInc = float( pt3->_pixel._x - pt1->_pixel._x ) / height;
	float leftUinc = ( pt2->_uv._x - pt1->_uv._x ) / height;
	float leftVinc = ( pt2->_uv._y - pt1->_uv._y ) / height;
	float rightUinc = ( pt3->_uv._x - pt1->_uv._x ) / height;
	float rightVinc = ( pt3->_uv._y - pt1->_uv._y ) / height;

	if( yBegin < 0 )
	{
		left += ( -yBegin ) * leftInc;
		right += ( -yBegin ) * rightInc;
		leftU += ( -yBegin ) * leftUinc;
		leftV += ( -yBegin ) * leftVinc;
		rightU += ( -yBegin ) * rightUinc;
		rightV += ( -yBegin ) * rightVinc;

		yBegin = 0;
	}
	if( yEnd >= g_RenderWnd->h )
	{
		yEnd = g_RenderWnd->h - 1;
	}

	buffer = (char*)g_RenderWnd->pixels + g_RenderWnd->pitch * yBegin;

	int i;
	int j;
	for( i = yBegin; i <= yEnd; i++ )
	{
		int leftPix = int( left );
		int rightPix = int( right );

		//UV��ֵ
		float lineU = leftU;
		float lineV = leftV;
		float lineUinc = ( rightU - leftU ) / float( rightPix - leftPix );
		float lineVinc = ( rightV - leftV ) / float( rightPix - leftPix );

		if( leftPix < 0 )
		{
			lineU += ( -leftPix ) * lineUinc;
			lineV += ( -leftPix ) * lineVinc;
			leftPix = 0;
		}
		if( rightPix >= g_RenderWnd->w )
		{
			rightPix = g_RenderWnd->w - 1;
		}

		bufferX = buffer + leftPix * 3;

		//��դ����
		for( j = leftPix; j <= rightPix; j++ )
		{
			hgl_sampler( g_RenderState.m_texture[HGL_Tex0], lineU, lineV, &r, &g, &b );
			bufferX[0] = unsigned char( b * lightPower );
			bufferX[1] = unsigned char( g * lightPower );
			bufferX[2] = unsigned char( r * lightPower );

			lineU += lineUinc;
			lineV += lineVinc;
			bufferX += 3;
		}

		left += leftInc;
		right += rightInc;

		leftU += leftUinc;
		leftV += leftVinc;
		rightU += rightUinc;
		rightV += rightVinc;

		buffer += g_RenderWnd->pitch;
	}
}

//��դ�������Σ���������ƽ̹��ɫ��
void hgl_RasterTrangleTex( const triangle& facet )
{
	const Vertex* vtx1 = &facet._vtx[0];
	const Vertex* vtx2 = &facet._vtx[1];
	const Vertex* vtx3 = &facet._vtx[2];

	//�˻������β���Ⱦ
	if( ( vtx1->_pixel._x == vtx2->_pixel._x && vtx1->_pixel._x == vtx3->_pixel._x ) ||
		( vtx1->_pixel._y == vtx2->_pixel._y && vtx1->_pixel._y == vtx3->_pixel._y ) )
	{
		return;
	}

	const Vertex* vtxTmp;
	//����Yֵ��������
	if( vtx1->_pixel._y > vtx2->_pixel._y )
	{
		vtxTmp = vtx1;
		vtx1 = vtx2;
		vtx2 = vtxTmp;
	}
	if( vtx2->_pixel._y > vtx3->_pixel._y )
	{
		vtxTmp = vtx2;
		vtx2 = vtx3;
		vtx3 = vtxTmp;
	}
	if( vtx1->_pixel._y > vtx2->_pixel._y )
	{
		vtxTmp = vtx1;
		vtx1 = vtx2;
		vtx2 = vtxTmp;
	}

	//������Ļ�ڵ��޳�
	int wid = g_RenderWnd->w - 1;
	int hei = g_RenderWnd->h - 1;
	if( vtx1->_pixel._y < 0 || vtx3->_pixel._y > hei || 
		( vtx1->_pixel._x < 0 && vtx2->_pixel._x < 0 && vtx3->_pixel._x < 0 ) || 
		( vtx1->_pixel._x > wid && vtx2->_pixel._x > wid && vtx3->_pixel._x > wid ) )
	{
		return;
	}

	//�и��ƽ�������κ�ƽ��������
	//ƽ��������
	if( vtx1->_pixel._y == vtx2->_pixel._y )
	{
		hgl_DrawDownTrangleTexFlat( vtx1, vtx2, vtx3 );
	}else
	//ƽ��������
	if( vtx2->_pixel._y == vtx3->_pixel._y )
	{
		hgl_DrawTopTrangleTexFlat( vtx1, vtx2, vtx3 );
	}else
	//��ֳ�ƽ�������κ�ƽ��������
	{
		Vertex tmp;

		//�����ֵ
		tmp._pixel._x = vtx1->_pixel._x + int( float(vtx2->_pixel._y - vtx1->_pixel._y) * float(vtx3->_pixel._x - vtx1->_pixel._x) / float(vtx3->_pixel._y - vtx1->_pixel._y) );
		tmp._pixel._y = vtx2->_pixel._y;
		//��ɫ���������ε���ɫ����Ϊ��ƽ̹��ɫ
		tmp._color = facet._color;
		//����Ҳ�ǣ���Ϊ��ƽ̹��ɫ
		tmp._normal = facet._normal;
		//����ǿ��Ҳ�ǣ���Ϊ��ƽ̹��ɫ
		tmp._power = vtx1->_power;
		//uvҪ��ֵ�õ�
		tmp._uv._x = vtx1->_uv._x + (vtx3->_uv._x - vtx1->_uv._x) * (vtx2->_pixel._y - vtx1->_pixel._y) / (vtx3->_pixel._y - vtx1->_pixel._y);
		tmp._uv._y = vtx1->_uv._y + (vtx3->_uv._y - vtx1->_uv._y) * (vtx2->_pixel._y - vtx1->_pixel._y) / (vtx3->_pixel._y - vtx1->_pixel._y);

		hgl_DrawDownTrangleTexFlat( vtx2, &tmp, vtx3 );
		hgl_DrawTopTrangleTexFlat( vtx1, vtx2, &tmp );
		
	}

}

//��դ�������Σ������������޵���ɫ��		��δ�꡿
void hgl_RasterGouraudTrangleTex( const triangle& facet )
{
}

//��Ⱦ״̬��ʼ��
void hgl_InitRenderState()
{
	int i;

	//��ʼ������޳�ģʽΪ�����޳�
	g_RenderState.m_CullMode = eHGLNonCull;

	//��ʼ��������ģʽΪʵ�����
	g_RenderState.m_FillMode = eHGLSolidMode;

	//������Ϣ��ʼ������������һ��ʼ�ر�
	g_RenderState.m_Lighting = false;
	for( i = 0; i < HGL_MaxLight; i++ )
	{
		g_RenderState.m_lightEnable[i] = false;
	}

	//��ʼ��ɫģʽ
	g_RenderState.m_shadeMode = HGL_FLAT;

	//�򵥹��ռ���һ��ʼ�ر�
	g_RenderState.m_SimpleLighting = false;
	for( i = 0; i < HGL_MaxSimpleLight; i++ )
	{
		g_RenderState.m_SimpleLightEnable[i] = false;
	}

	//��ʼ����ӳ��ģʽ(��ʼ�ر�)
	g_RenderState.m_TexMode = HGL_TexDisable;
	//�󶨵���ͼ��ʼΪ��
	for( i = 0; i < HGL_MaxTexLev; i++ )
	{
		g_RenderState.m_texture[i] = NULL;
	}

}

//���ռ���	��δ�꡿
void hgl_LightCalc( triangle* facet, HGLRenderState& state )
{
	switch( state.m_shadeMode )
	{
	case HGL_FLAT:					//ֻ���淨�߼�����պ������ɫ
		break;
	case HGL_GOURAUD:				//�ö��㷨�߼�����պ�Ķ�����ɫ���м����ز�ֵ
		break;
	case HGL_PHONG:					//��ֵ�õ���������صķ��ߣ����ݷ��߼������ع��պ����ɫ
		//[�ݲ�ʵ��]
		break;
	default:
		break;
	}
}

//�򵥹��ռ���		��δ�꡿
void hgl_SimpleLightCalc( triangle* facet, HGLRenderState& state )
{
	float power = 0.0f;
	float tmpPower;
	int i;
	vertex3 dir;

	switch( state.m_shadeMode )
	{
	case HGL_FLAT:					//��ƽ̹��ɫ��ֻ���淨�߼�����պ������ɫ
		//0�Ż�����
		if( state.m_SimpleLightEnable[HGL_SimpleLightAmbient] )
		{
			power = state.m_SimpleLights[HGL_SimpleLightAmbient]._power;
		}
		power = 1.0f - power;
		//1��2�������
		for( i = HGL_SimpleLightDiffuse1; i <= HGL_SimpleLightDiffuse2; i++ )
		{
			if( state.m_SimpleLightEnable[i] )
			{
				//��������
				if( state.m_SimpleLights[i]._type == eHGLLightDirectional )
				{
					tmpPower = state.m_SimpleLights[i]._power;
					dir = Vertex3MultiMatrix33( state.m_SimpleLights[i]._direction, g_RenderState.m_CameraMatrix );
					Vertex3Negate( dir );
					float lightFactor = Vertex3DotProduct( dir, facet->_normal );
					if( lightFactor >= 0 )
					{
						tmpPower = tmpPower * lightFactor;
						tmpPower = 1.0f - tmpPower;
					}else
					{
						tmpPower = 1;
					}

					power = power * tmpPower;
				}else 
				//���Դ����
				if( state.m_SimpleLights[i]._type == eHGLLightPoint )
				{
					;////////
				}

			}
		}

		power = 1.0f - power;

		facet->_color._r = unsigned char( facet->_color._r * power );
		facet->_color._g = unsigned char( facet->_color._g * power );
		facet->_color._b = unsigned char( facet->_color._b * power );

		//ÿ�������¼����ǿ��
		facet->_vtx[0]._power = power;
		facet->_vtx[1]._power = power;
		facet->_vtx[2]._power = power;
		break;
	case HGL_GOURAUD:				//�����޵���ɫ���ö��㷨�߼�����պ�Ķ�����ɫ���м����ز�ֵ
		;
		break;
	case HGL_PHONG:					//��������ɫ����ֵ�õ���������صط��ߣ����ݷ��߼������ع��պ����ɫ
		//[��ʵ��]
		break;
	default:
		break;
	}
}

//����λͼ
bool hgl_LockTexture( HGL_Texture* tex )
{
	if( tex->_locked )
	{
		return true;
	}

	if( !tex->_data )
	{
		return false;
	}

	int result = SDL_LockSurface( tex->_data );

	if( result == -1 )
	{
		return false;
	}

	tex->_locked = true;

	return true;
}

//����λͼ
bool hgl_UnlockTexture( HGL_Texture* tex )
{
	if( !tex->_locked )
	{
		return false;
	}

	if( !tex->_data )
	{
		return false;
	}

	int result = SDL_LockSurface( tex->_data );

	if( result == -1 )
	{
		return false;
	}

	tex->_locked = false;

	return true;
}

//��ɫ����
void hgl_sampler( HGL_Texture* tex, float u, float v, unsigned char* r, unsigned char* g, unsigned char* b )
{
	//��UVת��Ϊ����
	int x =	static_cast<int>( static_cast<float>(tex->_wid) * u );
	int y = static_cast<int>( static_cast<float>(tex->_hei) * v );

	char* pixel = (char*)tex->_data->pixels;
	pixel += ( tex->_data->pitch * y + 3 * x );

	*r = pixel[2];
	*g = pixel[1];
	*b = pixel[0];
}
