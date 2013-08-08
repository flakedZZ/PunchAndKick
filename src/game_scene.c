﻿#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_DISPLAY_H
#include "game.h"

#define STATUS_BAR_HEIGHT	90	/**< 状态栏的高度 */
#define CAMERA_X_PANDDING	200	/**< 镜头的水平内边距 */

static LCUI_Size scene_land_size = {0,0};
static LCUI_Pos scene_land_pos = {0,0};

/** 初始化战斗场景 */
int GameScene_Init( LCUI_Widget *scene )
{
	int ret;
	LCUI_Size scene_size;
	LCUI_Graph scene_graph;

	Graph_Init( &scene_graph );
	
	ret = GameGraphRes_LoadFromFile("scenes.data");
	if( ret != 0 ) {
		LCUI_MessageBoxW(
			MB_ICON_ERROR,
			L"场景资源载入出错，请检查程序的完整性！",
			L"错误", MB_BTN_OK );
		return ret;
	}
	/* 载入战斗场景的图形资源 */
	GameGraphRes_GetGraph( SCENES_RES, "default", &scene_graph );
	scene_size = Graph_GetSize( &scene_graph );
	/* 设置战斗场景 */
	Widget_SetBackgroundImage( scene, &scene_graph );
	Widget_SetBackgroundLayout( scene, LAYOUT_CENTER );
	Widget_SetBackgroundColor( scene, RGB(0,0,0) );
	Widget_SetBackgroundTransparent( scene, FALSE );
	/* 调整战斗场景的尺寸 */
	Widget_Resize( scene, scene_size );
	/* 设置战斗场景显示的位置 */
	Widget_SetAlign( scene, ALIGN_BOTTOM_CENTER, Pos(0,-STATUS_BAR_HEIGHT) );
	scene_land_pos.x = 20;
	scene_land_pos.y = 268;
	/* 记录当前战斗场景的地面尺寸 */
	scene_land_size.w = scene_size.w - 2*scene_land_pos.x;
	scene_land_size.h = scene_size.h - scene_land_pos.y;
	/* 设置战斗场景的空间边界 */
	GameSpace_SetBound(	scene_land_pos.x, scene_land_size.w, 
				scene_land_pos.y, 
				scene_land_size.h );
	return 0;
}

/** 获取战斗场景的地面尺寸 */
void GameScene_GetLandSize( LCUI_Widget *scene, LCUI_Size *size )
{
	*size = scene_land_size;
}

/** 获取空间地面在场景中的起点X轴坐标 */
void GameScene_GetLandStartX( LCUI_Widget *scene, int *x )
{
	*x = scene_land_pos.x;
}

/** 获取空间地面在场景中的起点Y轴坐标 */
void GameScene_GetLandStartY( LCUI_Widget *scene, int *y )
{
	*y = scene_land_pos.y;
}

/** 更新场景上的镜头位置，使目标游戏对象处于镜头区域内 */
int GameScene_UpdateCamera( LCUI_Widget *scene, LCUI_Widget *target )
{
	int target_x, wdg_scene_x, offset_x, screen_width, start_x;
	LCUI_Size scene_size;

	scene_size = Widget_GetSize( scene );
	screen_width = LCUIScreen_GetWidth();
	start_x = (screen_width - scene_size.w)/2;
	target_x = (int)GameObject_GetX( target );
	wdg_scene_x = Widget_GetPos( scene ).x;
	/* 既然大于0，则说明战斗场景小于屏幕区域，无需移动镜头 */
	if( wdg_scene_x > 0 ) {
		return 0;
	}
	/* 如果目标位置在镜头的左内边框的左边 */
	if( wdg_scene_x + target_x < CAMERA_X_PANDDING ) {
		/* 计算出战斗场景的位置 */
		offset_x = CAMERA_X_PANDDING - target_x;
		/* 判断镜头是否会超出战斗场景的范围 */
		if( offset_x > 0 ) {
			offset_x = 0;
		}
		/* 得出坐标偏移量 */
		offset_x = offset_x - start_x;
		/* 将战斗场景向右移动 */
		Widget_SetAlign( scene, ALIGN_BOTTOM_CENTER, Pos(offset_x,-STATUS_BAR_HEIGHT) );
		return 0;
	}
	/* 如果目标位置在镜头的右内边框的右边 */
	if( wdg_scene_x + target_x > screen_width - CAMERA_X_PANDDING ) {
		offset_x = screen_width - CAMERA_X_PANDDING - target_x;
		if( offset_x < screen_width - scene_size.w ) {
			offset_x = screen_width - scene_size.w;
		}
		offset_x = offset_x - start_x;
		Widget_SetAlign( scene, ALIGN_BOTTOM_CENTER, Pos(offset_x,-STATUS_BAR_HEIGHT) );
	}
	return 0;
}

/** 退出战斗场景，释放占用的内存资源 */
int GameScene_Quit( LCUI_Widget *scene )
{
	Widget_SetBackgroundImage( scene, NULL );
	GameGraphRes_Free( SCENES_RES );
	Widget_Destroy( scene );
}