//
//  Online.cpp
//  Gems
//
//  Created by Sunil Patel on 11/09/2017.
//
//

#include "Online.hpp"
#include "SaveData.hpp"

#include "json/document.h"
#include "json/rapidjson.h"
#include "json/stringbuffer.h"
#include "json/writer.h"

static void getKey(char *buffer) {
    const unsigned char source[] = {
        0xc3, 0x69, 0x8e, 0x43, 0xd7, 0x0a, 0x9a, 0x3a, 0xe4, 0x6a, 0x48, 0x0e, 
        0x04, 0x54, 0xca, 0x92, 0x94, 0x6c, 0xbd, 0xe1, 0x88, 0xfe, 0xe8, 0xf7, 
        0x17, 0x7a, 0xba, 0x94, 0xc0, 0x62, 0xe4, 0x0c, 0x36, 0x62, 0x18, 0x63, 
        0x39, 0xaf, 0x10, 0x6c, 0x35, 0x0e, 0x88, 0x05, 0xd1, 0x43, 0x2a, 0xc0, 
        0xd4, 0x7d, 0x45, 0xe3, 0xc5, 0xf7, 0x7a, 0xd6, 0x8a, 0x6a, 0xa4, 0x88, 
        0x74, 0x74, 0x49, 0xb1, 0x00
    };
    unsigned long a = 1664525;
    unsigned long c = 1013904223;
    unsigned long m = 4294967296;
    unsigned long x = 14764;
	
    for (int i = 0; i < 64; i++) {
        unsigned char X = (char) x;
        unsigned char k = (source[i] ^ X);
        buffer[i] = k;
        x = (a * x + c) % m;
    }
}

Online *Online::get() {
	static Online *instance = new Online();
	return instance;
}

void Online::submitArenaScore(ArenaScore score) {
	// ArenaScore + uuid in json
	// ignore rank (submission doesn't have it)
	// ...
	// encrypt json with the key above (xor)
	std::string uuid = SaveData::getUuid();
	
	rapidjson::Document doc;
	doc.SetObject();
	
	auto& allocator = doc.GetAllocator();
	
	//doc.AddMember("name", score.name, allocator);
	//doc.AddMember("avatar", score.avatar, allocator);
	//doc.AddMember("killedby", score.killedby, allocator);
	doc.AddMember("score", score.score, allocator);
	//doc.AddMember("uuid", uuid, allocator);
	rapidjson::Value theInventory(rapidjson::kArrayType);
	for (auto s : score.inventory) {
		//theInventory.PushBack(s, allocator);
	}
	doc.AddMember("inventory", theInventory, allocator);
	
	rapidjson::StringBuffer buffer{};
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	
	std::string payload(buffer.GetString(), buffer.GetSize());
	const char *encrypt_me = payload.c_str();
	auto len = strlen(encrypt_me);
	char *encrypted = new char[len + 1];
	char *decrypted = new char[len + 1];
	char magic[65];
	magic[64] = 0;
	getKey(magic);
	printf("magic: %s\n", magic);
	encrypted[len] = '\0';
	for (int i = 0; i < len; i++) {
		encrypted[i] = encrypt_me[i] ^ magic[i % 64];
		decrypted[i] = encrypted[i] ^ magic[i % 64];
	}
	printf("%s\n->\n%s\n->\n%s\n", encrypt_me, encrypted, decrypted);
	delete [] encrypted;
	delete [] decrypted;
}

ArenaScore Online::getMyArenaScore() {
	// ArenaScore + uuid in json
	// ...
	return {};
}

std::vector<ArenaScore> Online::getTopArenaScores() {
	// ArenaScore + uuid in json
	// ...
	return {{}};
}

Online::Online() {}
