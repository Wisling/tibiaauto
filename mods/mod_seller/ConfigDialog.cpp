// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_seller.h"
#include "ModuleUtil.h"
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

seller sellersInfo[MAX_NPCS];
int sellerNum;

extern CToolSellerState globalSellerState;
extern int GUIx,GUIy,GUIz,GUINum;

int initalizeSellers();
void saveSellers();
/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_SellerApp *app,CWnd* pParent /*=NULL*/)
: MyDialog(CConfigDialog::IDD, pParent) {
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app=app;
}


void CConfigDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDC_FRAME_BUYSELL_CONTROLS, m_BuySellControlsFrame);
	DDX_Control(pDX, IDC_FRAME_BUY_CONTROLS, m_BuyControlsFrame);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_FRAME_SELLER4, m_Seller4Frame);
	DDX_Control(pDX, IDC_FRAME_SELLER3, m_Seller3Frame);
	DDX_Control(pDX, IDC_FRAME_SELLER2, m_Seller2Frame);
	DDX_Control(pDX, IDC_FRAME_SELLER1, m_Seller1Frame);
	DDX_Control(pDX, IDC_SELLER_ITEM_LIST, m_tradeItemList);
	DDX_Control(pDX, IDC_SELLER_RETURN_ON_CAP, m_sellOnCap);
	DDX_Control(pDX, IDC_SELLER_RETURN_ON_NO_SPACE, m_sellOnSpace);
	DDX_Control(pDX, IDC_SELLER_CAP_LIMIT, m_sellWhen);
	DDX_Control(pDX, IDC_SELLER_QUANTITY, m_quantityBuySell);
	DDX_Control(pDX, IDC_SELLER_PRICE, m_buyPrice);
	DDX_Control(pDX, IDC_SELLER_TRIGGER_QUANTITY, m_buyTriggerQuantity);
	DDX_Control(pDX, IDC_SELLER_MODPRIORITY, m_modPriority);
	DDX_Control(pDX, IDC_SELLER_ADD, m_addSeller);
	DDX_Control(pDX, IDC_SELLER_ADD_NAME, m_addName);
	DDX_Control(pDX, IDC_SELLER_LISTFRAME, m_listFrame);
	DDX_Control(pDX, IDC_SELLER_STOPBY, m_stopBySeller);
	DDX_Control(pDX, IDC_SELLER_STATE,m_stateSeller);
	DDX_Control(pDX, IDC_SELLER_SUGGEST_BANKER, m_suggestBanker);
	DDX_Control(pDX, IDC_SELLER_SUGGESTION,m_bankerSuggestion);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	//}}AFX_DATA_MAP
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
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
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
	ON_BN_CLICKED(IDC_SELLER_RETURN_ON_CAP, onSellOnCap)
	ON_BN_CLICKED(IDC_SELLER_ADD, OnAddSeller)
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
	m_sellOnSpace.EnableWindow(false);
	m_modPriority.EnableWindow(false);
	m_sellWhen.EnableWindow(false);
	m_stopBySeller.EnableWindow(false);
	m_sellOnCap.EnableWindow(false);
	m_suggestBanker.EnableWindow(false);

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
	m_sellOnSpace.EnableWindow(true);
	m_sellOnCap.EnableWindow(true);
	m_modPriority.EnableWindow(true);
	m_sellWhen.EnableWindow(true);
	m_stopBySeller.EnableWindow(true);
	m_sellOnCap.EnableWindow(true);
	m_suggestBanker.EnableWindow(true);

	onSellOnCap();
}

