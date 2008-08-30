// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_seller.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"
#include <fstream>
#include <stdio.h>


using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

seller sellersInfo[15];

int initalizeSellers();
/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_SellerApp *app,CWnd* pParent /*=NULL*/)
: CDialog(CConfigDialog::IDD, pParent) {
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app=app;
}


void CConfigDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Control(pDX, IDC_SELLER_ITEM_LIST, m_tradeItemList);
	DDX_Control(pDX, IDC_SELLER_SELLER1, m_Seller[0]);
	DDX_Control(pDX, IDC_SELLER_SELLER2, m_Seller[1]);
	DDX_Control(pDX, IDC_SELLER_SELLER3, m_Seller[2]);
	DDX_Control(pDX, IDC_SELLER_SELLER4, m_Seller[3]);
	DDX_Control(pDX, IDC_SELLER_SELL1, m_SellBox[0]);
	DDX_Control(pDX, IDC_SELLER_SELL2, m_SellBox[1]);
	DDX_Control(pDX, IDC_SELLER_SELL3, m_SellBox[2]);
	DDX_Control(pDX, IDC_SELLER_SELL4, m_SellBox[3]);
	DDX_Control(pDX, IDC_SELLER_BUY1, m_BuyBox[0]);
	DDX_Control(pDX, IDC_SELLER_BUY2, m_BuyBox[1]);
	DDX_Control(pDX, IDC_SELLER_BUY3, m_BuyBox[2]);
	DDX_Control(pDX, IDC_SELLER_BUY4, m_BuyBox[3]);
	DDX_Control(pDX, IDC_SELLER_SELL1_ADD, m_SellBoxAdd[0]);
	DDX_Control(pDX, IDC_SELLER_SELL2_ADD, m_SellBoxAdd[1]);
	DDX_Control(pDX, IDC_SELLER_SELL3_ADD, m_SellBoxAdd[2]);
	DDX_Control(pDX, IDC_SELLER_SELL4_ADD, m_SellBoxAdd[3]);
	DDX_Control(pDX, IDC_SELLER_BUY1_ADD, m_BuyBoxAdd[0]);
	DDX_Control(pDX, IDC_SELLER_BUY2_ADD, m_BuyBoxAdd[1]);
	DDX_Control(pDX, IDC_SELLER_BUY3_ADD, m_BuyBoxAdd[2]);
	DDX_Control(pDX, IDC_SELLER_BUY4_ADD, m_BuyBoxAdd[3]);
	DDX_Control(pDX, IDC_SELLER_SELL1_REMOVE, m_SellBoxRemove[0]);
	DDX_Control(pDX, IDC_SELLER_SELL2_REMOVE, m_SellBoxRemove[1]);
	DDX_Control(pDX, IDC_SELLER_SELL3_REMOVE, m_SellBoxRemove[2]);
	DDX_Control(pDX, IDC_SELLER_SELL4_REMOVE, m_SellBoxRemove[3]);
	DDX_Control(pDX, IDC_SELLER_BUY1_REMOVE, m_BuyBoxRemove[0]);
	DDX_Control(pDX, IDC_SELLER_BUY2_REMOVE, m_BuyBoxRemove[1]);
	DDX_Control(pDX, IDC_SELLER_BUY3_REMOVE, m_BuyBoxRemove[2]);
	DDX_Control(pDX, IDC_SELLER_BUY4_REMOVE, m_BuyBoxRemove[3]);
	DDX_Control(pDX, IDC_SELLER_QUANTITY, m_quantityBuySell);
	DDX_Control(pDX, IDC_SELLER_TRIGGER_QUANTITY, m_buyTriggerQuantity);
	DDX_Control(pDX, IDC_SELLER_PRICE, m_buyPrice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
//{{AFX_MSG_MAP(CConfigDialog)
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_SELLER_SELL1_ADD, sellBoxAdd0)
ON_BN_CLICKED(IDC_SELLER_SELL2_ADD, sellBoxAdd1)
ON_BN_CLICKED(IDC_SELLER_SELL3_ADD, sellBoxAdd2)
ON_BN_CLICKED(IDC_SELLER_SELL4_ADD, sellBoxAdd3)
ON_BN_CLICKED(IDC_SELLER_BUY1_ADD, buyBoxAdd0)
ON_BN_CLICKED(IDC_SELLER_BUY2_ADD, buyBoxAdd1)
ON_BN_CLICKED(IDC_SELLER_BUY3_ADD, buyBoxAdd2)
ON_BN_CLICKED(IDC_SELLER_BUY4_ADD, buyBoxAdd3)
ON_BN_CLICKED(IDC_SELLER_SELL1_REMOVE, sellBoxRemove0)
ON_BN_CLICKED(IDC_SELLER_SELL2_REMOVE, sellBoxRemove1)
ON_BN_CLICKED(IDC_SELLER_SELL3_REMOVE, sellBoxRemove2)
ON_BN_CLICKED(IDC_SELLER_SELL4_REMOVE, sellBoxRemove3)
ON_BN_CLICKED(IDC_SELLER_BUY1_REMOVE, buyBoxRemove0)
ON_BN_CLICKED(IDC_SELLER_BUY2_REMOVE, buyBoxRemove1)
ON_BN_CLICKED(IDC_SELLER_BUY3_REMOVE, buyBoxRemove2)
ON_BN_CLICKED(IDC_SELLER_BUY4_REMOVE, buyBoxRemove3)
ON_BN_CLICKED(IDC_ENABLE, OnEnable)
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog message handlers

