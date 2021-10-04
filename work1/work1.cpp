// work1.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "work1.h"
#include "resource.h"
#include <vector>
#include <commdlg.h>
#include <Windowsx.h>

using namespace std;

#define MAX_LOADSTRING 100


// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

BOOL isPress;//鼠标是否按下
INT id;//记录形状类型，1是线，2是椭圆，3是矩形
POINT P, Q;//画图过程中鼠标的起点和终点
COLORREF Pen= RGB(0, 0, 0), Brush = RGB(255, 255, 255);  //画笔和画刷，画笔设置为黑色，画刷设置为白色
int flag1, flag2;//标记线的虚实和粗细
int flag3;//标记是否使用橡皮擦功能
int flag4;//标记橡皮擦的大小
int flag5=0;//标记是否使用自由画笔
HPEN pen;


// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

	//HWND hWnd;
	MSG msg;
	HACCEL hAccelTable;

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WORK1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WORK1));
    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WORK1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WORK1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

BOOL Color(COLORREF& color)//这里实现的功能是调用系统颜色对话框，并记录选择的颜色
{
	CHOOSECOLOR choosecolor;
	COLORREF CustColors[16];
	choosecolor.lStructSize = sizeof(CHOOSECOLOR);
	choosecolor.hwndOwner = NULL;
	choosecolor.rgbResult = RGB(0x80, 0x80, 0x80);	
	choosecolor.lpCustColors = CustColors;
	choosecolor.Flags = CC_RGBINIT | CC_FULLOPEN;//既可以选择颜色，也可以自定义一个颜色
	choosecolor.lCustData = 0;
	choosecolor.lpfnHook = NULL;
	choosecolor.lpTemplateName = NULL;
	if (ChooseColor(&choosecolor))
	{
		color = choosecolor.rgbResult;//记录用户选择的颜色
		return TRUE;
	}
	else return FALSE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{
	case WM_CHAR://如何退出画图的状态
	{
		if (id != 0 && wParam == 27)//按下esc按钮退出画图状态
		{
			isPress = FALSE;
			id = 0;//设置id为零，不能进行画图操作
			SetCursor(LoadCursor(0, IDC_ARROW));//改变鼠标形态为箭头
		}
		if (flag5 == 1 && wParam == 27) {
			flag5 = 0;
		}
	}
	break;
	case WM_LBUTTONDOWN://鼠标按下开始画图
	{
		if (id != 0)
		{
			isPress = TRUE;
			SetCapture(hWnd);
			SetCursor(LoadCursor(0, IDC_CROSS));//改变鼠标形态为十字，进入画图状态
			P.x = LOWORD(lParam);//把当前鼠标的坐标赋给P,Q
			Q.x = LOWORD(lParam);
			P.y = HIWORD(lParam);
			Q.y = HIWORD(lParam);
		}
		else id = 0;
		if (flag5 == 1) {
			SetCursor(LoadCursor(0, IDC_CROSS));
			isPress = TRUE;
			Q.x = LOWORD(lParam);
			Q.y = HIWORD(lParam);
		}
	}
	break;
	case WM_MOUSEMOVE://鼠标移动
	{
		HDC hdc;
		hdc = GetDC(hWnd);//取得屏幕的句柄
	    if (flag5 == 1 ) {//这里实现的功能是自由绘图，通过不断地画很短的线实现
			SetCursor(LoadCursor(0, IDC_CROSS));
			if (isPress) {
				HBRUSH brush = CreateSolidBrush(Brush);
				SelectObject(hdc, pen);
				P.x = LOWORD(lParam);//把当前鼠标的坐标赋给P
				P.y = HIWORD(lParam);
				MoveToEx(hdc, P.x, P.y, NULL);
				LineTo(hdc, Q.x, Q.y);
				Q.x = LOWORD(lParam);//然后将移动后的位置赋给Q
				Q.y = HIWORD(lParam);
				P = Q;//让P等于Q，然后进行下一次画线
			}

		}
		else if (flag3 == 0) {
			if (id != 0)
			{
				SetCursor(LoadCursor(0, IDC_CROSS));
				if (isPress)
				{
					SetROP2(hdc, R2_NOTXORPEN);//擦除之前画的图
					LOGBRUSH logBrush;
					memset(&logBrush, 0, sizeof(logBrush));
					logBrush.lbStyle = BS_SOLID;
					logBrush.lbColor = RGB(0, 0, 0);
					if (flag1 == 1 && flag2 == 1)				//这里实现的是画粗虚线的功能
					{											//因为在win32中不能画宽度大于1的虚线，即使你设置线的形式为PS_DASH，最后画出来的线还是实线
						pen = CreatePen(PS_DASH, 1, Pen);		//于是我先画一条宽度为1的虚线，然后向下平移一个像素点，再画一条线，如此循环
						HBRUSH brush = CreateSolidBrush(Brush);	//循环的次数多少决定了你虚线的宽度，最后就形成了一个粗虚线
						SelectObject(hdc, pen);					
						SelectObject(hdc, brush);				
						for (int i = 0; i < 10; i++) {
							if (id == 1) {
								MoveToEx(hdc, P.x, P.y + i, NULL);
								LineTo(hdc, Q.x, Q.y + i);
							}
							else if (id == 2)
							{
								Arc(hdc, P.x + i, P.y + i, Q.x + i, Q.x + i, P.x + i, P.y + i, Q.x + i, Q.x + i);
							}
							else if (id == 3)
							{
								Rectangle(hdc, P.x + i, P.y + i, Q.x + i, Q.x + i);
							}
							else
							{
								Ellipse(hdc, P.x + i, P.y + i, Q.x + i, Q.y + i);
							}
							Q.x = LOWORD(lParam);
							Q.y = HIWORD(lParam);
							if (id == 1) {
								MoveToEx(hdc, P.x, P.y + i, NULL);
								LineTo(hdc, Q.x, Q.y + i);
							}
							else if (id == 2)
							{
								Arc(hdc, P.x + i, P.y + i, Q.x + i, Q.x + i, P.x + i, P.y + i, Q.x + i, Q.x + i);
							}
							else if (id == 3)
							{
								Rectangle(hdc, P.x + i, P.y + i, Q.x + i, Q.y + i);
							}
							else
							{
								Ellipse(hdc, P.x + i, P.y + i, Q.x + i, Q.y + i);
							}
						}

					}
					else if (flag1 == 1 && flag2 == 0) pen = CreatePen(PS_SOLID, 5, Pen);
					else if (flag1 == 0 && flag2 == 1) pen = CreatePen(PS_DASH, 1, Pen);
					else if (flag1 == 0 && flag2 == 0) pen = CreatePen(PS_SOLID, 1, Pen);
					if (flag1 == 0 || flag2 == 0) {
						HBRUSH brush = CreateSolidBrush(Brush);
						SelectObject(hdc, pen);
						SelectObject(hdc, brush);//先画一遍当前鼠标移动位置的形状
						if (id == 1) {
							MoveToEx(hdc, P.x, P.y, NULL);
							LineTo(hdc, Q.x, Q.y);
						}
						else if (id == 2)
						{
							Arc(hdc, P.x, P.y, Q.x, Q.x, P.x, P.y, Q.x, Q.x);
						}
						else if (id == 3)
						{
							Rectangle(hdc, P.x, P.y, Q.x, Q.y);
						}
						else
						{
							Ellipse(hdc, P.x, P.y, Q.x, Q.y);
						}
						Q.x = LOWORD(lParam);//将鼠标移动后的位置赋值给Q
						Q.y = HIWORD(lParam);
						if (id == 1) {//再画一遍鼠标移动后位置的形状，实现一个动态画图的过程
							MoveToEx(hdc, P.x, P.y, NULL);
							LineTo(hdc, Q.x, Q.y);
						}
						else if (id == 2)
						{
							Arc(hdc, P.x, P.y, Q.x, Q.x, P.x, P.y, Q.x, Q.x);
						}
						else if (id == 3)
						{
							Rectangle(hdc, P.x, P.y, Q.x, Q.y);
						}
						else
						{
							Ellipse(hdc, P.x, P.y, Q.x, Q.y);
						}
					}
				}
			}
		}
		else if (flag3 == 1)//此处实现橡皮擦功能，基本思路是通过使用SetPixelV函数设置指定位置的颜色为白色
		{		//通过一个双层循环，来设置一个区域为白色，从而实现橡皮擦的功能
				//参数i,j来控制橡皮擦的大小
			HDC hdc;
			hdc = GetDC(hWnd);
			isPress = FALSE;//退出画图状态
			id = 0;
			SetCursor(LoadCursor(0, IDC_HAND));//设置鼠标形态为手形状
			if (flag4 == 0) {//小橡皮
				for (int i = 0; i < 5; i++) {
					for (int j = 0; j < 5; j++) {
						SetPixelV(hdc, LOWORD(lParam) + i, HIWORD(lParam) + j, RGB(255, 255, 255));
					}
				}
			}
			else {//大橡皮
				for (int i = 0; i < 10; i++) {
					for (int j = 0; j < 10; j++) {
						SetPixelV(hdc, LOWORD(lParam) + i, HIWORD(lParam) + j, RGB(255, 255, 255));
					}
				}
			}

		}
		
	}
		break;
	case WM_LBUTTONUP://鼠标松开
	{
		HDC hdc;
		hdc = GetDC(hWnd);//取得屏幕的句柄
		if (id != 0)
		{
			Q.x = LOWORD(lParam);
			Q.y = HIWORD(lParam);
			if (flag1 == 1 && flag2 == 1)
			{
				pen = CreatePen(PS_DASH, 1, Pen);
				HBRUSH brush = CreateSolidBrush(Brush);
				SelectObject(hdc, pen);
				SelectObject(hdc, brush);
				for (int i = 0; i < 10; i++) {
					if (id == 1) {
						MoveToEx(hdc, P.x, P.y + i, NULL);
						LineTo(hdc, Q.x, Q.y + i);
					}
					else if (id == 2)
					{
						Arc(hdc, P.x + i, P.y + i, Q.x + i, Q.x + i, P.x + i, P.y + i, Q.x + i, Q.x + i);
					}
					else if(id == 3)
					{
						Rectangle(hdc, P.x+i, P.y + i, Q.x+i, Q.y + i);
					}
					else
					{
						Ellipse(hdc, P.x + i, P.y + i, Q.x + i, Q.y + i);
					}
				}
			}
			isPress = FALSE;
			ReleaseCapture();
		}
		if (flag5 == 1)  isPress = FALSE;
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		COLORREF color;
		// 分析菜单选择:
		switch (wmId)
		{
		case ID_PEN://如果选择自由画笔，则设置flag5为1
			flag5 = 1;
			flag3 = 0;
			break;
		case ID_LINE://如果选择画线，则设置id为1
			id = 1;
			flag3 = 0;
			flag5 = 0;
			break;
		case ID_ARC://如果选择画弧线，则设置id为2
			id = 2;
			flag3 = 0;
			flag5 = 0;
			break;
		case ID_RECT://如果选择画矩形，则设置id为3
			id = 3;
			flag3 = 0;
			flag5 = 0;
			break;
		case ID_ELLIPSE://如果选择画椭圆，则设置id为0
			id = 4;
			flag3 = 0;
			flag5 = 0;
			break;
		case ID_PCLR://如果选择颜色，则弹出系统颜色对话框
			if (Color(color)) Pen = color;
			flag3 = 0;
			flag5 = 0;
			break;
		case ID_BCLR:
			if (Color(color)) Brush = color;
			flag3 = 0;
			flag5 = 0;
			break;
		case ID_WIDE://如果选择粗线，则设置flag1为1
			flag1 = 1;
			flag3 = 0;
			flag5 = 0;
			break;
		case ID_THIN://如果选择细线，则设置flag1为0
			flag1 = 0;
			flag3 = 0;
			flag5 = 0;
			break;
		case ID_DASH://如果选择虚线，则设置flag2为1
			flag2 = 1;
			flag3 = 0;
			flag5 = 0;
			break;
		case ID_SOLID://如果选择实线，则设置flag1为0
			flag2 = 0;
			flag3 = 0;
			flag5 = 0;
			break;
		case ID_BIG://如果选择大橡皮擦功能，则设置flag4为1，并设置flag3为1
			flag3 = 1;
			flag4 = 1;
			flag5 = 0;
			break;
		case ID_SMALL://如果选择大橡皮擦功能，则设置flag4为0，并设置flag3为1
			flag3 = 1;
			flag4 = 0;
			flag5 = 0;
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			id = 0;
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
