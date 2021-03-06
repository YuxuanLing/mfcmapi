#include <core/stdafx.h>
#include <core/smartview/RuleAction.h>
#include <core/smartview/EntryIdStruct.h>
#include <core/smartview/SPropValueStruct.h>
#include <core/interpret/guid.h>
#include <core/interpret/flags.h>
#include <core/mapi/extraPropTags.h>

namespace smartview
{
	// 2.2.5.1.2.1.1 ServerEid Structure
	// https://docs.microsoft.com/en-us/openspecs/exchange_server_protocols/ms-oxorule/07e8c314-0ab2-440e-9138-b96f93682bf1
	class ServerEID : public block
	{
	private:
		std::shared_ptr<blockT<BYTE>> Ours = emptyT<BYTE>();
		std::shared_ptr<blockT<LARGE_INTEGER>> FolderId = emptyT<LARGE_INTEGER>();
		std::shared_ptr<blockT<LARGE_INTEGER>> MessageId = emptyT<LARGE_INTEGER>();
		std::shared_ptr<blockT<DWORD>> Instance = emptyT<DWORD>();

		void parse() override
		{
			Ours = blockT<BYTE>::parse(parser);
			FolderId = blockT<LARGE_INTEGER>::parse(parser);
			MessageId = blockT<LARGE_INTEGER>::parse(parser);
			Instance = blockT<DWORD>::parse(parser);
		};
		void parseBlocks() override
		{
			setText(L"ServerEID");
			addChild(Ours, L"Ours: %1!ws!", *Ours ? L"true" : L"false");
			addChild(
				FolderId, L"FolderId: 0x%1!08X!:0x%2!08X!", FolderId->getData().HighPart, FolderId->getData().LowPart);
			addChild(
				MessageId,
				L"MessageId: 0x%1!08X!:0x%2!08X!",
				MessageId->getData().HighPart,
				MessageId->getData().LowPart);
			addChild(Instance, L"Instance: 0x%1!04X!", Instance->getData());
		};
	};

	// 2.2.5.1.2.1 OP_MOVE and OP_COPY ActionData Structure
	// https://docs.microsoft.com/en-us/openspecs/exchange_server_protocols/ms-oxorule/78bc6a96-f94c-43c6-afd3-7f8c39a2340c
	class ActionDataMoveCopy : public ActionData
	{
	protected:
		std::shared_ptr<blockT<BYTE>> FolderInThisStore = emptyT<BYTE>();
		std::shared_ptr<blockT<DWORD>> StoreEIDSize = emptyT<DWORD>();
		std::shared_ptr<EntryIdStruct> StoreEID;
		std::shared_ptr<blockBytes> StoreEIDBytes = emptyBB();
		std::shared_ptr<blockT<DWORD>> FolderEIDSize = emptyT<DWORD>();
		std::shared_ptr<EntryIdStruct> FolderEID;
		std::shared_ptr<ServerEID> FolderEIDserverEID;
		std::shared_ptr<blockBytes> FolderEIDBytes = emptyBB();
		void parse() override
		{
			if (m_bExtended)
			{
				StoreEIDSize = blockT<DWORD>::parse(parser);
				StoreEIDBytes = blockBytes::parse(parser, *StoreEIDSize);
				FolderEIDSize = blockT<DWORD>::parse(parser);
				FolderEID = block::parse<EntryIdStruct>(parser, *FolderEIDSize, true);
			}
			else
			{
				FolderInThisStore = blockT<BYTE>::parse(parser);
				StoreEIDSize = blockT<DWORD>::parse<WORD>(parser);
				if (*FolderInThisStore)
				{
					StoreEIDBytes = blockBytes::parse(parser, *StoreEIDSize);
				}
				else
				{
					StoreEID = block::parse<EntryIdStruct>(parser, *StoreEIDSize, true);
				}

				FolderEIDSize = blockT<DWORD>::parse<WORD>(parser);
				if (*FolderInThisStore)
				{
					FolderEIDserverEID = block::parse<ServerEID>(parser, *FolderEIDSize, true);
				}
				else
				{
					FolderEIDBytes = blockBytes::parse(parser, *FolderEIDSize);
				}
			}
		}
		void parseBlocks()
		{
			switch (actionType)
			{
			case OP_MOVE:
				setText(L"ActionDataMove");
				break;
			case OP_COPY:
				setText(L"ActionDataCopy");
				break;
			}

			if (m_bExtended)
			{
				addChild(StoreEIDSize, L"StoreEIDSize: 0x%1!08X!", StoreEIDSize->getData());
				addLabeledChild(L"StoreEIDBytes", StoreEIDBytes);
				addChild(FolderEIDSize, L"FolderEIDSize: 0x%1!08X!", FolderEIDSize->getData());
				addLabeledChild(L"FolderEID", FolderEID);
			}
			else
			{
				addChild(FolderInThisStore, L"FolderInThisStore: %1!ws!", *FolderInThisStore ? L"true" : L"false");
				addChild(StoreEIDSize, L"StoreEIDSize: 0x%1!04X!", StoreEIDSize->getData());
				if (*FolderInThisStore)
				{
					addChild(StoreEID, L"StoreEID");
				}
				else
				{
					addLabeledChild(L"StoreEIDBytes", StoreEIDBytes);
				}

				addChild(FolderEIDSize, L"FolderEIDSize: 0x%1!04X!", FolderEIDSize->getData());
				if (*FolderInThisStore)
				{
					addLabeledChild(L"FolderEIDserverEID", FolderEIDserverEID);
				}
				else
				{
					addLabeledChild(L"FolderEIDBytes", FolderEIDBytes);
				}
			}
		};
	};

