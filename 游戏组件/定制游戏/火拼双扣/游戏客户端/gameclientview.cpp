#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClient.h"
#include ".\gameclientview.h"
//////////////////////////////////////////////////////////////////////////
//按钮标识

//动画标识
#define IDI_BOMB_EFFECT         501               //爆炸标识

//宏定义
#define BOMB_EFFECT_COUNT       6               //爆炸数目

//////////////////////////////////////////////////////////////////////////
#define IDC_BT_ONE            100
#define IDC_BT_TWO            101
#define IDC_BT_THREE          102
#define IDC_BT_FOUR           103
#define IDC_ADMIN           226               //按钮标识

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
  ON_WM_TIMER()
  ON_WM_CREATE()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONUP()
  ON_WM_SETCURSOR()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONDBLCLK()
  ON_BN_CLICKED(IDC_START,OnStart)
  ON_BN_CLICKED(IDC_OUT_CARD,OnOutCard)
  ON_BN_CLICKED(IDC_PASS_CARD,OnPassCard)
  ON_BN_CLICKED(IDC_BT_HUNTER,OnReqHunter)
  ON_BN_CLICKED(IDC_SCORE, OnBnClickedScore)
  ON_BN_CLICKED(IDC_LAST_TURN_CARD,OnLastTurnCard)
  ON_BN_CLICKED(IDC_TRUSTEE_CONTROL,OnStusteeControl)
  ON_BN_CLICKED(IDC_SORT_BY_COUNT, OnSortByCount)
  ON_BN_CLICKED(IDC_BT_ONE,OnBnClickedOne)
  ON_BN_CLICKED(IDC_BT_TWO,OnBnClickedTwo)
  ON_BN_CLICKED(IDC_BT_THREE,OnBnClickedThree)
  ON_BN_CLICKED(IDC_BT_FOUR,OnBnClickedFour)
  ON_BN_CLICKED(IDC_ADMIN,OpenAdminWnd)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
  //移动变量
  m_bMoveMouse=false;
  m_bSelectCard=false;
  m_bSwitchCard=false;
  m_wHoverCardItem=INVALID_ITEM;
  m_wMouseDownItem=INVALID_ITEM;
  //上轮标志
  m_bLastTurnCard=false;

  //状态变量
  memset(m_bPass,0,sizeof(m_bPass));
  memset(m_bCardCount,0,sizeof(m_bCardCount));
  FillMemory(m_wWinOrder,sizeof(m_wWinOrder),0xFFFF);
  ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
  m_bShowScore=false;

  ZeroMemory(m_lBombScore,sizeof(m_lBombScore));

  //历史积分
  memset(m_lAllTurnScore,0,sizeof(m_lAllTurnScore));
  memset(m_lLastTurnScore,0,sizeof(m_lLastTurnScore));

  ZeroMemory(m_bUserCheat,sizeof(m_bUserCheat));

  m_bShowScoreView=false;

  //名次位置
  m_PointWinOrder[0].SetPoint(0,0);
  m_PointWinOrder[1].SetPoint(0,0);
  m_nXFace=48;
  m_nYFace=48;
  m_nXTimer=65;
  m_nYTimer=69;
  m_nXBorder=0;
  m_nYBorder=0;
  return;
}

//析构函数
CGameClientView::~CGameClientView(void)
{
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if(__super::OnCreate(lpCreateStruct)==-1)
  {
    return -1;
  }


  //加载资源
  HINSTANCE hInstance=AfxGetInstanceHandle();
  m_ImageViewFill.LoadFromResource(hInstance,IDB_VIEW_FILL);
  m_ImageViewBack.LoadFromResource(hInstance,IDB_VIEW_CENTER);
  m_ImageUserPass.LoadImage(hInstance,TEXT("USER_PASS"));
  m_ImageTrustee.LoadImage(hInstance,TEXT("TRUSTEE"));
  //m_ImageHistoryScore.LoadFromResource(hInstance,IDB_HISTORY_SCORE);
  m_ImageBombEffect.LoadFromResource(hInstance,IDB_BOMB_EFFECT);
  m_PngNumber.LoadImage(hInstance,TEXT("NUMBER"));
  m_PngHistoryScore.LoadImage(hInstance,TEXT("HISTORY_SCORE"));
  m_PngAvtarBack.LoadImage(hInstance,TEXT("AVTAR_BACK"));
  m_PngLastTurn.LoadImage(hInstance,TEXT("LAST_TURN"));
  m_PngUserFrame.LoadImage(hInstance,TEXT("USER_PLAYRE"));

  //时钟资源
  m_TextureClock.LoadImage(hInstance,TEXT("GAME_CLOCK"));
  m_TextureClockNumber.LoadImage(hInstance,TEXT("CLOCK_NUMBER"));

  //图片大小
  m_sizeHistory.SetSize(m_PngHistoryScore.GetWidth(),m_PngHistoryScore.GetHeight());


  //创建控件
  CRect CreateRect(0,0,0,0);
  //m_ScoreView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,CreateRect,this,9);

  //创建扑克
  WORD wIndex=0;
  for(wIndex=0; wIndex<GAME_PLAYER; wIndex++)
  {
    //用户扑克
    m_UserCardControl[wIndex].SetDirection(true);
    m_UserCardControl[wIndex].SetDisplayFlag(true);
    m_UserCardControl[wIndex].SetCardSpace(18,22,0);

    m_HandCardControl[wIndex].SetDirection(true);
    m_HandCardControl[wIndex].SetDisplayFlag(false);

    //用户扑克
    if(wIndex!=ME_VIEW_CHAIR)
    {
      m_HandCardControl[wIndex].SetCardSpace(21,8,0);
    }
    else
    {
      m_HandCardControl[wIndex].SetCardSpace(21,0,20);
    }

  }
  m_RandCardControl.SetDirection(true);
  m_RandCardControl.SetDisplayFlag(true);

  //创建按钮
  m_btStart.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_START);
  m_btOutCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_OUT_CARD);
  m_btPassCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_PASS_CARD);
  m_btHunter.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_BT_HUNTER);
  m_btStusteeControl.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE,CreateRect,this,IDC_TRUSTEE_CONTROL);
  m_btScore.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_SCORE);
  m_btLastTurnCard.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE,CreateRect,this,IDC_LAST_TURN_CARD);
  m_btOne.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE,CreateRect,this,IDC_BT_ONE);
  m_btTwo.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE,CreateRect,this,IDC_BT_TWO);
  m_btThree.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE,CreateRect,this,IDC_BT_THREE);
  m_btFour.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE,CreateRect,this,IDC_BT_FOUR);
  m_btSortByCount.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE,CreateRect,this,IDC_SORT_BY_COUNT);

  //设置按钮
  //HINSTANCE hInstance=AfxGetInstanceHandle();
  m_btStart.SetButtonImage(IDB_START,hInstance,false,false);
  m_btOutCard.SetButtonImage(IDB_OUT_CARD,hInstance,false,false);
  m_btPassCard.SetButtonImage(IDB_PASS,hInstance,false,false);
  m_btHunter.SetButtonImage(IDB_OUT_HUNTER,hInstance,false,false);
  m_btStusteeControl.SetButtonImage(IDB_BT_START_TRUSTEE,hInstance,false,false);
  m_btScore.SetButtonImage(IDB_BT_SHOW_SCORE,hInstance,false,false);
  m_btLastTurnCard.SetButtonImage(IDB_BT_LAST_TURN,hInstance,false,false);
  m_btOne.SetButtonImage(IDB_BT_ONE,hInstance,false,false);
  m_btTwo.SetButtonImage(IDB_BT_TWO,hInstance,false,false);
  m_btThree.SetButtonImage(IDB_BT_THREE,hInstance,false,false);
  m_btFour.SetButtonImage(IDB_BT_FOUR,hInstance,false,false);
  m_btSortByCount.SetButtonImage(IDB_BT_SORT_BY_COUNT,hInstance,false,false);

