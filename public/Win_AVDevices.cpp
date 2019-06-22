#include "Win_AVDevices.h"

#include <dvdmedia.h>
#include <atlconv.h>
#include <array>


bool SortDevice(const DeviceInfo& device1, const DeviceInfo& device2) {
	if (device1.BestParam.width > device2.BestParam.width)
		return true;
	return false;
}


int GuidToString(const GUID &guid, char* buffer) {
	int buf_len = 64;
	snprintf(
		buffer,
		buf_len,
		"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	return 0;
}

int GetMajorType(GUID guid, char* buffer) {
	memset(buffer, 0, 256);
	if (guid == MEDIATYPE_Video) {
		snprintf(buffer, 256, "MEDIATYPE_Video");
		return 0;
	}
	if (guid == MEDIATYPE_Audio) {
		snprintf(buffer, 256, "MEDIATYPE_Audio");
		return 0;
	}
	if (guid == MEDIASUBTYPE_RGB24) {
		snprintf(buffer, 256, "MEDIATYPE_Stream");
		return 0;
	}
	return -1;
}

int GetSubType(GUID guid, char* buffer) {
	memset(buffer, 0, 256);
	if (guid == MEDIASUBTYPE_YUY2) {
		snprintf(buffer, 256, "MEDIASUBTYPE_YUY2");
		return 0;
	}
	if (guid == MEDIASUBTYPE_MJPG) {
		snprintf(buffer, 256, "MEDIASUBTYPE_MJPG");
		return 0;
	}
	if (guid == MEDIASUBTYPE_RGB24) {
		snprintf(buffer, 256, "MEDIASUBTYPE_RGB24");
		return 0;
	}
	return -1;
}

int GetFormatType(GUID guid, char* buffer) {
	memset(buffer, 0, 256);
	if (guid == FORMAT_VideoInfo) {
		snprintf(buffer, 256, "FORMAT_VideoInfo");
		return 0;
	}
	if (guid == FORMAT_VideoInfo2) {
		snprintf(buffer, 256, "FORMAT_VideoInfo2");
		return 0;
	}
	return -1;
}


int DsGetOptionDevice(IMoniker* pMoniker, DeviceInfo& info) {
	USES_CONVERSION;
	HRESULT hr = NULL;
	IBaseFilter *pFilter;
	hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
	if (!pFilter) {
		return -1;
	}
	IEnumPins * pinEnum = NULL;
	IPin * pin = NULL;
	if (FAILED(pFilter->EnumPins(&pinEnum))) {
		pinEnum->Release();
		return -1;
	}
	pinEnum->Reset();
	ULONG pinFetched = 0;
	while (SUCCEEDED(pinEnum->Next(1, &pin, &pinFetched)) && pinFetched) {
		if (!pin) {
			continue;
		}
		PIN_INFO pinInfo;
		if (FAILED(pin->QueryPinInfo(&pinInfo))) {
			continue;
		}
		if (pinInfo.dir != PINDIR_OUTPUT) {
			continue;
		}

		IEnumMediaTypes *mtEnum = NULL;
		AM_MEDIA_TYPE   *mt = NULL;
		if (FAILED(pin->EnumMediaTypes(&mtEnum)))
			break;
		mtEnum->Reset();
		ULONG mtFetched = 0;
		while (SUCCEEDED(mtEnum->Next(1, &mt, &mtFetched)) && mtFetched) {
			char majorbuf[256];
			if (GetMajorType(mt->majortype, majorbuf) != 0) {
				GuidToString(mt->majortype, majorbuf);
			}
			char subtypebuf[256];
			if (GetSubType(mt->subtype, subtypebuf) != 0) {
				GuidToString(mt->subtype, subtypebuf);
			}
			char formatbuf[256];
			if (GetFormatType(mt->formattype, formatbuf) != 0) {
				GuidToString(mt->formattype, formatbuf);
			}
			BITMAPINFOHEADER* bmi = NULL;
			int avgTime;
			if (mt->formattype == FORMAT_VideoInfo) {
				if (mt->cbFormat >= sizeof(VIDEOINFOHEADER)) {
					VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(mt->pbFormat);
					bmi = &(pVih->bmiHeader);
					avgTime = pVih->AvgTimePerFrame;
				}
			}
			else if (mt->formattype == FORMAT_VideoInfo2) {
				if (mt->cbFormat >= sizeof(VIDEOINFOHEADER2)) {
					VIDEOINFOHEADER2* pVih = reinterpret_cast<VIDEOINFOHEADER2*>(mt->pbFormat);
					bmi = &(pVih->bmiHeader);
					avgTime = pVih->AvgTimePerFrame;
				}
			}
			if (bmi) {
				info.SetResolution(bmi->biWidth, bmi->biHeight, avgTime);
			}
			else {
				printf("\tNo find\n");
			}
		}
		pin->Release();
	}
	return 0;
}


HRESULT DsGetAudioVideoInputDevices(std::vector<DeviceInfo>& deviceVec, REFGUID guidValue)
{
	DeviceInfo info;
	HRESULT hr;

	// 初始化  
	deviceVec.clear();

	// 初始化COM  
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (FAILED(hr)) {
		printf("Init error!\n");
		return hr;
	}
	// 创建系统设备枚举器实例  
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr)) {
		CoUninitialize();
		printf("Create instance error!\n");
		return hr;
	}
	// 获取设备类枚举器  
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(guidValue, &pEnumCat, 0);
	if (hr != S_OK) {
		CoUninitialize();
		//pSysDevEnum->Release();
		return hr;
	}

	// 枚举设备名称  
	IMoniker *pMoniker = NULL;
	ULONG cFetched;
	while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK) {
		IPropertyBag *pPropBag;
		hr = pMoniker->BindToStorage(NULL, NULL, IID_IPropertyBag, (void **)&pPropBag);
		if (FAILED(hr)) {
			pMoniker->Release();
			continue;
		}

		// 获取设备友好名  
		VARIANT varName;
		VariantInit(&varName);
		hr = pPropBag->Read(L"FriendlyName", &varName, NULL);
		if (SUCCEEDED(hr)) {
			StringCchCopy(info.FriendlyName, MAX_FRIENDLY_NAME_LENGTH, varName.bstrVal);

			// 获取设备Moniker名  
			LPOLESTR pOleDisplayName = reinterpret_cast<LPOLESTR>(CoTaskMemAlloc(MAX_MONIKER_NAME_LENGTH * 2));
			if (pOleDisplayName != NULL) {
				hr = pMoniker->GetDisplayName(NULL, NULL, &pOleDisplayName);
				if (SUCCEEDED(hr)) {
					StringCchCopy(info.MonikerName, MAX_MONIKER_NAME_LENGTH, pOleDisplayName);
					//获取设备支持的分辨率
					DsGetOptionDevice(pMoniker, info);
					deviceVec.push_back(info);
				}
				CoTaskMemFree(pOleDisplayName);
			}
		}
		VariantClear(&varName);
		pPropBag->Release();
		pMoniker->Release();
	}

	pEnumCat->Release();
	pSysDevEnum->Release();
	CoUninitialize();

	std::sort(deviceVec.begin(), deviceVec.end(), SortDevice);

	return hr;
}