	// 2.2.5.1.2.2 OP_REPLY and OP_OOF_REPLY ActionData Structure
	// https://docs.microsoft.com/en-us/openspecs/exchange_server_protocols/ms-oxorule/d5c14d2f-3557-4b78-acfe-d949ef325792
	class ActionDataReply : public ActionData
	{
	protected:
		std::shared_ptr<blockT<LARGE_INTEGER>> ReplyTemplateFID = emptyT<LARGE_INTEGER>();
		std::shared_ptr<blockT<LARGE_INTEGER>> ReplyTemplateMID = emptyT<LARGE_INTEGER>();
		std::shared_ptr<blockT<DWORD>> MessageEIDSize = emptyT<DWORD>();
		std::shared_ptr<EntryIdStruct> ReplyTemplateMessageEID;
		std::shared_ptr<blockT<GUID>> ReplyTemplateGUID = emptyT<GUID>();
		void parse() override
		{
			if (m_bExtended)
			{
				MessageEIDSize = blockT<DWORD>::parse(parser);
				ReplyTemplateMessageEID = block::parse<EntryIdStruct>(parser, *MessageEIDSize, true);
			}
			else
			{
				ReplyTemplateFID = blockT<LARGE_INTEGER>::parse(parser);
				ReplyTemplateMID = blockT<LARGE_INTEGER>::parse(parser);
			}

			ReplyTemplateGUID = blockT<GUID>::parse(parser);
		}
		void parseBlocks()
		{
			switch (actionType)
			{
			case OP_REPLY:
				setText(L"ActionDataReply");
				break;
			case OP_OOF_REPLY:
				setText(L"ActionDataOOFReply");
				break;
			}

			if (m_bExtended)
			{
				addChild(MessageEIDSize, L"MessageEIDSize: 0x%1!08X!", MessageEIDSize->getData());
				addChild(ReplyTemplateMessageEID, L"ReplyTemplateMessageEID");
			}
			else
			{
				addChild(
					ReplyTemplateFID,
					L"ReplyTemplateFID: 0x%1!08X!:0x%2!08X!",
					ReplyTemplateFID->getData().HighPart,
					ReplyTemplateFID->getData().LowPart);
				addChild(
					ReplyTemplateMID,
					L"ReplyTemplateMID: 0x%1!08X!:0x%2!08X!",
					ReplyTemplateMID->getData().HighPart,
					ReplyTemplateMID->getData().LowPart);
			}

			addChild(
				ReplyTemplateGUID,
				L"ReplyTemplateGUID = %1!ws!",
				guid::GUIDToStringAndName(*ReplyTemplateGUID).c_str());
		};
	};

	// 2.2.5.1.2.3 OP_DEFER_ACTION ActionData Structure
	// https://docs.microsoft.com/en-us/openspecs/exchange_server_protocols/ms-oxorule/1d66ff31-fdc1-4b36-a040-75207e31dd18
	class ActionDataDefer : public ActionData
	{
	private:
		std::shared_ptr<blockBytes> deferBytes = emptyBB();
		void parse() override { deferBytes = blockBytes::parse(parser, parser->getSize()); }
		void parseBlocks()
		{
			setText(L"ActionDataDefer");
			addLabeledChild(L"deferBytes", deferBytes);
		};
	};

	// 2.2.5.1.2.4.1 RecipientBlockData Structure
	// https://docs.microsoft.com/en-us/openspecs/exchange_server_protocols/ms-oxorule/c6ad133d-7906-43aa-8420-3b40ac6be494
	class RecipientBlockData : public block
	{
	private:
		std::shared_ptr<blockT<BYTE>> Reserved = emptyT<BYTE>();
		std::shared_ptr<blockT<DWORD>> NoOfProperties = emptyT<DWORD>();
		std::vector<std::shared_ptr<SPropValueStruct>> PropertyValues;

