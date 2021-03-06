#include "GW_ItemSlotBundle.h"
#include "WvsUnified.h"
#include "Poco\Data\MySQL\MySQLException.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"

GW_ItemSlotBundle::GW_ItemSlotBundle()
{
	nInstanceType = GW_ItemSlotInstanceType::GW_ItemSlotBundle_Type;
}


GW_ItemSlotBundle::~GW_ItemSlotBundle()
{
}

void ConstructItemFromDBRecordSet(GW_ItemSlotBundle *pItem, int nType, Poco::Data::RecordSet& recordSet)
{
	pItem->nType = (GW_ItemSlotBase::GW_ItemSlotType)nType;
	if (nType == GW_ItemSlotBase::GW_ItemSlotType::CASH)
	{
		pItem->liCashItemSN = recordSet["CashItemSN"];
		pItem->bIsCash = true;
	}
	else
		pItem->liItemSN = recordSet["ItemSN"];

	pItem->nCharacterID = recordSet["CharacterID"];
	pItem->nItemID = recordSet["ItemID"];
	pItem->liExpireDate = recordSet["ExpireDate"];
	pItem->nAttribute = recordSet["Attribute"];
	pItem->nNumber = recordSet["Number"];
	pItem->nPOS = recordSet["POS"];
}

void GW_ItemSlotBundle::LoadAll(int nType, int nCharacterID, std::map<int, ZSharedPtr<GW_ItemSlotBase>>& mRes)
{
	std::string strTableName = "";

	if (nType == GW_ItemSlotType::CONSUME)
		strTableName = "ItemSlot_CON";
	else if (nType == GW_ItemSlotType::ETC)
		strTableName = "ItemSlot_ETC";
	else if (nType == GW_ItemSlotType::INSTALL)
		strTableName = "ItemSlot_INS";
	else if (nType == GW_ItemSlotType::CASH)
		strTableName = "CashItem_Bundle";
	else
		throw std::runtime_error("Invalid Item Slot Type.");

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM " << strTableName << " Where CharacterID = " << nCharacterID << " AND POS < " << GW_ItemSlotBase::LOCK_POS;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);

	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		auto pItem = MakeShared<GW_ItemSlotBundle>();
		ConstructItemFromDBRecordSet(pItem, nType, recordSet);
		mRes[pItem->nPOS] = pItem;
	}
}

void GW_ItemSlotBundle::Load(ATOMIC_COUNT_TYPE SN)
{
	std::string strTableName = "",
				sSNColumnName = (nType == GW_ItemSlotType::CASH ? "CashItemSN" : "ItemSN");

	if (nType == GW_ItemSlotType::CONSUME)
		strTableName = "ItemSlot_CON";
	else if (nType == GW_ItemSlotType::ETC)
		strTableName = "ItemSlot_ETC";
	else if (nType == GW_ItemSlotType::INSTALL)
		strTableName = "ItemSlot_INS";
	else if (nType == GW_ItemSlotType::CASH)
		strTableName = "CashItem_Bundle";
	else
		throw std::runtime_error("Invalid Item Slot Type.");
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM " << strTableName << " Where " + sSNColumnName + " = " << SN;
	queryStatement.execute();

	Poco::Data::RecordSet recordSet(queryStatement);
	ConstructItemFromDBRecordSet(this, nType, recordSet);
}

