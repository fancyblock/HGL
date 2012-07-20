#include "stdafx.h"
#include "HGLInnerFun.h"

//被渲染的窗口
extern SDL_Surface* g_RenderWnd;

//渲染状态
extern HGLRenderState g_RenderState;


//锁定缓存
void hgl_BeginDraw()
{
	SDL_LockSurface( g_RenderWnd );
}

//解锁缓存
void hgl_EndDraw()
{
	SDL_UnlockSurface( g_RenderWnd );
}

//写缓存（画点）
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

//裁减线（返回true代表整条线都被裁掉了）	【未完】
bool hgl_ClipLine( int& x1, int y1, int x2, int y2 )
{
	int wid = g_RenderWnd->w - 1;
	int hei = g_RenderWnd->h - 1;

	//点所处的位置定义宏
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

	//判断点的位置
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

	//该线段直接剔除（不画）
	//由于两个点在外面的位置一样，所以这两个点构成的线不经过画面
	if( p1State & p2State )
	{
		return true;
	}

	//该线全部位于空间中，不需要裁减
	if( p1State == 0 && p2State == 0 )
	{
		return false;
	}

	//对两个点位置进行修正
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
	//另一个点的裁剪
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

//写缓存（画线）	【未完】
void hgl_DrawLine( int x1, int y1, int x2, int y2, char r, char g, char b )
{
	//被整个裁减掉的线不画
	if( hgl_ClipLine( x1, y1, x2, y2) )
	{
		return;
	}

	//DDA算法
#ifdef DDA_LINE
	int temp;
	char* buffer;

	if( x1 == x2 )				//垂直线
	{
		//保证Y1小于等于Y2
		if( y2 < y1 )
		{
			temp = y2;
			y2 = y1;
			y1 = temp;
		}
		//裁剪
		if( y1 < 0 )
		{
			y1 = 0;
		}
		if( y2 >= g_RenderWnd->h )
		{
			y2 = g_RenderWnd->h - 1;
		}
		//初始缓存地址
		buffer = (char*)g_RenderWnd->pixels + ( g_RenderWnd->pitch * y1 + x1*3 );
		for( int i = y1; i <= y2; i++ )
		{
			buffer[0] = b;
			buffer[1] = g;
			buffer[2] = r;
			buffer += g_RenderWnd->pitch;
		}
	}else if( y1 == y2 )		//水平线
	{
		//保证X2大于等于X1
		if( x2 < x1 )
		{
			temp = x2;
			x2 = x1;
			x1 = temp;
		}
		//裁剪
		if( x1 < 0 )
		{
			x1 = 0;
		}
		if( x2 >= g_RenderWnd->w )
		{
			x2 = g_RenderWnd->w -1 ;
		}
		//初始缓存地址
		buffer = (char*)g_RenderWnd->pixels + ( g_RenderWnd->pitch * y1 + x1*3 );
		for( int i = x1; i <= x2; i ++ )
		{
			buffer[0] = b;
			buffer[1] = g;
			buffer[2] = r;
			buffer += 3;
		}
	}else						//斜线
	{
		float k = float( y2 - y1 ) / float( x2 - x1 );
		float absk = abs(k);
		//如果斜率小于1，保证x1小于等于x2；如果斜率大于1，保证y1小于等于y2
		if( ( absk < 1.0f && x2 < x1 ) || ( absk >= 1.0f && y2 < y1 ) )
		{
			x1 = x1 ^ x2;
			x2 = x1 ^ x2;
			x1 = x1 ^ x2;
			y1 = y1 ^ y2;
			y2 = y1 ^ y2;
			y1 = y1 ^ y2;
		}
		//斜率绝对值小于1的线
		if( absk < 1.0f )
		{
			float ypos = static_cast<float>(y1);
			for( int i = x1; i <= x2; i++ )
			{
				hgl_DrawPixel( i, int(ypos + 0.5f), r, g, b );
				ypos += k;
			}
		}else
		//斜率绝对值大于1的线
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

	//Bresenham算法【未完】
#ifdef BRESENHAM_LINE

#endif

}


//---------------
//数学函数
//---------------

//3d向量设置
vertex3 Vertex3( float x, float y, float z )
{
	vertex3 result;

	result._x = x;
	result._y = y;
	result._z = z;

	return result;
}

//4d向量设置
vertex4 Vertex4( float x, float y, float z, float w )
{
	vertex4 result;

	result._x = x;
	result._y = y;
	result._z = z;
	result._w = w;

	return result;
}

//齐次坐标转换为3D向量
vertex3 Vertex4To3( const vertex4& vtx4 )
{
	vertex3 vtx3;

	vtx3._x = vtx4._x / vtx4._w;
	vtx3._y = vtx4._y / vtx4._w;
	vtx3._z = vtx4._z / vtx4._w;

	return vtx3;
}

//3D向量转换为齐次坐标
vertex4 Vertex3To4( const vertex3& vtx3 )
{
	vertex4 vtx4;

	vtx4._x = vtx3._x;
	vtx4._y = vtx3._y;
	vtx4._z = vtx3._z;
	vtx4._w = 1.0f;

	return vtx4;
}

//向量取模
float Vertex3Length( const vertex3& vtx3 )
{
	return sqrt( vtx3._x * vtx3._x + vtx3._y * vtx3._y + vtx3._z * vtx3._z );
}

//设为零向量
void Vertex3Zero( vertex3& vtx3 )
{
	vtx3._x = 0.0f;
	vtx3._y = 0.0f;
	vtx3._z = 0.0f;
}

//变成单位向量
void Vertex3Normalize( vertex3& vtx3 )
{
	float mag = Vertex3Length( vtx3 );

	vtx3._x /= mag;
	vtx3._y /= mag;
	vtx3._z /= mag;
}

//向量加法
vertex3 Vertex3Add( const vertex3& vtx31, const vertex3& vtx32 )
{
	vertex3 result;

	result._x = vtx31._x + vtx32._x;
	result._y = vtx31._y + vtx32._y;
	result._z = vtx31._z + vtx32._z;

	return result;
}

//向量减法
vertex3 Vertex3Sub( const vertex3& vtx31, const vertex3& vtx32 )
{
	vertex3 result;

	result._x = vtx31._x - vtx32._x;
	result._y = vtx31._y - vtx32._y;
	result._z = vtx31._z - vtx32._z;

	return result;
}

//两点间距
float Vertex3Distance( const vertex3& vtx31, const vertex3& vtx32 )
{
	return Vertex3Length( Vertex3Sub( vtx31 , vtx32 ) );
}

//向量取反
void Vertex3Negate( vertex3& vtx3 )
{
	vtx3._x = -vtx3._x;
	vtx3._y = -vtx3._y;
	vtx3._z = -vtx3._z;
}

//标量乘
vertex3 Vertex3MutiScalar( const vertex3& vtx3, const float scalar )
{
	vertex3 result;

	result._x = vtx3._x * scalar;
	result._y = vtx3._y * scalar;
	result._z = vtx3._z * scalar;

	return result;
}

//标量除
vertex3 Vertex3DivScalar( const vertex3& vtx3, const float scalar )
{
	vertex3 result;

	result._x = vtx3._x / scalar;
	result._y = vtx3._y / scalar;
	result._z = vtx3._z / scalar;

	return result;
}

//向量点乘
float Vertex3DotProduct( const vertex3& vtx31, const vertex3& vtx32 )
{
	return vtx31._x * vtx32._x + vtx31._y * vtx32._y + vtx31._z * vtx32._z;
}

//向量叉乘
vertex3 Vertex3CorssProduct( const vertex3& vtx31, const vertex3& vtx32 )
{
	vertex3 result;

	result._x = vtx31._y * vtx32._z - vtx31._z * vtx32._y;
	result._y = vtx31._z * vtx32._x - vtx31._x * vtx32._z;
	result._z = vtx31._x * vtx32._y - vtx31._y * vtx32._x;

	return result;
}

//向量是否在平面前面
bool IsFront( const vertex3& vtx3, const plane& pln )
{
	float distance = Vertex3DotProduct( vtx3, pln._normal );

	if( distance - pln._distance > 0 )
	{
		return true;
	}

	return false;
}

//球是否在平面前面（相交不算）
bool IsFront( const vertex3& vtx3, float radius, const plane& pln )
{
	float distance = Vertex3DotProduct( vtx3, pln._normal );

	if( distance - pln._distance > radius )
	{
		return true;
	}

	return false;
}

//从三个点生成平面
plane GetPlane( const vertex3& vtx1, const vertex3& vtx2, const vertex3& vtx3 )
{
	plane result;

	vertex3 vc1 = Vertex3Sub( vtx2, vtx1 );
	vertex3 vc2 = Vertex3Sub( vtx3, vtx2 );
	result._normal = Vertex3CorssProduct( vc1, vc2 );
	Vertex3Normalize( result._normal );						//向量单位化

	result._distance = Vertex3DotProduct( result._normal, vtx1 );

	return result;
}

//44矩阵变成单位矩阵
void Matrix44Identity( matrix44& mat44 )
{
	memset( mat44._m, 0, sizeof( float ) * 16 );

	mat44._v1._x = 1.0f;
	mat44._v2._y = 1.0f;
	mat44._v3._z = 1.0f;
	mat44._v4._w = 1.0f;
}

//44矩阵相乘
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

//生成一个平移矩阵
void Matrix44Translate( matrix44& mat44, float x, float y, float z )
{
	Matrix44Identity( mat44 );

	mat44._v4._x = x;
	mat44._v4._y = y;
	mat44._v4._z = z;

}

//生成一个缩放矩阵
void Matrix44Scale( matrix44& mat44, float xScale, float yScale, float zScale )
{
	Matrix44Identity( mat44 );

	mat44._v1._x = xScale;
	mat44._v2._y = yScale;
	mat44._v3._z = zScale;

}

//生成一个沿指定轴缩放的矩阵
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

//生成一个绕X轴旋转矩阵（弧度制）
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

//生成一个绕Y轴旋转矩阵（弧度制）
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

//生成一个绕Z轴旋转矩阵（弧度制）
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

//生成一个绕任意轴旋转的矩阵（弧度制）
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

//齐次坐标乘矩阵44
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

//齐次坐标乘矩阵43
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

//坐标乘矩阵43（内部转换成齐次坐标再乘）
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

//坐标乘矩阵33
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

//计算投影坐标
void hgl_Projection( short* x, short *y, const perspectiveInfo& fun, const vertex3& vtx )
{
	*x = short( fun.mXfactor * vtx._x / vtx._z + fun.mXoffset );
	*y = short( fun.mYfactor * vtx._y / vtx._z + fun.mYoffset );
}

//物体剔除
bool hgl_CullPrimitive( const HGL_Primitive& primitive, const matrix44& transMatrix, const frustum& space )
{
	vertex3 obj = Vertex3( transMatrix._v4._x, transMatrix._v4._y, transMatrix._v4._z );

	//物体是否超出远近剪裁面
	//注意比较近剪裁面时只要物体包围球有一点超出就全部剔除，这是在不做剪裁的情况下防止那种可怕的错误透视发生
	if( obj._z - primitive.radius <= space._nearZ || 
		obj._z - primitive.radius >= space._farZ )
	{
		return true;
	}

	//视椎体左右剪裁面剔除
	if( IsFront( obj, primitive.radius, space._left ) || 
		IsFront( obj, primitive.radius, space._right ) )
	{
		return true;
	}

	//视椎体上下剪裁面剔除
	if( IsFront( obj, primitive.radius, space._top ) || 
		IsFront( obj, primitive.radius, space._bottom ) )
	{
		return true;
	}

	return false;
}

//初始化渲染列表（引擎初始化时做）
void hgl_InitRenderList( renderList& rl )
{
	rl.triangleCount = 0;
	rl.triList = new triangle();
	rl.curTriangle = rl.triList;

	rl.triList->next = NULL;
}

//清除渲染列表
void hgl_DestroyRenderList( renderList& rl )
{
	while( rl.triList )
	{
		rl.curTriangle = rl.triList;
		rl.triList = rl.triList->next;

		delete rl.curTriangle;
	}
}

//背面剔除判断
bool hgl_CullBack( triangle* facet )
{
	Vertex* vtxs = facet->_vtx;
	vertex3 facetDir;

	//该三角形的面法线
	facetDir = Vertex3CorssProduct( Vertex3Sub( vtxs[2]._position, vtxs[1]._position ), Vertex3Sub( vtxs[1]._position, vtxs[0]._position ) );

	if( Vertex3DotProduct( facetDir, vtxs[0]._position ) > 0 )
	{
		return true;
	}

	return false;
}

//光栅化一个线框三角形
void hgl_RasterLineTriangle( const triangle& facet )
{
	//画三条边
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

//光栅化一个上三角形（平底三角形）（实心）
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

	//确保第二个点位于第三个点的左边
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

	//按照Y值裁剪
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

	//X值裁剪判断
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
	//X值需要裁减
	{
		for( tmpy = y1; tmpy <= y3; tmpy++, buffer += g_RenderWnd->pitch )
		{
			iLeft = (unsigned int)left;
			iRight = (unsigned int)right;

			left += dxLeft;
			right += dxRight;

			//超出屏幕左边
			if( iLeft < 0 )
			{
				iLeft = 0;

				if( iRight < 0 )
				{
					continue;
				}
			}
			//超出屏幕右边
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

//光栅化一个下三角形（平顶三角形）（实心）
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

	//确保第一个点位于第二个点的左边
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

	//按照Y值裁剪
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

	//X值裁剪判断
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
	//X值需要裁减
	{
		for( tmpy = y1; tmpy <= y3; tmpy++, buffer += g_RenderWnd->pitch )
		{
			iLeft = (unsigned int)left;
			iRight = (unsigned int)right;

			left += dxLeft;
			right += dxRight;

			//超出屏幕左边
			if( iLeft < 0 )
			{
				iLeft = 0;

				if( iRight < 0 )
				{
					continue;
				}
			}
			//超出屏幕右边
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

//光栅化三角形（实心）
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

	//退化三角形不作渲染
	if( (x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3) )
	{
		return;
	}

	int tmpx;
	int tmpy;
	//按照Y值来排序，Y1-Y3从小到大
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

	//剔除掉不在屏幕范围内的三角形
	int wid = g_RenderWnd->w - 1;
	int hei = g_RenderWnd->h - 1;
	if( y3 < 0 || y1 > hei || ( x1 < 0 && x2 < 0 && x3 < 0 ) || ( x1 > wid && x2 > wid && x3 > wid ) )
	{
		return;
	}

	//切割成平顶三角形和平底三角形
	int newx;
	//平顶三角形
	if( y1 == y2 )
	{
		hgl_DrawDownTrangle( x1, y1, x2, y2, x3, y3, r, g, b );
	}else
	//平底三角形
	if( y2 == y3 )
	{
		hgl_DrawTopTrangle( x1, y1, x2, y2, x3, y3, r, g, b );
	}else
	//需要拆分
	{
		newx = x1 + (int)(0.5+(float)(y2-y1)*(float)(x3-x1)/(float)(y3-y1));

		hgl_DrawTopTrangle( x1, y1, x2, y2, newx, y2, r, g, b );
		hgl_DrawDownTrangle( x2, y2, newx, y2, x3, y3, r, g, b );
	}

}

//光栅化三角形（实心）（高洛德着色实心）	【未完】
void hgl_RasterGouraudTrangle( const triangle& facet )
{
}

//光栅化平顶三角形（带纹理）（平坦着色）
void hgl_DrawDownTrangleTexFlat( const Vertex* vtx1, const Vertex* vtx2, const Vertex* vtx3 )
{
	const Vertex* pt1 = vtx1;
	const Vertex* pt2 = vtx2;
	const Vertex* pt3 = vtx3;

	//确保第一个点在第二个点左边
	if( pt1->_pixel._x > pt2->_pixel._x )
	{
		const Vertex* tmp = pt1;
		pt1 = pt2;
		pt2 = tmp;
	}

	char* buffer;
	char* bufferX;

	float lightPower = pt1->_power;		//光照强度
	unsigned char r,g,b;				//颜色缓存

	int yBegin = pt1->_pixel._y;		//Y坐标起始点
	int yEnd = pt3->_pixel._y;			//Y坐标结束点

	float left = float( pt1->_pixel._x );
	float right = float( pt2->_pixel._x );

	float leftU = pt1->_uv._x;
	float leftV = pt1->_uv._y;
	float rightU = pt2->_uv._x;
	float rightV = pt2->_uv._y;

	//坐标及UV的增长值
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

		//UV插值
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

		//光栅化行
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

//光栅化平底三角形（带纹理）（平坦着色）
void hgl_DrawTopTrangleTexFlat( const Vertex* vtx1, const Vertex* vtx2, const Vertex* vtx3 )
{
	const Vertex* pt1 = vtx1;
	const Vertex* pt2 = vtx2;
	const Vertex* pt3 = vtx3;

	//确保第二个点在第三个点左边
	if( pt2->_pixel._x > pt3->_pixel._x )
	{
		const Vertex* tmp = pt2;
		pt2 = pt3;
		pt3 = tmp;
	}

	char* buffer;
	char* bufferX;

	float lightPower = pt1->_power;		//光照强度
	unsigned char r,g,b;				//颜色缓存

	int yBegin = pt1->_pixel._y;		//Y坐标起始点
	int yEnd = pt3->_pixel._y;			//Y坐标结束点

	float left = float( pt1->_pixel._x );
	float right = left;

	float leftU = pt1->_uv._x;
	float leftV = pt1->_uv._y;
	float rightU = leftU;
	float rightV = leftV;

	//坐标及UV的增长值
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

		//UV插值
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

		//光栅化行
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

//光栅化三角形（带纹理）（平坦着色）
void hgl_RasterTrangleTex( const triangle& facet )
{
	const Vertex* vtx1 = &facet._vtx[0];
	const Vertex* vtx2 = &facet._vtx[1];
	const Vertex* vtx3 = &facet._vtx[2];

	//退化三角形不渲染
	if( ( vtx1->_pixel._x == vtx2->_pixel._x && vtx1->_pixel._x == vtx3->_pixel._x ) ||
		( vtx1->_pixel._y == vtx2->_pixel._y && vtx1->_pixel._y == vtx3->_pixel._y ) )
	{
		return;
	}

	const Vertex* vtxTmp;
	//按照Y值进行排序
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

	//不在屏幕内的剔除
	int wid = g_RenderWnd->w - 1;
	int hei = g_RenderWnd->h - 1;
	if( vtx1->_pixel._y < 0 || vtx3->_pixel._y > hei || 
		( vtx1->_pixel._x < 0 && vtx2->_pixel._x < 0 && vtx3->_pixel._x < 0 ) || 
		( vtx1->_pixel._x > wid && vtx2->_pixel._x > wid && vtx3->_pixel._x > wid ) )
	{
		return;
	}

	//切割成平顶三角形和平底三角形
	//平顶三角形
	if( vtx1->_pixel._y == vtx2->_pixel._y )
	{
		hgl_DrawDownTrangleTexFlat( vtx1, vtx2, vtx3 );
	}else
	//平底三角形
	if( vtx2->_pixel._y == vtx3->_pixel._y )
	{
		hgl_DrawTopTrangleTexFlat( vtx1, vtx2, vtx3 );
	}else
	//拆分成平顶三角形和平底三角形
	{
		Vertex tmp;

		//坐标插值
		tmp._pixel._x = vtx1->_pixel._x + int( float(vtx2->_pixel._y - vtx1->_pixel._y) * float(vtx3->_pixel._x - vtx1->_pixel._x) / float(vtx3->_pixel._y - vtx1->_pixel._y) );
		tmp._pixel._y = vtx2->_pixel._y;
		//颜色沿用三角形的颜色，因为是平坦着色
		tmp._color = facet._color;
		//法线也是，因为是平坦着色
		tmp._normal = facet._normal;
		//光照强度也是，因为是平坦着色
		tmp._power = vtx1->_power;
		//uv要插值得到
		tmp._uv._x = vtx1->_uv._x + (vtx3->_uv._x - vtx1->_uv._x) * (vtx2->_pixel._y - vtx1->_pixel._y) / (vtx3->_pixel._y - vtx1->_pixel._y);
		tmp._uv._y = vtx1->_uv._y + (vtx3->_uv._y - vtx1->_uv._y) * (vtx2->_pixel._y - vtx1->_pixel._y) / (vtx3->_pixel._y - vtx1->_pixel._y);

		hgl_DrawDownTrangleTexFlat( vtx2, &tmp, vtx3 );
		hgl_DrawTopTrangleTexFlat( vtx1, vtx2, &tmp );
		
	}

}

//光栅化三角形（带纹理）（高罗德着色）		【未完】
void hgl_RasterGouraudTrangleTex( const triangle& facet )
{
}

//渲染状态初始化
void hgl_InitRenderState()
{
	int i;

	//初始多边形剔除模式为不做剔除
	g_RenderState.m_CullMode = eHGLNonCull;

	//初始多边形填充模式为实体填充
	g_RenderState.m_FillMode = eHGLSolidMode;

	//光照信息初始化，正常光照一开始关闭
	g_RenderState.m_Lighting = false;
	for( i = 0; i < HGL_MaxLight; i++ )
	{
		g_RenderState.m_lightEnable[i] = false;
	}

	//初始着色模式
	g_RenderState.m_shadeMode = HGL_FLAT;

	//简单光照计算一开始关闭
	g_RenderState.m_SimpleLighting = false;
	for( i = 0; i < HGL_MaxSimpleLight; i++ )
	{
		g_RenderState.m_SimpleLightEnable[i] = false;
	}

	//初始纹理映射模式(初始关闭)
	g_RenderState.m_TexMode = HGL_TexDisable;
	//绑定的贴图初始为空
	for( i = 0; i < HGL_MaxTexLev; i++ )
	{
		g_RenderState.m_texture[i] = NULL;
	}

}

//光照计算	【未完】
void hgl_LightCalc( triangle* facet, HGLRenderState& state )
{
	switch( state.m_shadeMode )
	{
	case HGL_FLAT:					//只用面法线计算光照后的面颜色
		break;
	case HGL_GOURAUD:				//用顶点法线计算光照后的顶点颜色，中间像素插值
		break;
	case HGL_PHONG:					//插值得到顶点间像素的法线，根据法线计算像素光照后的颜色
		//[暂不实现]
		break;
	default:
		break;
	}
}

//简单光照计算		【未完】
void hgl_SimpleLightCalc( triangle* facet, HGLRenderState& state )
{
	float power = 0.0f;
	float tmpPower;
	int i;
	vertex3 dir;

	switch( state.m_shadeMode )
	{
	case HGL_FLAT:					//【平坦着色】只用面法线计算光照后的面颜色
		//0号环境光
		if( state.m_SimpleLightEnable[HGL_SimpleLightAmbient] )
		{
			power = state.m_SimpleLights[HGL_SimpleLightAmbient]._power;
		}
		power = 1.0f - power;
		//1号2号漫射光
		for( i = HGL_SimpleLightDiffuse1; i <= HGL_SimpleLightDiffuse2; i++ )
		{
			if( state.m_SimpleLightEnable[i] )
			{
				//方向光计算
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
				//点光源计算
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

		//每个顶点记录光照强度
		facet->_vtx[0]._power = power;
		facet->_vtx[1]._power = power;
		facet->_vtx[2]._power = power;
		break;
	case HGL_GOURAUD:				//【高罗德着色】用顶点法线计算光照后的顶点颜色，中间像素插值
		;
		break;
	case HGL_PHONG:					//【冯氏着色】插值得到顶点间像素地法线，根据法线计算像素光照后的颜色
		//[不实现]
		break;
	default:
		break;
	}
}

//锁定位图
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

//解锁位图
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

//颜色采样
void hgl_sampler( HGL_Texture* tex, float u, float v, unsigned char* r, unsigned char* g, unsigned char* b )
{
	//将UV转换为坐标
	int x =	static_cast<int>( static_cast<float>(tex->_wid) * u );
	int y = static_cast<int>( static_cast<float>(tex->_hei) * v );

	char* pixel = (char*)tex->_data->pixels;
	pixel += ( tex->_data->pitch * y + 3 * x );

	*r = pixel[2];
	*g = pixel[1];
	*b = pixel[0];
}
