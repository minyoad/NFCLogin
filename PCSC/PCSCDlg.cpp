// PCSCDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "PCSC.h"
#include "PCSCDlg.h"
#include "winscard.h"
#include <wtypes.h>
#include <afxwin.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
SCARDCONTEXT hSC;
LPTSTR pReader, pReaderName[2];
LONG lReturn;
//与智能卡连接句柄
SCARDHANDLE hCardHandle[2];
//实际使用的协议
SCARD_IO_REQUEST dwap;


// CPCSCDlg 对话框




CPCSCDlg::CPCSCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPCSCDlg::IDD, pParent)
	, edit(_T(""))
	, port(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPCSCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, edit);
	DDX_CBString(pDX, IDC_COMBO1, port);
	DDX_Control(pDX, IDC_EDIT2, EControl);
}

BEGIN_MESSAGE_MAP(CPCSCDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	//ON_BN_CLICKED(IDC_SCardEstablishContext, &CPCSCDlg::OnBnClickedScardestablishcontext)
	ON_BN_CLICKED(IDC_BUTTON9, &CPCSCDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_Clear, &CPCSCDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDC_SCardListReaders, &CPCSCDlg::OnBnClickedScardlistreaders)
	ON_BN_CLICKED(IDC_SCardConnect, &CPCSCDlg::OnBnClickedScardconnect)
	ON_BN_CLICKED(IDC_SCardDisconnect, &CPCSCDlg::OnBnClickedScarddisconnect)
	//ON_BN_CLICKED(IDC_SCardBeginTransaction, &CPCSCDlg::OnBnClickedScardbegintransaction)
	ON_BN_CLICKED(IDC_SCardStatus, &CPCSCDlg::OnBnClickedScardstatus)
	ON_BN_CLICKED(IDC_SCardTransmit, &CPCSCDlg::OnBnClickedScardtransmit)
	//ON_BN_CLICKED(IDC_SCardEndTransaction, &CPCSCDlg::OnBnClickedScardendtransaction)
	ON_BN_CLICKED(IDC_Clearinput, &CPCSCDlg::OnBnClickedClearinput)
	ON_EN_CHANGE(IDC_EDIT1, &CPCSCDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_APDU, &CPCSCDlg::OnBnClickedApdu)
	ON_BN_CLICKED(IDC_VERSION, &CPCSCDlg::OnBnClickedVersion)
	ON_BN_CLICKED(IDC_GETUID_1, &CPCSCDlg::OnBnClickedGetuid1)
	ON_BN_CLICKED(IDC_GETUID_2, &CPCSCDlg::OnBnClickedGetuid2)
	ON_BN_CLICKED(IDC_VERSION, &CPCSCDlg::OnBnClickedVison)
END_MESSAGE_MAP()


// CPCSCDlg 消息处理程序



BOOL CPCSCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_editFont.CreatePointFont(120, _T("黑体"));
	EControl.SetFont(&m_editFont);
	CenterWindow( GetDesktopWindow() );

	CEdit* editCtrl = (CEdit*)GetDlgItem(IDC_EDIT1);
	if (editCtrl)
	{
		// 设置默认字符串
		editCtrl->SetWindowText(_T("FF CA 00 00 00"));
	}
	// TODO: 在此添加额外的初始化代码
#if 1
	//lReturn = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hSC);
	UpdateData(TRUE);
	//if (lReturn != SCARD_S_SUCCESS)
	//	edit = _T(" SCardEstablishContext...Failed");
	//else
	//	edit = _T(" SCardEstablishContext...OK");
 
	UpdateData(FALSE);
	GetDlgItem(IDC_SCardListReaders)->EnableWindow(TRUE);
	GetDlgItem(IDC_SCardStatus)->EnableWindow(FALSE);
	GetDlgItem(IDC_SCardTransmit)->EnableWindow(FALSE);
	//GetDlgItem(IDC_SCardEndTransaction)->EnableWindow(FALSE);
	GetDlgItem(IDC_SCardDisconnect)->EnableWindow(FALSE);
	//GetDlgItem(IDC_SCardBeginTransaction)->EnableWindow(FALSE);
	GetDlgItem(IDC_Clearinput)->EnableWindow(FALSE);
	GetDlgItem(IDC_VERSION)->EnableWindow(FALSE);
	GetDlgItem(IDC_GETUID_1)->EnableWindow(FALSE);
	GetDlgItem(IDC_GETUID_2)->EnableWindow(FALSE);
	//GetDlgItem(IDC_SCardEstablishContext)->EnableWindow(FALSE);
#endif

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPCSCDlg::DispScreenInfo(char *value, BYTE *sendInfo, int len)
{
#if 1
	CString APDU, receieve,Str(value);
	int i;
	BYTE select[512], recvBuffer[512];

	UpdateData(TRUE);
	//edit = edit + _T(">");
	//edit = edit + CA2T(value); Str
    edit = edit + Str;
	for (i = 0; i < len; i++)
	{
		receieve.Format(_T("%.2x"), sendInfo[i]);
		edit = edit + _T(" ") + receieve;
	}
	edit = edit + _T("\r\n");//+_T("SCardTransmit...OK");
	UpdateData(FALSE);
	EControl.LineScroll(EControl.GetLineCount() - 1, 0);
#endif


}
void CPCSCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPCSCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//建立资源管理器的上下文
void CPCSCDlg::OnBnClickedScardestablishcontext()
{
	// TODO: Add your control notification handler code here
	//SCARDCONTEXT hSC;
	//LONG lReturn;
	
	lReturn = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hSC);
    UpdateData(TRUE);
	if ( lReturn!=SCARD_S_SUCCESS )
		edit=_T(" SCardEstablishContext...Failed\r\n");
	  
 	else
	   edit=_T(" SCardEstablishContext...OK\r\n");
	   
	UpdateData(FALSE);
	GetDlgItem(IDC_SCardListReaders)->EnableWindow(TRUE);
	//GetDlgItem(IDC_SCardEstablishContext)->EnableWindow(FALSE);


}

