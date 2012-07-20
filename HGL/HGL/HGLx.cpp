#include "stdafx.h"
#include "HGLx.h"
#include "HGL.h"
#include "HGLInnerFun.h"

#include <vector>

using namespace std;


//��ȡOBJ�ļ���һ��
bool OBJ_ReadLine( FILE* file, char* lineBuffer )
{
	//�ѵ����ļ�β
	if( feof( file ) )
	{
		return false;
	}

	int count = 0;
	bool finish = false;

	while( !finish )
	{
		do 
		{
			if( feof( file ) )
			{
				lineBuffer[count] = 0;
				finish = true;
				break;
			}
			lineBuffer[count] = fgetc( file );
			count++;
		}while( lineBuffer[count-1] != '\n' );

		count -= 2;

		//���ڴ������ʹ��'\'������һ������
		if( lineBuffer[count] != '\\' )
		{
			lineBuffer[count] = 0;
			finish = true;
		}
	}

	return true;
}


//����һ��OBJģ��	��δ�꡿
bool HGLx_LoadModelOBJ( HGL_Primitive& model, char* fileName )
{
	FILE* mod;

	mod = fopen( fileName, "rb" );

	//�ļ������ڣ�ģ�ͼ���ʧ��
	if( mod == NULL )
	{
		return false;
	}

	//�ȷ���OBJ�ļ�
	char lineBuffer[128];
	while( OBJ_ReadLine( mod, lineBuffer ) )
	{
		printf( "%s\n", lineBuffer );
	}

	//����ģ��
	HGL_Begin( model, 20 );
	HGL_End();

	fclose( mod );

	return true;
}