		void parse() override
		{
			Reserved = blockT<BYTE>::parse(parser);
			NoOfProperties = blockT<DWORD>::parse(parser);
			if (*NoOfProperties && *NoOfProperties < _MaxEntriesLarge)
			{
				PropertyValues.reserve(*NoOfProperties);
				for (DWORD i = 0; i < *NoOfProperties; i++)
				{
					auto prop = std::make_shared<SPropValueStruct>(i, false, true);
					prop->block::parse(parser, false);
					PropertyValues.push_back(prop);
				}
			}
		};
		void parseBlocks() override
		{
			setText(L"RecipientBlockData");
			addChild(Reserved, L"Reserved: 0x%1!01X!", Reserved->getData());
			addChild(NoOfProperties, L"NoOfProperties: 0x%1!08X!", NoOfProperties->getData());
			auto i = 0;
			for (const auto& propertyValue : PropertyValues)
			{
				addChild(propertyValue, L"PropertyValues[%1!d!]", i++);
			}
		};
	};

	// 2.2.5.1.2.4 OP_FORWARD and OP_DELEGATE ActionData Structure
	// https://docs.microsoft.com/en-us/openspecs/exchange_server_protocols/ms-oxorule/1b8a2b3f-9fff-4e07-9820-c3d2287a8e5c
	class ActionDataForwardDelegate : public ActionData
	{
	protected:
		std::shared_ptr<blockT<DWORD>> RecipientCount = emptyT<DWORD>();
		std::vector<std::shared_ptr<RecipientBlockData>> RecipientBlocks;
		void parse() override
		{
			RecipientCount = blockT<DWORD>::parse(parser);
			if (*RecipientCount && *RecipientCount < _MaxEntriesLarge)
			{
				RecipientBlocks.reserve(*RecipientCount);
				for (DWORD i = 0; i < *RecipientCount; i++)
				{
					RecipientBlocks.push_back(block::parse<RecipientBlockData>(parser, 0, false));
				}
			}
		}
		void parseBlocks()
		{
			switch (actionType)
			{
			case OP_FORWARD:
				setText(L"ActionDataForward");
				break;
			case OP_DELEGATE:
				setText(L"ActionDataDelegate");
				break;
			}

			addChild(RecipientCount, L"RecipientCount: 0x%1!08X!", RecipientCount->getData());
			auto i = 0;
			for (const auto& recipient : RecipientBlocks)
			{
				addChild(recipient, L"RecipientBlocks[%1!d!]", i++);
			}
		};
	};

	// 2.2.5.1.2.5 OP_BOUNCE ActionData Structure
	// https://docs.microsoft.com/en-us/openspecs/exchange_server_protocols/ms-oxorule/c6ceb0c2-96a2-4337-a4fb-f2e23cfe4284
	class ActionDataBounce : public ActionData
	{
	private:
		std::shared_ptr<blockT<DWORD>> BounceCode = emptyT<DWORD>();
		void parse() override { BounceCode = blockT<DWORD>::parse(parser); }

		void parseBlocks()
		{
			setText(L"ActionDataBounce");
			addChild(BounceCode, L"BounceCode: 0x%1!08X!", BounceCode->getData());
		}
	};

	// 2.2.5.1.2.6 OP_TAG ActionData Structure
	// https://docs.microsoft.com/en-us/openspecs/exchange_server_protocols/ms-oxorule/2fe6b110-5e1e-4a97-aeeb-9103cf76a0e0
	class ActionDataTag : public ActionData
	{
	private:
		std::shared_ptr<SPropValueStruct> TaggedPropertyValue;
		void parse() override
		{
			TaggedPropertyValue = std::make_shared<SPropValueStruct>(0, false, true);
			TaggedPropertyValue->block::parse(parser, false);
		}

		void parseBlocks()
		{
			setText(L"ActionDataTag");
			addLabeledChild(L"TaggedPropertyValue", TaggedPropertyValue);
		}
	};

	// 2.2.5.1.2.7 OP_DELETE or OP_MARK_AS_READ ActionData Structure
	// https://docs.microsoft.com/en-us/openspecs/exchange_server_protocols/ms-oxorule/c180c7cb-474a-46f2-95ee-568ec61f1043
	class ActionDataDeleteMarkRead : public ActionData
	{
	private:
		void parse() override {}
		void parseBlocks()
		{
			switch (actionType)
			{
			case OP_DELETE:
				setText(L"ActionDataDelete");
				break;
			case OP_MARK_AS_READ:
				setText(L"ActionDataMarkRead");
				break;
			}
		}
	};

