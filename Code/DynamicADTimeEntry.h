#ifndef _DYNAMIC_AD_TIME_ENTRY_H_
#define _DYNAMIC_AD_TIME_ENTRY_H_

#include <string>

class DynamicADTimeGroup;

class DynamicADTimeEntry
{
public:
	static enum TimeType {
		TimeType_Day=0,
		TimeType_Hour,
		NUM_TIMETYPES
	};

	static const std::string s_vStrTimeType[];

public:
	DynamicADTimeEntry(const TimeType& timeType);
	~DynamicADTimeEntry();

	void SetKeyData(const std::string& keyData) {m_strKeyData = keyData;}
	const std::string& GetKeyData() {return m_strKeyData;}
	const std::string& GetKeyDataInv() {return m_strKeyDataInv;}

	void SetTimeData(const std::string& timeData) {m_strTimeData = timeData;}
	const std::string& GetTimeData() {return m_strTimeData;}

	void SetTimeGroup(DynamicADTimeGroup* tGroup) {m_timeGroup = tGroup;}
	DynamicADTimeGroup* GetTimeGroup() {return m_timeGroup;}
	TimeType GeTimeType(){return m_timeType;}
	void FixDataFormat();

private:
	TimeType m_timeType;
	std::string m_strKeyData;
	std::string m_strKeyDataInv;
	std::string m_strTimeData;

	DynamicADTimeGroup* m_timeGroup;
};

#endif