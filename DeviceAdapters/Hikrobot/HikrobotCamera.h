///////////////////////////////////////////////////////////////////////////////
// FILE:          HikrobotCamera.h
// SUBSYSTEM:     DeviceAdapters
//-----------------------------------------------------------------------------
// DESCRIPTION:   Adapter for hikrobot  Cameras
//
// Copyright 2023 andy.xin
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this 
// list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this
// list of conditions and the following disclaimer in the documentation and/or other 
// materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may
// be used to endorse or promote products derived from this software without specific 
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
// DAMAGE.


#pragma once

#include "DeviceBase.h"
#include "DeviceThreads.h"
#include <string>
#include <vector>
#include <map>
#include "ImageMetadata.h"
#include "ImgBuffer.h"
#include <iostream>
#include "MvCamera.h"


//////////////////////////////////////////////////////////////////////////////
// Error codes
//
//#define ERR_UNKNOWN_BINNING_MODE 410
enum
{
	ERR_SERIAL_NUMBER_REQUIRED = 20001,
	ERR_SERIAL_NUMBER_NOT_FOUND,
	ERR_CANNOT_CONNECT,
};

//////////////////////////////////////////////////////////////////////////////
// hikrobot camera class
//////////////////////////////////////////////////////////////////////////////
//Callback class for putting frames in circular buffer as they arrive

//class CTempCameraEventHandler;
//class CircularBufferInserter;
class HikrobotCamera : public CCameraBase<HikrobotCamera>  {
public:
	HikrobotCamera();
	~HikrobotCamera();

	// MMDevice API
	// ------------
	int Initialize();
	int Shutdown();

	void GetName(char* name) const;      
	bool Busy() {return false;}
	

	// MMCamera API
	// ------------
	int SnapImage();
	const unsigned char* GetImageBuffer();
	void* Buffer4ContinuesShot;
	
	unsigned  GetNumberOfComponents() const;
	unsigned GetImageWidth() const;
	unsigned GetImageHeight() const;
	unsigned GetImageBytesPerPixel() const;

	unsigned GetBitDepth() const;
	long GetImageBufferSize() const;
	double GetExposure() const;
	void SetExposure(double exp);
	int SetROI(unsigned x, unsigned y, unsigned xSize, unsigned ySize); 
	int GetROI(unsigned& x, unsigned& y, unsigned& xSize, unsigned& ySize); 
	int ClearROI();
	void ReduceImageSize(int64_t Width, int64_t Height);
	int GetBinning() const;
	int SetBinning(int binSize);
	int IsExposureSequenceable(bool& seq) const {seq = false; return DEVICE_OK;}
	//void RGBPackedtoRGB(void* destbuffer, const CGrabResultPtr& ptrGrabResult);
	int SetProperty(const char* name, const char* value);
	int CheckForBinningMode(CPropertyAction *pAct);

	void AddToLog(std::string msg) const;			//不推荐使用
	void MvWriteLog(char* file, int line, char* pDevID, const char* fmt, ...) const ;	//推荐使用


	void CopyToImageBuffer(MV_FRAME_OUT* pstFrameOut);
	//CImageFormatConverter *converter;
    //CircularBufferInserter *ImageHandler_;
	//std::string EnumToString(EDeviceAccessiblityInfo DeviceAccessiblityInfo);
	void UpdateTemperature();

	/**
	* Starts continuous acquisition.
	*/
	int StartSequenceAcquisition(long numImages, double interval_ms, bool stopOnOverflow);
	int StartSequenceAcquisition(double interval_ms);
	int StopSequenceAcquisition();
	int PrepareSequenceAcquisition();

	/**
	* Flag to indicate whether Sequence Acquisition is currently running.
	* Return true when Sequence acquisition is active, false otherwise
	*/
	bool IsCapturing();

	//Genicam Callback
	//void ResultingFramerateCallback(GenApi::INode* pNode);


	// action interface
	// ----------------
	int OnAcqFramerate(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnAcqFramerateEnable(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnAutoExpore(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnAutoGain(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnTestPattern(MM::PropertyBase* pProp, MM::ActionType eAct);

	int OnBinning(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnBinningMode(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnDeviceLinkThroughputLimit(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnExposure(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnGain(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnHeight(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnInterPacketDelay(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnLightSourcePreset(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnOffset(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnPixelType(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnResultingFramerate(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnReverseX(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnReverseY(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnSensorReadoutMode(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnShutterMode(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnTemperature(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnTemperatureState(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnTriggerMode(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnTriggerSource(MM::PropertyBase* pProp, MM::ActionType eAct);
	int OnWidth(MM::PropertyBase* pProp, MM::ActionType eAct);
public:
	CMvCamera* GetCamera() {
		return m_pCamera;
	}


	bool IsColor(MvGvspPixelType enType);
	bool IsMono(MvGvspPixelType enType);

	void SetPixConfig(bool bMono);


private:
	bool IsAvailable(const char* strName);
	bool IsWritable(const char* strName);

	int EnumDevice();

	void SetLogBasicInfo(std::string msg);


	static  unsigned int  __stdcall ImageRecvThread(void* pUser);
	void    ImageRecvThreadProc();


	unsigned PixTypeProc(MvGvspPixelType enPixelType, unsigned int& nChannelNum, MvGvspPixelType& enDstPixelType);
private:

	// 基础日志记录信息
	std::string m_strBasiceLog;
	char      m_chDevID[128];// 设备ID

	MV_CC_DEVICE_INFO_LIST  m_stDevList;
	CMvCamera * m_pCamera;


    int m_nComponents;		//组件个数（通道个数）
	unsigned m_nbitDepth;    //图像 占的字节个数

	unsigned maxWidth_, maxHeight_;
	int64_t DeviceLinkThroughputLimit_;
	int64_t InterPacketDelay_;
	double ResultingFrameRatePrevious;
	double acqFramerate_, acqFramerateMax_, acqFramerateMin_;
	double exposure_us_, exposureMax_, exposureMin_;
	double gain_, gainMax_, gainMin_;
	double offset_, offsetMin_, offsetMax_;


	std::string binningFactor_;
	std::string pixelType_;
	std::string reverseX_, reverseY_;
	std::string sensorReadoutMode_;
	std::string setAcqFrm_;
	std::string shutterMode_;
	std::string temperature_;
	std::string temperatureState_;
	

	void* imgBuffer_;
	long imgBufferSize_;
	ImgBuffer img_;


	unsigned char* m_pConvertData;
	long m_nConvertDataLen; 


	bool m_bInitialized;

	void ResizeSnapBuffer();
	bool m_bGrabbing;			//取流工作状态

	bool  m_bRecvRuning;		//取流线程状态
	HANDLE m_hImageRecvThreadHandle;	//取流线程句柄
	
};

//Enumeration used for distinguishing different events.
enum TemperatureEvents
{
	TempCritical = 100,
	TempOverTemp = 200    
};

// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 5;


// Example handler for camera events.
// class CTempCameraEventHandler : public CBaslerUniversalCameraEventHandler
// {
// private:
// 	HikrobotCamera* dev_;
// public:
// 	CTempCameraEventHandler(HikrobotCamera* dev);
// 	virtual void OnCameraEvent(CBaslerUniversalInstantCamera& camera, intptr_t userProvidedId, GenApi::INode* pNode);
// };
// 
// 
// class CircularBufferInserter : public CImageEventHandler {
// private:
// 	HikrobotCamera* dev_;
// 
// public:
// 	CircularBufferInserter(HikrobotCamera* dev);
// 
// 	virtual void OnImageGrabbed( CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult);
// };

