// EnterCode.cpp : implementation file
//

#include "stdafx.h"
#include "tibiaauto.h"
#include "EnterCode.h"
#include "MemUtil.h"
#include <MemReader.h>
#include "md5class.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEnterCode dialog


CEnterCode::CEnterCode(CWnd* pParent /*=NULL*/)
	: MyDialog(CEnterCode::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnterCode)
	//}}AFX_DATA_INIT
	parent = (CTibiaautoDlg *)pParent;
}

void CEnterCode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnterCode)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_RUNTIMEID, m_runtimeId);
	DDX_Control(pDX, IDC_CODE, m_code);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEnterCode, CDialog)
//{{AFX_MSG_MAP(CEnterCode)
ON_WM_CLOSE()
ON_WM_ERASEBKGND()
ON_WM_DRAWITEM()
ON_WM_CTLCOLOR()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnterCode message handlers

void CEnterCode::OnOK()
{
	int ok;

	if (!auth())
	{
		PostQuitMessage(0);
		CMemUtil::setGlobalProcessId(GetCurrentProcessId());
		CMemReader& reader = CMemReader::getMemReader();
		reader.setProcessId(GetCurrentProcessId());
		PostQuitMessage(0);
	}
	else
	{
		parent->shutdownCounter = -rand();
		ok                      = 1;
	}
	CDialog::OnOK();
}

/*

   int getVal(int p1, int p2)
   {
        if (p1==4&&p2==17)
        {
                return 91967;
        }
        if (p1==17&&p2==4)
        {
                return 17;
        }

        return 0;
   }
 */


BOOL CEnterCode::OnInitDialog()
{
	CDialog::OnInitDialog();
	skin.SetButtonSkin(m_OK);
	/*
	   char pBuf[64];
	   int pLen;
	   int uId;

	   //0x5F6CBC
	   long int pAddress = getVal(4,17) * getVal(17,4) * 4;
	   long int uAddress = pAddress+32;


	   uId = CMemUtil::GetMemIntValue(uAddress);
	   CMemUtil::GetMemRange(pAddress,pAddress+32,pBuf);

	   pLen = strlen(pBuf);
	   if (!pLen)
	   {
	        sprintf(pBuf,"TA");
	        pLen=2;
	   }

	   int randStart=rand()%pLen;
	   int randVal = rand()+(rand()<<15);
	   int randPosOffset = rand()%200;
	   int uIdRand = randVal+uId+9812731;
	   int pPos1 = rand()%pLen;
	   int pVal1 = pBuf[pPos1]+17;
	   int pPos2 = rand()%pLen;
	   int pVal2 = pBuf[pPos2]+17;
	   pPos1+=randPosOffset;
	   pPos2+=randPosOffset;

	   char buf[128];
	   sprintf(buf,"%x-%x-%x-%x-%x-%x-%x",pVal2,randPosOffset,pPos1,randVal,uIdRand,pVal1,pPos2);
	   m_runtimeId.SetWindowText(buf);
	 */
	int i;
	char buf[128];

	unsigned long serialNumber;


	GetVolumeInformation(NULL, NULL, 0, &serialNumber, NULL, NULL, NULL, 0);

	//for (i=0;i<8;i++) randTab[i]=rand()%60000;
	for (i = 0; i < 8; i++)
		randTab[i] = 0;
	serialNumber += 133;
	randTab[0]    = (serialNumber & 0xffff);
	randTab[1]    = ((serialNumber >> 16) & 0xffff);
	randTab[2]    = (randTab[0] + randTab[1]) & 0xffff;
	randTab[3]    = (randTab[2] * randTab[1]) & 0xffff;
	randTab[4]    = (randTab[1] / (randTab[2] + 1)) & 0xffff;
	randTab[5]    = (randTab[3] + randTab[1] * randTab[4]) & 0xffff;
	randTab[6]    = (randTab[5] + randTab[2] + 56) & 0xffff;
	randTab[7]    = (randTab[0] + randTab[1] + randTab[4] + 123) & 0xffff;

	// avoid 4-digits overflow


	for (i = 0; i < 8; i++)
		if (randTab[i] > 0xff00)
			randTab[i] -= 200;

	testsum = randTab[0] * randTab[0] * 11 + randTab[1] * 121 + randTab[2] * randTab[1] * 27 + randTab[3] * 23 + randTab[4] * 15 + randTab[5] * randTab[6] * randTab[7] * 11 - randTab[3] * (randTab[5] + randTab[4]);


	int rSum = 1;
	for (i = 0; i < 8; i++)
	{
		rSum = (rSum * randTab[i] + 13) % 65536;
		if (rSum < 0)
			rSum += 65536;
	}


	sprintf(buf, "%.4x%.4x%.4x%.4x%.4x%.4x%.4x%.4x%.4x", randTab[0] + 17, randTab[1] + 13, randTab[2] + 13, randTab[3] + 18, randTab[4] + 14, randTab[5] + 19, randTab[6] + 5, randTab[7] + 31, rSum);
	m_runtimeId.SetWindowText(buf);

	FILE *f = fopen("tacode.txt", "rt");
	if (f)
	{
		char buf[128];
		fscanf(f, "%s", buf);
		m_code.SetWindowText(buf);
		fclose(f);
	}


	if (auth())
		OnOK();
	else
		m_code.SetWindowText("");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEnterCode::OnClose()
{
}

int CEnterCode::auth()
{
	char buf3[128];
	m_code.GetWindowText(buf3, 127);

	char buf2[128];
	// we calculate good sum
	int goodSum  = randTab[0] * randTab[0] * 11 + randTab[1] * 121 + randTab[2] * randTab[1] * 27 + randTab[3] * 23 + randTab[4] * 15 + randTab[5] * randTab[6] * randTab[7] * 11 - randTab[3] * (randTab[5] + randTab[4]);
	int goodSum2 = goodSum;
	goodSum %= 100000000;
	// then md5 it
	char buf[128];

	sprintf(buf, "%d", goodSum);
	CMD5 md5;
	md5.setPlainText(buf);
	sprintf(buf2, "%s", md5.getMD5Digest());

	FILE *f = fopen("tacode.txt", "wt");
	if (f)
	{
		fprintf(f, "%s", buf3);
		fclose(f);
	}
	if (strcmp(buf2, buf3))
	{
		int i, l1 = strlen(buf2), l2 = strlen(buf3);
		//AfxMessageBox(buf2);
		//AfxMessageBox(buf3);
		for (i = 0; i < l1; i++)
			buf2[i] = rand();
		for (i = 0; i < l1; i++)
			buf3[i] = rand();
		return 0;
	}
	else
	{
		int i, l1 = strlen(buf2), l2 = strlen(buf3);
		for (i = 0; i < l1; i++)
			buf2[i] = rand();
		for (i = 0; i < l1; i++)
			buf3[i] = rand();

		return (testsum == goodSum2);
	}
}
