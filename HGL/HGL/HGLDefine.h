#pragma once
//һЩö�١��ṹ��Ķ���


//�������Ŀǰ������״̬ö��
enum HGLState
{
	eHGLNon = 0,		//�հ�״̬����δ��ʼ��
	eHGLInit,			//��ʼ�����ˣ���δ�д���
	eHGLWnd,			//ֻ�д�����һ������״̬
	eHGLReady,			//������͸��ͶӰ�����Կ�ʼ������
};

//�¼�����ö��
enum HGLEvtType
{
	eHGLQuit = 0,		//�˳��¼�
	eHGLKeyboardDown,	//���̰����¼�
	eHGLKeyboardUp,		//����̧���¼�
	eHGLMouseDown,		//��갴���¼�
	eHGLMouseUp,		//���̧���¼�
	eHGLWndActive,		//���ڻ�¼�
	eHGLSystem,			//ϵͳ�¼�
	eHGLUser,			//�û��¼�
};

//�¼��ṹ��δ�꡿
struct HGLEvt
{
	HGLEvtType m_evtType;	//�¼�����
	struct					//���λ��
	{
		int _x;
		int _y;
	}m_mousePos;
};

//����һЩ���ð���
enum HGLCommonKey
{
	eHGL_UP = SDLK_UP,				//��
	eHGL_DOWN = SDLK_DOWN,			//��
	eHGL_LEFT = SDLK_LEFT,			//��
	eHGL_RIGHT = SDLK_RIGHT,		//��
	eHGL_SPACE = SDLK_SPACE,		//�ո�
	eHGL_ENTER = SDLK_RETURN,		//�س�
	eHGL_ESC = SDLK_ESCAPE,			//�˳�
};

//ÿ���ص����
const int PIXCEL_DEPTH = 24;

//��������ö��
enum HGLMatrixType
{
	eHGLWorldMatrix = 0,		//�������
	eHGLCameraMatrix,			//Camera����
};

//���ƵĻ�Ԫ����
enum HGLPrimitiveType
{
	eHGLPrimitivePoint = 0,		//��
	eHGLPrimitiveLine,			//�߶�
	eHGLPrimitiveTriangle,		//������
};

//������޳�ģʽö��
enum HGLCullMode
{
	eHGLNonCull = 0,			//���޳�
	eHGLBackCull,				//�޳�����
	eHGLFrontCull,				//�޳�����
};

//���ģʽ����
enum HGLFillMode
{
	eHGLLineMode = 0,			//�߿�ģʽ
	eHGLSolidMode,				//ʵ��ģʽ
};

//4D���㶨��
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

//3D���㶨��
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

//2D���㶨��
union vertex2
{
	struct
	{
		float _x;
		float _y;
	};
	float _m[2];
};

//3x3������
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

//4x3������
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

//4x4������
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

//ƽ�涨��
struct plane
{
	vertex3 _normal;
	float _distance;
};


//���ڱ�ʾ��׶��
struct frustum
{
	float _nearZ;
	float _farZ;
	plane _left;
	plane _right;
	plane _top;
	plane _bottom;
};

//͸�Ӽ�����Ҫ����Ϣ��
struct perspectiveInfo
{
	int _screenWid;				//���ڿ��
	int _screenHei;				//���ڸ߶�
	float _fovy;				//��Ұ���Ƕȣ������ƣ�
	float _aspect;				//��߱�

	float mXfactor;				//X����Ҫ�˵�ֵ	�����ĸ�ֵͨ�����ڿ�ߡ���Ұ�Ƕȡ���߱������㣩
	float mXoffset;				//X����Ҫ�ӵ�ֵ
	float mYfactor;				//Y����Ҫ�˵�ֵ
	float mYoffset;				//Y����Ҫ�ӵ�ֵ
};

//���ظ�ʽ
struct pixel
{
	short _x;
	short _y;
};

//��ɫ��ʽ
struct RGBcolor
{
	unsigned char _r;
	unsigned char _g;
	unsigned char _b;
};

//��ɫ��ʽ����͸���ȣ�
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

//�����ʽ
struct Vertex
{
	vertex3 _position;			//����λ��
	vertex3 _normal;			//��������
	RGBcolor _color;			//������ɫ
	vertex2 _uv;				//��ͼ����
	float _power;				//����ǿ��

	pixel _pixel;				//ת���������λ��
};

//���嶨��
struct HGL_Primitive
{
	HGL_Primitive():vertexCount(0),buffer(NULL){}

	int vertexCount;			//��������
	Vertex* buffer;				//������
	float radius;				//������İ�Χ��뾶

};

//���嶨�壨ʹ�ö���������
struct HGL_IndexPrimitive
{
	HGL_IndexPrimitive():vertexCount(0),vtxBuffer(NULL){}

	int vertexCount;			//��������
	Vertex* vtxBuffer;			//������
	int indexBuffer;			//������
	float radius;				//������İ�Χ��뾶
};

//��ǰ�༭���嶨��
struct EditPrimitive
{
	EditPrimitive():exist(false){}

	bool exist;								//��ǰ�б༭����񣿣�ֻ��BEGIN��END֮��Ϊtrue��

	HGL_Primitive* curPrimitive;			//��ǰ���༭������
	HGL_IndexPrimitive* curIndexPrimitive;	//��ǰ���༭�����壨����������

	int vertexIndex;						//��ǰ�༭���Ķ���������
	int indexIndex;							//��ǰ�༭��������������

};

//�����ζ���
struct triangle
{
	Vertex _vtx[3];				//�������ε���������
	vertex3 _normal;			//�������η���
	RGBcolor _color;			//����������ɫ��flat��ɫģʽ����

	triangle* next;
};

//��Ⱦ�б���
struct renderList
{
	int triangleCount;			//����Ⱦ�б��������������

	triangle* triList;			//ָ���������б��ָ��
	triangle* curTriangle;		//��ǰ������

};

//���ʶ���
struct HGL_Matrial
{
	int attri;					//��������

	RGBAColor ambient;			//�����ⷴ�����
	RGBAColor diffuse;			//����ⷴ�����
	RGBAColor specular;			//�߹ⷴ�����
	RGBAColor emissive;			//�Է������

};

//��Դ����
enum HGLLightType
{
	eHGLLightPoint = 0,			//���Դ
	eHGLLightSpot,				//�۹��
	eHGLLightDirectional,		//�����
};

//��Դ����
struct HGL_Light
{
	HGLLightType _type;			//��Դ����

	vertex3 _position;			//���Դ�;۹�Ƶ�λ��
	vertex3 _direction;			//�����ķ���
	float _spotInner;			//�۹����׶��
	float _spotOuter;			//�۹����׶��
	float _fallOff;				//�۹��˥��ָ��
	float _attenuation0;		//˥������1
	float _attenuation1;		//˥������2
	float _attenuation2;		//˥������3

	vertex4 _ambient;			//���������
	vertex4 _diffuse;			//��������
	vertex4 _specular;			//�߹����

};

//�򵥹��չ�Դ���壬ֻ���������ͻ�����
struct HGL_SimpleLight
{
	HGLLightType _type;			//��Դ���ͣ�ֻ����ƽ�й�͵��Դ

	vertex3 _position;			//���Դλ��
	vertex3 _direction;			//ƽ�йⷽ��

	float _fallOff;				//���Դ˥������
	float _power;				//��Դ�Ĺ�ǿ�ȣ�0Ϊ���1Ϊ�޹⣩

};

//��Դ�ŵ�ö�٣����֧��8����Դͬʱ����
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

//�򵥹�Դ����Դ�ŵ�ö�٣����3����Դ
enum HGL_SimpleLightNum
{
	HGL_SimpleLightAmbient = 0,		//0�Ź�Դֻ�ǻ�����
	HGL_SimpleLightDiffuse1,		//1�Ź�Դ��2�Ź�Դ���������
	HGL_SimpleLightDiffuse2,
	HGL_MaxSimpleLight,
};

//��ɫģ��
enum HGL_ShadeModel
{
	HGL_FLAT = 0,			//ƽ̹ģʽ�����㷨�߲������ã������淨����������գ�
	HGL_GOURAUD,			//�������ɫ
	HGL_PHONG,				//������ɫ����ʱ����ʵ�֣�
};

//��Ⱦѡ��	��δ�꡿
enum HGL_RenderOption
{
	HGL_LIGHT = 0,			//����
	HGL_SIMPLELIGHT,		//�򵥹���
	HGL_ZBUFFER,			//Z����
	HGL_TEXTURE,			//����ӳ��
};

//��ͼģʽ
enum HGL_TextureMode
{
	HGL_TexDisable = 0,		//δ��������ӳ��
	HGL_TexEnable,			//��������ӳ��
	HGL_MutiTexture,		//������������ӳ��
};

//��ͼ�ȼ�
enum HGL_TextureLevel
{
	HGL_Tex0 = 0,			//0����ͼ
	HGL_Tex1,
	HGL_Tex2,
	HGL_Tex3,
	HGL_MaxTexLev,			//��ͼ������
};


//������ͼ
struct HGL_Texture
{
	int _wid;				//����ĳߴ�
	int _hei;

	SDL_Surface* _data;		//��ͼ

	bool _locked;			//����ס��

	HGL_Texture():_wid(0),_hei(0),_data(0),_locked(false){}
	~HGL_Texture()
	{
		if( _data )
		{
			SDL_FreeSurface( _data );
		}
	}
};


//һЩ��Ⱦ״̬
struct HGLRenderState
{
	HGLCullMode m_CullMode;				//������޳�ģʽ
	HGLFillMode m_FillMode;				//��������ģʽ
	HGL_Matrial m_matrial;				//��ǰ����

	bool m_Lighting;					//�������ռ���
	HGL_Light m_lights[HGL_MaxLight];	//��Դ��
	bool m_lightEnable[HGL_MaxLight];	//��Դ������־

	bool m_SimpleLighting;									//�����򵥹��ռ���
	HGL_SimpleLight m_SimpleLights[HGL_MaxSimpleLight];		//�򵥹�Դ��
	bool m_SimpleLightEnable[HGL_MaxSimpleLight];			//�򵥹�Դ������־

	HGL_TextureMode m_TexMode;								//����ӳ��ģʽ
	HGL_Texture* m_texture[HGL_MaxTexLev];					//ʹ�õ���ͼ

	HGL_ShadeModel m_shadeMode;			//��ɫģʽ

	matrix44 m_WorldMatrix;				//�������
	matrix44 m_CameraMatrix;			//Camera����
	matrix44 m_ModelView;				//�任�������������������

};
