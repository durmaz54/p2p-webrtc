//
// Created by durmaz on 30.06.2025.
//

#ifndef P2S_H
#define P2S_H

#include <string>
#include <optional>


class Peer2ServerCommunication {
public:
    Peer2ServerCommunication() : m_sdp(std::nullopt), m_candidate(std::nullopt) {
        // Constructor implementation
    }

    void connect() {
    }

    bool sendSdp(const std::string &sdp) {
    }

    bool sendCandidate(const std::string &candidate) {
    }

    std::optional<std::string> &getAnswerCandidate() {
        return m_candidate;
    }

    std::optional<std::string> &getAnswerSdp() {
        return m_sdp;
    }

private:
    std::optional<std::string> m_sdp, m_candidate;
};


#endif //P2S_H