void CConfigDialog::configToControls(CConfigData *configData) {
	char buf[128];
	reloadSaleItems();
	reloadSellers();
	for (int x = 0; x < MAX_SELLERS; x++) {
		while (m_SellBox[x].GetCount()) m_SellBox[x].DeleteString(0);
		while (m_BuyBox[x].GetCount()) m_BuyBox[x].DeleteString(0);
	}
	for (int i = 0;i < MAX_SELLERS; i++) {
		m_Seller[i].SelectString(0, configData->sellerList[i].sellerName);
		for (int j = 0; j < 32; j++) {
			if (strlen(configData->sellItem[i].tradeItem[j].itemName)) {
				sprintf(buf, "%s->%d", configData->sellItem[i].tradeItem[j].itemName, configData->sellItem[i].tradeItem[j].quantityBuySell);
				m_SellBox[i].AddString(buf);
				m_tradeItemList.DeleteString(m_tradeItemList.FindStringExact(-1,configData->sellItem[i].tradeItem[j].itemName));
			}
			if (strlen(configData->buyItem[i].tradeItem[j].itemName)) {
				sprintf(buf, "%s->%d:%d@%dgp", configData->buyItem[i].tradeItem[j].itemName, configData->buyItem[i].tradeItem[j].triggerQuantity, configData->buyItem[i].tradeItem[j].quantityBuySell, configData->buyItem[i].tradeItem[j].salePrice);
				m_BuyBox[i].AddString(buf);
				m_tradeItemList.DeleteString(m_tradeItemList.FindStringExact(-1,configData->buyItem[i].tradeItem[j].itemName));
			}
		}
	}
	sprintf(buf,"%d",configData->sellWhen);		m_sellWhen.SetWindowText(buf);
	m_sellOnCap.SetCheck(configData->sellOnCap);
	m_sellOnSpace.SetCheck(configData->sellOnSpace);

	sprintf(buf, "%d", 0);
	m_buyPrice.SetWindowText(buf);
	m_quantityBuySell.SetWindowText(buf);
	m_buyTriggerQuantity.SetWindowText(buf);
	m_tradeItemList.SetCurSel(0);
	m_modPriority.SetCurSel(atoi(configData->modPriorityStr) - 1);
	m_stopBySeller.SetCheck(configData->stopBySeller);
	m_suggestBanker.SetCheck(configData->suggestBanker);
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
		m_Seller[i].GetLBText(index,buf);
		
		for (index=0;index<sellerNum && strcmp(sellersInfo[index].name,buf);index++){}
		strcpy(newConfigData->sellerList[i].sellerName, sellersInfo[index].name);
		newConfigData->sellerList[i].sellerX = sellersInfo[index].xPos;
		newConfigData->sellerList[i].sellerY = sellersInfo[index].yPos;
		newConfigData->sellerList[i].sellerZ = sellersInfo[index].zPos;
	}
	newConfigData->sellOnCap = m_sellOnCap.GetCheck();
	newConfigData->sellOnSpace = m_sellOnSpace.GetCheck();
	m_sellWhen.GetWindowText(buf,127);newConfigData->sellWhen=atoi(buf);
	sprintf(newConfigData->modPriorityStr,"%d",m_modPriority.GetCurSel()+1);
	newConfigData->stopBySeller=m_stopBySeller.GetCheck();
	newConfigData->suggestBanker= m_suggestBanker.GetCheck();
	
	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent) {
	if (nIDEvent==1001) {
		KillTimer(1001);
		CMemReaderProxy reader;

		char buf[256];
		switch(globalSellerState){
		case CToolSellerState_notRunning:
			m_stateSeller.SetWindowText("Not running");
			break;
		case CToolSellerState_halfSleep:
			sprintf(buf,"Module sleep by %s:%s",reader.getGlobalVariable("walking_control"),reader.getGlobalVariable("walking_priority"));
			m_stateSeller.SetWindowText(buf);
			break;
		case CToolSellerState_noPathFound:
			sprintf(buf,"Path not found (%d,%d,%d)",GUIx,GUIy,GUIz);
			m_stateSeller.SetWindowText(buf);
			break;
		case CToolSellerState_walking:
			sprintf(buf,"Walking to Seller %d (%d,%d,%d)",GUINum,GUIx,GUIy,GUIz);
			m_stateSeller.SetWindowText(buf);
			break;
		case CToolSellerState_talking:
			m_stateSeller.SetWindowText("Talking with Seller");
			break;
		default:
			m_stateSeller.SetWindowText("Unknown state");
			break;
		}
		if(atoi(reader.getGlobalVariable("banker_suggestion"))){
			sprintf(buf,"(%s gold)",reader.getGlobalVariable("banker_suggestion"));
			m_bankerSuggestion.SetWindowText(buf);
		}else{
			m_bankerSuggestion.SetWindowText("(none)");
		}
		SetTimer(1001,250,NULL);
	}
	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::DoSetButtonSkin(){
	skin.SetButtonSkin(m_OK);
	skin.SetButtonSkin(m_enable);
	skin.SetButtonSkin(m_addSeller);
	for (int loop = 0; loop < MAX_SELLERS; loop++) {
		skin.SetButtonSkin( m_BuyBoxAdd[loop]);
		skin.SetButtonSkin( m_BuyBoxRemove[loop]);
		skin.SetButtonSkin( m_SellBoxAdd[loop]);
		skin.SetButtonSkin( m_SellBoxRemove[loop]);
		m_Seller[loop].LimitText(127);
	}

}

BOOL CConfigDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	DoSetButtonSkin();

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
	for (int count = 0; count < sellerNum; count++) {
		m_Seller[0].AddString(sellersInfo[count].name);
		m_Seller[1].AddString(sellersInfo[count].name);
		m_Seller[2].AddString(sellersInfo[count].name);
		m_Seller[3].AddString(sellersInfo[count].name);
	}
	m_Seller[0].SetCurSel(0);
	m_Seller[1].SetCurSel(1);
	m_Seller[2].SetCurSel(2);
	m_Seller[3].SetCurSel(3);
}