void CConfigDialog::OnOK() {
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnClose() {
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnEnable() {
	if (m_enable.GetCheck()) {
		m_app->controlsToConfig();
		if (m_app->validateConfig(1))			
			m_app->start();
		else
			m_enable.SetCheck(0);
	} 
	else
		m_app->stop(); 
}

void CConfigDialog::disableControls() {
	for (int x = 0; x < MAX_SELLERS; x++) {
		m_Seller[x].EnableWindow(false);
		m_SellBox[x].EnableWindow(false);
		m_BuyBox[x].EnableWindow(false);
		m_SellBoxAdd[x].EnableWindow(false);
		m_BuyBoxAdd[x].EnableWindow(false);
		m_SellBoxRemove[x].EnableWindow(false);
		m_BuyBoxRemove[x].EnableWindow(false); 
	}
	m_buyPrice.EnableWindow(false);
	m_quantityBuySell.EnableWindow(false);
	m_tradeItemList.EnableWindow(false);
	m_buyTriggerQuantity.EnableWindow(false);
}

void CConfigDialog::enableControls() {
	for (int x = 0; x < MAX_SELLERS; x++) {
		m_Seller[x].EnableWindow(true);
		m_SellBox[x].EnableWindow(true);
		m_BuyBox[x].EnableWindow(true);
		m_SellBoxAdd[x].EnableWindow(true);
		m_BuyBoxAdd[x].EnableWindow(true);
		m_SellBoxRemove[x].EnableWindow(true);
		m_BuyBoxRemove[x].EnableWindow(true); 
	}
	m_buyPrice.EnableWindow(true);
	m_quantityBuySell.EnableWindow(true);
	m_tradeItemList.EnableWindow(true);
	m_buyTriggerQuantity.EnableWindow(true);
}

void CConfigDialog::configToControls(CConfigData *configData) {
	char buf[128];
	for (int x = 0; x < MAX_SELLERS; x++) {
		while (m_SellBox[x].GetCount()) m_SellBox[x].DeleteString(0);
		while (m_BuyBox[x].GetCount()) m_BuyBox[x].DeleteString(0);
	}
	for (int i = 0;i < MAX_SELLERS; i++) {
		for (int j = 0; j < 32; j++) {
			if (strlen(configData->sellItem[i].tradeItem[j].itemName)) {
				sprintf(buf, "%s->%s", configData->sellItem[i].tradeItem[j].itemName, configData->sellItem[i].tradeItem[j].quantityBuySell);
				m_SellBox[i].AddString(buf);
			}
			if (strlen(configData->buyItem[i].tradeItem[j].itemName)) {
				sprintf(buf, "%s %s@%sgp", configData->buyItem[i].tradeItem[j].itemName, configData->sellItem[i].tradeItem[j].quantityBuySell, configData->sellItem[i].tradeItem[j].salePrice);
				sprintf(buf,"%s",configData->buyItem[i].tradeItem[j].itemName);
				m_BuyBox[i].AddString(buf);
			}
		}
	}
	sprintf(buf,"%d",configData->quantity);			m_quantityBuySell.SetWindowText(buf);
	sprintf(buf,"%d",configData->price);			m_buyPrice.SetWindowText(buf);

	reloadSaleItems();
	reloadSellers();
}

CConfigData * CConfigDialog::controlsToConfig() {
	char buf[128];
	CConfigData *newConfigData = new CConfigData();
	char paramString[128];
	for (int i = 0; i < MAX_SELLERS;i++) {
		for (int j = 0; j < 32; j++) {
			newConfigData->sellItem[i].tradeItem[j].itemName[0] = '\0';
			newConfigData->sellItem[i].tradeItem[j].quantityBuySell = 0;
			newConfigData->sellItem[i].tradeItem[j].salePrice = 0;
			newConfigData->buyItem[i].tradeItem[j].itemName[0] = '\0';
			newConfigData->buyItem[i].tradeItem[j].quantityBuySell = 0;
			newConfigData->buyItem[i].tradeItem[j].salePrice = 0;
		}
		for (j = 0; j < m_SellBox[i].GetCount(); j++) {
			m_SellBox[i].GetText(j,newConfigData->sellItem[i].tradeItem[j].itemName);
			for (int k = strlen(newConfigData->sellItem[i].tradeItem[j].itemName)-1;k>0;k--) {
				if (newConfigData->sellItem[i].tradeItem[j].itemName[k]=='-' && newConfigData->sellItem[i].tradeItem[j].itemName[k + 1]=='>') {
					memcpy(paramString,newConfigData->sellItem[i].tradeItem[j].itemName+k+2,strlen(newConfigData->sellItem[i].tradeItem[j].itemName)-k);
					newConfigData->sellItem[i].tradeItem[j].itemName[k]='\0';
					break;
				}
			}
			newConfigData->sellItem[i].tradeItem[j].quantityBuySell = atoi(paramString);
		}
		for (j = 0; j < m_BuyBox[i].GetCount(); j++) {
			m_BuyBox[i].GetText(j,newConfigData->buyItem[i].tradeItem[j].itemName);
			for (int k = strlen(newConfigData->buyItem[i].tradeItem[j].itemName)-1;k>0;k--)	{
				if (newConfigData->buyItem[i].tradeItem[j].itemName[k]=='-' && newConfigData->buyItem[i].tradeItem[j].itemName[k + 1]=='>') {
					memcpy(paramString,newConfigData->buyItem[i].tradeItem[j].itemName+k+2,strlen(newConfigData->buyItem[i].tradeItem[j].itemName)-k);
					newConfigData->buyItem[i].tradeItem[j].itemName[k]='\0';
					break;
				}
			}
			sscanf(paramString, "%d:%d@%dgp", &newConfigData->buyItem[i].tradeItem[j].triggerQuantity, &newConfigData->buyItem[i].tradeItem[j].quantityBuySell, &newConfigData->buyItem[i].tradeItem[j].salePrice);
		}
		int index = m_Seller[i].GetCurSel();
		for (int loop = 0; loop < 10; loop++) {
			newConfigData->sellerList[i].position[loop].sellerX = sellersInfo[index].xPos[loop];
			newConfigData->sellerList[i].position[loop].sellerY = sellersInfo[index].yPos[loop];
			newConfigData->sellerList[i].position[loop].sellerZ = sellersInfo[index].zPos[loop];
		}
	}
	m_quantityBuySell.GetWindowText(buf,127);newConfigData->quantity=atoi(buf);
	m_buyPrice.GetWindowText(buf,127);newConfigData->price=atoi(buf);
	
	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent) {
	if (nIDEvent==1001) {
		KillTimer(1001);
		SetTimer(1001,250,NULL);
	}	
	CDialog::OnTimer(nIDEvent);
}

BOOL CConfigDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	initalizeSellers();
	SetTimer(1001,250,NULL);	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CConfigDialog::PreTranslateMessage(MSG* pMsg) {
	return CDialog::PreTranslateMessage(pMsg);
}

void CConfigDialog::activateEnableButton(int enable) {
	m_enable.SetCheck(enable);
}

void CConfigDialog::reloadSellers() {
	CTibiaItemProxy itemProxy;
	while (m_Seller[0].GetCount()>0) m_Seller[0].DeleteString(0);
	while (m_Seller[1].GetCount()>0) m_Seller[1].DeleteString(0);
	while (m_Seller[2].GetCount()>0) m_Seller[2].DeleteString(0);
	while (m_Seller[3].GetCount()>0) m_Seller[3].DeleteString(0);
	for (int count = 0; count < 15; count++) {
		if (strcmp(sellersInfo[count].name, "End of Sellers")) {			
			m_Seller[0].AddString(sellersInfo[count].name);
			m_Seller[1].AddString(sellersInfo[count].name);
			m_Seller[2].AddString(sellersInfo[count].name);
			m_Seller[3].AddString(sellersInfo[count].name);
		}
		else
			break;
	}
	m_Seller[0].SetCurSel(0);
	m_Seller[1].SetCurSel(1);
	m_Seller[2].SetCurSel(2);
	m_Seller[3].SetCurSel(3);
}

int initalizeSellers() {
	char installPath[1024];
	unsigned long installPathLen=1023;
	installPath[0]='\0';
	HKEY hkey=NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey)) {
		RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
		RegCloseKey(hkey);
	}
	if (!strlen(installPath)) {
		AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
		exit(1);
	}
	
	char pathBuf[2048];    
	
	sprintf(pathBuf,"%s\\mods\\tibiaauto-sellers.csv",installPath);
	
	ifstream sellerFile (pathBuf, ios::in);
	if (!sellerFile.is_open()) {	AfxMessageBox("File TibiaAuto-Sellers.csv Not found!"); sellerFile.close(); return 0;}
	char buf[128] = {0};
	int sellerNum = 0;
	int posNum = 0;
	
	while (!sellerFile.eof()) {
		sellerFile.getline(sellersInfo[sellerNum].name, 128, ',');
		for (int posNum = 0; posNum < 9; posNum++) {
			sellerFile.getline(buf, 128, ',');
			sellersInfo[sellerNum].xPos[posNum] = atoi(buf);
			sellerFile.getline(buf, 128, ',');
			sellersInfo[sellerNum].yPos[posNum] = atoi(buf);
			sellerFile.getline(buf, 128, ',');
			sellersInfo[sellerNum].zPos[posNum] = atoi(buf);
		}
		sellerFile.getline(buf, 128, ',');
		sellersInfo[sellerNum].xPos[9] = atoi(buf);
		sellerFile.getline(buf, 128, ',');
		sellersInfo[sellerNum].yPos[9] = atoi(buf);
		sellerFile.getline(buf, 128, '\n');
		sellersInfo[sellerNum++].zPos[9] = atoi(buf);		
	}
	sellerFile.close();
	
	return 1;
}