void GW_ItemSlotBundle::Save(int nCharacterID, bool bRemoveRecord, bool bExpired)
{
	std::string strTableName = "",
		sSNColumnName = nType == GW_ItemSlotType::CASH ? "CashItemSN" : "ItemSN";

	if (bExpired)
		strTableName = "ItemExpired_Bundle";
	else if (nType == GW_ItemSlotType::CONSUME)
		strTableName = "ItemSlot_CON";
	else if (nType == GW_ItemSlotType::ETC)
		strTableName = "ItemSlot_ETC";
	else if (nType == GW_ItemSlotType::INSTALL)
		strTableName = "ItemSlot_INS";
	else if (nType == GW_ItemSlotType::CASH)
		strTableName = "CashItem_Bundle";
	else
		throw std::runtime_error("Invalid Item Slot Type.");
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	try 
	{
		//09/12/2019 modified, for CASH ITEMs (nTI = 5) support.
		auto pSN = (nType == GW_ItemSlotType::CASH ? &liCashItemSN : &liItemSN);
		if (*pSN < -1 && bRemoveRecord) //DROPPED or DELETED
		{
			*pSN *= -1;
			queryStatement << "UPDATE " << strTableName
				<< " Set CharacterID = -1 Where CharacterID = " << nCharacterID
				<< " and " + sSNColumnName + " = " << *pSN;
			queryStatement.execute();
			return;
		}
		else
		{
			if (nType != GW_ItemSlotType::CASH && liItemSN <= 0)
				liItemSN = IncItemSN(nType);
			if (nType == GW_ItemSlotType::CASH && liCashItemSN == -1)
				liCashItemSN = IncItemSN(nType);

			queryStatement << "INSERT INTO " << strTableName << " (" + sSNColumnName + ", ItemID, CharacterID, ExpireDate, Attribute, POS, Number) VALUES("
				<< (nType == GW_ItemSlotType::CASH ? liCashItemSN : liItemSN) << ", "
				<< nItemID << ", "
				<< nCharacterID << ", "
				<< liExpireDate << ", "
				<< nAttribute << ", "
				<< nPOS << ", "
				<< nNumber << ")";
			queryStatement << " ON DUPLICATE KEY UPDATE "
				<< "ItemID = '" << nItemID << "', "
				<< "CharacterID = '" << nCharacterID << "', "
				<< "ExpireDate = '" << liExpireDate << "', "
				<< "Attribute = '" << nAttribute << "', "
				<< "POS ='" << nPOS << "', "
				<< "Number = '" << nNumber << "'";
				//<< "' WHERE " + sSNColumnName + " = " << (nType == GW_ItemSlotType::CASH ? liCashItemSN : liItemSN);
		}
		queryStatement.execute();
	}
	catch (Poco::Data::MySQL::StatementException &se) 
	{
		WvsLogger::LogFormat("SQL Exception Occurred: %s\nRaw Query = %s\n", se.message().c_str(), queryStatement.toString().c_str());
	}
}

void GW_ItemSlotBundle::Encode(OutPacket *oPacket, bool bForInternal) const
{
	EncodeInventoryPosition(oPacket);
	if (bForInternal)
		oPacket->Encode8(liItemSN);
	RawEncode(oPacket);
}

/*
Encode Bundle Item Info.
*/
void GW_ItemSlotBundle::RawEncode(OutPacket *oPacket) const
{
	GW_ItemSlotBase::RawEncode(oPacket);
	oPacket->Encode2(nNumber);
	oPacket->EncodeStr("");
	oPacket->Encode2(nAttribute);

	//          Throwing Start            Bullet
	if ((nItemID / 10000 == 207) || (nItemID / 10000 == 233))
		oPacket->Encode8(liItemSN);
}

void GW_ItemSlotBundle::Decode(InPacket *iPacket, bool bForInternal)
{
	if (bForInternal)
		liItemSN = iPacket->Decode8();
	RawDecode(iPacket);
}

void GW_ItemSlotBundle::RawDecode(InPacket *iPacket)
{
	GW_ItemSlotBase::RawDecode(iPacket);
	nNumber = iPacket->Decode2();
	std::string strTitle = iPacket->DecodeStr();
	nAttribute = iPacket->Decode2();
	if ((nItemID / 10000 == 207) || (nItemID / 10000 == 233))
		liItemSN = iPacket->Decode8();
}

GW_ItemSlotBase * GW_ItemSlotBundle::MakeClone() const
{
	GW_ItemSlotBundle* ret = AllocObj(GW_ItemSlotBundle);
	*ret = *this;
	ret->liItemSN = -1;
	ret->liCashItemSN = -1;

	return ret;
}