#include "stdafx.h"
#include "HGLx.h"
#include "HGL.h"
#include "HGLInnerFun.h"

#include <vector>

using namespace std;


//读取OBJ文件的一行
bool OBJ_ReadLine( FILE* file, char* lineBuffer )
{
	//已到大文件尾
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

		//用于处理多行使用'\'连接在一起的情况
		if( lineBuffer[count] != '\\' )
		{
			lineBuffer[count] = 0;
			finish = true;
		}
	}

	return true;
}


//加载一个OBJ模型	【未完】
bool HGLx_LoadModelOBJ( HGL_Primitive& model, char* fileName )
{
	FILE* mod;

	mod = fopen( fileName, "rb" );

	//文件不存在，模型加载失败
	if( mod == NULL )
	{
		return false;
	}

	//先分析OBJ文件
	char lineBuffer[128];
	while( OBJ_ReadLine( mod, lineBuffer ) )
	{
		printf( "%s\n", lineBuffer );
	}

	//构建模型
	HGL_Begin( model, 20 );
	HGL_End();

	fclose( mod );

	return true;
}
