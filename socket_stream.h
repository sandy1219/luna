#pragma once

#include "socket_helper.h"
#include "socket_mgr.h"
#include "io_buffer.h"

struct XSocketStream : ISocketStream
{
	XSocketStream() { m_szError[0] = 0; }
	~XSocketStream();

	void Activate();
	void Send(const void* pvData, size_t uDataLen) override;
	void SetDataCallback(const std::function<void(BYTE*, size_t)>& callback) override { m_DataCallback = callback; }
	void SetErrorCallback(const std::function<void()>& callback) override { m_ErrorCallback = callback; }
	const char* GetRemoteAddress() override { return m_strRemoteIP.c_str(); }
	const char* GetError(int* pnError = nullptr) override { if (pnError) *pnError = m_nError; return m_szError; };
	void Close() override;

	void StreamSend(const void* pvData, size_t uDataLen);

#ifdef _MSC_VER
	void AsyncSend();
	void AsyncRecv();
	void OnComplete(WSAOVERLAPPED* pOVL, DWORD dwLen);
	void OnRecvComplete(size_t uLen);
	void OnSendComplete(size_t uLen);
#endif

#if defined(__linux) || defined(__APPLE__)
	void OnSendAble();
	void OnRecvAble();
#endif

	void DispatchPackage();
	void SetError(int nError = 0);
	void SetError(const char szUserError[]);

	std::string m_strRemoteIP;
	socket_t m_nSocket = INVALID_SOCKET;
	bool m_bErrored = false;
	int  m_nError = 0;
	char m_szError[120];
	bool m_bErrorNotified = false;
	bool m_bUserClosed = false;
	XSocketBuffer m_RecvBuffer;
	XSocketBuffer m_SendBuffer;

#ifdef _MSC_VER
	DWORD m_dwCompletionKey = 0;
	WSAOVERLAPPED m_wsSendOVL;
	WSAOVERLAPPED m_wsRecvOVL;
	bool m_bSendComplete = true;
	bool m_bRecvComplete = true;
#endif

#if defined(__linux) || defined(__APPLE__)
	bool m_bWriteAble = true;
#endif

	std::function<void(BYTE*, size_t)> m_DataCallback;
	std::function<void()> m_ErrorCallback;
};