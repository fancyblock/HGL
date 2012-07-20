#pragma once
//一些枚举、结构体的定义


//这个引擎目前所处的状态枚举
enum HGLState
{
	eHGLNon = 0,		//空白状态，尚未初始化
	eHGLInit,			//初始化过了，尚未有窗口
	eHGLWnd,			//只有创建了一个窗口状态
	eHGLReady,			//设置完透视投影，可以开始绘制了
};

//事件类型枚举
enum HGLEvtType
{
	eHGLQuit = 0,		//退出事件
	eHGLKeyboardDown,	//键盘按下事件
	eHGLKeyboardUp,		//键盘抬起事件
	eHGLMouseDown,		//鼠标按下事件
	eHGLMouseUp,		//鼠标抬起事件
	eHGLWndActive,		//窗口活动事件
	eHGLSystem,			//系统事件
	eHGLUser,			//用户事件
};

//事件结构【未完】
struct HGLEvt
{
	HGLEvtType m_evtType;	//事件类型
	struct					//鼠标位置
	{
		int _x;
		int _y;
	}m_mousePos;
};

//定义一些常用按键
enum HGLCommonKey
{
	eHGL_UP = SDLK_UP,				//上
	eHGL_DOWN = SDLK_DOWN,			//下
	eHGL_LEFT = SDLK_LEFT,			//左
	eHGL_RIGHT = SDLK_RIGHT,		//右
	eHGL_SPACE = SDLK_SPACE,		//空格
	eHGL_ENTER = SDLK_RETURN,		//回车
	eHGL_ESC = SDLK_ESCAPE,			//退出
};

//每像素的深度
const int PIXCEL_DEPTH = 24;

//矩阵类型枚举
enum HGLMatrixType
{
	eHGLWorldMatrix = 0,		//世界矩阵
	eHGLCameraMatrix,			//Camera矩阵
};

//绘制的基元类型
enum HGLPrimitiveType
{
	eHGLPrimitivePoint = 0,		//点
	eHGLPrimitiveLine,			//线段
	eHGLPrimitiveTriangle,		//三角形
};

//多边形剔除模式枚举
enum HGLCullMode
{
	eHGLNonCull = 0,			//不剔除
	eHGLBackCull,				//剔除背面
	eHGLFrontCull,				//剔除正面
};

//填充模式定义
enum HGLFillMode
{
	eHGLLineMode = 0,			//线框模式
	eHGLSolidMode,				//实心模式
};

//4D顶点定义
union vertex4
{
	struct
	{
		float _x;
		float _y;
		float _z;
		float _w;
	};
	float _m[4];
};

//3D顶点定义
union vertex3
{
	struct
	{
		float _x;
		float _y;
		float _z;
	};
	float _m[3];
};

//2D顶点定义
union vertex2
{
	struct
	{
		float _x;
		float _y;
	};
	float _m[2];
};

//3x3矩阵定义
union matrix33
{
	struct
	{
		vertex3 _v1;
		vertex3 _v2;
		vertex3 _v3;
	};
	vertex3 _vv[3];
	float _m[9];
};

//4x3矩阵定义
union matrix43
{
	struct
	{
		vertex3 _v1;
		vertex3 _v2;
		vertex3 _v3;
		vertex3 _v4;
	};
	vertex3 _vv[4];
	float _m[12];
};

//4x4矩阵定义
union matrix44
{
	struct
	{
		vertex4 _v1;
		vertex4 _v2;
		vertex4 _v3;
		vertex4 _v4;
	};
	vertex4 _vv[4];
	float _m[16];
};

//平面定义
struct plane
{
	vertex3 _normal;
	float _distance;
};


//用于表示视锥体
struct frustum
{
	float _nearZ;
	float _farZ;
	plane _left;
	plane _right;
	plane _top;
	plane _bottom;
};

//透视计算需要的信息们
struct perspectiveInfo
{
	int _screenWid;				//窗口宽度
	int _screenHei;				//窗口高度
	float _fovy;				//视野（角度，弧度制）
	float _aspect;				//宽高比

	float mXfactor;				//X坐标要乘的值	（这四个值通过窗口宽高、视野角度、宽高比来计算）
	float mXoffset;				//X坐标要加的值
	float mYfactor;				//Y坐标要乘的值
	float mYoffset;				//Y坐标要加的值
};

//像素格式
struct pixel
{
	short _x;
	short _y;
};

//颜色格式
struct RGBcolor
{
	unsigned char _r;
	unsigned char _g;
	unsigned char _b;
};

//颜色格式（带透明度）
union RGBAColor
{
	struct 
	{
		unsigned char _r;
		unsigned char _g;
		unsigned char _b;
		unsigned char _a;
	};

	int _col;
};

//顶点格式
struct Vertex
{
	vertex3 _position;			//顶点位置
	vertex3 _normal;			//法线向量
	RGBcolor _color;			//顶点颜色
	vertex2 _uv;				//贴图坐标
	float _power;				//光照强度

	pixel _pixel;				//转换后的像素位置
};

//物体定义
struct HGL_Primitive
{
	HGL_Primitive():vertexCount(0),buffer(NULL){}

	int vertexCount;			//顶点数量
	Vertex* buffer;				//顶点们
	float radius;				//该物体的包围球半径

};

//物体定义（使用顶点索引）
struct HGL_IndexPrimitive
{
	HGL_IndexPrimitive():vertexCount(0),vtxBuffer(NULL){}

	int vertexCount;			//顶点数量
	Vertex* vtxBuffer;			//定点们
	int indexBuffer;			//索引们
	float radius;				//该物体的包围球半径
};

//当前编辑物体定义
struct EditPrimitive
{
	EditPrimitive():exist(false){}