#ifdef VIDEO_GAME
  //创建视频
  for(wIndex=0; wIndex<GAME_PLAYER; wIndex++)
  {
    //创建视频
    m_DlgVedioService[wIndex].Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CreateRect,this,200+wIndex);
    m_DlgVedioService[wIndex].InitVideoService(ME_VIEW_CHAIR==wIndex,ME_VIEW_CHAIR==wIndex);
    //设置视频
    g_VedioServiceManager.SetVideoServiceControl(wIndex,&m_DlgVedioService[wIndex]);
  }
#endif


  m_btOpenAdmin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(4,4,11,11),this,IDC_ADMIN);
  m_btOpenAdmin.ShowWindow(SW_HIDE);

  //控制
  m_hInst = NULL;
  m_pAdminControl = NULL;
  m_hInst = LoadLibrary(TEXT("ShuangKouHPClientControl.dll"));
  if(m_hInst)
  {
    typedef void * (*CREATE)(CWnd* pParentWnd);
    CREATE ClientControl = (CREATE)GetProcAddress(m_hInst,"CreateClientControl");
    if(ClientControl)
    {
      m_pAdminControl = static_cast<IClientControlDlg*>(ClientControl(this));
      m_pAdminControl->ShowWindow(SW_HIDE);

    }
  }

  ////TEST
  //m_btHunter.ShowWindow(SW_SHOW);
  //m_btOutCard.ShowWindow(SW_SHOW);
  //m_btPassCard.ShowWindow(SW_SHOW);

  return 0;
}

//鼠标消息
void CGameClientView::OnRButtonUp(UINT nFlags, CPoint Point)
{
  __super::OnRButtonUp(nFlags, Point);

  //变量定义
  CPoint  ptOrigin;
  CSize sizeCardControl;
  CRect rcCardControl;
  //扑克范围
  m_HandCardControl[ME_VIEW_CHAIR].GetOriginPoint(ptOrigin);
  m_HandCardControl[ME_VIEW_CHAIR].GetControlSize(sizeCardControl);
  rcCardControl.SetRect(ptOrigin.x,ptOrigin.y,ptOrigin.x+sizeCardControl.cx,ptOrigin.y+sizeCardControl.cy);

  //设置扑克
  if(rcCardControl.PtInRect(Point))
  {
    if(m_btOutCard.IsWindowVisible())
    {
      SendEngineMessage(IDM_OUT_CARD,1,1);
    }
  }
  else
  {
    m_btOutCard.EnableWindow(FALSE);
    m_HandCardControl[ME_VIEW_CHAIR].ShootAllCard(false);
  }
  RefreshGameView();
}

//重置界面
void CGameClientView::ResetGameView()
{
  //移动变量
  m_bMoveMouse=false;
  m_bSelectCard=false;
  m_bSwitchCard=false;
  m_wHoverCardItem=INVALID_ITEM;
  m_wMouseDownItem=INVALID_ITEM;
  //上轮标志
  m_bLastTurnCard=false;
  //状态变量
  memset(m_bPass,0,sizeof(m_bPass));
  memset(m_bCardCount,0,sizeof(m_bCardCount));
  FillMemory(m_wWinOrder,sizeof(m_wWinOrder),0xFFFF);
  m_bShowScore=false;
  //历史积分
  memset(m_lAllTurnScore,0,sizeof(m_lAllTurnScore));
  memset(m_lLastTurnScore,0,sizeof(m_lLastTurnScore));
  //隐藏控件
  //m_ScoreView.ShowWindow(SW_HIDE);
  //隐藏按钮
  m_btStart.ShowWindow(SW_HIDE);
  m_btOutCard.ShowWindow(SW_HIDE);
  m_btPassCard.ShowWindow(SW_HIDE);
  m_btScore.ShowWindow(SW_HIDE);

  ZeroMemory(m_bUserCheat,sizeof(m_bUserCheat));

  //禁用控件
  m_btOutCard.EnableWindow(FALSE);
  m_btPassCard.EnableWindow(FALSE);
  m_btStusteeControl.EnableWindow(FALSE);
  m_btSortByCount.EnableWindow(FALSE);
  m_btOne.EnableWindow(FALSE);
  m_btTwo.EnableWindow(FALSE);
  m_btThree.EnableWindow(FALSE);
  m_btFour.EnableWindow(FALSE);
  m_btLastTurnCard.EnableWindow(FALSE);

  //扑克控件
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    m_UserCardControl[i].SetCardData(NULL,0);
    m_HandCardControl[i].SetCardData(NULL,0);
    m_HandCardControl[i].SetPositively(false);
    m_HandCardControl[i].SetDisplayFlag(false);
  }
  m_RandCardControl.SetCardData(NULL,0);

  ZeroMemory(m_lBombScore,sizeof(m_lBombScore));

  return;
}

//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
  //变量定义
  int nViewHeight=nHeight-m_nYBorder;

