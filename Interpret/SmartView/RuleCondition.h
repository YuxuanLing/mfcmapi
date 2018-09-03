#pragma once
#include <Interpret/SmartView/SmartViewParser.h>
#include <Interpret/SmartView/RestrictionStruct.h>

namespace smartview
{
	// http://msdn.microsoft.com/en-us/library/ee158295.aspx
	// http://msdn.microsoft.com/en-us/library/ee179073.aspx

	// [MS-OXCDATA]
	// PropertyName
	// =====================
	//   This structure specifies a Property Name
	//
	struct PropertyName
	{
		blockT<BYTE> Kind{};
		blockT<GUID> Guid{};
		blockT<DWORD> LID{};
		blockT<BYTE> NameSize{};
		blockStringW Name;
	};

	// [MS-OXORULE]
	// NamedPropertyInformation
	// =====================
	//   This structure specifies named property information for a rule condition
	//
	struct NamedPropertyInformation
	{
		blockT<WORD> NoOfNamedProps{};
		std::vector<blockT<WORD>> PropId;
		blockT<DWORD> NamedPropertiesSize{};
		std::vector<PropertyName> PropertyName;
	};

	class RuleCondition : public SmartViewParser
	{
	public:
		void Init(bool bExtended);

	private:
		void Parse() override;
		void ParseBlocks() override;

		NamedPropertyInformation m_NamedPropertyInformation;
		RestrictionStruct m_lpRes;
		bool m_bExtended = false;
	};
}