void CPCSCDlg::OnBnClickedButton9()
{
	// TODO: Add your control notification handler code here
	//ExitProcess(0);
	CDialog::OnCancel();
}
//按回车发送命令
BOOL CPCSCDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN) 
	{
		if(pMsg->wParam == VK_RETURN)//当按下键为回车键时
		{
			OnBnClickedScardtransmit();//调用按钮函数
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
//清除输出文本框的内容
void CPCSCDlg::OnBnClickedClear()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	edit=_T("");
	UpdateData(FALSE);
}
//获得系统中安装的读卡器列表
void CPCSCDlg::OnBnClickedScardlistreaders()
{
	// TODO: Add your control notification handler code here
	char mszReaders[1024];
	CComboBox* combownd1=NULL;
	combownd1=(CComboBox*)GetDlgItem(IDC_COMBO1);
	

	OnBnClickedScardestablishcontext();//建立资源链接

	UpdateData(TRUE);
	combownd1->ResetContent();//claer
	UpdateData(FALSE);

	DWORD dwLen=sizeof(mszReaders);
	int nReaders=0;
	memset((void *)mszReaders,0x00,sizeof(mszReaders));
	lReturn = SCardListReaders(hSC, NULL, (LPTSTR)mszReaders, &dwLen);
	if ( lReturn==SCARD_S_SUCCESS )
	{
		pReader = (LPTSTR)mszReaders;
		port=pReader;
		while (*pReader !='\0'  )
		{
			
			if ( nReaders<2 ) //使用系统中前2个读卡器
				pReaderName[nReaders++]=pReader;
			UpdateData(TRUE);
			combownd1->AddString(pReader);
			UpdateData(FALSE);
			//下一个读卡器名
			pReader = pReader +strlen((char *)pReader) +1;
			
		}
	 UpdateData(TRUE);
	 edit=edit+_T("\r\n")+_T("ScardListReaders...OK");
	 UpdateData(FALSE);
	 GetDlgItem(IDC_SCardConnect)->EnableWindow(TRUE);
	 combownd1->SetCurSel(0);
	}
	else
	{
		UpdateData(TRUE);
		edit=edit+_T("\r\n")+_T("ScardListReaders...Failed");
		UpdateData(FALSE);
	}

}


//与读卡器（智能卡）连接
void CPCSCDlg::OnBnClickedScardconnect()
{
	// TODO: Add your control notification handler code here
	
	//DWORD dwAP;
	UpdateData(TRUE);
	edit=edit+_T("\r\n")+_T("The CardReader is: ")+port;
	UpdateData(FALSE);
	dwap.cbPciLength=sizeof(SCARD_IO_REQUEST);
	lReturn = SCardConnect( hSC,port,SCARD_SHARE_SHARED, 
	SCARD_PROTOCOL_T0  | SCARD_PROTOCOL_T1, &hCardHandle[0], &dwap.dwProtocol);
	if ( lReturn!=SCARD_S_SUCCESS )
	{
		UpdateData(TRUE);
		edit=edit+_T("\r\n")+_T("ScardConnect...Failed\r\n");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount()-1,0);
	}
	else
	{
		UpdateData(TRUE);
		edit=edit+_T("\r\n")+_T("ScardConnect...OK\r\n");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount()-1,0);
		//GetDlgItem(IDC_SCardBeginTransaction)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCardListReaders)->EnableWindow(FALSE);
	
		GetDlgItem(IDC_SCardStatus)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCardTransmit)->EnableWindow(TRUE);
		//GetDlgItem(IDC_SCardEndTransaction)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCardDisconnect)->EnableWindow(TRUE);
		//GetDlgItem(IDC_SCardBeginTransaction)->EnableWindow(FALSE);
		GetDlgItem(IDC_Clearinput)->EnableWindow(TRUE);
		GetDlgItem(IDC_VERSION)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETUID_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETUID_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
		
	}

	

}
//断开与读卡器（智能卡）的连接
void CPCSCDlg::OnBnClickedScarddisconnect()
{
	// TODO: Add your control notification handler code here
	lReturn = SCardDisconnect(hCardHandle[0], SCARD_LEAVE_CARD);
	if ( lReturn == SCARD_S_SUCCESS )
	{
		UpdateData(TRUE);
		edit=edit+_T("\r\n")+_T("ScardDisConnect...OK");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount()-1,0);
		//GetDlgItem(IDC_SCardEstablishContext)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCardStatus)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCardTransmit)->EnableWindow(FALSE);
		//GetDlgItem(IDC_SCardEndTransaction)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCardDisconnect)->EnableWindow(FALSE);
		//GetDlgItem(IDC_SCardBeginTransaction)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCardListReaders)->EnableWindow(TRUE);
		GetDlgItem(IDC_Clearinput)->EnableWindow(FALSE);
		GetDlgItem(IDC_VERSION)->EnableWindow(FALSE);
		GetDlgItem(IDC_GETUID_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_GETUID_2)->EnableWindow(FALSE);
	}
	else
	{
		UpdateData(TRUE);
		edit=edit+_T("\r\n")+_T("ScardDisConnect...Failed");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount()-1,0);
	}
	
}
//开始发送命令
void CPCSCDlg::OnBnClickedScardbegintransaction()
{
	// TODO: Add your control notification handler code here
	lReturn=SCardBeginTransaction(hCardHandle[0]);
	if ( lReturn == SCARD_S_SUCCESS )
	{
		UpdateData(TRUE);
		edit=edit+_T("\r\n")+_T("ScardBeginTransaction...OK");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount()-1,0);
		//GetDlgItem(IDC_SCardBeginTransaction)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCardStatus)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
		//GetDlgItem(IDC_SCardEndTransaction)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCardTransmit)->EnableWindow(TRUE);

	}
	else
	{
		UpdateData(TRUE);
		edit=edit+_T("\r\n")+_T("ScardBeginTransaction...OK");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount()-1,0);
	}
}
//获取ATR
void CPCSCDlg::OnBnClickedScardstatus()
{
	// TODO: 在此添加控件通知处理程序代码
	unsigned char Atr[32];
	BYTE r[4];
	CString rAtr,state,protocol,str[32],bt,Mclock,TP;
	int i=0,hlen,position,j,Fi,Di,btl,clock;
	DWORD AtrLen;
	DWORD NameLen;
	CHAR name[100];
	DWORD dwState;
	NameLen = sizeof(name);
	AtrLen = sizeof( Atr );
	lReturn = SCardState(hCardHandle[0],&dwState,&dwap.dwProtocol,Atr, &AtrLen);
	if (lReturn != SCARD_S_SUCCESS)
	{
		UpdateData(TRUE);
		edit=edit+_T("\r\n")+_T("ScardStatus...Failed");
		UpdateData(FALSE);
	}
	UpdateData(TRUE);
	edit=edit+_T("\r\n")+_T("ATR Value:");
	for(i=0;i<AtrLen;i++)
	{
         rAtr.Format(_T("%.2x"),Atr[i]);
		 str[i]=rAtr;
		 edit=edit+_T(" ")+rAtr;

	}
	UpdateData(FALSE);
	UpdateData(TRUE);
	switch(dwState)
	{
	case 0:
		state="SCARD_UNKNOWN";
		break;
	case 1:
		state="SCARD_ABSENT";
		break;
	case 2:
		state="SCARD_PRESENT";
		break;
	case 3:
		state="SCARD_SWALLOWED";
		break;
	case 4:
		state="SCARD_POWERED";
		break;
	case 5:
		state="SCARD_NEGOTIABLE";
		break;
	case 6:
		state="SCARD_SPECIFIC";
		break;
	}
	switch(dwap.dwProtocol)
	{
	case 0:
		protocol="SCARD_PROTOCOL_UNDEFINED";
		break;
	case 1:
		protocol="SCARD_PROTOCOL_T0";
		break;
	case 2:
		protocol="SCARD_PROTOCOL_T1";
		break;
	default:
		protocol="SCARD_PROTOCOL_RAW";
		break;
	}
	edit=edit+_T("\r\n")+_T("State:")+state;
	edit=edit+_T("\r\n")+_T("Protocol:")+protocol;
	UpdateData(FALSE);
	hlen=i;
	edit=edit+_T("\r\n");
	position=0;
	UpdateData(FALSE);
	EControl.LineScroll(EControl.GetLineCount()-1,0);
	UpdateData(TRUE);
	if(str[0]=="3b")
		edit=edit+_T("初始字符TS=3B表示正向传输。")+_T("\r\n");
	if(str[0]=="3f")
		edit=edit+_T("初始字符TS=3F表示反向传输。")+_T("\r\n");
	edit=edit+_T("格式字符T0=")+str[1]+_T("\r\n");
	UpdateData(FALSE);
	EControl.LineScroll(EControl.GetLineCount()-1,0);
	UpdateData(TRUE);
	position=1;
	if((Atr[1]&0x10)==0x10)
	{
 		edit=edit+_T("TA1=")+str[++position]+_T(" ");
		Fi= Atr[position]&0xf0;
		switch(Fi)
		{
		case 0:
			Fi=372;
			clock=4;
			break;
		case 16:
			Fi=372;
			clock=5;
			break;
		case 32:
			Fi=558;
			clock=6;
			break;
		case 48:
			Fi=744;
			clock=8;
			break;
		case 64:
			Fi=1116;
			clock=12;
			break;
		case 80:
			Fi=1488;
			clock=16;
			break;
		case 96:
			Fi=1860;
			clock=20;
			break;
		case 144:
			Fi=512;
			clock=5;
			break;
		case 160:
			Fi=768;
			clock=7.5;
			break;
		case 176:
			Fi=1024;
			clock=10;
			break;
		case 192:
			Fi=1536;
			clock=15;
			break;
		case 208:
			Fi=2048;
			clock=20;
			break;
		
		
		}
		Di= Atr[position]&0x0F;
		switch(Di)
		{
		case 1:
			Di=1;
			break;
		case 2:
			Di=2;
			break;
		case 3:
			Di=4;
			break;
		case 4:
			Di=8;
			break;
		case 5:
			Di=16;
			break;
		case 6:
			Di=1860;
			break;
		case 9:
			Di=512;
			break;
		case 10:
			Di=768;
			break;
		case 11:
			Di=1024;
			break;
		case 12:
			Di=1536;
			break;
		case 13:
			Di=2048;
			break;
		
		
		}
		btl=3571200 /(Fi/Di);
		bt.Format(_T("%d"),btl);
		Mclock.Format(_T("%d"),clock);
		edit=edit+_T(" 波特率 = ")+bt+_T(" 最高时钟MHZ = ")+Mclock+_T("\r\n");
	
	}
	UpdateData(FALSE);
	EControl.LineScroll(EControl.GetLineCount()-1,0);
	UpdateData(TRUE);
	if((Atr[1]&0x20)==0x20)
	{
		edit=edit+_T("TB1=")+str[++position]+_T(" ");
	}
	UpdateData(FALSE);
	EControl.LineScroll(EControl.GetLineCount()-1,0);
	UpdateData(TRUE);
	if((Atr[1]&0x40)==0x40)
	{
		edit=edit+_T("TC1=")+str[++position]+_T(" ");

	}
	UpdateData(FALSE);
	EControl.LineScroll(EControl.GetLineCount()-1,0);
	
	if((Atr[1]&0x80)==0x80)
	{
		UpdateData(TRUE);
		edit=edit+_T("TD1=")+str[++position]+_T("\r\n");
		UpdateData(FALSE);
		if((Atr[position]&0x0F)==0)
			TP="0";
		else
			TP="1";

		if(str[position]!="00")
		{
	
			UpdateData(TRUE);
			j=position;
			if((Atr[j]&0x10)==0x10)
			{
				edit=edit+_T("\r\n")+_T("TA2=")+str[++position]+_T(" ");

			}
			UpdateData(FALSE);
			UpdateData(TRUE);
			if((Atr[j]&0x20)==0x20)
			{
				edit=edit+_T("TB2=")+str[++position]+_T(" ");
			}
			UpdateData(FALSE);

			UpdateData(TRUE);
			if((Atr[j]&0x40)==0x40)
			{
				edit=edit+_T("TC2=")+str[++position]+_T(" ");
			}
			UpdateData(FALSE);
			
			if((Atr[j]&0x80)==0x80)
			{
				UpdateData(TRUE);
				edit=edit+_T("TD2=")+str[++position];
				UpdateData(FALSE);
				if((Atr[position]&0x0F)==0)
					TP="0";
				else
					TP="1";
		
				if(str[position]!="00")
				{
					j=position;
					UpdateData(TRUE);
					if((Atr[j]&0x10)==0x10)
					{
						edit=edit+_T("\r\n")+_T("TA3=")+str[++position]+_T(" ");

					}
					UpdateData(FALSE);
					UpdateData(TRUE);
					if((Atr[j]&0x20)==0x20)
					{
						edit=edit+_T("TB3=")+str[++position]+_T(" ");
					}
					UpdateData(FALSE);

					UpdateData(TRUE);
					if((Atr[j]&0x40)==0x40)
					{
						edit=edit+_T("TC3=")+str[++position]+_T(" ");
					}
					UpdateData(FALSE);
					UpdateData(TRUE);
					if((Atr[j]&0x80)==0x80)
					{
						edit=edit+_T("TD3=")+str[++position];
					
					}
					UpdateData(FALSE);
					if((Atr[position]&0x0F)==0)
						TP="0";
					else
						TP="1";

				}
			}
			

		}

	}
	UpdateData(TRUE);
	edit=edit+_T("\r\n")+_T("通讯协议为T=")+TP;
	UpdateData(FALSE);
	EControl.LineScroll(EControl.GetLineCount()-1,0);
	hlen=(Atr[1]&0x0F);
	UpdateData(TRUE);
	edit=edit+_T("\r\n")+_T("历史字节TK:");
	for(i=position+1;i<hlen+position+1;i++)
		edit=edit+_T(" ")+str[i];
	UpdateData(FALSE);
	EControl.LineScroll(EControl.GetLineCount()-1,0);
	UpdateData(TRUE);

}
//发命令
void CPCSCDlg::OnBnClickedScardtransmit()
{
	// TODO: 在此添加控件通知处理程序代码
	CString APDU,receieve,str;
	BYTE select[256],recvBuffer[260],sw1,sw2;
	int i,j;
	int sendSize,recvSize;
   GetDlgItemText(IDC_EDIT1,APDU);
   UpdateData(TRUE);
   edit=edit+_T("\r\n")+_T("< ")+APDU+_T("\r\n");
   UpdateData(FALSE);
   EControl.LineScroll(EControl.GetLineCount()-1,0);
   APDU.Remove(' ');
#if 1
   for(i=0;i<APDU.GetLength()/2;i++)
   {
	   str=APDU.Mid(i*2,2);
	   //select[i]=strtol(str,NULL,16); 
	   select[i] = strtol((char*)((LPCTSTR)str), NULL, 16);
   }
#else
   for (i = 0,j=0; i < APDU.GetLength(); i+=2)
   {
	   select[j++] = (BYTE)APDU[i]| (BYTE)APDU[i+1]; // 将CString中的每个字符转换为BYTE并存储
   }
#endif
   sendSize=APDU.GetLength()/2;
   recvSize=sizeof(recvBuffer);
   DispScreenInfo(">", select, sendSize);
   lReturn = SCardTransmit(hCardHandle[0],&dwap, select, sendSize, 
	   NULL, recvBuffer, (LPDWORD)&recvSize);
   if ( lReturn != SCARD_S_SUCCESS )
   {
	  //printf("Failed SCardTransmit\n");
	    UpdateData(TRUE);
		edit=edit+_T("\r\n")+_T("SCardTransmit...Failed");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount()-1,0);
   }
   else
   {

	   sw1=recvBuffer[recvSize-2];
	   sw2=recvBuffer[recvSize-1];
	   UpdateData(TRUE);
	   edit=edit+_T("<");
	   for(i=0;i<recvSize;i++)
	   {
		   receieve.Format(_T("%.2x"),recvBuffer[i]);
		   edit=edit+_T(" ")+receieve;
	   }
	   edit=edit+_T("\r\n");//+_T("SCardTransmit...OK");
	   UpdateData(FALSE);
	   EControl.LineScroll(EControl.GetLineCount()-1,0);
   }
	    

}
//结束发命令
void CPCSCDlg::OnBnClickedScardendtransaction()
{
	// TODO: 在此添加控件通知处理程序代码
	lReturn = SCardEndTransaction(hCardHandle[0], 
		SCARD_LEAVE_CARD);
	if ( SCARD_S_SUCCESS != lReturn )
	{
		UpdateData(TRUE);
		edit=edit+_T("\r\n")+_T("SCardEndTransaction...Failed");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount()-1,0);
	}else
	{
		UpdateData(TRUE);
		edit=edit+_T("\r\n")+_T("SCardEndTransaction...OK");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount()-1,0);
		//GetDlgItem(IDC_SCardBeginTransaction)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCardStatus)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCardTransmit)->EnableWindow(FALSE);
		//GetDlgItem(IDC_SCardEndTransaction)->EnableWindow(FALSE);

	}
		


}
//清除输入框里面的内容
void CPCSCDlg::OnBnClickedClearinput()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT1)->SetWindowText(_T(""));
	UpdateData(FALSE);
	EControl.LineScroll(EControl.GetLineCount()-1,0);
}


void CPCSCDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CPCSCDlg::OnBnClickedApdu()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CPCSCDlg::OnBnClickedGetuid()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CPCSCDlg::OnBnClickedVersion()
{
	// TODO: 在此添加控件通知处理程序代码
	CString APDU, receieve, str;
	BYTE select[256], recvBuffer[260], sw1, sw2;
	int i, j;
	int sendSize, recvSize;

	
	memcpy(select,"\xFF\xFF\x00\x00\x00",5);
	sendSize = 5;
	recvSize = sizeof(recvBuffer);

	DispScreenInfo(">", select, sendSize);

	lReturn = SCardTransmit(hCardHandle[0], &dwap, select, sendSize,
		NULL, recvBuffer, (LPDWORD)&recvSize);
	if (lReturn != SCARD_S_SUCCESS)
	{
		//printf("Failed SCardTransmit\n");
		UpdateData(TRUE);
		edit = edit + _T("\r\n") + _T("SCardTransmit...Failed");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount() - 1, 0);
	}
	else
	{

		sw1 = recvBuffer[recvSize - 2];
		sw2 = recvBuffer[recvSize - 1];
		UpdateData(TRUE);
		edit = edit + _T("<");
		for (i = 0; i < recvSize; i++)
		{
			receieve.Format(_T("%.2x"), recvBuffer[i]);
			edit = edit + _T(" ") + receieve;
		}
		edit = edit + _T("\r\n");//+_T("SCardTransmit...OK");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount() - 1, 0);
	}
}