#ifdef VIDEO_GAME

  CRect rect ;
  m_DlgVedioService[0].GetWindowRect(&rect) ;

  m_ptAvatar[0].x=nWidth/2-m_nXFace/2;
  m_ptAvatar[0].y=m_nYBorder+5;
  m_ptNickName[0].x=nWidth/2+5+m_nXFace/2;
  m_ptNickName[0].y=m_nYBorder+5;
  m_ptClock[0].x=nWidth/2;
  m_ptClock[0].y=m_nYBorder+200;
  m_ptReady[0].x=nWidth/2;
  m_ptReady[0].y=m_nYBorder+m_nYFace+35;
  m_PointWinOrder[0].x=m_ptAvatar[0].x-220;
  m_PointWinOrder[0].y=m_nYBorder+5;
  m_PointUserPass[0].x=nWidth/2-21;
  m_PointUserPass[0].y=nHeight/2-140;
  m_PointTrustee[0].x=m_ptNickName[0].x+100+m_nXTimer;
  m_PointTrustee[0].y=m_nYBorder+5;
  m_PointMessageDlg[0].x=nWidth/2+m_nXFace/2-5;
  m_PointMessageDlg[0].y=m_nYBorder+5+m_nYFace;
  m_ptAvatar[1].x=nWidth/2-m_nXFace/2;
  m_ptAvatar[1].y=nViewHeight-m_nYFace-5;
  m_ptNickName[1].x=nWidth/2+5+m_nXFace/2;
  m_ptNickName[1].y=nViewHeight-m_nYFace-2;
  m_ptClock[1].x=nWidth/2;
  m_ptClock[1].y=nHeight-m_nYTimer/2-50-m_nYBorder;
  m_ptReady[1].x=nWidth/2;
  m_ptReady[1].y=nViewHeight-m_nYBorder-220;
  m_PointWinOrder[1].x=m_ptAvatar[1].x-220;
  m_PointWinOrder[1].y=nViewHeight-m_nYFace-5;
  m_PointUserPass[1].x=nWidth/2-21;
  m_PointUserPass[1].y=nHeight-m_nYBorder-m_nYFace-210;
  m_PointTrustee[1].x=m_ptNickName[1].x+100+m_nXTimer;
  m_PointTrustee[1].y=nViewHeight-m_nYFace-5;
  m_PointMessageDlg[1].x=m_ptAvatar[1].x-m_ShowCharacter[1].GetWidth()+5;
  m_PointMessageDlg[1].y=nViewHeight-m_nYFace-5-m_ShowCharacter[1].GetHight();
  //移动按钮
  m_ShowCharacter[0].SetWindowPos(&wndTop,m_PointMessageDlg[0].x,m_PointMessageDlg[0].y,0,0,SWP_NOSIZE);
  m_ShowCharacter[1].SetWindowPos(&wndTop,m_PointMessageDlg[1].x,m_PointMessageDlg[1].y,0,0,SWP_NOSIZE);

  CRect rcButton;
  HDWP hDwp=BeginDeferWindowPos(25);
  const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

  //开始按钮
  m_btStart.GetWindowRect(&rcButton);
  DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2,nHeight-197-m_nYBorder,0,0,uFlags);

  //出牌按钮
  m_btOutCard.GetWindowRect(&rcButton);
  DeferWindowPos(hDwp,m_btOutCard,NULL,nWidth/2-rcButton.Width()-20,nHeight-m_nYFace-m_nYBorder-206,0,0,uFlags);
  DeferWindowPos(hDwp,m_btHunter,NULL,(nWidth-rcButton.Width())/2,nHeight-m_nYFace-m_nYBorder-206,0,0,uFlags);
  DeferWindowPos(hDwp,m_btPassCard,NULL,nWidth/2+20,nHeight-m_nYFace-m_nYBorder-206,0,0,uFlags);

  //托管按钮
  m_btStusteeControl.GetWindowRect(&rcButton);
  DeferWindowPos(hDwp,m_btLastTurnCard,NULL,nWidth-m_nXBorder-(rcButton.Width()+5),nHeight-m_nYBorder-5-rcButton.Height()*3-10,0,0,uFlags);
  DeferWindowPos(hDwp,m_btStusteeControl,NULL,nWidth-m_nXBorder-(rcButton.Width()+5),nHeight-m_nYBorder-5-rcButton.Height()*2-5,0,0,uFlags);

  //视频窗口
  CRect rcAVDlg;
  m_DlgVedioService[0].GetWindowRect(&rcAVDlg);
  DeferWindowPos(hDwp,m_DlgVedioService[0],NULL,m_nXBorder+5,50,0,0,uFlags);
  SetFlowerControlInfo(0,m_nXBorder+5,5);
  m_DlgVedioService[1].GetWindowRect(&rcAVDlg);
  DeferWindowPos(hDwp,m_DlgVedioService[1],NULL,m_nXBorder+5,nHeight-m_nYBorder-3-rcAVDlg.Height(),0,0,uFlags);
  SetFlowerControlInfo(1,m_nXBorder+5,nHeight-m_nYBorder-3-rcAVDlg.Height(),5);
  //查分按钮
  CRect rcScore;
  m_btScore.GetWindowRect(&rcScore);
  DeferWindowPos(hDwp,m_btScore,NULL,nWidth-(rcScore.Width()+m_sizeHistory.cx)/2,0,0,0,uFlags);
  EndDeferWindowPos(hDwp);
  //移动扑克
  m_HandCardControl[0].SetBenchmarkPos(nWidth/2,m_nYBorder+m_nYFace+20,enXCenter,enYTop);
  m_HandCardControl[1].SetBenchmarkPos(nWidth/2,nHeight-m_nYFace-m_nYBorder-70,enXCenter,enYBottom);
  m_UserCardControl[0].SetBenchmarkPos(nWidth/2,nHeight/2-150,enXCenter,enYTop);
  m_UserCardControl[1].SetBenchmarkPos(nWidth/2,nHeight-212-m_nYBorder,enXCenter,enYBottom);

  m_RandCardControl.SetBenchmarkPos(nWidth/2-20,nHeight/2-50,enXCenter,enYTop);
  //积分视图
  //CRect rcControl;
  //m_ScoreView.GetWindowRect(&rcControl);
  //m_ScoreView.SetWindowPos(NULL,(nWidth-rcControl.Width())/2,(nHeight-rcControl.Height()-100)/2,0,0,SWP_NOZORDER|SWP_NOSIZE);

  //积分视图
  m_ptScoreView.SetPoint((nWidth-315)/2,nViewHeight/2-131/2);

