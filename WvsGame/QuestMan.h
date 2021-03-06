#pragma once
#include <map>
#include <vector>
#include <set>

struct QuestAct;
struct QuestDemand;

class User;

class QuestMan
{
	std::map<int, QuestAct*> m_mStartAct, m_mCompleteAct;
	std::map<int, QuestDemand*> m_mStartDemand, m_mCompleteDemand;
	std::set<int> m_mAutoStartQuest, m_mAutoCompleteQuest;
	std::map<int, int> m_mItemQuest;
	std::map<int, std::vector<int>> m_mMobQuest;

	QuestMan();
	~QuestMan();

	void LoadAct();
	void LoadDemand();
	void RegisterAct(void* pProp);
	void RegisterDemand(void* pProp);
public:
	void Initialize(bool bReleaseWzResource = true);
	static QuestMan* GetInstance();

	bool IsAutoStartQuest(int nQuestID);
	bool IsAutoCompleteQuest(int nQuestID);
	bool CheckStartDemand(int nQuestID, User *pUser);
	bool CheckCompleteDemand(int nQuestID, User *pUser);

	QuestAct* GetStartAct(int nQuestID);
	QuestAct* GetCompleteAct(int nQuestID);
	QuestDemand* GetStartDemand(int nQuestID);
	QuestDemand* GetCompleteDemand(int nQuestID);
	int GetQuestByItem(int nItemID);
	const std::vector<int>& GetQuestByMob(int nMobID);
};