int initalizeSellers() {
	char installPath[1024];
	CModuleUtil::getInstallPath(installPath);
	char pathBuf[2048];
	sprintf(pathBuf,"%s\\mods\\tibiaauto-sellers.csv",installPath);
	
	ifstream sellerFile (pathBuf, ios::in);
	if (!sellerFile.is_open()) {	AfxMessageBox("File TibiaAuto-Sellers.csv Not found!"); sellerFile.close(); return 0;}
	char buf[128] = {0};
	
	sellerNum = 0;
	while (!sellerFile.eof() && sellerNum<MAX_NPCS) {
		sellerFile.getline(buf, 128, ',');
		if (strlen(buf) == 0) break;
		strcpy(sellersInfo[sellerNum].name,buf);
		sellerFile.getline(buf, 128, ',');
		sellersInfo[sellerNum].xPos = atoi(buf);
		sellerFile.getline(buf, 128, ',');
		sellersInfo[sellerNum].yPos = atoi(buf);
		sellerFile.getline(buf, 128, '\n');
		sellersInfo[sellerNum++].zPos = atoi(buf);
	}
	sellerFile.close();
	
	return 1;
}

void saveSellers(){
	char installPath[1024];
	CModuleUtil::getInstallPath(installPath);
	char pathBuf[2048];
	sprintf(pathBuf,"%s\\mods\\tibiaauto-sellers.csv",installPath);
	
	ofstream sellerFile (pathBuf, ios::out);
	if (!sellerFile.is_open()) {
		AfxMessageBox("File TibiaAuto-Sellers.csv Not found!");
		sellerFile.close();
		return;
	}
	char buf[256];
	for (int num=0;num<sellerNum;num++) {
		sprintf(buf,"%s,%d,%d,%d\n",sellersInfo[num].name,sellersInfo[num].xPos,sellersInfo[num].yPos,sellersInfo[num].zPos);
		sellerFile.write(buf,strlen(buf));
	}
	sellerFile.close();
}

	
void CConfigDialog::reloadSaleItems() {
	CTibiaItemProxy itemProxy;
	while (m_tradeItemList.GetCount()>0) m_tradeItemList.DeleteString(0);
	int count = itemProxy.getItemCount();
	for (int i = 0; i < count; i++)
		m_tradeItemList.AddString(itemProxy.getItemNameAtIndex(i));
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
	m_tradeItemList.DeleteString(m_tradeItemList.FindStringExact(-1,itemName));
	m_buyPrice.SetWindowText("0");
	m_quantityBuySell.SetWindowText("0");
	m_buyTriggerQuantity.SetWindowText("0");
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
	m_tradeItemList.DeleteString(m_tradeItemList.FindStringExact(-1,itemName));
	m_buyPrice.SetWindowText("0");
	m_quantityBuySell.SetWindowText("0");
	m_buyTriggerQuantity.SetWindowText("0");
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
	m_tradeItemList.DeleteString(m_tradeItemList.FindStringExact(-1,itemName));
	m_buyPrice.SetWindowText("0");
	m_quantityBuySell.SetWindowText("0");
	m_buyTriggerQuantity.SetWindowText("0");
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
	m_tradeItemList.DeleteString(m_tradeItemList.FindStringExact(-1,itemName));
	m_buyPrice.SetWindowText("0");
	m_quantityBuySell.SetWindowText("0");
	m_buyTriggerQuantity.SetWindowText("0");
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
	m_tradeItemList.DeleteString(m_tradeItemList.FindStringExact(-1,itemName));
	m_buyPrice.SetWindowText("0");
	m_quantityBuySell.SetWindowText("0");
	m_buyTriggerQuantity.SetWindowText("0");
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
	m_tradeItemList.DeleteString(m_tradeItemList.FindStringExact(-1,itemName));
	m_buyPrice.SetWindowText("0");
	m_quantityBuySell.SetWindowText("0");
	m_buyTriggerQuantity.SetWindowText("0");
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
	m_tradeItemList.DeleteString(m_tradeItemList.FindStringExact(-1,itemName));
	m_buyPrice.SetWindowText("0");
	m_quantityBuySell.SetWindowText("0");
	m_buyTriggerQuantity.SetWindowText("0");
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
	m_tradeItemList.DeleteString(m_tradeItemList.FindStringExact(-1,itemName));
	m_buyPrice.SetWindowText("0");
	m_quantityBuySell.SetWindowText("0");
	m_buyTriggerQuantity.SetWindowText("0");
	m_tradeItemList.SetCurSel(0);
}
void CConfigDialog::sellBoxRemove0() {
	char paramString[128];
	char itemName[128];
	int sel=m_SellBox[0].GetCurSel();
	if (sel==-1) return;
	m_SellBox[0].GetText(sel,itemName);
	int j;
	for (j=strlen(itemName)-1;j>0;j--) {
		if (itemName[j]=='-' && itemName[j + 1]=='>') {
			memcpy(paramString,itemName+j+2,strlen(itemName)-j);
			itemName[j]='\0';
			break;
		}
	}
	m_tradeItemList.AddString(itemName);
	m_quantityBuySell.SetWindowText(paramString);
	m_buyPrice.SetWindowText("0");
	m_tradeItemList.SetCurSel(m_tradeItemList.FindStringExact(-1,itemName));
	m_SellBox[0].DeleteString(sel);
}
void CConfigDialog::sellBoxRemove1() {
	char paramString[128];
	char itemName[128];
	int sel=m_SellBox[1].GetCurSel();
	if (sel==-1) return;
	m_SellBox[1].GetText(sel,itemName);
	int j;
	for (j=strlen(itemName)-1;j>0;j--) {
		if (itemName[j]=='-' && itemName[j + 1]=='>') {
			memcpy(paramString,itemName+j+2,strlen(itemName)-j);
			itemName[j]='\0';
			break;
		}
	}
	m_tradeItemList.AddString(itemName);
	m_quantityBuySell.SetWindowText(paramString);
	m_buyPrice.SetWindowText("0");
	m_tradeItemList.SetCurSel(m_tradeItemList.FindStringExact(-1,itemName));
	m_SellBox[1].DeleteString(sel);
}
void CConfigDialog::sellBoxRemove2() {
	char paramString[128];
	char itemName[128];
	int sel=m_SellBox[2].GetCurSel();
	if (sel==-1) return;
	m_SellBox[2].GetText(sel,itemName);
	int j;
	for (j=strlen(itemName)-1;j>0;j--) {
		if (itemName[j]=='-' && itemName[j + 1]=='>') {
			memcpy(paramString,itemName+j+2,strlen(itemName)-j);
			itemName[j]='\0';
			break;
		}
	}
	m_tradeItemList.AddString(itemName);
	m_quantityBuySell.SetWindowText(paramString);
	m_buyPrice.SetWindowText("0");
	m_tradeItemList.SetCurSel(m_tradeItemList.FindStringExact(-1,itemName));
	m_SellBox[2].DeleteString(sel);
}
void CConfigDialog::sellBoxRemove3() {
	char paramString[128];
	char itemName[128];
	int sel=m_SellBox[3].GetCurSel();
	if (sel==-1) return;
	m_SellBox[3].GetText(sel,itemName);
	int j;
	for (j=strlen(itemName)-1;j>0;j--) {
		if (itemName[j]=='-' && itemName[j + 1]=='>') {
			memcpy(paramString,itemName+j+2,strlen(itemName)-j);
			itemName[j]='\0';
			break;
		}
	}
	m_tradeItemList.AddString(itemName);
	m_quantityBuySell.SetWindowText(paramString);
	m_buyPrice.SetWindowText("0");
	m_tradeItemList.SetCurSel(m_tradeItemList.FindStringExact(-1,itemName));
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
	sscanf(paramString, "%d:%d@%dgp", &trigger, &quantity, &price);
	m_tradeItemList.AddString(itemName);
	sprintf(buf, "%d", quantity);
	m_quantityBuySell.SetWindowText(buf);
	sprintf(buf, "%d", price);
	m_buyPrice.SetWindowText(buf);
	sprintf(buf, "%d", trigger);
	m_buyTriggerQuantity.SetWindowText(buf);
	m_tradeItemList.SetCurSel(m_tradeItemList.FindStringExact(-1,itemName));
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
	sscanf(paramString, "%d:%d@%dgp", &trigger, &quantity, &price);
	m_tradeItemList.AddString(itemName);
	sprintf(buf, "%d", quantity);
	m_quantityBuySell.SetWindowText(buf);
	sprintf(buf, "%d", price);
	m_buyPrice.SetWindowText(buf);
	sprintf(buf, "%d", trigger);
	m_buyTriggerQuantity.SetWindowText(buf);
	m_tradeItemList.SetCurSel(m_tradeItemList.FindStringExact(-1,itemName));
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
		if (itemName[j]=='-' && itemName[j + 1]=='>') {
			memcpy(paramString,itemName+j+2,strlen(itemName)-j);
			itemName[j]='\0';
			break;
		}
	}
	sscanf(paramString, "%d:%d@%dgp", &trigger, &quantity, &price);
	m_tradeItemList.AddString(itemName);
	sprintf(buf, "%d", quantity);
	m_quantityBuySell.SetWindowText(buf);
	sprintf(buf, "%d", price);
	m_buyPrice.SetWindowText(buf);
	sprintf(buf, "%d", trigger);
	m_buyTriggerQuantity.SetWindowText(buf);
	m_tradeItemList.SetCurSel(m_tradeItemList.FindStringExact(-1,itemName));
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
	sscanf(paramString, "%d:%d@%dgp", &trigger, &quantity, &price);
	m_tradeItemList.AddString(itemName);
	sprintf(buf, "%d", quantity);
	m_quantityBuySell.SetWindowText(buf);
	sprintf(buf, "%d", price);
	m_buyPrice.SetWindowText(buf);
	sprintf(buf, "%d", trigger);
	m_buyTriggerQuantity.SetWindowText(buf);
	m_tradeItemList.SetCurSel(m_tradeItemList.FindStringExact(-1,itemName));
	m_BuyBox[3].DeleteString(sel);
}