#else

  m_ptAvatar[0].x=nWidth/2-m_nXFace/2;
  m_ptAvatar[0].y=m_nYBorder+15;
  m_ptNickName[0].x=nWidth/2+6+m_nXFace/2;
  m_ptNickName[0].y=m_nYBorder+5;
  m_ptClock[0].x=nWidth/2-200+35;
  m_ptClock[0].y=m_nYBorder+200+40+15;
  m_ptReady[0].x=nWidth/2;
  m_ptReady[0].y=m_nYBorder+m_nYFace+65;
  m_PointWinOrder[0].x=m_ptAvatar[0].x-220;
  m_PointWinOrder[0].y=m_nYBorder+5;
  m_PointUserPass[0].x=nWidth/2-31;
  m_PointUserPass[0].y=nHeight/2-140;
  m_PointTrustee[0].x=m_ptNickName[0].x+90+m_nXTimer;
  m_PointTrustee[0].y=m_nYBorder+20;
  m_PointMessageDlg[0].x=nWidth/2+m_nXFace/2-5;
  m_PointMessageDlg[0].y=m_nYBorder+5+m_nYFace;

  m_PtBombScore[0].x=nWidth/2+100;
  m_PtBombScore[0].y=m_nYBorder+20;

  m_ptAvatar[1].x=nWidth/2-m_nXFace/2;
  m_ptAvatar[1].y=nViewHeight-m_nYFace-15;
  m_ptNickName[1].x=nWidth/2+8+m_nXFace/2;
  m_ptNickName[1].y=nViewHeight-m_nYFace-3;
  m_ptClock[1].x=nWidth/2-200+35;
  m_ptClock[1].y=nViewHeight-300+40-5;
  m_ptReady[1].x=nWidth/2;
  m_ptReady[1].y=nViewHeight-m_nYBorder-220;
  m_PointWinOrder[1].x=m_ptAvatar[1].x-220;
  m_PointWinOrder[1].y=nViewHeight-m_nYFace-5;
  m_PointUserPass[1].x=nWidth/2-31;
  m_PointUserPass[1].y=nHeight/2+70;
  m_PointTrustee[1].x=m_ptNickName[1].x+90+m_nXTimer;
  m_PointTrustee[1].y=nViewHeight-m_nYFace-5;

  m_PtBombScore[1].x=nWidth/2+100;
  m_PtBombScore[1].y=nViewHeight-m_nYFace+10;

  //移动按钮
  CRect rcButton;
  HDWP hDwp=BeginDeferWindowPos(21);
  const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
  //开始按钮
  m_btStart.GetWindowRect(&rcButton);
  DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2,nHeight-300/*218*/-m_nYBorder,0,0,uFlags);
  //出牌按钮
  m_btOutCard.GetWindowRect(&rcButton);
  DeferWindowPos(hDwp,m_btOutCard,NULL,nWidth/2-rcButton.Width()-15,nHeight-m_nYFace-m_nYBorder-235,0,0,uFlags);
  DeferWindowPos(hDwp,m_btHunter,NULL,nWidth/2+120,nHeight-m_nYFace-m_nYBorder-235,0,0,uFlags);
  DeferWindowPos(hDwp,m_btPassCard,NULL,nWidth/2+15,nHeight-m_nYFace-m_nYBorder-235,0,0,uFlags);
  //按钮位置
  m_btOne.GetWindowRect(&rcButton);
  int nDistanceY=1;
  int nStartY=nHeight - (rcButton.Height()+nDistanceY)*7-20;
  DeferWindowPos(hDwp,m_btOne,NULL,nWidth-(rcButton.Width()+5),nStartY,0,0,uFlags);
  DeferWindowPos(hDwp,m_btTwo,NULL,nWidth-(rcButton.Width()+5),nStartY+(rcButton.Height()+nDistanceY),0,0,uFlags);
  DeferWindowPos(hDwp,m_btThree,NULL,nWidth-(rcButton.Width()+5),nStartY+(rcButton.Height()+nDistanceY)*2,0,0,uFlags);
  DeferWindowPos(hDwp,m_btFour,NULL,nWidth-(rcButton.Width()+5),nStartY+(rcButton.Height()+nDistanceY)*3,0,0,uFlags);
  DeferWindowPos(hDwp,m_btLastTurnCard,NULL,nWidth-(rcButton.Width()+5),nStartY+(rcButton.Height()+nDistanceY)*4,0,0,uFlags);

  m_btSortByCount.GetWindowRect(&rcButton);
  DeferWindowPos(hDwp,m_btSortByCount,NULL,nWidth-(rcButton.Width()+5),nStartY+(rcButton.Height()+nDistanceY)*5-40,0,0,uFlags);
  DeferWindowPos(hDwp,m_btStusteeControl,NULL,nWidth-(rcButton.Width()+5),nStartY+(rcButton.Height()+nDistanceY)*6-40,0,0,uFlags);
  //查分按钮
  CRect rcScore;
  m_btScore.GetWindowRect(&rcScore);
  DeferWindowPos(hDwp,m_btScore,NULL,nWidth-39-1,2,0,0,uFlags);
  EndDeferWindowPos(hDwp);
  //移动扑克
  m_HandCardControl[0].SetBenchmarkPos(nWidth/2,m_nYBorder+m_nYFace+50,enXCenter,enYTop);
  m_HandCardControl[1].SetBenchmarkPos(nWidth/2,nHeight-m_nYFace-m_nYBorder-50,enXCenter,enYBottom);
  m_UserCardControl[0].SetBenchmarkPos(nWidth/2,nHeight/2-170,enXCenter,enYTop);
  m_UserCardControl[1].SetBenchmarkPos(nWidth/2,nHeight-242-m_nYBorder,enXCenter,enYBottom);

  m_RandCardControl.SetBenchmarkPos(nWidth/2-20,nHeight/2-50,enXCenter,enYTop);
  //积分视图
  //CRect rcControl;
  //m_ScoreView.GetWindowRect(&rcControl);
  //m_ScoreView.SetWindowPos(NULL,(nWidth-rcControl.Width())/2,(nHeight-rcControl.Height()-100)/2,0,0,SWP_NOZORDER|SWP_NOSIZE);
  //积分视图
  m_ptScoreView.SetPoint((nWidth-315)/2,nViewHeight/2-250/2);
