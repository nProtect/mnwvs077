#pragma once
#include <memory>
#include <vector>
#include <map>
#include "..\WvsLib\Memory\ZMemory.h"

struct GW_ItemSlotBase;

struct RewardInfo
{
	int m_nItemID = 0,
		m_nType = 0,
		m_nMoney = 0,
		m_nMax = 1,
		m_nMin = 1,
		m_nPeriod = 0,
		m_nMaxCount = 1;

	unsigned int m_unWeight = 0;
	int m_usQRKey = 0;
	bool m_bPremiumMap = false;
};

class Reward
{
private:
	ZSharedPtr<GW_ItemSlotBase> m_pItem;

	int m_nMoney = 0, 
		m_nPeriod = 0,
		m_nType = 0,
		m_n;

	RewardInfo* m_pInfo = nullptr;
	static std::map<int, std::vector<RewardInfo*>> stMobRewardInfo;
	static std::map<int, std::vector<RewardInfo*>> stReactorRewardInfo;
	static double ms_fIncDropRate;

public:
	Reward();
	Reward(Reward* pOther);
	~Reward();

	int GetType() { return m_nType; }
	int GetMoney() { return m_nMoney; }
	int GetPeriod() { return m_nPeriod; }

	void SetType(int nType) { m_nType = nType; }
	void SetMoney(int nMoney) { m_nMoney = nMoney; }
	void SetPeriod(int nPeriod) { m_nPeriod = nPeriod; }

	ZSharedPtr<GW_ItemSlotBase> GetItem() { return m_pItem; }
	void SetItem(const ZSharedPtr<GW_ItemSlotBase>& pItem) { m_pItem = pItem; }
	RewardInfo* GetRewardInfo() { return m_pInfo; }

	static void LoadReward();
	static const std::vector<RewardInfo*>* GetMobReward(int nTemplateID);
	static const std::vector<RewardInfo*>* GetReactorReward(int nTemplateID);
	static std::vector<ZUniquePtr<Reward>> Create(const std::vector<RewardInfo*> *aRewardInfo, bool bPremiumMap, double dRegionalIncRate, double dShowdown, double dOwnerDropRate, double dOwnerDropRate_Ticket, double *pRewardRate);
};

