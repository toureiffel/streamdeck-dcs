//==============================================================================
/**
@file       MyStreamDeckPlugin.h

@brief      DCS-BIOS Commands from Streamdeck plugin

@copyright  (c) 2019, Madjack Avionics, based on 
			(c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "pdh.h"
#include <mutex>
#include "Common/ESDBasePlugin.h"
#include "Common/EPLJSONUtils.h"


typedef int socklen_t;
typedef int ssize_t;


class MyStreamDeckPlugin : public ESDBasePlugin
{
public:
	MyStreamDeckPlugin();
	virtual ~MyStreamDeckPlugin();

	void KeyDownForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	void KeyUpForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;

	void WillAppearForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	void WillDisappearForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;

	void DeviceDidConnect(const std::string& inDeviceID, const json &inDeviceInfo) override;
	void DeviceDidDisconnect(const std::string& inDeviceID) override;

	void SendToPlugin(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	void DidReceiveSettings(const std::string& inAction, const std::string& inContext, const json& inPayload, const std::string& inDeviceID);

private:
	SOCKET m_sockfd;
	struct sockaddr_in m_serv_addr;
	int socket_error;
	
	int send_message(std::string buf);

	std::mutex mVisibleContextsMutex;
	std::set<std::string> mVisibleContexts;
};