#endif

  //SetShowScoreView(false);

  return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
  //绘画背景
  CDC* pDCBuff = pDC;

  for(int iW = 0 ; iW < nWidth; iW += m_ImageViewFill.GetWidth())
  {
    for(int iH = 0;  iH < nHeight; iH += m_ImageViewFill.GetHeight())
    {
      m_ImageViewFill.BitBlt(pDC->GetSafeHdc(), iW, iH);
    }
  }
  m_ImageViewBack.BitBlt(pDC->GetSafeHdc(), nWidth/2 - m_ImageViewBack.GetWidth()/2, nHeight/2 - m_ImageViewBack.GetHeight()/2);

  //玩家出牌
  for(int i=0; i<GAME_PLAYER; ++i)
  {
    m_HandCardControl[i].DrawCardControl(pDCBuff);
    m_UserCardControl[i].DrawCardControl(pDCBuff);
  }

  m_RandCardControl.DrawCardControl(pDCBuff);

  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //变量定义
    WORD wUserTimer=GetUserClock(i);
    IClientUserItem * pClientUserItem=GetClientUserItem(i);

    //绘画用户
    if(pClientUserItem!=NULL)
    {
      //绘画用户

      CSize NameSize,ScoreSize,LeftCardSize;
      int  nLong=0,nXPrintPos=0,nYPrintPos=0;
      CString szScore,szLeftCard,szBombScore,szCardCount;
      nLong=m_nXFace+10;
      NameSize=pDC->GetTextExtent(pClientUserItem->GetNickName());
      NameSize.cx=100;//(NameSize.cx>100)?100:NameSize.cx;
      nLong+=NameSize.cx;
      nLong+=10;

      szScore.Format(_T("%I64d"),pClientUserItem->GetUserScore());
      ScoreSize=pDC->GetTextExtent(szScore);
      nLong+=ScoreSize.cx;
      nLong+=10;

      szLeftCard.Format(_T("剩余       张"));
      LeftCardSize=pDC->GetTextExtent(szLeftCard);
      nLong+=LeftCardSize.cx;
      nLong+=10;

      nXPrintPos=(nWidth-m_PngUserFrame.GetWidth())/2+15;

      //头像相框
      m_PngUserFrame.DrawImage(pDCBuff,nXPrintPos - 15,m_ptAvatar[i].y - 10);

      //用户头像
      //m_PngAvtarBack.DrawImage(pDCBuff,nXPrintPos-2,m_ptAvatar[i].y-2);
      DrawUserAvatar(pDCBuff,nXPrintPos-5,m_ptAvatar[i].y,pClientUserItem);


      nXPrintPos+=m_nXFace+28;
      //nXPrintPos+=10;

      nYPrintPos = m_ptAvatar[i].y-4;

      //用户名字
      CRect rcNickName(nXPrintPos,nYPrintPos,nXPrintPos + 92,nYPrintPos + 14);
      DrawTextString(pDCBuff,pClientUserItem->GetNickName(),RGB(254,231,130),RGB(0,0,0),&rcNickName,14,700,DT_CENTER|DT_END_ELLIPSIS);

      nYPrintPos+=21;

      //剩余扑克
      //DrawTextString(pDCBuff,szLeftCard,RGB(255,255,255),RGB(0,0,0),nXPrintPos,nYPrintPos+2);
      //DrawSymbolNumberString(pDCBuff,m_bCardCount[i],nXPrintPos+40,nYPrintPos+7);

      //剩余扑克
      szCardCount.Format(_T("%d"),m_bCardCount[i]);
      CRect rcCardCount(nXPrintPos+33,nYPrintPos,nXPrintPos+77,nYPrintPos+14);
      DrawTextString(pDCBuff,szCardCount,RGB(254,231,130),RGB(0,0,0),&rcCardCount,14,400,DT_CENTER|DT_END_ELLIPSIS);

      nYPrintPos+=19;

      //金币数目
      CRect rcScore(nXPrintPos,nYPrintPos,nXPrintPos + 92,nYPrintPos + 14);
      DrawTextString(pDCBuff,szScore,RGB(123,223,255),RGB(0,0,0),&rcScore,14,400,DT_CENTER|DT_END_ELLIPSIS);

      //贡献分数
      szBombScore.Format(TEXT("贡献分：%I64d"),m_lBombScore[i]);
      nYPrintPos+=18;
      CRect rcBombScore(m_PtBombScore[i].x,m_PtBombScore[i].y+4,m_PtBombScore[i].x+120,m_PtBombScore[i].y+20);
      DrawTextString(pDCBuff,szBombScore,RGB(187,239,255),RGB(0,255,0),&rcBombScore,14,600,DT_LEFT);

      ////其他信息
      //if (wUserTimer!=0) DrawUserClock(pDCBuff,m_ptClock[i].x,m_ptClock[i].y,wUserTimer);

      //绘画时钟
      if(wUserTimer>0)
      {
        DrawUserTimer(pDC,m_ptClock[i].x, m_ptClock[i].y, wUserTimer);

        //m_TextureClock.DrawImage(pDC,m_ptClock[i].x,m_ptClock[i].y);
        //DrawNumber_Clock(pDC,m_ptClock[i].x + 5,m_ptClock[i].y + 17,GetUserClock(i));
        //DrawNumber_Clock(pDC,m_ptClock[i].x - 12,m_ptClock[i].y + 25,4);
      }

      if(pClientUserItem->GetUserStatus()==US_READY)
      {
        DrawUserReady(pDCBuff,m_ptReady[i].x,m_ptReady[i].y);
      }

      //托管
      if(m_bTrustee[i])
      {
        m_ImageTrustee.DrawImage(pDCBuff,m_PointTrustee[i].x,m_PointTrustee[i].y);
      }
    }
  }

  //历史积分
  if(m_bShowScore==true)
  {
    //计算位置
    INT nYBenchmark=5;
    INT nXBenchmark=nWidth-m_PngHistoryScore.GetWidth()-2*m_nYBorder-40;
    UINT nFormat=DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER;

    m_PngHistoryScore.DrawImage(pDCBuff,nXBenchmark,nYBenchmark);

    //绘画信息
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      //获取用户
      IClientUserItem * pClientUserItem=GetClientUserItem(i);
      if((pClientUserItem==NULL))
      {
        continue;
      }

      //位置计算
      CRect rcAccounts(nXBenchmark + 5, nYBenchmark + 34 + i*20, nXBenchmark + 103, nYBenchmark + 34 + i*20 + 12);
      CRect rcTurnScore(nXBenchmark + 106,nYBenchmark + 34 + i*20, nXBenchmark + 165, nYBenchmark + 34 + i*20 +12);
      CRect rcCollectScore(nXBenchmark + 167,nYBenchmark + 34 + i*20, nXBenchmark+ 226,nYBenchmark + 34 + i*20 + 12);

      //构造信息
      TCHAR szTurnScore[32]=TEXT(""),szCollectScore[32]=TEXT("");
      _sntprintf(szTurnScore,sizeof(szTurnScore),TEXT("%ld"),m_lLastTurnScore[i]);
      _sntprintf(szCollectScore,sizeof(szCollectScore),TEXT("%ld"),m_lAllTurnScore[i]);

      DrawTextString(pDCBuff, pClientUserItem->GetNickName(),RGB(235,236,7),RGB(0,0,0),rcAccounts, nFormat);

      DrawTextString(pDCBuff, szTurnScore,RGB(2,244,150),RGB(0,0,0),rcTurnScore, nFormat);

      DrawTextString(pDCBuff, szCollectScore,RGB(255,246,0),RGB(0,0,0),rcCollectScore, nFormat);
    }
  }

  //放弃标志
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    if(m_bPass[i]==true)
    {
      m_ImageUserPass.DrawImage(pDCBuff,m_PointUserPass[i].x,m_PointUserPass[i].y);
    }
  }

  //绘画爆炸
  if(m_bBombEffect==true)
  {
    //绘画效果
    INT nImageHeight=m_ImageBombEffect.GetHeight();
    INT nImageWidth=m_ImageBombEffect.GetWidth()/BOMB_EFFECT_COUNT;
    m_ImageBombEffect.TransDrawImage(pDCBuff,(nWidth-nImageWidth)/2,0,nImageWidth,nImageHeight,
                                     nImageWidth*(m_cbBombFrameIndex%BOMB_EFFECT_COUNT),0,RGB(255,0,255));
  }

  //上轮标志
  if(m_bLastTurnCard==true)
  {
    //绘画标志
    int nImageWidth=m_PngLastTurn.GetWidth();
    int nImageHegith=m_PngLastTurn.GetHeight();
    m_PngLastTurn.DrawImage(pDCBuff,(nWidth-nImageWidth)/2,(nHeight-nImageHegith)/2);
  }

  //显示积分
  if(m_bShowScoreView)
  {
    m_ScoreView.DrawScoreView(pDC,m_ptScoreView.x,m_ptScoreView.y,this);
  }
  return;
}