void CConfigDialog::reloadSaleItems() {
	CTibiaItemProxy itemProxy;
	while (m_tradeItemList.GetCount()>0) m_tradeItemList.DeleteString(0);
	int count = itemProxy.getItemsItemsCount();
	for (int i = 0; i < count; i++)
		m_tradeItemList.AddString(itemProxy.getItemsItems(i));
	m_tradeItemList.SetCurSel(0);
}

void CConfigDialog::sellBoxAdd0() {
	char amount[16];
	int sel=m_tradeItemList.GetCurSel();
	m_quantityBuySell.GetWindowText(amount,15);
	char itemSaleTag[256];
	char itemName[128];
	m_tradeItemList.GetLBText(sel,itemName);
	if (sel==-1) return;
	sprintf(itemSaleTag, "%s->%s", itemName, amount);
	m_SellBox[0].AddString(itemSaleTag);
	m_tradeItemList.DeleteString(m_tradeItemList.FindString(0,itemName));
	m_tradeItemList.SetCurSel(0);
}
void CConfigDialog::sellBoxAdd1() {	
	char amount[16];
	int sel=m_tradeItemList.GetCurSel();
	m_quantityBuySell.GetWindowText(amount,15);
	char itemSaleTag[256];
	char itemName[128];
	m_tradeItemList.GetLBText(sel,itemName);
	if (sel==-1) return;
	sprintf(itemSaleTag, "%s->%s", itemName, amount);
	m_SellBox[1].AddString(itemSaleTag);
	m_tradeItemList.DeleteString(m_tradeItemList.FindString(0,itemName));
	m_tradeItemList.SetCurSel(0);
}
void CConfigDialog::sellBoxAdd2() {	
	char amount[16];
	int sel=m_tradeItemList.GetCurSel();
	m_quantityBuySell.GetWindowText(amount,15);
	char itemSaleTag[256];
	char itemName[128];
	m_tradeItemList.GetLBText(sel,itemName);
	if (sel==-1) return;
	sprintf(itemSaleTag, "%s->%s", itemName, amount);
	m_SellBox[2].AddString(itemSaleTag);
	m_tradeItemList.DeleteString(m_tradeItemList.FindString(0,itemName));
	m_tradeItemList.SetCurSel(0);
}
void CConfigDialog::sellBoxAdd3() {	
	char amount[16];
	int sel=m_tradeItemList.GetCurSel();
	m_quantityBuySell.GetWindowText(amount,15);
	char itemSaleTag[256];
	char itemName[128];
	m_tradeItemList.GetLBText(sel,itemName);
	if (sel==-1) return;
	sprintf(itemSaleTag, "%s->%s", itemName, amount);
	m_SellBox[3].AddString(itemSaleTag);
	m_tradeItemList.DeleteString(m_tradeItemList.FindString(0,itemName));
	m_tradeItemList.SetCurSel(0);
}
void CConfigDialog::buyBoxAdd0() {	
	char amount[16];
	char salePrice[16];
	char quantity[16];
	int sel=m_tradeItemList.GetCurSel();
	m_quantityBuySell.GetWindowText(amount, 15);
	m_buyPrice.GetWindowText(salePrice, 15);
	m_buyTriggerQuantity.GetWindowText(quantity, 15);
	char itemSaleTag[256];
	char itemName[128];
	m_tradeItemList.GetLBText(sel,itemName);
	if (sel==-1) return;
	sprintf(itemSaleTag, "%s->%d:%d@%dgp", itemName, atoi(quantity), atoi(amount), atoi(salePrice));
	m_BuyBox[0].AddString(itemSaleTag);
	m_tradeItemList.DeleteString(m_tradeItemList.FindString(0,itemName));
	m_tradeItemList.SetCurSel(0);
}
void CConfigDialog::buyBoxAdd1() {	
	char amount[16];
	char salePrice[16];
	char quantity[16];
	int sel=m_tradeItemList.GetCurSel();
	m_quantityBuySell.GetWindowText(amount, 15);
	m_buyPrice.GetWindowText(salePrice, 15);
	m_buyTriggerQuantity.GetWindowText(quantity, 15);
	char itemSaleTag[256];
	char itemName[128];
	m_tradeItemList.GetLBText(sel,itemName);
	if (sel==-1) return;
	sprintf(itemSaleTag, "%s->%d:%d@%dgp", itemName, atoi(quantity), atoi(amount), atoi(salePrice));
	m_BuyBox[1].AddString(itemSaleTag);
	m_tradeItemList.DeleteString(m_tradeItemList.FindString(0,itemName));
	m_tradeItemList.SetCurSel(0);
}
void CConfigDialog::buyBoxAdd2() {	
	char amount[16];
	char salePrice[16];
	char quantity[16];
	int sel=m_tradeItemList.GetCurSel();
	m_quantityBuySell.GetWindowText(amount, 15);
	m_buyPrice.GetWindowText(salePrice, 15);
	m_buyTriggerQuantity.GetWindowText(quantity, 15);
	char itemSaleTag[256];
	char itemName[128];
	m_tradeItemList.GetLBText(sel,itemName);
	if (sel==-1) return;
	sprintf(itemSaleTag, "%s->%d:%d@%dgp", itemName, atoi(quantity), atoi(amount), atoi(salePrice));
	m_BuyBox[2].AddString(itemSaleTag);
	m_tradeItemList.DeleteString(m_tradeItemList.FindString(0,itemName));
	m_tradeItemList.SetCurSel(0);
}
void CConfigDialog::buyBoxAdd3() {	
	char amount[16];
	char salePrice[16];
	char quantity[16];
	int sel=m_tradeItemList.GetCurSel();
	m_quantityBuySell.GetWindowText(amount,15);
	m_buyPrice.GetWindowText(salePrice,15);
	m_buyTriggerQuantity.GetWindowText(quantity,15);
	char itemSaleTag[256];
	char itemName[128];
	m_tradeItemList.GetLBText(sel,itemName);
	if (sel==-1) return;
	sprintf(itemSaleTag, "%s->%d:%d@%dgp", itemName, atoi(quantity), atoi(amount), atoi(salePrice));
	m_BuyBox[3].AddString(itemSaleTag);
	m_tradeItemList.DeleteString(m_tradeItemList.FindString(0,itemName));
	m_tradeItemList.SetCurSel(0);
}
void CConfigDialog::sellBoxRemove0() {
	char paramString[128];
	char itemName[128];
	int sel=m_SellBox[0].GetCurSel();
	if (sel==-1) return;	
	m_SellBox[0].GetText(sel,itemName);
	int j;
	for (j=strlen(itemName)-1;j>0;j--)
	{
		if (itemName[j]=='-' && itemName[j + 1]=='>')
		{
			memcpy(paramString,itemName+j+2,strlen(itemName)-j);
			itemName[j]='\0';
			break;
		}
	}
	m_tradeItemList.AddString(itemName);
	m_quantityBuySell.SetWindowText(paramString);
	m_buyPrice.SetWindowText("0");
	m_tradeItemList.SetCurSel(m_tradeItemList.FindString(0,itemName));
	m_SellBox[0].DeleteString(sel);	
}
void CConfigDialog::sellBoxRemove1() {
	char paramString[128];
	char itemName[128];
	int sel=m_SellBox[1].GetCurSel();
	if (sel==-1) return;	
	m_SellBox[1].GetText(sel,itemName);
	int j;
	for (j=strlen(itemName)-1;j>0;j--)
	{
		if (itemName[j]=='-' && itemName[j + 1]=='>')
		{
			memcpy(paramString,itemName+j+2,strlen(itemName)-j);
			itemName[j]='\0';
			break;
		}
	}
	m_tradeItemList.AddString(itemName);
	m_quantityBuySell.SetWindowText(paramString);
	m_buyPrice.SetWindowText("0");
	m_tradeItemList.SetCurSel(m_tradeItemList.FindString(0,itemName));
	m_SellBox[1].DeleteString(sel);	
}
void CConfigDialog::sellBoxRemove2() {
	char paramString[128];
	char itemName[128];
	int sel=m_SellBox[2].GetCurSel();
	if (sel==-1) return;	
	m_SellBox[2].GetText(sel,itemName);
	int j;
	for (j=strlen(itemName)-1;j>0;j--)
	{
		if (itemName[j]=='-' && itemName[j + 1]=='>')
		{
			memcpy(paramString,itemName+j+2,strlen(itemName)-j);
			itemName[j]='\0';
			break;
		}
	}
	m_tradeItemList.AddString(itemName);
	m_quantityBuySell.SetWindowText(paramString);
	m_buyPrice.SetWindowText("0");
	m_tradeItemList.SetCurSel(m_tradeItemList.FindString(0,itemName));
	m_SellBox[2].DeleteString(sel);	
}
void CConfigDialog::sellBoxRemove3() {
	char paramString[128];
	char itemName[128];
	int sel=m_SellBox[3].GetCurSel();
	if (sel==-1) return;	
	m_SellBox[3].GetText(sel,itemName);
	int j;
	for (j=strlen(itemName)-1;j>0;j--)
	{
		if (itemName[j]=='-' && itemName[j + 1]=='>')
		{
			memcpy(paramString,itemName+j+2,strlen(itemName)-j);
			itemName[j]='\0';
			break;
		}
	}
	m_tradeItemList.AddString(itemName);
	m_quantityBuySell.SetWindowText(paramString);
	m_buyPrice.SetWindowText("0");
	m_tradeItemList.SetCurSel(m_tradeItemList.FindString(0,itemName));
	m_SellBox[3].DeleteString(sel);	
}
void CConfigDialog::buyBoxRemove0() {
	char buf[64];
	char paramString[128];
	char itemName[128];
	int quantity, price, trigger;
	int sel=m_BuyBox[0].GetCurSel();
	if (sel==-1) return;	
	m_BuyBox[0].GetText(sel,itemName);
	int j;
	for (j=strlen(itemName)-1;j>0;j--) {
		if (itemName[j]=='-' && itemName[j + 1]=='>') {
			memcpy(paramString,itemName+j+2,strlen(itemName)-j);
			itemName[j]='\0';
			break;
		}
	}
	sscanf(paramString, "%d:%d@%dgps", &trigger, &quantity, &price);
	m_tradeItemList.AddString(itemName);
	sprintf(buf, "%d", quantity);
	m_quantityBuySell.SetWindowText(buf);
	sprintf(buf, "%d", price);
	m_buyPrice.SetWindowText(buf);
	sprintf(buf, "%d", trigger);
	m_buyTriggerQuantity.SetWindowText(buf);
	m_tradeItemList.SetCurSel(m_tradeItemList.FindString(0,itemName));
	m_BuyBox[0].DeleteString(sel);	
}
void CConfigDialog::buyBoxRemove1() {
	char buf[64];
	char paramString[128];
	char itemName[128];
	int quantity, price, trigger;
	int sel=m_BuyBox[1].GetCurSel();
	if (sel==-1) return;	
	m_BuyBox[1].GetText(sel,itemName);
	int j;
	for (j=strlen(itemName)-1;j>0;j--) {
		if (itemName[j]=='-' && itemName[j + 1]=='>') {
			memcpy(paramString,itemName+j+2,strlen(itemName)-j);
			itemName[j]='\0';
			break;
		}
	}
	sscanf(paramString, "%d:%d@%dgps", &trigger, &quantity, &price);
	m_tradeItemList.AddString(itemName);
	sprintf(buf, "%d", quantity);
	m_quantityBuySell.SetWindowText(buf);
	sprintf(buf, "%d", price);
	m_buyPrice.SetWindowText(buf);
	sprintf(buf, "%d", trigger);
	m_buyTriggerQuantity.SetWindowText(buf);
	m_tradeItemList.SetCurSel(m_tradeItemList.FindString(0,itemName));
	m_BuyBox[1].DeleteString(sel);	
}
void CConfigDialog::buyBoxRemove2() {
	char buf[64];
	char paramString[128];
	char itemName[128];
	int quantity, price, trigger;
	int sel=m_BuyBox[2].GetCurSel();
	if (sel==-1) return;	
	m_BuyBox[2].GetText(sel,itemName);
	int j;
	for (j=strlen(itemName)-1;j>0;j--) {
		if (itemName[j]=='-' && itemName[j]=='>') {
			memcpy(paramString,itemName+j+2,strlen(itemName)-j);
			itemName[j]='\0';
			break;
		}
	}
	sscanf(paramString, "%d:%d@%dgps", &trigger, &quantity, &price);
	m_tradeItemList.AddString(itemName);
	sprintf(buf, "%d", quantity);
	m_quantityBuySell.SetWindowText(buf);
	sprintf(buf, "%d", price);
	m_buyPrice.SetWindowText(buf);
	sprintf(buf, "%d", trigger);
	m_buyTriggerQuantity.SetWindowText(buf);
	m_tradeItemList.SetCurSel(m_tradeItemList.FindString(0,itemName));
	m_BuyBox[2].DeleteString(sel);	
}
void CConfigDialog::buyBoxRemove3() {
	char buf[64];
	char paramString[128];
	char itemName[128];
	int quantity, price, trigger;
	int sel=m_BuyBox[3].GetCurSel();
	if (sel==-1) return;	
	m_BuyBox[3].GetText(sel,itemName);
	int j;
	for (j=strlen(itemName)-1;j>0;j--) {
		if (itemName[j]=='-' && itemName[j + 1]=='>') {
			memcpy(paramString,itemName+j+2,strlen(itemName)-j);
			itemName[j]='\0';
			break;
		}
	}
	sscanf(paramString, "%d:%d@%dgps", & trigger, &quantity, &price);
	m_tradeItemList.AddString(itemName);
	sprintf(buf, "%d", quantity);
	m_quantityBuySell.SetWindowText(buf);
	sprintf(buf, "%d", price);
	m_buyPrice.SetWindowText(buf);
	sprintf(buf, "%d", trigger);
	m_buyTriggerQuantity.SetWindowText(buf);
	m_tradeItemList.SetCurSel(m_tradeItemList.FindString(0,itemName));
	m_BuyBox[3].DeleteString(sel);	
}


