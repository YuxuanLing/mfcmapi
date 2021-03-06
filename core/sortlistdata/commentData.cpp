﻿#include <core/stdafx.h>
#include <core/sortlistdata/commentData.h>
#include <core/sortlistdata/sortListData.h>

namespace sortlistdata
{
	void commentData::init(sortListData* data, _In_opt_ const _SPropValue* lpOldProp)
	{
		if (!data) return;
		data->init(std::make_shared<commentData>(lpOldProp), true);
	}

	commentData::commentData(_In_opt_ const _SPropValue* lpOldProp) noexcept
	{
		m_lpOldProp = lpOldProp;
		m_lpNewProp = nullptr;
	}
} // namespace sortlistdata