//时钟数字
void CGameClientView::DrawNumber_Clock(CDC *pDC,int x,int y,int iNum)
{
  int iWidth=m_TextureClockNumber.GetWidth()/10;
  int iNumber=1;
  BYTE NumberSize=0;
  while(iNum >= iNumber)
  {
    if(iNum >= 10000)
    {
      m_TextureClockNumber.DrawImage(pDC,x+iWidth*5-NumberSize*iWidth,y,iWidth,m_TextureClockNumber.GetHeight(),iWidth*((iNum/iNumber)%10),0);
    }
    else if(iNum >= 1000)
    {
      m_TextureClockNumber.DrawImage(pDC,x+iWidth*4-NumberSize*iWidth,y,iWidth,m_TextureClockNumber.GetHeight(),iWidth*((iNum/iNumber)%10),0);
    }
    else if(iNum >= 100)
    {
      m_TextureClockNumber.DrawImage(pDC,x+iWidth*3-NumberSize*iWidth,y,iWidth,m_TextureClockNumber.GetHeight(),iWidth*((iNum/iNumber)%10),0);
    }
    else if(iNum >= 10)
    {
      m_TextureClockNumber.DrawImage(pDC,x+iWidth*2-NumberSize*iWidth,y,iWidth,m_TextureClockNumber.GetHeight(),iWidth*((iNum/iNumber)%10),0);
    }
    else if(iNum >= 0)
    {
      m_TextureClockNumber.DrawImage(pDC,x+iWidth*2-NumberSize*iWidth-4,y,iWidth,m_TextureClockNumber.GetHeight(),iWidth*((iNum/iNumber)%10),0);
    }

    iNumber*=10;
    NumberSize++;
  }
}

//绘画时间
void CGameClientView::DrawUserTimer(CDC * pDC, INT nXPos, INT nYPos, WORD wTime)
{
  //获取属性
  const INT nNumberHeight=m_TextureClockNumber.GetHeight();
  const INT nNumberWidth=m_TextureClockNumber.GetWidth()/10;

  //计算数目
  LONG lNumberCount=0;
  WORD wNumberTemp=wTime;

  do
  {
    lNumberCount++;
    wNumberTemp/=10;
  }
  while(wNumberTemp>0L);

  //位置定义
  INT nXDrawPos=nXPos + (lNumberCount*nNumberWidth)/2 - nNumberWidth - 0;
  INT nYDrawPos=nYPos - nNumberHeight/2 - 1;


  //绘画背景
  CSize SizeTimeBack(m_TextureClock.GetWidth(),m_TextureClock.GetHeight());
  m_TextureClock.DrawImage(pDC,nXPos-SizeTimeBack.cx/2,nYPos-SizeTimeBack.cy/2);

  //绘画号码
  for(LONG i=0; i<lNumberCount; i++)
  {
    //绘画号码
    WORD wCellNumber=wTime%10;
    m_TextureClockNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,wCellNumber*nNumberWidth,0);

    //设置变量
    wTime/=10;
    nXDrawPos-=nNumberWidth;
  }
  return;
}


//绘画数字
void CGameClientView::DrawSymbolNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos)
{
  //加载资源
  CSize SizeScoreNumber(m_PngNumber.GetWidth()/10,m_PngNumber.GetHeight());

  //计算数目
  LONGLONG lNumberCount=0;
  LONGLONG lNumberTemp=lNumber;
  do
  {
    lNumberCount++;
    lNumberTemp/=10;
  }
  while(lNumberTemp>0);

  //位置定义
  INT nOffset=8;
  INT nYDrawPos=nYPos-SizeScoreNumber.cy/2;
  INT nXDrawPos=(INT)(nXPos)+ lNumberCount*(SizeScoreNumber.cx-7) ;

  //绘画桌号
  for(LONGLONG i=0; i<lNumberCount; i++)
  {

    LONG lCellNumber=(LONG)(lNumber%10);
    {
      int yIndex=0,yLength=SizeScoreNumber.cy;

      m_PngNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,yLength,
                            lCellNumber*SizeScoreNumber.cx,yIndex,SizeScoreNumber.cx,yLength);
    }

    //设置变量
    lNumber/=10;
    nXDrawPos-=(SizeScoreNumber.cx);
  };
  return;
}

//设置放弃
void CGameClientView::SetPassFlag(WORD wChairID, bool bPass)
{
  //设置变量
  if(wChairID==INVALID_CHAIR)
  {
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      m_bPass[i]=bPass;
    }
  }
  else
  {
    m_bPass[wChairID]=bPass;
  }

  //更新界面
  RefreshGameView();
  return;
}


void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame,CRect*rect)
{
  //变量定义
  int nStringLength=lstrlen(pszString);
  int nXExcursion[8]= {1,1,1,0,-1,-1,-1,0};
  int nYExcursion[8]= {-1,0,1,1,1,0,-1,-1};

  //绘画边框
  pDC->SetTextColor(crFrame);
  for(int i=0; i<CountArray(nXExcursion); i++)
  {
    //DrawText(pDC,pszString,&CRect(rect->left+nXExcursion[i],rect->top+nYExcursion[i],rect->right+nXExcursion[i],rect->bottom+nYExcursion[i]),DT_END_ELLIPSIS |DT_LEFT|DT_SINGLELINE);
  }

  //绘画字体
  //pDC->SetTextColor(crText);
  //DrawText(pDC,pszString,rect,DT_END_ELLIPSIS |DT_LEFT|DT_SINGLELINE);

  //绘画字符
  CDFontEx::DrawText(this,pDC,  12, 400, pszString, rect, crText, DT_END_ELLIPSIS |DT_LEFT|DT_SINGLELINE);

}

//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
  //变量定义
  int nStringLength=lstrlen(pszString);
  int nXExcursion[8]= {1,1,1,0,-1,-1,-1,0};
  int nYExcursion[8]= {-1,0,1,1,1,0,-1,-1};

  //绘画边框
  pDC->SetTextColor(crFrame);
  for(int i=0; i<CountArray(nXExcursion); i++)
  {
    //TextOut(pDC,nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
  }

  //绘画字体
  pDC->SetTextColor(crText);
  //TextOut(pDC,nXPos,nYPos,pszString,nStringLength);

  //绘画字符
  CDFontEx::DrawText(this,pDC,  12, 400, pszString, nXPos,nYPos, crText, DT_END_ELLIPSIS |DT_LEFT|DT_SINGLELINE);

  return;
}

//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect,int nWidth, int nWeight,UINT nFormat)
{
  //变量定义
  int nStringLength=lstrlen(pszString);
  int nXExcursion[8]= {1,1,1,0,-1,-1,-1,0};
  int nYExcursion[8]= {-1,0,1,1,1,0,-1,-1};

  //绘画边框
  //pDC->SetTextColor(crFrame);
  CRect rcDraw;
  for(int i=0; i<CountArray(nXExcursion); i++)
  {
    //rcDraw.CopyRect(lpRect);
    //rcDraw.OffsetRect(nXExcursion[i],nYExcursion[i]);
    //CDFontEx::DrawText(this, pDC, nWidth, nWeight,pszString,&rcDraw, crFrame, nFormat);

  }

  //绘画字体
  rcDraw.CopyRect(lpRect);
  CDFontEx::DrawText(this, pDC, nWidth, nWeight,pszString,&rcDraw, crText, nFormat);
  return;
}

