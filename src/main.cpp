/**
 * libdatachannel media sender example
 * Copyright (c) 2020 Staz Modrzynski
 * Copyright (c) 2020 Paul-Louis Ageneau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "rtc/rtc.hpp"
#include "rtc/rtp.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <nlohmann/json.hpp>

#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
typedef int SOCKET;
#endif

using nlohmann::json;

const int BUFFER_SIZE = 2048;

json wrap_offer(const rtc::Description& desc) {
    return { { "type", "offer"  },
             { "payload", desc } };                // rtc::Description JSON’e gömülür
}

json wrap_answer(const rtc::Description& desc) {
    return { { "type", "answer" },
             { "payload", desc } };
}

json wrap_candidate(const rtc::Candidate& cand) {
    return { { "type", "candidate" },
             { "payload", cand } };                // rtc::Candidate yine JSON’e gömülür
}

int main()
{
	rtc::InitLogger(rtc::LogLevel::Error);
	bool isUser = false;
	auto ws = std::make_shared<rtc::WebSocket>();
	auto pc = std::make_shared<rtc::PeerConnection>();

	ws->open("ws://feedacat.live:8000/ws?room=cam42&role=camera");

	while(!ws->isOpen()){
		std::cout << "Failed to open WebSocket connection\n";
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	std::cout << "WebSocket connection opened successfully\n";

	ws->onMessage([&](rtc::message_variant data){
        auto m = json::parse(std::get<std::string>(data));
		// std::cout << "Received message: " << m.dump() << "\n";
  		if(m["type"] == "answer") {
			rtc::Description answer(m["payload"]["sdp"].get<std::string>(), "answer");
			pc->setRemoteDescription(answer);
			std::cout << "Received answer: " << m["payload"]["sdp"] << "\n";
		} 
		else if(m["type"] == "join") {
            std::cout << "Joined room: " << m["room"] << "\n";}
		else if(m["type"] == "ready"){
			isUser = true;
		}
        else if(m["type"] == "bye") {
			std::cout << "client bye bye" << "\n";
        } });


	try {

		pc->onStateChange(
		    [](rtc::PeerConnection::State state) { std::cout << "State: " << state << std::endl; });

		pc->onGatheringStateChange([&pc, &ws](rtc::PeerConnection::GatheringState state) {
			std::cout << "Gathering State: " << state << std::endl;
			if (state == rtc::PeerConnection::GatheringState::Complete) {
				auto description = pc->localDescription();
				if (description.has_value()) {
					    json msg = {
							{"type", "offer"},
							{"payload", {
								{"type", "offer"},
								{"sdp", description},
							}}          // libdatachannel to_json hazır
						};
					ws->send( msg.dump() );
					std::cout << msg.dump() << "\n";
				} else {
					std::cerr << "No local description available." << std::endl;
				}
			}
		});

		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
		struct sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addr.sin_port = htons(6000);

		if (bind(sock, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) < 0)
			throw std::runtime_error("Failed to bind UDP socket on 127.0.0.1:6000");

		int rcvBufSize = 212992;
		setsockopt(sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char *>(&rcvBufSize),
		           sizeof(rcvBufSize));

		const rtc::SSRC ssrc = 42;
		rtc::Description::Video media("video", rtc::Description::Direction::SendOnly);
		media.addH264Codec(96); // Must match the payload type of the external h264 RTP stream
		media.addSSRC(ssrc, "video-send");
		auto track = pc->addTrack(media);

		pc->setLocalDescription();
		
		// std::cout << "RTP video stream expected on localhost:6000" << std::endl;
		// std::cout << "Please copy/paste the answer provided by the browser: " << std::endl;
		// std::string sdp;
		// std::getline(std::cin, sdp);

		// json j = json::parse(sdp);
		// rtc::Description answer(j["sdp"].get<std::string>(), j["type"].get<std::string>());
		// pc->setRemoteDescription(answer);

		// Receive from UDP
		while(pc->state() != rtc::PeerConnection::State::Connected) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		char buffer[BUFFER_SIZE];
		int len;
		while ((len = recv(sock, buffer, BUFFER_SIZE, 0)) >= 0) {
			if (len < sizeof(rtc::RtpHeader) || !track->isOpen())
				continue;

			auto rtp = reinterpret_cast<rtc::RtpHeader *>(buffer);
			rtp->setSsrc(ssrc);

			track->send(reinterpret_cast<const std::byte *>(buffer), len);
		}

	} catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}