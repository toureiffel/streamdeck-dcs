//==============================================================================
/**
@file       MyStreamDeckPlugin.cpp

@brief      DCS-BIOS Commands from Streamdeck plugin

@copyright  (c) 2019, Madjack Avionics, based on 
			(c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "MyStreamDeckPlugin.h"
#include <atomic>
#include <set>

#include "Common/ESDConnectionManager.h"


#pragma comment(lib, "Winmm.lib")


#define DCS_ADDRESS "127.0.0.1"
#define DCS_PORT 7778


MyStreamDeckPlugin::MyStreamDeckPlugin()
{
	unsigned short port = DCS_PORT;
	char address_ascii[16] = DCS_ADDRESS;

	memset(&m_serv_addr, 0, sizeof(m_serv_addr));
	m_serv_addr.sin_family = AF_INET;
	m_serv_addr.sin_port = htons(port);
	socket_error = inet_pton(AF_INET, address_ascii, &(m_serv_addr.sin_addr));
	if (socket_error != 0) {		// if != 0, OK
		WSADATA wsaData;
		socket_error = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (socket_error == 0) {	// if == 0, OK
			if ((m_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
				socket_error = 1;	// if socket_error, there was an error
			}
		}
	}
}

MyStreamDeckPlugin::~MyStreamDeckPlugin()
{
	closesocket(m_sockfd);
	WSACleanup();
}

int MyStreamDeckPlugin::send_message(std::string buf)
{
	ssize_t bytes_sent;
	bytes_sent = sendto(m_sockfd, buf.c_str(), buf.length(), 0, (struct sockaddr*)&m_serv_addr, sizeof(m_serv_addr));
	return bytes_sent == -1 ? -1 : 0;
}

void MyStreamDeckPlugin::KeyDownForAction(const std::string& inAction, const std::string& inContext, const json& inPayload, const std::string& inDeviceID)
{
	json jsonSettings;
	EPLJSONUtils::GetObjectByName(inPayload, "settings", jsonSettings);
	std::string command = EPLJSONUtils::GetStringByName(jsonSettings, "command");
	std::string value = EPLJSONUtils::GetStringByName(jsonSettings, "valueOn");
	std::string valueAlt = EPLJSONUtils::GetStringByName(jsonSettings, "valueOff");

	int res = 0;
	std::string message(command + " " + value + "\n");
	if (valueAlt.compare("TOGGLE"))
		res = send_message(message);
}

void MyStreamDeckPlugin::KeyUpForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
{
	json jsonSettings;
	EPLJSONUtils::GetObjectByName(inPayload, "settings", jsonSettings);
	std::string command = EPLJSONUtils::GetStringByName(jsonSettings, "command");
	std::string value = EPLJSONUtils::GetStringByName(jsonSettings, "valueOff");
	std::string valueAlt = EPLJSONUtils::GetStringByName(jsonSettings, "valueOn");
	mConnectionManager->LogMessage(command + " " + value + "\n");

	int res = 0;
	std::string message(command + " " + value + "\n");
	if (valueAlt.compare("TOGGLE"))
		res = send_message(message);
}

void MyStreamDeckPlugin::WillAppearForAction(const std::string& inAction, const std::string& inContext, const json& inPayload, const std::string& inDeviceID)
{
	// Remember the context
	mVisibleContextsMutex.lock();
	mVisibleContexts.insert(inContext);
	mVisibleContextsMutex.unlock();
}

void MyStreamDeckPlugin::WillDisappearForAction(const std::string& inAction, const std::string& inContext, const json& inPayload, const std::string& inDeviceID)
{
	// Remove the context
	mVisibleContextsMutex.lock();
	mVisibleContexts.erase(inContext);
	mVisibleContextsMutex.unlock();
}

void MyStreamDeckPlugin::DeviceDidConnect(const std::string& inDeviceID, const json& inDeviceInfo)
{
	// Nothing to do
}

void MyStreamDeckPlugin::DeviceDidDisconnect(const std::string& inDeviceID)
{
	// Nothing to do
}

void MyStreamDeckPlugin::SendToPlugin(const std::string& inAction, const std::string& inContext, const json& inPayload, const std::string& inDeviceID)
{
	// Nothing to do
}

void MyStreamDeckPlugin::DidReceiveSettings(const std::string& inAction, const std::string& inContext, const json& inPayload, const std::string& inDeviceID)
{
	WillAppearForAction(inAction, inContext, inPayload, inDeviceID);
}
