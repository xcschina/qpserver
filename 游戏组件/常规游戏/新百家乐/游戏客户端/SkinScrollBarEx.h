
#pragma once
#include "stdafx.h"
//////////////////////////////////////////////////////////////////////////////////

//滚动条类
class  CSkinScrollBarEx
{

  //资源变量
public:
  CBitImage           m_ImageScroll;            //滚动资源
  //函数定义
public:
  //构造函数
  CSkinScrollBarEx();
  //析构函数
  virtual ~CSkinScrollBarEx();

  //功能函数
public:
  //配置滚动
  VOID InitScroolBar(CWnd * pWnd);
};

//////////////////////////////////////////////////////////////////////////////////