//绘画字符
VOID CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, INT nDrawFormat)
{
  //变量定义
  INT nStringLength=lstrlen(pszString);
  INT nXExcursion[8]= {1,1,1,0,-1,-1,-1,0};
  INT nYExcursion[8]= {-1,0,1,1,1,0,-1,-1};

  //绘画边框
  for(INT i=0; i<CountArray(nXExcursion); i++)
  {
    //计算位置
    CRect rcFrame;
    rcFrame.top=rcRect.top+nYExcursion[i];
    rcFrame.left=rcRect.left+nXExcursion[i];
    rcFrame.right=rcRect.right+nXExcursion[i];
    rcFrame.bottom=rcRect.bottom+nYExcursion[i];

    //绘画字符
    CDFontEx::DrawText(this,pDC,  12, 400, pszString, rcFrame, crFrame, nDrawFormat);
  }

  //绘画字符
  CDFontEx::DrawText(this,pDC,  12, 400, pszString, rcRect, crText, nDrawFormat);

  return;
}


//扑克数目
void CGameClientView::SetCardCount(WORD wChairID, BYTE bCardCount)
{
  //设置变量
  if(wChairID==INVALID_CHAIR)
  {
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      m_bCardCount[i]=bCardCount;
    }
  }
  else
  {
    m_bCardCount[wChairID]=bCardCount;
  }

  //更新界面
  RefreshGameView();

  return;
}

//设置名次
bool CGameClientView::SetUserWinOrder(WORD wChairID, WORD wWinOrder)
{
  //设置变量
  if(wChairID==INVALID_CHAIR)
  {
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      m_wWinOrder[i]=wWinOrder;
    }
  }
  else
  {
    ASSERT(wChairID<GAME_PLAYER);
    m_wWinOrder[wChairID]=wWinOrder;
  }

  //更新界面
  RefreshGameView();

  return true;
}
//设置托管
void CGameClientView::SetTrustee(WORD wTrusteeUser,bool bTrustee)
{
  //校验数据
  ASSERT(wTrusteeUser>=0&&wTrusteeUser<GAME_PLAYER);

  if(m_bTrustee[wTrusteeUser] !=bTrustee)
  {
    //设置数据
    m_bTrustee[wTrusteeUser]=bTrustee;
    //更新界面
    RefreshGameView();
  }
  return;

}
//设置爆炸
bool CGameClientView::SetBombEffect(bool bBombEffect)
{
  if(bBombEffect==true)
  {
    //设置变量
    m_bBombEffect=true;
    m_cbBombFrameIndex=0;

    //启动时间
    SetTimer(IDI_BOMB_EFFECT,100,NULL);
  }
  else
  {
    //停止动画
    if(m_bBombEffect==true)
    {
      //删除时间
      KillTimer(IDI_BOMB_EFFECT);

      //设置变量
      m_bBombEffect=false;
      m_cbBombFrameIndex=0;

      //更新界面
      RefreshGameView();
    }
  }

  return true;
}

//设置上轮
void CGameClientView::SetLastTurnCard(bool bLastTurnCard)
{
  //设置变量
  if(bLastTurnCard!=m_bLastTurnCard)
  {
    //设置变量
    m_bLastTurnCard=bLastTurnCard;

    //更新界面
    RefreshGameView();
  }

  return;
}

//更新视图
void CGameClientView::RefreshGameView()
{
  CRect rect;
  GetClientRect(&rect);
  InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());

  return;
}

//开始按钮
void CGameClientView::OnStart()
{
  SendEngineMessage(IDM_START,0,0);
  return;
}

//出牌按钮
void CGameClientView::OnOutCard()
{
  SendEngineMessage(IDM_OUT_CARD,1,1);
  return;
}

//放弃按钮
void CGameClientView::OnPassCard()
{
  SendEngineMessage(IDM_PASS_CARD,1,1);
  return;
}

void CGameClientView::OnReqHunter()
{
  SendEngineMessage(IDM_REQ_HUNTER,1,1);
}

//申请托管
void CGameClientView::OnStusteeControl()
{
  SendEngineMessage(IDM_TRUSTEE_CONTROL,0,0);
  return;
}

//积分按钮
VOID CGameClientView::OnBnClickedScore()
{
  //设置变量
  m_bShowScore=!m_bShowScore;

  //设置按钮
  HINSTANCE hResInstance=AfxGetInstanceHandle();
  m_btScore.SetButtonImage((m_bShowScore==true)?IDB_BT_CLOSE_SCORE:IDB_BT_SHOW_SCORE,hResInstance,false,false);

  //更新界面
  RefreshGameView();

  return;
}

//上轮扑克
void CGameClientView::OnLastTurnCard()
{
  SendEngineMessage(IDM_LAST_TURN_CARD,0,0);
  return;
}


//1张按钮
void CGameClientView::OnBnClickedOne()
{
  SendEngineMessage(IDM_CARD_TYPE,1,1);
  return;
}
//2张按钮
void CGameClientView::OnBnClickedTwo()
{
  SendEngineMessage(IDM_CARD_TYPE,2,2);
  return;
}
//3张按钮
void CGameClientView::OnBnClickedThree()
{
  SendEngineMessage(IDM_CARD_TYPE,3,3);
  return;
}
//4张按钮
void CGameClientView::OnBnClickedFour()
{
  SendEngineMessage(IDM_CARD_TYPE,4,4);
  return;
}

//数目排序
void CGameClientView::OnSortByCount()
{
  SendEngineMessage(IDM_SORT_BY_COUNT,0,0);
  return;
}
//////////////////////////////////////////////////////////////////////////


void CGameClientView::OnTimer(UINT nIDEvent)
{
  switch(nIDEvent)
  {
    case IDI_BOMB_EFFECT: //爆炸动画
    {
      //停止判断
      if(m_bBombEffect==false)
      {
        KillTimer(IDI_BOMB_EFFECT);
        return;
      }

      //设置变量
      if((m_cbBombFrameIndex+1)>=BOMB_EFFECT_COUNT)
      {
        //删除时间
        KillTimer(IDI_BOMB_EFFECT);

        //设置变量
        m_bBombEffect=false;
        m_cbBombFrameIndex=0;
      }
      else
      {
        m_cbBombFrameIndex++;
      }

      //更新界面
      RefreshGameView();

      return;

    }
  }
  __super::OnTimer(nIDEvent);
}
//鼠标消息
VOID CGameClientView::OnLButtonUp(UINT nFlags, CPoint Point)
{
  __super::OnLButtonUp(nFlags, Point);

  //选择处理
  if((m_bSelectCard==true)&&(m_bMoveMouse==true))
  {
    //设置扑克
    if(m_wHoverCardItem>m_wMouseDownItem)
    {
      m_HandCardControl[ME_VIEW_CHAIR].SetShootIndex(m_wMouseDownItem,m_wHoverCardItem);
    }
    else
    {
      m_HandCardControl[ME_VIEW_CHAIR].SetShootIndex(m_wHoverCardItem,m_wMouseDownItem);
    }

    //发送消息
    SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);
    //更新界面
    RefreshGameView();
  }

  //设置变量
  m_bMoveMouse=false;
  m_bSwitchCard=false;
  m_bSelectCard=false;

  //设置索引
  m_wMouseDownItem=INVALID_ITEM;
  m_wHoverCardItem=INVALID_ITEM;

  //释放鼠标
  ReleaseCapture();

  return;
}

