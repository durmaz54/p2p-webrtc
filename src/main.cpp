#include <iostream>
#include <chrono>
#include <thread>

#include <pqxx/pqxx>
#include "rtc/rtc.hpp"
#include <nlohmann/json.hpp>

using nlohmann::json;


int main() {
    rtc::Configuration config;
    config.iceServers.emplace_back("mystunserver.org:3478");

    auto pc = std::make_shared<rtc::PeerConnection>(config);

    pc->onLocalDescription([](rtc::Description sdp) {
        // Send the SDP to the remote peer
        std::cout << "Local SDP: \n" << std::string(sdp) << "\n -------------- \n";
    });

    pc->onLocalCandidate([](rtc::Candidate candidate) {
        // Send the candidate to the remote peer
        std::cout << "Local Candidate: \n" << std::string(candidate) << "\n -------------- \n";
    });

    auto dc = pc->createDataChannel("test");
    std::this_thread::sleep_for(std::chrono::seconds(5));   

    auto descp = pc->localDescription();
    std::cerr << std::string(descp.value());

    return 0;
}