void CPCSCDlg::OnBnClickedGetuid1()
{
	// TODO: 在此添加控件通知处理程序代码

	// TODO: 在此添加控件通知处理程序代码
	CString APDU, receieve, str;
	BYTE select[256], recvBuffer[260], sw1, sw2;
	int i, j;
	int sendSize, recvSize;


	memcpy(select, "\xFF\x59\x00\x00\x00", 5);
	sendSize = 5;
	recvSize = sizeof(recvBuffer);

	DispScreenInfo(">", select, sendSize);

	lReturn = SCardTransmit(hCardHandle[0], &dwap, select, sendSize,
		NULL, recvBuffer, (LPDWORD)&recvSize);
	if (lReturn != SCARD_S_SUCCESS)
	{
		//printf("Failed SCardTransmit\n");
		UpdateData(TRUE);
		edit = edit + _T("\r\n") + _T("SCardTransmit...Failed");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount() - 1, 0);
	}
	else
	{

		sw1 = recvBuffer[recvSize - 2];
		sw2 = recvBuffer[recvSize - 1];
		UpdateData(TRUE);
		edit = edit + _T("<");
		for (i = 0; i < recvSize; i++)
		{
			receieve.Format(_T("%.2x"), recvBuffer[i]);
			edit = edit + _T(" ") + receieve;
		}
		edit = edit + _T("\r\n");//+_T("SCardTransmit...OK");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount() - 1, 0);
	}
}