	std::shared_ptr<ActionData> getActionDataParser(BYTE at, bool bExtended)
	{
		std::shared_ptr<ActionData> ret{};
		switch (at)
		{
		case OP_MOVE:
			ret = std::make_shared<ActionDataMoveCopy>();
			break;
		case OP_COPY:
			ret = std::make_shared<ActionDataMoveCopy>();
			break;
		case OP_REPLY:
			ret = std::make_shared<ActionDataReply>();
			break;
		case OP_OOF_REPLY:
			ret = std::make_shared<ActionDataReply>();
			break;
		case OP_DEFER_ACTION:
			ret = std::make_shared<ActionDataDefer>();
			break;
		case OP_BOUNCE:
			ret = std::make_shared<ActionDataBounce>();
			break;
		case OP_FORWARD:
			ret = std::make_shared<ActionDataForwardDelegate>();
			break;
		case OP_DELEGATE:
			ret = std::make_shared<ActionDataForwardDelegate>();
			break;
		case OP_TAG:
			ret = std::make_shared<ActionDataTag>();
			break;
		case OP_DELETE:
			ret = std::make_shared<ActionDataDeleteMarkRead>();
			break;
		case OP_MARK_AS_READ:
			ret = std::make_shared<ActionDataDeleteMarkRead>();
			break;
		}

		if (ret) ret->init(at, bExtended);
		return ret;
	}

	void ActionBlock ::parse()
	{
		if (m_bExtended)
		{
			ActionLength = blockT<DWORD>::parse(parser);
		}
		else
		{
			ActionLength = blockT<DWORD>::parse<WORD>(parser);
		}

		ActionType = blockT<BYTE>::parse(parser);
		ActionFlavor = blockT<DWORD>::parse(parser);
		ActionFlags = blockT<DWORD>::parse(parser);
		const auto headerSize = sizeof BYTE + 2 * sizeof DWORD;

		// ActionLength includes the size of ActionType, ActionFlavor and Action Flags
		if (*ActionLength > headerSize)
		{
			ActionData = getActionDataParser(*ActionType, m_bExtended);
			if (ActionData)
			{
				ActionData->block::parse(parser, *ActionLength - headerSize, true);
			}
		}
	}

	void ActionBlock ::parseBlocks()
	{
		setText(L"ActionBlock");
		addChild(ActionLength, L"ActionLength: 0x%1!08X!", ActionLength->getData());
		addChild(
			ActionType,
			L"ActionType: %1!ws! = %2!d!",
			flags::InterpretFlags(flagActionType, ActionType->getData()).c_str(),
			ActionType->getData());

		switch (*ActionType)
		{
		case OP_FORWARD:
			addChild(
				ActionFlavor,
				L"ActionFlavor: %1!ws! = 0x%2!08X!",
				flags::InterpretFlags(flagOpForward, ActionFlavor->getData()).c_str(),
				ActionFlavor->getData());
			break;
		case OP_REPLY:
		case OP_OOF_REPLY:
			addChild(
				ActionFlavor,
				L"ActionFlavor: %1!ws! = 0x%2!08X!",
				flags::InterpretFlags(flagOPReply, ActionFlavor->getData()).c_str(),
				ActionFlavor->getData());
			break;
		default:
			addChild(ActionFlavor, L"ActionFlavor: 0x%1!08X!", ActionFlavor->getData());
			break;
		}

		addChild(ActionFlags, L"ActionFlags: 0x%1!08X!", ActionFlags->getData());
		addChild(ActionData);
	}

	void RuleAction ::parse()
	{
		if (m_bExtended)
		{
			namedPropertyInformation = block::parse<NamedPropertyInformation>(parser, 0, false);
			RuleVersion = blockT<DWORD>::parse(parser);
		}

		NoOfActions = m_bExtended ? blockT<DWORD>::parse(parser) : blockT<DWORD>::parse<WORD>(parser);
		if (*NoOfActions < _MaxEntriesSmall)
		{
			ActionBlocks.reserve(*NoOfActions);
			for (DWORD i = 0; i < *NoOfActions; i++)
			{
				auto actionBlock = std::make_shared<ActionBlock>(m_bExtended);
				actionBlock->block::parse(parser, false);
				ActionBlocks.push_back(actionBlock);
			}
		}
	}

	void RuleAction ::parseBlocks()
	{
		setText(m_bExtended ? L"Extended Rule Action" : L"Rule Action");
		if (m_bExtended)
		{
			addChild(namedPropertyInformation);
			addChild(RuleVersion, L"RuleVersion: 0x%1!08X!", RuleVersion->getData());
		}

		addChild(NoOfActions, L"NoOfActions: 0x%1!08X!", NoOfActions->getData());
		auto i = 0;
		for (const auto& actionBlock : ActionBlocks)
		{
			addChild(actionBlock, L"ActionBlocks[%1!d!]", i++);
		}
	}
} // namespace smartview