	bool exist;								//当前有编辑物体否？（只在BEGIN和END之间为true）

	HGL_Primitive* curPrimitive;			//当前被编辑的物体
	HGL_IndexPrimitive* curIndexPrimitive;	//当前被编辑的物体（顶点索引）

	int vertexIndex;						//当前编辑到的顶点索引号
	int indexIndex;							//当前编辑到得索引索引号

};

//三角形定义
struct triangle
{
	Vertex _vtx[3];				//该三角形的三个顶点
	vertex3 _normal;			//该三角形法线
	RGBcolor _color;			//该三角形颜色，flat着色模式下用

	triangle* next;
};

//渲染列表定义
struct renderList
{
	int triangleCount;			//该渲染列表包含的三角形数

	triangle* triList;			//指向三角形列表的指针
	triangle* curTriangle;		//当前三角形

};

//材质定义
struct HGL_Matrial
{
	int attri;					//材质属性

	RGBAColor ambient;			//环境光反射参数
	RGBAColor diffuse;			//漫射光反射参数
	RGBAColor specular;			//高光反射参数
	RGBAColor emissive;			//自发光参数

};

//光源类型
enum HGLLightType
{
	eHGLLightPoint = 0,			//点光源
	eHGLLightSpot,				//聚光灯
	eHGLLightDirectional,		//方向光
};

//光源定义
struct HGL_Light
{
	HGLLightType _type;			//光源类型

	vertex3 _position;			//点光源和聚光灯的位置
	vertex3 _direction;			//方向光的方向
	float _spotInner;			//聚光灯内锥角
	float _spotOuter;			//聚光灯外锥角
	float _fallOff;				//聚光灯衰减指数
	float _attenuation0;		//衰减因子1
	float _attenuation1;		//衰减因子2
	float _attenuation2;		//衰减因子3

	vertex4 _ambient;			//环境光分量
	vertex4 _diffuse;			//漫射光分量
	vertex4 _specular;			//高光分量

};

//简单光照光源定义，只计算漫射光和环境光
struct HGL_SimpleLight
{
	HGLLightType _type;			//光源类型，只包含平行光和点光源

	vertex3 _position;			//点光源位置
	vertex3 _direction;			//平行光方向

	float _fallOff;				//点光源衰减因子
	float _power;				//光源的光强度（0为最大，1为无光）

};

//光源号的枚举，最多支持8个光源同时存在
enum HGL_LightNum
{
	HGL_Light0 = 0,
	HGL_Light1,
	HGL_Light2,
	HGL_Light3,
	HGL_Light4,
	HGL_Light5,
	HGL_Light6,
	HGL_Light7,
	HGL_MaxLight,
};

//简单光源，光源号的枚举，最大3个光源
enum HGL_SimpleLightNum
{
	HGL_SimpleLightAmbient = 0,		//0号光源只是环境光
	HGL_SimpleLightDiffuse1,		//1号光源和2号光源都是漫射光
	HGL_SimpleLightDiffuse2,
	HGL_MaxSimpleLight,
};

//着色模型
enum HGL_ShadeModel
{
	HGL_FLAT = 0,			//平坦模式，顶点法线不起作用（按照面法线来计算光照）
	HGL_GOURAUD,			//高洛德着色
	HGL_PHONG,				//冯氏着色（暂时不予实现）
};

//渲染选项	【未完】
enum HGL_RenderOption
{
	HGL_LIGHT = 0,			//光照
	HGL_SIMPLELIGHT,		//简单光照
	HGL_ZBUFFER,			//Z缓冲
	HGL_TEXTURE,			//纹理映射
};

//贴图模式
enum HGL_TextureMode
{
	HGL_TexDisable = 0,		//未开启纹理映射
	HGL_TexEnable,			//开启纹理映射
	HGL_MutiTexture,		//开启多重纹理映射
};

//贴图等级
enum HGL_TextureLevel
{
	HGL_Tex0 = 0,			//0号贴图
	HGL_Tex1,
	HGL_Tex2,
	HGL_Tex3,
	HGL_MaxTexLev,			//贴图最大层数
};


//纹理贴图
struct HGL_Texture
{
	int _wid;				//纹理的尺寸
	int _hei;

	SDL_Surface* _data;		//贴图

	bool _locked;			//锁定住了

	HGL_Texture():_wid(0),_hei(0),_data(0),_locked(false){}
	~HGL_Texture()
	{
		if( _data )
		{
			SDL_FreeSurface( _data );
		}
	}
};


//一些渲染状态
struct HGLRenderState
{
	HGLCullMode m_CullMode;				//多边形剔除模式
	HGLFillMode m_FillMode;				//多边形填充模式
	HGL_Matrial m_matrial;				//当前材质

	bool m_Lighting;					//开启光照计算
	HGL_Light m_lights[HGL_MaxLight];	//光源们
	bool m_lightEnable[HGL_MaxLight];	//光源开启标志

	bool m_SimpleLighting;									//开启简单光照计算
	HGL_SimpleLight m_SimpleLights[HGL_MaxSimpleLight];		//简单光源们
	bool m_SimpleLightEnable[HGL_MaxSimpleLight];			//简单光源开启标志

	HGL_TextureMode m_TexMode;								//纹理映射模式
	HGL_Texture* m_texture[HGL_MaxTexLev];					//使用的贴图

	HGL_ShadeModel m_shadeMode;			//着色模式

	matrix44 m_WorldMatrix;				//世界矩阵
	matrix44 m_CameraMatrix;			//Camera矩阵
	matrix44 m_ModelView;				//变换矩阵（世界矩阵乘相机矩阵）

};