void CPCSCDlg::OnBnClickedGetuid2()
{
	// TODO: 在此添加控件通知处理程序代码

	CString APDU, receieve, str;
	BYTE select[256], recvBuffer[260], sw1, sw2;
	int i, j;
	int sendSize, recvSize;


	memcpy(select, "\xFF\xCA\x00\x00\x00", 5);
	sendSize = 5;
	recvSize = sizeof(recvBuffer);

#if 0
	UpdateData(TRUE);
	edit = edit + _T(">");
	for (i = 0; i < sendSize; i++)
	{
		receieve.Format("%.2x", select[i]);
		edit = edit + _T(" ") + receieve;
	}
	edit = edit + _T("\r\n");//+_T("SCardTransmit...OK");
	UpdateData(FALSE);
	EControl.LineScroll(EControl.GetLineCount() - 1, 0);
#else
	DispScreenInfo(">", select, sendSize);
#endif
	lReturn = SCardTransmit(hCardHandle[0], &dwap, select, sendSize,
		NULL, recvBuffer, (LPDWORD)&recvSize);
	if (lReturn != SCARD_S_SUCCESS)
	{
		//printf("Failed SCardTransmit\n");
		UpdateData(TRUE);
		edit = edit + _T("\r\n") + _T("SCardTransmit...Failed");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount() - 1, 0);
	}
	else
	{

		sw1 = recvBuffer[recvSize - 2];
		sw2 = recvBuffer[recvSize - 1];
		UpdateData(TRUE);
		edit = edit + _T("<");
		for (i = 0; i < recvSize; i++)
		{
			receieve.Format(_T("%.2x"), recvBuffer[i]);
			edit = edit + _T(" ") + receieve;
		}
		edit = edit + _T("\r\n");//+_T("SCardTransmit...OK");
		UpdateData(FALSE);
		EControl.LineScroll(EControl.GetLineCount() - 1, 0);
	}
}


void CPCSCDlg::OnBnClickedVison()
{
	// TODO: 在此添加控件通知处理程序代码
}