//鼠标消息
VOID CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
  __super::OnLButtonDown(nFlags, Point);
  //状态判断
  if(m_HandCardControl[ME_VIEW_CHAIR].GetPositively()==true)
  {
    //获取扑克
    m_wMouseDownItem=m_HandCardControl[ME_VIEW_CHAIR].SwitchCardPoint(Point);
    //设置扑克
    if(m_wMouseDownItem!=INVALID_ITEM)
    {
      //获取扑克
      tagCardItem * pCardItem=m_HandCardControl[ME_VIEW_CHAIR].GetCardFromIndex(m_wMouseDownItem);
      //设置变量
      m_bMoveMouse=false;
      m_wHoverCardItem=m_wMouseDownItem;

      //操作变量
      if(pCardItem->bShoot==false)
      {
        m_bSelectCard=true;
      }
      else
      {
        m_bSwitchCard=true;
      }
    }

    //默认处理
    if(m_bMoveMouse==false)
    {
      //获取索引
      m_wMouseDownItem=m_HandCardControl[ME_VIEW_CHAIR].SwitchCardPoint(Point);
      if(m_wMouseDownItem==INVALID_ITEM)
      {
        return ;
      }
      //设置控件
      tagCardItem * pCardItem=m_HandCardControl[ME_VIEW_CHAIR].GetCardFromPoint(Point);
      pCardItem->bShoot=!pCardItem->bShoot;
      //发送消息
      SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);
    }
  }
  //调整控件
  RefreshGameView();
  return;
}

//鼠标消息
VOID CGameClientView::OnMouseMove(UINT nFlags, CPoint Point)
{
  __super::OnMouseMove(nFlags, Point);

  //变量定义
  WORD wHoverCardItem=INVALID_ITEM;
  //移动判断
  if(m_wHoverCardItem!=INVALID_ITEM)
  {
    //获取光标
    CPoint MousePoint;
    GetCursorPos(&MousePoint);
    ScreenToClient(&MousePoint);
    //扑克大小
    CSize ControlSize;
    m_HandCardControl[ME_VIEW_CHAIR].GetControlSize(ControlSize);

    //扑克位置
    CPoint OriginPoint;
    m_HandCardControl[ME_VIEW_CHAIR].GetOriginPoint(OriginPoint);

    //横行调整
    if(MousePoint.x<OriginPoint.x)
    {
      MousePoint.x=OriginPoint.x;
    }
    if(MousePoint.x>(OriginPoint.x+ControlSize.cx))
    {
      MousePoint.x=(OriginPoint.x+ControlSize.cx);
    }

    //获取索引
    MousePoint.y=OriginPoint.y+DEF_SHOOT_DISTANCE;
    wHoverCardItem=m_HandCardControl[ME_VIEW_CHAIR].SwitchCardPoint(MousePoint);

    //移动变量
    if(wHoverCardItem!=m_wHoverCardItem)
    {
      m_bMoveMouse=true;
    }
  }

  //选择扑克
  if((m_bSelectCard==true)&&(wHoverCardItem!=m_wHoverCardItem))
  {
    //设置扑克
    if(wHoverCardItem>m_wMouseDownItem)
    {
      m_HandCardControl[ME_VIEW_CHAIR].SetSelectIndex(m_wMouseDownItem,wHoverCardItem);
    }
    else
    {
      m_HandCardControl[ME_VIEW_CHAIR].SetSelectIndex(wHoverCardItem,m_wMouseDownItem);
    }

    //更新界面
    RefreshGameView();
  }

  //设置变量
  m_wHoverCardItem=wHoverCardItem;

  return;
}

//鼠标双击
void CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  __super::OnLButtonDblClk(nFlags, point);

  //状态判断
  if(m_HandCardControl[ME_VIEW_CHAIR].GetPositively()==true)
  {
    //获取扑克
    m_wMouseDownItem=m_HandCardControl[ME_VIEW_CHAIR].SwitchCardPoint(point);

    //设置扑克
    if(m_wMouseDownItem!=INVALID_ITEM)
    {
      //获取扑克
      tagCardItem * pCardItem=m_HandCardControl[ME_VIEW_CHAIR].GetCardFromIndex(m_wMouseDownItem);

      //设置变量
      m_bMoveMouse=false;
      m_wHoverCardItem=m_wMouseDownItem;
      //操作变量
      if(pCardItem->bShoot)
      {
        m_bSwitchCard=true;
      }
      SendEngineMessage(IDM_LEFT_DBHIT_CARD,pCardItem->cbCardData,0);
    }
    else
    {
      //设置扑克
      m_btOutCard.EnableWindow(FALSE);
      m_HandCardControl[ME_VIEW_CHAIR].ShootAllCard(false);
    }
  }
  else
  {
    //设置扑克
    m_btOutCard.EnableWindow(FALSE);
    m_HandCardControl[ME_VIEW_CHAIR].ShootAllCard(false);
  }
  RefreshGameView();
  return;
}

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
  //获取光标
  CPoint MousePoint;
  GetCursorPos(&MousePoint);
  ScreenToClient(&MousePoint);

  //扑克测试
  if(m_HandCardControl[ME_VIEW_CHAIR].OnEventSetCursor(MousePoint)==true)
  {
    return TRUE;
  }

  return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//设置显示
void CGameClientView::SetShowScoreView(bool bShow)
{
  m_bShowScoreView=bShow;
}

//贡献分数
void CGameClientView::SetBombScore(WORD wChairID,LONGLONG lBombScore)
{
  m_lBombScore[wChairID]=lBombScore;
}

//管理员控制
void CGameClientView::OpenAdminWnd()
{
  //有权限
  //if(m_pAdminControl!=NULL&&(GetUserInfo(m_wMeChairID)-> &UR_GAME_CONTROL)!=0)
  {


    if(!m_pAdminControl->IsWindowVisible())
    {
      m_pAdminControl->ShowWindow(SW_SHOW);
      m_pAdminControl->InitControl();
    }
    else
    {
      m_pAdminControl->ShowWindow(SW_HIDE);
    }
  }
}
BOOL CGameClientView::PreTranslateMessage(MSG* pMsg)
{
  // TODO: 在此添加专用代码和/或调用基类

  return CGameFrameViewGDI::PreTranslateMessage(pMsg);
}