void CConfigDialog::onSellOnCap() {
	m_sellWhen.EnableWindow(m_sellOnCap.GetCheck());
}


void CConfigDialog::OnAddSeller(){
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	char buf[128];
	m_addName.GetWindowText(buf,127);
	int sellerFound=0;
	int i;
	for (i=0;i<memConstData.m_memMaxCreatures;i++){
		CTibiaCharacter* mon=reader.readVisibleCreature(i);
		if (mon->tibiaId == 0){
			delete mon;
			break;
		}
		int lenName=strlen(mon->name);
		if (strncmp(buf,mon->name,lenName)==0 && (buf[lenName]==0 || buf[lenName]==' ')){
			sellerFound=1;
			break;
		}
		delete mon;
	}
	if (!sellerFound){
		AfxMessageBox("NPC not found in recently seen creatures list.");
		return;
	}

	CTibiaCharacter* NPCSeller=reader.readVisibleCreature(i);
	if (sellerNum>=MAX_NPCS){
		AfxMessageBox("NPC list is full.");
		return;
	}
	strcpy(sellersInfo[sellerNum].name,buf);
	sellersInfo[sellerNum].xPos=NPCSeller->x;
	sellersInfo[sellerNum].yPos=NPCSeller->y;
	sellersInfo[sellerNum].zPos=NPCSeller->z;
	delete NPCSeller;

	sprintf(buf,"Added %s sucessfully to the list at position (%d,%d,%d)",sellersInfo[sellerNum].name,sellersInfo[sellerNum].xPos,sellersInfo[sellerNum].yPos,sellersInfo[sellerNum].zPos);
	AfxMessageBox(buf);
	sellerNum++;

	saveSellers();
	reloadSellers();
	return;

	
}